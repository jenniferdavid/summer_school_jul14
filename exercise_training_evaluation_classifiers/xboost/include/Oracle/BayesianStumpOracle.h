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

#ifndef _BAYESIAN_STUMP_ORACLE_H
#define _BAYESIAN_STUMP_ORACLE_H

/** @file BayesianStumpOracle.h
  * @brief methods concerning obtain best threshold for a BayesianStump classifier
  **/

#include "Classifier/BayesianStump.h"

#ifdef _MULTITHREAD
template<class T>
struct detail_thread_bayesian_stump {
    double w; 	// response
    T h;	   	// hypothesis
    int *mem;	// response memory buffer
};
#endif

/** The hypothesis generator for BayesianStump Weak Classifier */
template<class FeatureGenerator, class Aggregator>
class BayesianStumpOracle {

public:


    /// The Feature Extracted by FeatureGenerator
    typedef typename FeatureGenerator::FeatureType FeatureType;

    /// The interal weak classifier
    typedef BinaryClassifier<FeatureType, BayesianStump<int> > WeakClassifierType;

    /// The weak classifier reported by this oracle
    typedef BoostableClassifier< WeakClassifierType > ClassifierType;

    DECLARE_AGGREGATOR

private:

    /// number of thread
    int m_threadCount;

    /// preload count
    int m_preloadCount;

    /// The Feature Generator Handle
    FeatureGenerator * m_featureGenerator;

    /** Training Set: pattern used to compute threshold */
    DataSetHandle<Aggregator> m_training_set;

    /// features response
    int *response;

private:

//   /// return correct detection W
//   /// @note generalizzare
//   double evaluate(const WeakClassifierType & h) const {
//     double w = 0.0;
// //      std::cout << "\nevaluate:";
//     for(int i =0;i<m_training_set.Size();++i)
//     {
//       int n = h.classify(m_store[i].a, m_store[i].b);
// // 	std::cout << i << ',' << m_store[i].a <<';' << m_store[i].b << ',' << m_training_set.templates[i].d << ',' << m_training_set.templates[i].category << ';' << n << ' ';
//       if( n == m_training_set.templates[i].category)
//       {
// // 	std::cout << i << ' ';
// 	w+=m_training_set.templates[i].d;
//       }
//     }
//
//     return w;
//   };
//
    /** optimize classifier */
    double OptimizeClassifier(WeakClassifierType & h_best, int *mem)
    {
//     static const char *class_name[]={"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32"};
        int f_min, f_max;
        double p[32], n[32];

        // compute response to feature operator h, min and max
        for(int i =0; i<m_training_set.Size(); ++i)
        {
            mem[i] = h_best.response( getData1( m_training_set.templates[i], m_training_set), getData2(m_training_set.templates[i], m_training_set) );
// 	std::cout << response[i] << ' ';
            if(i==0) {
                f_min = f_max = mem[0];
            }
            else
            {
                if(mem[i] < f_min)
                    f_min = mem[i];
                if(mem[i] > f_max)
                    f_max = mem[i];
            }
        }

        for(int i =0; i<32; ++i) p[i]=n[i]=0.0;

        h_best.m_featRange = f_max - f_min + 1;
        h_best.m_minFeatValue = f_min;
        h_best.m_truthTable = 0;

        // create histogram
        for(int i =0; i<m_training_set.Size(); ++i)
        {
            int bin = (h_best.bin_size*(mem[i] - h_best.m_minFeatValue)) / h_best.m_featRange;
//      nt bin = (response[i] - h.m_minFeatValue) / h.m_featRange;

            if(bin<0 || bin > h_best.bin_size)
            {
                std::cerr << "invalid feature: " << bin << "/" << h_best.bin_size << " | min = " << f_min << " | max = " << f_max << " " << h_best.debug_name() << std::endl;
                return 0.0;
            }

            if( m_training_set.templates[i].category == 1)
                p[bin] += m_training_set.templates[i].d;
            else
                n[bin] += m_training_set.templates[i].d;
        }

        // compute w and thruth-table
        double w = 0.0;


        for(int i =0; i<h_best.bin_size; ++i) {
            if(p[i]>n[i])
            {
                h_best.m_truthTable |= 1<<i;
                w+= p[i];
            }
            else
            {
                w+= n[i];
            }
        }

        return w;
    }
#ifdef _MULTITHREAD
    void Thread_OptimizeClassifier(detail_thread_bayesian_stump<WeakClassifierType> * ptr )
    {
        ptr->w = OptimizeClassifier(ptr->h, ptr->mem);
    }
#endif
public:

    /// Set the training set used to recover the threshold
    void SetTrainingSet(const DataSetHandle<Aggregator> & set)
    {
        m_training_set = set;
    }

    /// Associate a Feature Generator to Decision Stump Generator
    void SetFeatureGenerator(FeatureGenerator & f)
    {
        m_featureGenerator = &f;
    }

    void SetFastHeuristic(bool enable, bool reoptimize, int size) {
        // TODO
    }

    /** MAdaBoost limits the weight of training set */
    void LimitTrainingSetWeight(double d)
    {
        int n = 0;
        for(unsigned int i =0; i<m_training_set.Size(); i++)
            if( m_training_set.templates[i].d > d)
            {
                m_training_set.templates[i].d = d;
                ++n;
            }

        std::cout << n << " samples of " << m_training_set.Size() << " (" << (n*100)/m_training_set.Size() << "%) have been limited in weight (" << d <<")" << std::endl;
    }

    /// return R/W the training set
    DataSetHandle<Aggregator> & GetTrainingSet()
    {
        return m_training_set;
    }

    /// Change the thread number used in Multi Threading training
    void SetNumberOfThreads(int th)
    {
#ifdef _MULTITHREAD
        m_threadCount = th;
#endif
    }

    void SetPreloadSize(int n) {
        m_preloadCount = n;
    }

    /// Using current metrics try to recompute parameters associated to this feature
    double Optimize(ClassifierType & h)
    {
        double w;
        response = new int [m_training_set.Size() ];
        w =  OptimizeClassifier(h, response);

        delete [] response;
        response = 0;
        return w;
    }

    bool GetHypothesis(WeakClassifierType & bestH)
    {

        Timer t;
        double bestW = 0.0;	// NOTA: sotto a 0.5 vuol dire che non puo' migliorare
        int count = 0;

        if(m_featureGenerator == 0)
        {
            std::cerr << "No Feature Generator loaded. Use SetFeatureGenerator API before call GetHypothesis" << std::endl;
            return false;
        }

        bestH.debug_name("internal error");
        t.Start();

        if(m_training_set.Size() == 0)
        {
            std::cerr << "No pattern loaded. Init Trainer Failed" << std::endl;
            return false;
        }

        if(m_threadCount<=1) m_threadCount = 1;

        // if(m_preloadCount<=1)
        if(1)
        {
            m_featureGenerator->Reset();

            response = new int [m_training_set.Size() * m_threadCount ];

            // PER OGNI FEATURE POSSIBILE
#ifdef _MULTITHREAD
            detail_thread_bayesian_stump<WeakClassifierType> * storage = new detail_thread_bayesian_stump<WeakClassifierType>[m_threadCount];
            int token = 0;
            for(int i =0; i<m_threadCount; ++i)
                storage[i].mem = response + i * m_training_set.Size();

            while( m_featureGenerator->Next( storage[token].h ) )
            {
                token++;
                if(token == m_threadCount)
                {
                    token = 0;

                    if(m_threadCount>1)
                    {
                        sprint::thread_group thread_pool_;
                        for(int i =0; i<m_threadCount; ++i)
                            thread_pool_.create_thread(sprint::thread_bind(&BayesianStumpOracle::Thread_OptimizeClassifier, this, &storage[i]));
                        thread_pool_.join_all();
                    }
                    else
                    {
                        Thread_OptimizeClassifier(&storage[0]);
                    }

                    for(int i =0; i<m_threadCount; ++i)
                    {
                        count ++;
                        double w = storage[i].w;
                        if(w > bestW)
                        {
                            bestW = w;
                            bestH = storage[i].h;
                            std::cout << count <<" (" << (100.0f * (double)count/(double)m_featureGenerator->Count()) << "%): name:" << bestH.debug_name() << " w+:"<< bestW << std::endl;
                        }

                        if((count & (32*1024-1))==0)
                        {
                            float fs = count/t.GetTime();
                            float pr = (float)count/(float)m_featureGenerator->Count();
                            int rem = (m_featureGenerator->Count() - count)/fs;
                            std::cout << fs << " feature/secs: " << (100.0f * pr)  << "% ";
                            if(rem<120)
                                std::cout << rem << "s remaining" << std::endl;
                            else
                                std::cout << rem/60 << "min remaining" << std::endl;
                        }
                    }

                }
            }

            if(token>0)
            {
                if(token>1)
                {
                    sprint::thread_group thread_pool_;
                    for(int i =0; i<token; ++i)
                        thread_pool_.create_thread(sprint::thread_bind(&BayesianStumpOracle::Thread_OptimizeClassifier, this, &storage[i]));
                    thread_pool_.join_all();
                }
                else
                {
                    Thread_OptimizeClassifier(&storage[0]);
                }

                for(int i =0; i<token; ++i)
                {
                    count ++;
                    double w = storage[i].w;
                    if(w > bestW)
                    {
                        bestW = w;
                        bestH = storage[i].h;
                        std::cout << count <<" (" << (100.0f * (double)count/(double)m_featureGenerator->Count()) << "%): name:" << bestH.debug_name() << " w+:"<< bestW << std::endl;
                    }

                }
            }

            delete [] storage;

#else
            WeakClassifierType h;

            while( m_featureGenerator->Next(h) )
            {
                double w = OptimizeClassifier(h, response);

                count ++;

                if(w > bestW)
                {
                    bestW = w;
                    bestH = h;
                    std::cout << count <<" (" << (100.0f * (double)count/(double)m_featureGenerator->Count()) << "%): name:" << bestH.debug_name() << " w+:"<< bestW << std::endl;
                }

                if((count & (32*1024-1))==0)
                {
                    float fs = count/t.GetTime();
                    float pr = (float)count/(float)m_featureGenerator->Count();
                    int rem = (m_featureGenerator->Count() - count)/fs;
                    std::cout << fs << " feature/secs: " << (100.0f * pr)  << "% ";
                    if(rem<120)
                        std::cout << rem << "s remaining" << std::endl;
                    else
                        std::cout << rem/60 << "min remaining" << std::endl;
                }


            }
#endif

            delete [] response;
            response = 0;

            std::cout << "Expected W:" << bestW<< std::endl;
            return true;
        }
        else
        {
            // WITH PRELOAD



        }

    }
};


#endif

