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

#ifndef _NAIVE_DECISION_STUMP_ORACLE_H
#define _NAIVE_DECISION_STUMP_ORACLE_H

/** @file NaiveDecisionStumpOracle.h
  * @brief methods concerning obtain best threshold for a NaiveDecisionStump classifier
  **/

// questa classe di oracoli (Naive e Bayes) ottimizza, usando pero' i pattern diretti, non eseguendo un sorting preventivo
//  c'e' il problema dell'inversione della polarita'/parity da gestire.

#include "Classifier/NaiveDecisionStump.h"

#ifdef _MULTITHREAD
/// data used in the inner thread
template<class T>
struct detail_thread_naive_decision_stump {
    double w; 	///< response (output)
    T h;	   	///< hypothesis (input)
};
#endif

/** The hypothesis generator for NaiveDecisionStump Weak Classifier */
template<class FeatureGenerator, class Aggregator >
class NaiveDecisionStumpOracle {

public:


    /// The Feature Extracted by FeatureGenerator
    typedef typename FeatureGenerator::FeatureType FeatureType;

    /// The weak classifier
    typedef BinaryClassifier<FeatureType, NaiveDecisionStump> WeakClassifierType;

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

private:
#ifdef _MULTITHREAD
    // since the threshold is fixed, this code can only evaluate performance
    void Thread_OptimizeClassifier(detail_thread_naive_decision_stump<WeakClassifierType> * ptr )
    {
        ptr->w = Optimize(ptr->h);
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

    /** MAdaBoost limits the weight of training set */
//   void LimitTrainingSetWeight(double d)
//   {
//     int n = 0;
//     for(unsigned int i =0;i<m_training_set.Size();i++)
//      if( m_training_set.templates[i].d > d)
//      {
//         m_training_set.templates[i].d = d;
// 	++n;
//      }
//
//     std::cout << n << " samples of " << m_training_set.Size() << " (" << (n*100)/m_training_set.Size() << "%) have been limited in weight (" << d <<")" << std::endl;
//   }
//
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
    
  void SetFastHeuristic(bool enable, bool reoptimize, int size) {
    // TODO
  }    

    /// Using current metrics try to recompute parameters associated to this feature
    /// since the threshold is fixed, this code can only evaluate performance (at least invert polarity of classifier)
    double Optimize(WeakClassifierType & h)
    {
        double pp,pn,nn,np;
        double pz,nz;
        double w = 0.0;
        pp = pn = nn = np = pz = nz = 0.0;

        // compute response to feature operator h
        for(int i =0; i<m_training_set.Size(); ++i)
        {
            int value = h.response( getData1( m_training_set.templates[i], m_training_set), getData2(m_training_set.templates[i], m_training_set) );
            double d =  m_training_set.templates[i].d;

            if( m_training_set.templates[i].category == 1)
            {
                if(value > 0)
                    pp += d;
                else if(value == 0)
                    pz += d;
                else
                    pn += d;
            }
            else
            {
                if(value > 0)
                    np += d;
                else if(value == 0)
                    nz += d;
                else
                    nn += d;

            }

        }

        // parity=+1 pn+=pz, nn+=nn
        double w1 = pp + nn + nz;
        // parity=-1 pp+=pz, np+=nz
        double w2 = pn + np + nz;

        if(w1 >= w2)
        {
            return w1;
        }
        else
        {
            h.invert_polarity();
            return w2;
        }
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

            // for any possible features:
#ifdef _MULTITHREAD
            detail_thread_naive_decision_stump<WeakClassifierType> * storage = new detail_thread_naive_decision_stump<WeakClassifierType>[m_threadCount];
            int token = 0;

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
                            thread_pool_.create_thread(sprint::thread_bind(&NaiveDecisionStumpOracle::Thread_OptimizeClassifier, this, &storage[i]));
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
                        thread_pool_.create_thread(sprint::thread_bind(&NaiveDecisionStumpOracle::Thread_OptimizeClassifier, this, &storage[i]));
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
            // TODO: unimplemented yet
//             WeakClassifierType h;
// 
//             while( m_featureGenerator->Next(h) )
//             {
//                 double w = OptimizeClassifier(h, response);
// 
//                 count ++;
// 
//                 if(w > bestW)
//                 {
//                     bestW = w;
//                     bestH = h;
//                     std::cout << count <<" (" << (100.0f * (double)count/(double)m_featureGenerator->Count()) << "%): name:" << bestH.debug_name() << " w+:"<< bestW << std::endl;
//                 }
// 
//                 if((count & (32*1024-1))==0)
//                 {
//                     float fs = count/t.GetTime();
//                     float pr = (float)count/(float)m_featureGenerator->Count();
//                     int rem = (m_featureGenerator->Count() - count)/fs;
//                     std::cout << fs << " feature/secs: " << (100.0f * pr)  << "% ";
//                     if(rem<120)
//                         std::cout << rem << "s remaining" << std::endl;
//                     else
//                         std::cout << rem/60 << "min remaining" << std::endl;
//                 }
// 
// 
//             }
#endif

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

