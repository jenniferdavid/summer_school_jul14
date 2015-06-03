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

#ifndef _PRECOMPUTED_PATTERN_RESPONSE_H
#define _PRECOMPUTED_PATTERN_RESPONSE_H

/** @file PrecomputedPatternResponse.h
 * @brief this header contains PrecomputedPatternResponse class, used to store hash information provided by a matrix of samples and features */

#include <vector>
#include <cmath> // abs
#include "_stdint.h"

#ifdef _MULTITHREAD
# include "Thread/thread_group.h"
# include "Thread/bind.h"
#endif

// #include <iostream>

/** */
// TODO: for caching-reason it is better to invert samples and feature order in the matrixes
template<class ResponseType>
struct PrecomputedPatternResponse {
public:
    /// number of samples
    int n_samples;
    /// number of feature preallocated
    int n_features;
    /// category list
    int *category;
    /// weights list
    double *weights; // TODO: EXTRA PARAMETERS! TODO: si puo' mettere fuori (?)
    /// response matrix
    ResponseType *response;
    /// precomputed bin
    int *bin;
    /// precomputed response ranges
    std::pair<ResponseType, ResponseType> *response_range;
    /// Quantization
    int number_of_bins;

private:

  /// internal precomputed used in multithread environment
    template<class TrainintSetType, class FeatureType>
    void internal_precompute_feat_wide(int f0, int f1, const TrainintSetType * set, const std::vector<FeatureType> * feats)
    {
        int feat_size = feats->size();
        int sample_size = set->Size();
        // export the data
        for(int j=f0; j<f1; ++j)
        {
            for(int i =0; i<sample_size; ++i)
            {
                category[i] = set->templates[i].category;

                ResponseType v = (*feats)[j].response(getData1(set->templates[i], *set), getData2(set->templates[i], *set));
                response[i + j * sample_size] = v;
                if(i == 0)
                {
                    response_range[j].first =response_range[j].second = v;
                }
                else
                {
                    // min
                    if(response_range[j].first > v) response_range[j].first = v;
                    // max
                    if(response_range[j].second < v) response_range[j].second = v;
                }

            }

            if(number_of_bins>0)
            {
                ResponseType delta = (response_range[j].second > response_range[j].first) ? (response_range[j].second - response_range[j].first + 1) : ResponseType(1);

                for(int i =0; i<set->Size(); ++i)
                {
                    // create quantization table
                    bin[i + j * sample_size] = (int) (  ((response[i + j * sample_size] - response_range[j].first) * number_of_bins) / delta );
                }
            }

        }

    }
public:

    PrecomputedPatternResponse(int n_bins) : n_samples(0), n_features(0), category(0), weights(0), response(0), response_range(0), bin(0), number_of_bins(n_bins) {}
    ~PrecomputedPatternResponse() {
        delete [] category;
        delete [] weights;
        delete [] response;
        delete [] response_range;
        delete [] bin;
    }

    /// allocate the response matrix
    void allocate(int samples, int feats) {
//     std::cout << "allocate memory for " << samples << " samples and "<< feats << " features" << std::endl;
        if(samples!=n_samples)
        {
            delete [] category;
            delete [] weights;
            weights = new double[samples];
            category = new int[samples];
        }
        if(samples!=n_samples || feats != n_features)
        {
            delete [] response;
            response = new ResponseType[static_cast<int64_t>(samples) * feats];
            delete [] bin;
            if(number_of_bins>0)
                bin = new int[static_cast<int64_t>(samples) * feats];
        }
        if(feats!=n_features)
        {
            delete [] response_range;
            response_range = new std::pair<ResponseType, ResponseType> [feats];
        }
        n_samples = samples;
        n_features = feats;

    }

    /// @param feats a vector of features to be precomputed
    /// @note it is possible to Precompute again features keeping old weights if the size is the same
    /// @todo multithreading!
    template<class TrainintSetType, class FeatureType>
    void Precompute(const TrainintSetType & set, const std::vector<FeatureType> & feats, int max_concurrent_jobs )
    {
        int feat_size = feats.size();
        int sample_size = set.Size();

        // allocate memory
        allocate(sample_size, feat_size);

#ifdef _MULTITHREAD
        if(max_concurrent_jobs > 1)
        {
            sprint::thread_group thread_pool_;

            for(int k=0; k<max_concurrent_jobs; ++k)
            {
                int f0 = (k * feat_size) / max_concurrent_jobs;
                int f1 = ((k+1) * feat_size) / max_concurrent_jobs;

                thread_pool_.create_thread(sprint::thread_bind(&PrecomputedPatternResponse::internal_precompute_feat_wide<TrainintSetType,FeatureType>, this, f0, f1, &set, &feats));

            }
            thread_pool_.join_all();
        }
        else
#endif          
        {
          /////// single thread /////////////////
            // export the data
            for(int i =0; i<sample_size; ++i)
            {
                category[i] = set.templates[i].category;

                for(int j=0; j<feat_size; ++j)
                {
                    ResponseType v = feats[j].response(getData1(set.templates[i], set), getData2(set.templates[i], set));
                    response[i + j * sample_size] = v;
                    if(i == 0)
                    {
                        response_range[j].first =response_range[j].second = v;
                    }
                    else
                    {
                        // min
                        if(response_range[j].first > v) response_range[j].first = v;
                        // max
                        if(response_range[j].second < v) response_range[j].second = v;
                    }

                }
            }
            
            if(number_of_bins>0)
            {
                // PRECOMPUTE BIN
                for(int j=0; j<feat_size; ++j)
                {
                    ResponseType delta = (response_range[j].second > response_range[j].first) ? (response_range[j].second - response_range[j].first + 1) : ResponseType(1);

                    for(int i =0; i<set.Size(); ++i)
                    {
                        // create quantization table
                        bin[i + j * sample_size] = (int) (  ((response[i + j * sample_size] - response_range[j].first) * number_of_bins) / delta );
                    }

                }

            }


            /////// single thread /////////////////
        }

    }
    
    /// compute bin
    void PrepareBins()
    {
        // export the data
        for(int j=0; j<n_features; ++j)
        {
            for(int i =0; i<n_samples; ++i)
            {
                ResponseType v = response[i + j * n_samples];
                if(i == 0)
                {
                    response_range[j].first =response_range[j].second = v;
                }
                else
                {
                    // min
                    if(response_range[j].first > v) response_range[j].first = v;
                    // max
                    if(response_range[j].second < v) response_range[j].second = v;
                }

            }

            if(number_of_bins>0)
            {
                ResponseType delta = (response_range[j].second > response_range[j].first) ? (response_range[j].second - response_range[j].first + 1) : ResponseType(1);

                for(int i =0; i<n_samples; ++i)
                {
                    // create quantization table
                    bin[i + j * n_samples] = (int) (  ((response[i + j * n_samples] - response_range[j].first) * number_of_bins) / delta );
                }
            }

        }
    }
    

    /// Initialize ugual weights
   void InitializeWeights()
   {
     double w = 1.0 / (double)n_samples;
      for(int i=0; i<n_samples; ++i)
        {
          weights[i] = (category[i] == 1) ? w : -w;
        }
   }
    
    /// Run an Indirect Classifier (classifier running on precomputed feature response) and report statistics
    /// @param c a classifier
    /// @param correct_tables an array of bool, n_samples long
    /// @param _wp,_wn,_match pointer to a single variable, modified 
    template<class Classifier>
    void ExecuteClassifier(const Classifier & c, bool *correct_tables, double *_wp, double *_wn, int *_match) const
    {
            double wp = 0.0, wn = 0.0;
            int match = 0;
            for(int i =0; i<n_samples; ++i)
            {
                int test = c(&response[i], n_samples );
                if(test == category[i])
                {
                    wp += std::abs(weights[i]);
                    correct_tables[i] = true;
                    match++;
                }
                else
                {
                    wn += std::abs(weights[i]);
                    correct_tables[i] = false;
                }
            }      
            
            *_wp = wp;
            *_wn = wn;
            *_match = match;
    }
    
    
};


#endif
