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

#ifndef _ORACLE_DECISION_TREE_H
#define _ORACLE_DECISION_TREE_H

/** @file DecisionTree.h
 * @brief A preliminary header to hold class to create an IndirectDecisionTree using a PrecomputedPatternResponse
 * */

#include <cstring>
#include "Classifier/IndirectDecisionTree.h"
#include "Pattern/PrecomputedPatternResponse.h"
#include "Thread/thread_group.h"
#include "Thread/bind.h"
#include "detail/BinaryClassifierOptimize.h"

/// create an "Indirect" decision tree:
template<class DataType>
double BuildDecisionTree(IndirectDecisionTree<DataType> &root, const PrecomputedPatternResponse<DataType> & src, int max_depth, const std::vector<int> & indexes, int max_concurrent_jobs)
{
    double bestw = 0.0;

    const int n_samples  = indexes.size();
    // precompute Wn, Wp
    double wp = 0.0, wn = 0.0;
    for(int i=0; i<n_samples; ++i)
    {
        int k = indexes[i];
        if(src.category[k] == 1)
            wp += src.weights[k];
        else
            wn -= src.weights[k];
    }

#ifdef verbose
    std::cout << max_depth << "-- build decision tree with " << n_samples << " elements. wp:" << wp << ", wn:" << wn << ", wp+wn:" << wp+wn << std::endl;
#endif

    if(max_depth == 0)
    {
        if(wp>wn)
        {
#ifdef verbose
            std::cout << "\tmax depth reached: positive with " << wp << "\n";
#endif
            root.category = 1;
            return wp;
        }
        else
        {
#ifdef verbose
            std::cout << "\tmax depth reached: negative with " << wn << "\n";
#endif
            root.category = -1;
            return wn;
        }
    }

    // TODO: split using threads

    // temporary list used in optimizer
    std::pair<DataType, double> * store = new std::pair<DataType, double> [n_samples];

    ///// todo: place in library BEGIN ///////
    for(int i = 0; i<src.n_features; ++i)
    {
        // create Pattern Response
        for(int j=0; j< n_samples; ++j)
        {
            int k = indexes[j];
            store[j].first = src.response[i * src.n_samples + k];
            store[j].second =  src.weights[k]; /* (double) src.category[k] * src.weights[k]; */
        }

        // sort
        std::sort(&store[0], &store[n_samples]);

        ///// todo: place in library END ///////

        // evaluate feature: th and direction
        std::pair<DataType, int> ret;

        double w = Optimize(ret, store, n_samples, wp, wn, false);
        // ret contain the best solution (threshold and diretion). Direction is not so important due to nature of decision tree

        // evaluate threshold
        int left_size = 0;
        double w_left = 0.0;
//     double ewp = 0.0;

        for(int j=0; j< n_samples; ++j)
        {

            int k = indexes[j];
            // CORRECT SIDE
            if(src.response[i * src.n_samples + k] < ret.first)
            {
                left_size++;
                w_left += std::abs(src.weights[k]);
            }
        }

#ifdef verbose
        std::cout << "\t" <<  i << " | th: " << ret.first << ", polarity:" << ret.second << /*", w+:" << ewp << */ ", w:" << w << ", left:" << left_size << "(" << w_left << "), right:" << n_samples - left_size << "(" << wn+wp-w_left <<")"<< std::endl;
#endif

        if((left_size == 0) || (left_size == n_samples ))
        {
            // unusefull solution
#ifdef verbose
            std::cout << "\t invalid solution" << std::endl;
#endif
        }
        else
        {

            // BUILD PRECOMPUTE LEAVES
            std::vector<int> left_resp, right_resp;

            left_resp.reserve(left_size);
            right_resp.reserve(n_samples - left_size);

            // create leaves:
            for(int j=0; j< n_samples; ++j)
            {
                int k = indexes[j];
                if(src.response[i * src.n_samples + k] < ret.first)
                {
                    left_resp.push_back(k);
                }
                else
                {
                    right_resp.push_back(k);
                }
            }

            double w;

            IndirectDecisionTree<DataType> *left = new IndirectDecisionTree<DataType>();
            IndirectDecisionTree<DataType> *right = new IndirectDecisionTree<DataType>();

#ifdef verbose
            std::cout << max_depth << "-- left --\n";
#endif
            w = BuildDecisionTree(*left,  src, max_depth-1, left_resp, 1);
#ifdef verbose
            std::cout << max_depth << "-- right --\n";
#endif
            w += BuildDecisionTree(*right, src, max_depth-1, right_resp, 1);

#ifdef verbose
            std::cout << max_depth << "-- w:" << w << std::endl;
#endif

            if(w > bestw)
            {
                delete root.left;
                delete root.right;
                root.left = left;
                root.right = right;
                root.th = ret.first;
                root.classifier = i;
                bestw = w;
            }
            else
            {
                // free unused memory
                delete left;
                delete right;
            }

        }

    }

    delete [] store;

    if(bestw==0.0)
    {
#ifdef verbose
        std::cout << "no solution found:";
#endif
        if(wp>wn)
        {
#ifdef verbose
            std::cout << "\tpositive with w:" << wp <<"\n";
#endif
            root.category = 1;
            return wp;
        }
        else
        {
#ifdef verbose
            std::cout << "\tnegative with w:" << wn <<"\n";
#endif
            root.category = -1;
            return wn;
        }


    }

#ifdef verbose
    std::cout << "solution found with w:" << bestw << std::endl;
#endif

    return bestw;
}

namespace detail {

template<class DataType>
void build_decision_tree_heuristic(double *outw, IndirectDecisionTree<DataType> * root, const PrecomputedPatternResponse<DataType> * src, int f0, int f1, int max_depth, const int * indexes, int n_samples, int histo_bin_size, float wp, float wn, bool optimize)
{
    double bestw = 0.0;
    // temporary list used in optimizer
    float *store = new float[histo_bin_size];

    for(int i = f0; i< f1; ++i)
    {
        // reset and create histogram

        ///// todo: place in library BEGIN ///////

        for(int j = 0; j<histo_bin_size; ++j)
        {
            store[j] = 0.0;
        }

        // create Pattern Response histogram
        for(int j=0; j<n_samples; ++j)
        {
            int k = indexes[j];
            // DataType v = src->GetResponse(k)[i];
            int bin = src->bin[i * src->n_samples + k];
            store[bin] += src->weights[k];
        }
        ///// todo: place in library END ///////

        // evaluate feature: th and direction
        std::pair<DataType, int> ret;

        double w = SimpleOptimize(ret, store, histo_bin_size, wp, wn);
        // ret contain the best solution (threshold and diretion). Direction is not so important due to nature of decision tree

        // evaluate threshold
        int left_size = 0;

        // SPLIT: precompute vector size in order to reserve memory
        for(int j=0; j<n_samples; ++j)
        {

            int k = indexes[j];
            // CORRECT SIDE
            if(src->bin[i * src->n_samples + k] < ret.first)
            {
                left_size++;
#ifdef verbose
                w_left += std::abs(src.weights[k]);
#endif
            }
        }

        if((left_size == 0) || (left_size == n_samples ))
        {
            // unusefull solution
        }
        else
        {

            ///// todo: place in library BEGIN ///////

            // BUILD PRECOMPUTE LEAVES
            std::vector<int> left_resp, right_resp;

            left_resp.reserve(left_size);
            right_resp.reserve(n_samples - left_size);

            // create leaves:
            for(int j=0; j<n_samples; ++j)
            {
                int k = indexes[j];
                if(src->bin[i * src->n_samples + k] < ret.first)
                {
                    left_resp.push_back(k);
                }
                else
                {
                    right_resp.push_back(k);
                }
            }

            ///// todo: place in library END ///////

            double w;

            IndirectDecisionTree<DataType> *left = new IndirectDecisionTree<DataType>();
            IndirectDecisionTree<DataType> *right = new IndirectDecisionTree<DataType>();

            w = BuildDecisionTreeHeuristic(*left,  *src, max_depth-1, left_resp, 1, histo_bin_size, optimize);
            w += BuildDecisionTreeHeuristic(*right, *src, max_depth-1, right_resp, 1, histo_bin_size, optimize);

            if(w > bestw)
            {
                delete root->left;
                delete root->right;
                root->left = left;
                root->right = right;

                //////// convert bin to threshold BEGIN ////////
                DataType minr, maxr;

                minr = src->response_range[i].first;
                maxr = src->response_range[i].second;

                DataType delta = (maxr > minr) ? (maxr - minr + 1) : (1);

                root->th = minr + ((ret.first * delta) / histo_bin_size); // ret.first;

                //////// convert bin to threshold END ////////

                root->classifier = i;
                bestw = w;
            }
            else
            {
                // free unused memory
                delete left;
                delete right;
            }

        }

    }

    delete [] store;

    *outw= bestw;
}

}

/// create an "Indirect" decision tree using Heuristic instead of Sorting
template<class DataType>
double BuildDecisionTreeHeuristic(IndirectDecisionTree<DataType> &root, const PrecomputedPatternResponse<DataType> & src, int max_depth, const std::vector<int> & indexes, int max_concurrent_jobs, int histo_bin_size, bool reoptimize)
{
    double bestw = 0.0;

    const int n_samples = indexes.size();

    // precompute Wn, Wp
    float wp = 0.0, wn = 0.0;
    for(int i=0; i<n_samples; ++i)
    {
        int k = indexes[i];
        if(src.category[k] == 1)
            wp += src.weights[k];
        else
            wn -= src.weights[k];
    }

#ifdef verbose
    std::cout << max_depth << "-- build decision tree with " << n_samples << " elements. wp:" << wp << ", wn:" << wn << ", wp+wn:" << wp+wn << std::endl;
#endif

    if(max_depth == 0)
    {
        if(wp>wn)
        {
#ifdef verbose
            std::cout << "\tmax depth reached: positive with " << wp << "\n";
#endif
            root.category = 1;
            return wp;
        }
        else
        {
#ifdef verbose
            std::cout << "\tmax depth reached: negative with " << wn << "\n";
#endif
            root.category = -1;
            return wn;
        }
    }

    // split using threads

    if(max_concurrent_jobs>1)
    {
        sprint::thread_group pool;

        IndirectDecisionTree<DataType> *candidates = new IndirectDecisionTree<DataType>[max_concurrent_jobs];
        double *outw = new double[max_concurrent_jobs];

        for(int i =0; i<max_concurrent_jobs; ++i)
        {
            int f0 = (src.n_features * i) / max_concurrent_jobs;
            int f1 = (src.n_features * (i+1) ) / max_concurrent_jobs;

            pool.create_thread(sprint::thread_bind(&detail::build_decision_tree_heuristic<DataType> , &outw[i], &candidates[i], &src, f0, f1, max_depth, &indexes[0], n_samples, histo_bin_size, wp, wn, reoptimize) );
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
    {
        detail::build_decision_tree_heuristic(&bestw, &root, &src, 0, src.n_features, max_depth, &indexes[0], n_samples, histo_bin_size, wp, wn, reoptimize);
    }


///////////////////////////////////

    if(bestw==0.0)
    {
#ifdef verbose
        std::cout << "no solution found:";
#endif
        if(wp>wn)
        {
#ifdef verbose
            std::cout << "\tpositive with w:" << wp <<"\n";
#endif
            root.category = 1;
            return wp;
        }
        else
        {
#ifdef verbose
            std::cout << "\tnegative with w:" << wn <<"\n";
#endif
            root.category = -1;
            return wn;
        }


    }

#ifdef verbose
    std::cout << "solution found with w:" << bestw << std::endl;
#endif

    return bestw;
}

// fw
template<class DataType>
double BuildDecisionTreeHeuristic_v2(IndirectDecisionTree<DataType> &root, const PrecomputedPatternResponse<DataType> & src, int max_depth, const std::vector<int> & indexes, int max_concurrent_jobs, int histo_bin_size, bool reoptimize);


namespace detail {

template<class DataType>
void build_decision_tree_heuristic_v2(double *outw, IndirectDecisionTree<DataType> * root, const PrecomputedPatternResponse<DataType> * src, int f0, int f1, int max_depth, const int * indexes, int n_samples, int histo_bin_size, float wp, float wn, bool reoptimize)
{
    double bestw = 0.0;
    std::pair<DataType, int> best_ret;
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
            int k = indexes[j];
            // DataType v = src->GetResponse(k)[i];
            int bin = src->bin[i * src->n_samples + k];
            store[bin] += src->weights[k];
        }
        ///// todo: place in library END ///////

        // evaluate feature: th and direction
        std::pair<DataType, int> ret;

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

    root->classifier = best_i;
    
    if(reoptimize)
    {
      // REOPTIMIZE 
      std::pair<DataType, float> * store = new std::pair<DataType, float> [n_samples];

    ///// todo: place in library BEGIN ///////
    
      // create Pattern Response
        for(int j=0; j< n_samples; ++j)
        {
            int k = indexes[j];
            store[j].first = src->response[best_i * src->n_samples + k];
            store[j].second =  src->weights[k]; /* (double) src.category[k] * src.weights[k]; */
        }

        // sort
        std::sort(&store[0], &store[n_samples]);

        ///// todo: place in library END ///////

        // evaluate feature: th and direction
        float w = Optimize(best_ret, store, n_samples, wp, wn, false);
        
        root->th = best_ret.first;
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
        
        root->th = minr + ((best_ret.first * delta) / histo_bin_size); // ret.first;
        //////// convert bin to threshold END ////////
    }

    
    // evaluate threshold
    int left_size = 0;
    
    // SPLIT: precompute vector size in order to reserve memory
    for(int j=0; j<n_samples; ++j)
    {

        int k = indexes[j];
        // CORRECT SIDE
        if(src->response[best_i * src->n_samples + k] <  root->th)
        {
            left_size++;
#ifdef verbose
            w_left += std::abs(src.weights[k]);
#endif
        }
    }

    if((left_size == 0) || (left_size == n_samples ))
    {
        // unusefull solution
        *outw = 0.0;
    }
    else
    {

        ///// todo: place in library BEGIN ///////

        // BUILD PRECOMPUTE LEAVES
        std::vector<int> left_resp, right_resp;

        left_resp.reserve(left_size);
        right_resp.reserve(n_samples - left_size);

        // create leaves:
        for(int j=0; j<n_samples; ++j)
        {
            int k = indexes[j];
            if(src->response[best_i * src->n_samples + k] <  root->th)
            {
                left_resp.push_back(k);
            }
            else
            {
                right_resp.push_back(k);
            }
        }

        ///// todo: place in library END ///////

        IndirectDecisionTree<DataType> *left = new IndirectDecisionTree<DataType>();
        IndirectDecisionTree<DataType> *right = new IndirectDecisionTree<DataType>();

        double w;
        w = BuildDecisionTreeHeuristic_v2(*left,  *src, max_depth-1, left_resp, 1, histo_bin_size, reoptimize);
        w += BuildDecisionTreeHeuristic_v2(*right, *src, max_depth-1, right_resp, 1, histo_bin_size, reoptimize);

        root->left = left;
        root->right = right;

        *outw= w;
    }

}

}


/// create an "Indirect" decision tree using Heuristic instead of Sorting
///  instead of evaluate any possible tree, extract for each node, the best response.
template<class DataType>
double BuildDecisionTreeHeuristic_v2(IndirectDecisionTree<DataType> &root, const PrecomputedPatternResponse<DataType> & src, int max_depth, const std::vector<int> & indexes, int max_concurrent_jobs, int histo_bin_size, bool reoptimize)
{
    double bestw = 0.0;

    const int n_samples = indexes.size();

    // precompute Wn, Wp
    float wp = 0.0, wn = 0.0;
    for(int i=0; i<n_samples; ++i)
    {
        int k = indexes[i];
        if(src.category[k] == 1)
            wp += src.weights[k];
        else
            wn -= src.weights[k];
    }

    if(max_depth == 0)
    {
        if(wp>wn)
        {
            root.category = 1;
            return wp;
        }
        else
        {
            root.category = -1;
            return wn;
        }
    }

    // split using threads

    if(max_concurrent_jobs>1)
    {
        sprint::thread_group pool;

        IndirectDecisionTree<DataType> *candidates = new IndirectDecisionTree<DataType>[max_concurrent_jobs];
        double *outw = new double[max_concurrent_jobs];

        for(int i =0; i<max_concurrent_jobs; ++i)
        {
            int f0 = (src.n_features * i) / max_concurrent_jobs;
            int f1 = (src.n_features * (i+1) ) / max_concurrent_jobs;

            pool.create_thread(sprint::thread_bind(&detail::build_decision_tree_heuristic_v2<DataType> , &outw[i], &candidates[i], &src, f0, f1, max_depth, &indexes[0], n_samples, histo_bin_size, wp, wn, reoptimize) );
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
    {
        detail::build_decision_tree_heuristic_v2(&bestw, &root, &src, 0, src.n_features, max_depth, &indexes[0], n_samples, histo_bin_size, wp, wn, reoptimize);
    }


///////////////////////////////////

    if(bestw==0.0)
    {
        if(wp>wn)
        {
            root.category = 1;
            return wp;
        }
        else
        {
            root.category = -1;
            return wn;
        }


    }

    return bestw;
}

#endif
