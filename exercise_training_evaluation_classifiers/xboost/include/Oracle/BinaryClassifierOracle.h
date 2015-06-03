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

#ifndef _BINARY_CLASSIFIER_ORACLE_H
#define _BINARY_CLASSIFIER_ORACLE_H

/** @file BinaryClassifierOracle.h
  * @brief methods concerning obtain best threshold (under some metrics) for a Binary Classifier Policy
  *        using a "sorted" response patterns.
  **/

#ifdef _MULTITHREAD
# include "Thread/thread.h"
# include "Thread/bind.h"
# include "Thread/thread_group.h"
#endif

#include "DataSet.h"
#include "Classifier/BoostableClassifier.h"
#include "Utility/timer.h"
#include "Classifier/BinaryClassifier.h"
#include "WeightedPatternResponse.h"

/** A base class for training a classifier using Integral Images with some kind of Features (HaarFeature).
  *  Find the best threshold of a scalar feature on the trainingset using associated weights, try to maximize AdaBoost W+ param or others Metric
  *
  *  Implements metric based on a list of sorted PatternResponse
  * @param FeatureGenerator generates Feature of type FeatureType
  * @note FeatureType is the type of feature involved in training
  * @note PatternType must have a GetWeight() method implemented.
  **/
template<class FeatureGenerator, class ClassifierPolicy, class Metric, class Aggregator>
struct BinaryClassifierOracle {
public:

    /// The Feature Extracted by FeatureGenerator
    typedef typename FeatureGenerator::FeatureType FeatureType;

    /// The weak classifier provided by this oracle
    typedef BinaryClassifier<FeatureType, ClassifierPolicy > WeakClassifierType;

    /// Additive Regression Final Classifier
    typedef BoostableClassifier< WeakClassifierType > ClassifierType;

    DECLARE_AGGREGATOR

private:
    /// Sum of weight of positive and negative on training set
    double m_dP, m_dN;

    /// Cache (Size() * m_preloadCount elements)
    PatternResponse *m_store;

    /// Preload Count
    int m_preloadCount;

    /// choose the fast mode heuristic to find the best feature
    bool m_useFastOptimize;

    /// number of bin in fast-optimization mode
    unsigned int m_fastOptimizeBinSize;

#ifdef _MULTITHREAD
    /// MultiThread
    int m_threadCount;
#endif

    /// The Feature Generator Handle
    FeatureGenerator * m_featureGenerator;

    /** Training Set: pattern used to compute threshold */
    DataSetHandle<Aggregator> m_training_set;

private:

    /// used internally for MultiThread processing
    /// evaluate @a h  with preload @a size from pattern @a i0 to @a i1
    /// @note This is the version with preload
    template<class R>
    void Internal_StreamEvaluate_1(const R * h, int size, unsigned int i0, unsigned int i1);

    /// used internally for MultiThread processing
    /// evaluate @a h  with preload @a size from feature @a i0 to @a i1
    /// @note This is the version with preload
    template<class R>
    void Internal_StreamEvaluateAndBucket_1(const R * h, int size, unsigned int i0, unsigned int i1);

    /// sort the m_store rows (in multithread processing)
    void Internal_Sort_Store(int i0, int i1)
    {
        for(int i=i0; i<i1; ++i)
        {
            sort_pattern(&m_store[i * m_training_set.Size()], m_training_set.Size());
            // std::sort(&m_store[i * Size()], &m_store[i * Size()+Size()]);
        }
    }

    /// Optimize feature range from i0 to i1 (m_store)
    /// @param i0 starting feature index
    /// @param i1 last feature index (not included)
    /// @param bestH output weak classifier
    /// @param bestW [in/out] vote of best classifier
    /// @note m_store have to be initialized with feature response
    void Internal_Optimize(int i0, int i1, WeakClassifierType *bestH, double *bestW, WeakClassifierType *h)
    {
        unsigned int n = m_training_set.Size();
        int n_feat = m_featureGenerator->Count();
        PatternResponse * work = new PatternResponse[m_fastOptimizeBinSize]; // histogram generated (used only if m_useFastOptimize is enabled)

        for(int i=i0; i<i1; ++i)
        {
            double curW;

            if(m_useFastOptimize)
            {
                // compute the bucket for feature h[i], with response precomputed in m_store
                GenerateBucket(work, m_fastOptimizeBinSize, &m_store[i * n], n);
                // chiama l'ottimizzazione su m_store
                curW = Metric::optimize(h[i], work, m_fastOptimizeBinSize, m_dP, m_dN);
            }
            else
            {
// ordino m_store in ordine crescente di value della feature
// il 54.98% del tempo viene perso su questa riga

                sort_pattern(&m_store[i * n], n);

// compute the best threshold for classifier H on sorted feature response
                curW = Metric::optimize(h[i], &m_store[i * n], n, m_dP, m_dN );
            }

            if(curW > *bestW)
            {
                *bestH = h[i];
                *bestW = curW;
            }
        }

        delete [] work;
    }

    /*
    template<class FeatureExtractor>
    void Evaluate_1(PatternResponse *store, const FeatureExtractor & h, const PatternType* templates, int n) const
    {
        for(unsigned int i =0; i<n; i++)
        {
            // feature value: nota la memoria punta al pixel (-1,-1)
            store[i].value = h.response( getData1( templates[i].data+m_training_set.width+1+1, m_training_set.width+1);
            // variation of weight d associated
            // d, peso associato al pattern, value >0 se e' uno di tipo A, altri < 0
            // note: non gestisce il caso nativo di astensione
            store[i].d = templates[i].category * templates[i].d;
        }
    }
    */
    /*
    template<class FeatureExtractor>
    void Default_Evaluate_1(PatternResponse *store, const FeatureExtractor & h, const PatternType* templates, int n) const
    {
        for(unsigned int i =0; i<n; i++)
        {
            store[i].value = h.response(m_training_set.templates[i].data+m_training_set.width+1+1, m_training_set.width+1);
            store[i].d = templates[i].category;
        }
    }
    */
    /** stage1: compute feature response to any pattern and initialize value.
     *  FeatureExtractor must have 'response' method
     * Use @a h feature capabilities in order to initialize cache values: m_store
     * @note this is the version without preload
     **/
    template<class FeatureExtractor>
    void Evaluate_1(const FeatureExtractor & h)
    {
        ExtractFeature(m_store, h, m_training_set);
    }

//     /// riempie m_store con l'istogramma
//     template<class FeatureExtractor>
//     void EvaluateAndBucket_1(const FeatureExtractor & h)
//     {
//         int *work = new int [ m_training_set.Size() ];
//         ExtractFeatureAndBucket(m_store, h, m_training_set, m_fastOptimizeBinSize, work);
//         delete [] work;
//     }

    /// Use @a h feature capabilities in order to initialize cache values: m_store
    /// @note parallellizza sul traning set
    template<class R>
    void StreamEvaluate_1(const R * h, int size);

    /// Use @a h feature capabilities in order to initialize cache values: m_store
    /// @note parallellizza sul traning set
    template<class R>
    void StreamEvaluateAndBucket_1(const R * h, int size);

    /// (pre)compute m_dP, m_dN (sum of weight of Positive and Negative sets) reading them from training set
    void InitWeight();


public:
    BinaryClassifierOracle() : m_store(0), m_preloadCount(0), 
#ifdef _MULTITHREAD
    m_threadCount(1), 
#endif
    m_featureGenerator(0), m_useFastOptimize(false), m_fastOptimizeBinSize(1) { }

    ~BinaryClassifierOracle();

    /// Set the training set used to recover the threshold
    template<class SourceDataSet>
    void SetTrainingSet(const SourceDataSet & set)
    {
        m_training_set = set;
    }

    /// Associate a Feature Generator to Decision Stump Generator
    void SetFeatureGenerator(FeatureGenerator & f)
    {
        m_featureGenerator = &f;
    }

    /// return R/W the training set
    DataSetHandle<Aggregator> & GetTrainingSet()
    {
        return m_training_set;
    }

#ifdef _MULTITHREAD
    /// Change the thread number used in Multi Threading training
    void SetNumberOfThreads(int th) {
        m_threadCount = th;
    }
#endif

    /** Set the preload size */
    void SetPreloadSize(int n) {
        m_preloadCount = n;
    }

    /// Enable/Disable the Fast-optimize heuristic
    /// @param enable enable/disable
    /// @param size   number of bins
    void SetFastHeuristic(bool enable, bool reoptimize, int size) {
        if(size<1)
        {
            std::cerr << "SetFastHeuristic called with an invalid parameter: size=" << size << std::endl;
            size = 1;
        }

        m_useFastOptimize = enable;
        m_fastOptimizeBinSize = size;
    }

    /// Return the best @a h for current data set.
    /// This is the real core of this module:
    /// test all FeatureType and produce the bestH classifier with the best of them
    bool GetHypothesis(WeakClassifierType & bestH);

    /// versione veloce per calcolare la miglior response che fa uso di
    ///  un istogramma/bin/bucker per evitare la sort
    /// TODO serve, per WeakClassifierType, il valore minimo e massimo della response
    double Fast_Optimize(WeakClassifierType & h)
    {
        // salva la response nei bucket
        int *work = new int [ m_training_set.Size() ];
        ExtractFeatureAndBucket(m_store, h, m_training_set, m_fastOptimizeBinSize, work);
        delete [] work;

        // chiama l'ottimizzazione
        return Metric::optimize(h, m_store, m_fastOptimizeBinSize, m_dP, m_dN);
    }

    /// Using current metrics try to recompute parameters associated to this feature
    double Optimize(WeakClassifierType & h)
    {
        // calcolo l'output di tutte i pattern usando h in m_store
        // il 30.71% viene perso qua
        Evaluate_1(h);

        // ordino m_store in ordine crescente di value della feature
        // il 54.98% del tempo viene perso su questa riga
        // std::sort(&m_store[0], &m_store[Size()]);
        Internal_Sort_Store(0, 1);
        //BubbleSort(&m_store[0], Size());

        // trovo la soglia ottima per questo classificatore
        return Metric::optimize(h, m_store, m_training_set.Size(), m_dP, m_dN);
    }

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::~BinaryClassifierOracle()
{
    delete [] m_store;
}

// TODO: should be outside
template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
void BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::InitWeight()
{
    // TODO: Adaboost?
    double energy;

    // conto i pesi dei due schieramenti
    m_dP = m_dN = 0.0f;
    for(unsigned int i =0; i<m_training_set.Size(); i++)
        if(m_training_set.templates[i].category == 1)
            m_dP += m_training_set.templates[i].d;    // positive set
        else
            m_dN += m_training_set.templates[i].d;    // negative set

    energy = m_dP + m_dN;
    double f = 1.0 / energy;
    m_dP *= f;
    m_dN *= f;
    for(unsigned int i =0; i<m_training_set.Size(); i++)
        m_training_set.templates[i].d *= f;
    std::cout << "* W+:" << m_dP  << " W-:" << m_dN << " (reweighted from " << energy << ")" << std::endl;
}

template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
template<class R>
void BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::Internal_StreamEvaluate_1(const R * h, int size, unsigned int i0, unsigned int i1)
{
  // Cio
    ComputeFeaturesResponse(m_store, h, size, m_training_set, i0, i1);
}

// template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
// template<class R>
// void BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::Internal_StreamEvaluateAndBucket_1(const R * h, int size, unsigned int i0, unsigned int i1)
// {
//     // setup store[i] memory
//     for(int i = m_fastOptimizeBinSize * i0;i<m_fastOptimizeBinSize * j0; ++i)
//     {
//       m_store[i].value = min_resp + bin_size * i;
//       m_store[i].d = 0.0;
//     }
//
//     for(int i=i0; i<i1; ++i)
//     {
//         ExtractFeatureAndBucket(&m_store[i * m_fastOptimizeBinSize], h[i], m_training_set, m_fastOptimizeBinSize, heuristicSupportVector);
//     }
// }


template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
template<class R>
void BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::StreamEvaluate_1(const R * h, int size)
{
#ifdef _MULTITHREAD
    if(m_threadCount>1)
    {
        sprint::thread_group thread_pool_;
        for(int k=0; k<m_threadCount; ++k)
        {
            unsigned int i0 = (m_training_set.Size() * k) / m_threadCount;
            unsigned int i1 = (m_training_set.Size() * (k+1)) / m_threadCount;

            thread_pool_.create_thread(sprint::thread_bind(&BinaryClassifierOracle::template Internal_StreamEvaluate_1<R>, this, h, size, i0, i1));
        }

        thread_pool_.join_all();
    }
    else
#endif
        Internal_StreamEvaluate_1(h, size, 0, m_training_set.Size());
}


// template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
// template<class R>
// void BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::StreamEvaluateAndBucket_1(const R * h, int size)
// {
//     int n = m_training_set.Size();
//
// #ifdef _MULTITHREAD
//     if(m_threadCount>1)
//     {
//         sprint::thread_group thread_pool_;
//         for(int k=0; k<m_threadCount; ++k)
//         {
//             unsigned int i0 = (n * k) / m_threadCount;
//             unsigned int i1 = (n * (k+1)) / m_threadCount;
//
//             thread_pool_.create_thread(sprint::thread_bind(&BinaryClassifierOracle::template Internal_StreamEvaluateAndBucket_1<R>, this, h, size, i0, i1));
//         }
//
//         thread_pool_.join_all();
//     }
//     else
// #endif
//         Internal_StreamEvaluateAndBucket_1(h, size, 0, n);
// }

template<class FeatureType, class ClassifierPolicy, class Metric, class Aggregator>
bool BinaryClassifierOracle<FeatureType, ClassifierPolicy, Metric, Aggregator>::GetHypothesis(WeakClassifierType & bestH)
{
    double bestW = 0.0;	// NOTA: sotto a 0.5 vuol dire che non puo' migliorare
    double maxW;
    int count = 0;
    int n = m_training_set.Size();
    // Internal training timer
    Timer t;
    double last_check;


    if(m_featureGenerator == 0)
    {
        std::cerr << "No Feature Generator loaded. Use SetFeatureGenerator API before call GetHypothesis" << std::endl;
        return false;
    }

    bestH.debug_name( "internal error" );

    if(n == 0)
    {
        std::cerr << "No pattern loaded. Init Trainer Failed" << std::endl;
        return false;
    }

    // release memory allocated in previous step (vabbe)
    delete [] m_store;
    m_store = 0;

    std::cout << "Train with:" << m_training_set.n_patternP << "(+), " << m_training_set.n_patternN << "(-) using " << m_featureGenerator->Count() << " features.\n";

    InitWeight();

    maxW = m_dP + m_dN; // TODO ASSERT != 1

    m_featureGenerator->Reset();

    int n_feat = m_featureGenerator->Count();

    t.Start();

    last_check = t.GetTime();

    ////////////////////////////////////////////// no preload ///////////////////////////////////////////////////////////
    if(m_preloadCount<=1)
    {
        WeakClassifierType h;

        // **** without preload *****

        // m_store deve poter memorizzare o m_training_set o i bin
        // TODO: allocare anche la memoria per salvare le response della Fast_Optimize
        m_store = new PatternResponse [std::max<int>(n, m_fastOptimizeBinSize) ];
        
        // PER OGNI FEATURE POSSIBILE
        while(m_featureGenerator->Next(h))
        {
            double curW;

            if(m_useFastOptimize)
                curW = Fast_Optimize(h);
            else
                curW = Optimize(h);

            count ++;

            if(curW > bestW)
            {
                bestH = h;
                bestW = curW;

                std::cout << count <<" (" << (100.0f * (double)count/(double)n_feat) << "%): ";
                if(bestH.debug_name()) std::cout << "name:" << bestH.debug_name() << " ";
                std::cout << "parity:" << bestH.parity << " th:" << bestH.th << " w+:"<< bestW << ", w-:" << maxW-bestW << std::endl;
            }

            // some stats
            if ((t.GetTime() - last_check) > 10.0)
            {
                float fs = count/t.GetTime();
                float pr = (float)count/(float)n_feat;
                int rem = (n_feat - count)/fs;
                std::cout << fs << " feature/secs: " << (100.0f * pr)  << "% ";
                if(rem<120)
                    std::cout << rem << "s remaining" << std::endl;
                else
                    std::cout << rem/60 << "min remaining" << std::endl;

                last_check = t.GetTime();
            }

        }


    }
    ///////////////////////////////////////// preload and/or multithread ///////////////////////////////////////////
    else
    {
#ifndef _MULTITHREAD
        static const int m_threadCount = 1;
#endif
        // some vars usend in multithread
        double *t_bestW = new double[m_threadCount];
        WeakClassifierType *t_bestH = new WeakClassifierType[m_threadCount];
        double prev_w = 0.0;

// **** with preload  ***
// analizzo m_preloadCount feature contemporaneamente
        int h_count = 0; // number of feature to evaluate in this step

        // in modalita' multithread vengono processari i pattern in parallelo. Le response vengono calcolate in maniera multithread e salvate in m_store
        m_store = new PatternResponse [n * m_preloadCount];
        std::cout << (sizeof(PatternResponse) * n * m_preloadCount + sizeof(WeakClassifierType) * m_preloadCount)/(1024*1204) << "Mb used for cache" << std::endl;
        
        // an array of hypothesis to be evaluated (in multithread)
        WeakClassifierType *h = new WeakClassifierType[m_preloadCount];

// analizzo a blocchi di [m_preloadCount] alla volta
        do {
            /* questa parte sviene parallelizzata dividendo gli h in sottoparti su piu' thread **/
            h_count =0;

// inizializzo [m_preloadCount] classifier (per via della non perfetta corrispondenza, potrebbero essere anche meno)
// carico m_preloadCount feature in memoria
            for(int i =0; i<m_preloadCount; i++)
                if(m_featureGenerator->Next(h[h_count]) )
                    h_count++;
                else
                    break;

// Calcolo gli output di un certo numero di feature in parallelo
// genera gli m_store: NOTE per massimizzare l'uso della cache, viene internamente paralellizato per pattern del training set
            StreamEvaluate_1(h, h_count);

#ifdef _MULTITHREAD
// NOTE: viene paralellizato per feature
            if(m_threadCount>1)
            {
                sprint::thread_group thread_pool_;
                for(int k=0; k<m_threadCount; ++k)
                {
                    unsigned int i0 = (h_count * k) / m_threadCount;
                    unsigned int i1 = (h_count * (k+1)) / m_threadCount;

                    t_bestW[k] = bestW;

                    thread_pool_.create_thread(sprint::thread_bind(&BinaryClassifierOracle::Internal_Optimize, this, i0, i1, &t_bestH[k], &t_bestW[k], h));
                }

                thread_pool_.join_all();

                // search for the best-best classifier
                for(int k=0; k<m_threadCount; ++k)
                {
                    if(t_bestW[k] > bestW)
                    {
                        bestW = t_bestW[k];
                        bestH = t_bestH[k];
                    }
                }
            }
            else
#endif
            {

                Internal_Optimize(0, h_count, &bestH, &bestW, h);

            }

            count += h_count;

            // check if the feature is changed and print some infos
            if(bestW > prev_w)
            {
                float pr = (float)count/(float)n_feat;
                std::cout << " (" << 100.0f * pr << "%) ";
                if(bestH.debug_name()) std::cout << "name:" << bestH.debug_name() << " ";
                std::cout << "parity:" << bestH.parity << " th:" << bestH.th << " w+:"<< bestW << " w-:" << maxW - bestW << std::endl;
                prev_w = bestW;
            }

            // some stats
            if ((t.GetTime() - last_check) > 10.0)
            {
                float fs = count/t.GetTime();
                float pr = (float)count/(float)n_feat;
                int rem = (n_feat - count)/fs;
                std::cout << fs << " feature/secs: " << (100.0f * pr)  << "% ";
                if(rem<120)
                    std::cout << rem << "s remaining" << std::endl;
                else
                    std::cout << rem/60 << "min remaining" << std::endl;

                last_check = t.GetTime();
            }

        } while(h_count==m_preloadCount);

        // free memory for heuristic and threads

        delete [] h;

        delete [] t_bestW;
        delete [] t_bestH;

    }


    if(m_useFastOptimize)
    {
        std::cout << "[reoptimize] [before] parity:" << bestH.parity << " th:" << bestH.th << " w+:"<< bestW << ", w-:" << maxW-bestW << std::endl;

        // reoptimize
        bestW = Optimize(bestH);

        std::cout << "[reoptimize] [after] parity:" << bestH.parity << " th:" << bestH.th << " w+:"<< bestW << ", w-:" << maxW-bestW << std::endl;
    }

    if(bestW<maxW*0.5)
        std::cout << "best classifier: " << bestW << "/" << maxW <<": exit" << std::endl;

// no more
    return bestW>=maxW*0.5;
}


#endif
