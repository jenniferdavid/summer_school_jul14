/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _PRECOMPUTED_BINARY_CLASSIFIER_ORACLE_H
#define _PRECOMPUTED_BINARY_CLASSIFIER_ORACLE_H

/** @file PrecomputedBinaryClassifierOracle.h
  * @brief methods concerning obtain best threshold (under some metrics) for a Binary Classifier Policy
  *        using a "sorted" response patterns.
  **/

#ifdef _MULTITHREAD
# include "Thread/thread.h"
# include "Thread/bind.h"
# include "Thread/thread_group.h"
#endif

#include <algorithm> // sort

#include "Pattern/PrecomputedPatternResponse.h"
#include "Classifier/IndirectBinaryClassifier.h"
#include "Classifier/BinaryClassifier.h"
#include "detail/BinaryClassifierOptimize.h"

template<class Policy>
class PrecomputedPatternBinaryClassifierOracle {
public:
    typedef IndirectBinaryClassifier<Policy> ClassifierType;

    template<class FeatureType>
    struct Traits {
        typedef BinaryClassifier<FeatureType, Policy> ClassifierType;
    };

    bool reoptimize;

   

private:

  /// TODO: questo e' il trainer per i DecisionStump<T> generalizzare
    template<class DataType>
    void build_decision_stump_heuristic(double *out_w, IndirectBinaryClassifier<Policy> *c, const PrecomputedPatternResponse<DataType> *src, int f0, int f1, float wp, float wn)
    {
        const int histo_bin_size = src->number_of_bins;
        double bestw = 0.0;
        std::pair<DataType, int> best_ret;
        const int n_samples = src->n_samples;
        int best_i = -1;
        // temporary list used in optimizer
        float *store = new float[histo_bin_size];

        // for each features
        for(int i = f0; i< f1; ++i)
        {
            // reset and create histogram

            ///// todo: place in library BEGIN ///////

            // reset store:
            for(int j = 0; j<histo_bin_size; ++j)
            {
                store[j] = 0.0f;
            }

            // create Pattern Response histogram
            for(int j=0; j<n_samples; ++j)
            {
                int k = j; //indexes[j];
                // DataType v = src->GetResponse(k)[i];
                int bin = src->bin[i * src->n_samples + k];
                store[bin] += src->weights[k];
            }
            ///// todo: place in library END ///////

            // evaluate feature: bucketindex and direction
            std::pair<int, int> ret;

            double w = SimpleOptimize(ret, store, histo_bin_size, wp, wn);
            // ret contain the best solution (threshold and diretion). Direction is not so important due to nature of decision tree

            if(w > bestw)
            {
                bestw = w;
                best_ret = ret;
                best_i = i;
            }

        }
        delete [] store;

        c->feature_index = best_i;

        if(reoptimize)
        {
            // REOPTIMIZE
            std::pair<DataType, float> * store = new std::pair<DataType, float> [n_samples];

            ///// todo: place in library BEGIN ///////

            // create Pattern Response
            for(int j=0; j< n_samples; ++j)
            {
                int k = j; // indexes[j];
                store[j].first = src->response[best_i * src->n_samples + k];
                store[j].second = src->weights[k]; /* (double) src.category[k] * src.weights[k]; */
            }

            // sort
            std::sort(&store[0], &store[n_samples]);

            ///// todo: place in library END ///////

            // evaluate feature: th and direction
            *out_w = Optimize(best_ret, store, n_samples, wp, wn, false);

            c->th = best_ret.first;
            c->parity = best_ret.second;
            //////

            delete [] store;

        }
        else
        {
            //////// convert bin to threshold BEGIN ////////
            DataType minr, maxr;

            minr = src->response_range[best_i].first;
            maxr = src->response_range[best_i].second;

            DataType delta = (maxr > minr) ? (maxr - minr + 1) : (1);

            *out_w = bestw;
            c->th = minr + ((best_ret.first * delta) / histo_bin_size); // ret.first;
            c->parity = best_ret.second;
            //////// convert bin to threshold END ////////
        }


    }

public:

    PrecomputedPatternBinaryClassifierOracle() : reoptimize(true) { }

    template<class DataType>
    double Train(IndirectBinaryClassifier<Policy> &root, const PrecomputedPatternResponse<DataType> & src, int max_concurrent_jobs)   {
        if(src.number_of_bins > 0)
        {
            // find the best feature!
            double bestw = 0.0;

            const int n_samples = src.n_samples;

            // precompute Wn, Wp
            float wp = 0.0, wn = 0.0;
            for(int i=0; i<n_samples; ++i)
            {
                const int k = i;
                if(src.category[k] == 1)
                    wp += src.weights[k];
                else
                    wn -= src.weights[k];
            }
            // split using threads

#ifdef _MULTITHREAD            
            if(max_concurrent_jobs>1)
            {
                sprint::thread_group pool;

                IndirectBinaryClassifier<Policy> *candidates = new IndirectBinaryClassifier<Policy>[max_concurrent_jobs];
                double *outw = new double[max_concurrent_jobs];

                for(int i =0; i<max_concurrent_jobs; ++i)
                {
                    int f0 = (src.n_features * i) / max_concurrent_jobs;
                    int f1 = (src.n_features * (i+1) ) / max_concurrent_jobs;

                    pool.create_thread(sprint::thread_bind(&PrecomputedPatternBinaryClassifierOracle<Policy>::template build_decision_stump_heuristic<DataType>, this, &outw[i], &candidates[i], &src, f0, f1, wp, wn) );
                }

                pool.join_all();

                for(int i=0; i<max_concurrent_jobs; ++i)
                {
                    if(outw[i] > bestw)
                    {
                        bestw = outw[i];
                        root = candidates[i];
                    }
                }

                delete [] outw;
                delete [] candidates;
            }
            else
#endif // #ifdef _MULTITHREAD              
            {
                build_decision_stump_heuristic(&bestw, &root, &src, 0, src.n_features, wp, wn);
            }


            return bestw;

        }
        else
        {
            std::cerr << "Error: no precompute bin. This oracle is not implemented now." << std::endl;
            return 0.0;
        }
    }
};


#endif
