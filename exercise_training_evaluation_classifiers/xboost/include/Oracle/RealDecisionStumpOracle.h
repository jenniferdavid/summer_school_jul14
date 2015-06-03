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

#ifndef _REAL_DECISION_STUMP_ORACLE_H
#define _REAL_DECISION_STUMP_ORACLE_H

/** @file RealDecisionStumpOracle.h
  * @brief methods concerning obtain threshold and responses for a RealDecisionStump classifier
  **/

#ifdef _MULTITHREAD
# include "Thread/thread.h"
#endif

#include "Utility/timer.h"

#include "Classifier/RealDecisionStump.h"
#include "Classifier/BinaryClassifier.h"

#include "DataSet.h"
#include "FeatureGenerator/HaarFeatureGenerator.h"
#include "WeightedPatternResponse.h"


  
/** A base class for training a Real Decision Stump classifier using scalar features
  *  Find the threshold and the weight for a RealDecisionStump using different Metric(s) (for example RealAdaBoost or GentleAdaBoost metrics)
  * @param FeatureGenerator generates Feature of type FeatureType
  * @param Metric metric used to choose the best feature. @See RealAdaBoostMetric or GentleAdaBoostMetric.
  * @note FeatureType is the type of feature involved in training
  * @note PatternType must have a GetWeight() method implemented.
  **/
template<class FeatureGenerator, class Aggregator, class Metric>
struct RealDecisionStumpOracle: public Metric {
public:

    /// The Feature Extracted by FeatureGenerator
    typedef typename FeatureGenerator::FeatureType FeatureType;

    /// The weak classifier provided by this oracle
    typedef BinaryClassifier<FeatureType, RealDecisionStump<int> > ClassifierType;

    DECLARE_AGGREGATOR
    

private:
    /// Sum of weight of positive and negative on training set
    double m_dP, m_dN;

    /// Cache (Size() * m_preloadCount elements)
    PatternResponse *m_store;

    /// Preload Count
    int m_preloadCount;

#ifdef _MULTITHREAD
    /// MultiThread
    int m_threadCount;
#endif

    /// The Feature Generator Handle
    FeatureGenerator * m_featureGenerator;

    /** Training Set: pattern used to compute threshold */
    DataSetHandle<Aggregator> m_training_set;

private:
  
  void InitWeight();

    /// used internally for MultiThread processing
    /// evaluate @a h  with preload @a size from pattern @a i0 to @a i1
    /// @note This is the version with preload
    template<class R>
    void Internal_StreamEvaluate_1(const R * h, int size, unsigned int i0, unsigned int i1);
  
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

    /// Use @a h feature capabilities in order to initialize cache values: m_store
    template<class R>
    void StreamEvaluate_1(const R * h, int size);

    /// sort the m_store rows (in multithread processing)
    void Internal_Sort_Store(int i0, int i1)
    {
        for(int i=i0; i<i1; ++i)
        {
            sort_pattern(&m_store[i * m_training_set.Size()], m_training_set.Size());
            // std::sort(&m_store[i * Size()], &m_store[i * Size()+Size()]);
        }
    }    
    
    
    /// Optimize on store slot, the weak classifier H
    /// @return the minimized value s
    double Optimize(PatternResponse *store, ClassifierType & h)
    {
      int samples = m_training_set.Size();
      
      // ordino m_store in ordine crescente di value della feature
      // il 54.98% del tempo viene perso su questa riga
      std::sort(&store[0], &store[samples]);
      // Internal_Sort_Store(0, 1);

      // trovo la soglia ottima per questo classificatore, usando la matrica least squares
      // TRA GENTLE E REAL CAMBIA SOLO QUASTA RIGA
      return Metric::optimize(h, store, samples, m_dP, m_dN);      
    }
                    
  
  /// Ottimziza e calcola le risposte di tutte i weak learner
  void Internal_Optimize(double *out, ClassifierType *h, int i0, int i1)
  {
    for(int i = i0; i< i1; ++i)
    {
            out[i] = Optimize(&m_store[i * m_training_set.Size()], h[i]);
    }

  }
  
                      
public:
    RealDecisionStumpOracle() : m_store(0), m_preloadCount(0), 
#ifdef _MULTITHREAD
    m_threadCount(1), 
#endif
    m_featureGenerator(0) { }

    ~RealDecisionStumpOracle();

    /// Set the training set used train the classifier
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

    /// Return the best @a h for current data set.
    /// This is the real core of this module:
    /// test all FeatureType and produce the bestH classifier with the best of them
    bool GetHypothesis(ClassifierType & bestH);

    /// Using current metrics try to recompute parameters associated to this feature
    double Optimize(ClassifierType & h)
    {
      Evaluate_1(h);
      // optimize h using response store in 'm_store'
      return  Optimize(m_store, h);      
    }

  
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class FeatureType, class Aggregator, class Metric>
RealDecisionStumpOracle<FeatureType, Aggregator, Metric>::~RealDecisionStumpOracle()
{
    delete [] m_store;
}

// TODO: should be outside
template<class FeatureType, class Aggregator, class Metric>
void RealDecisionStumpOracle<FeatureType, Aggregator, Metric>::InitWeight()
{
    // TODO: Adaboost?
    double energy;

    // conto i pesi dei due schieramenti
    m_dP = m_dN = 0.0f;
    for(unsigned int i =0; i<m_training_set.Size(); i++)
        if(m_training_set.templates[i].category == 1)
            m_dP += m_training_set.templates[i].d;    // A
        else
            m_dN += m_training_set.templates[i].d;    // B
}


template<class FeatureType, class Aggregator, class Metric>
bool RealDecisionStumpOracle<FeatureType, Aggregator, Metric>::GetHypothesis(ClassifierType & bestH)
{
    double bestS = 1.0; 
    Timer t;
    int count = 0;

    if(m_featureGenerator == 0)
    {
        std::cerr << "No Feature Generator loaded. Use SetFeatureGenerator API before call GetHypothesis" << std::endl;
        return false;
    }

    bestH.debug_name( "internal error" );
    t.Start();

    if(m_training_set.Size() == 0)
    {
        std::cerr << "No pattern loaded. Init Trainer Failed" << std::endl;
        return false;
    }

    delete [] m_store;
    m_store = 0;

    std::cout << "Train with:" << m_training_set.n_patternP << "(+), " << m_training_set.n_patternN << "(-) using " << m_featureGenerator->Count() << " features.\n";

    InitWeight();

    m_featureGenerator->Reset();
    
    if(m_preloadCount<=1)
    {
    ClassifierType h;
     // **** without preload *****
     m_store = new PatternResponse [m_training_set.Size()];

    // PER OGNI FEATURE POSSIBILE
     while(m_featureGenerator->Next(h))
        {
            double s;

            // compute weight of feature h
            s = Optimize(h);

            count ++;

            if(s < bestS)
            {
                bestH = h;
                bestS = s;

                std::cout << count <<" (" << (100.0f * (double)count/(double)m_featureGenerator->Count()) << "%): name:" << bestH.debug_name() << " th:" << bestH.th << " p:" << bestH.pr << " n:" << bestH.nr << " Z:" << 2.0*bestS << std::endl;
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
    else
    {
#ifndef _MULTITHREAD
      static const int m_threadCount = 1;
#endif      
// **** with preload  ***
// analizzo m_preloadCount feature contemporaneamente
        int h_count = 0; // number of feature to evaluate in this step

        m_store = new PatternResponse [m_training_set.Size() * m_preloadCount];
        
// an array of hypothesis to be evaluated
        ClassifierType *h = new ClassifierType[m_preloadCount];
        double *responses = new double[m_preloadCount];

        std::cout << (sizeof(PatternResponse) * m_training_set.Size() * m_preloadCount + sizeof(ClassifierType) * m_preloadCount)/(1024*1204) << "Mb used for cache ("<<  m_preloadCount << " features evaluated on " << m_threadCount << " threads)" << std::endl;

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
// genera gli m_store: NOTE viene paralellizato per feature
            StreamEvaluate_1(h, h_count);
            
            // NOTE: Optimize can destroy m_store

#ifdef _MULTITHREAD
// parallelizzo per feature
            if(m_threadCount>1)
            {
                sprint::thread_group thread_pool_;
                for(int k=0; k<m_threadCount; ++k)
                {
                    unsigned int i0 = (h_count * k) / m_threadCount;
                    unsigned int i1 = (h_count * (k+1)) / m_threadCount;

                    thread_pool_.create_thread(sprint::thread_bind(&RealDecisionStumpOracle::Internal_Optimize, this, responses, h, i0, i1));
                }

                thread_pool_.join_all();
            }
            else
#endif
           Internal_Optimize(responses, h, 0, h_count);
           
           for(int i =0; i<h_count; ++i)
           {
              if(responses[i] < bestS)
              {
                bestS = responses[i];
                bestH = h[i];
                
                std::cout << count <<" (" << (100.0f * (double)count/(double)m_featureGenerator->Count()) << "%): name:" << bestH.debug_name() << " th:" << bestH.th << " p:" << bestH.pr << " n:" << bestH.nr << " s:" << bestS << std::endl;
              }

            count++;
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

        } while(h_count==m_preloadCount);

        delete [] h;
        delete [] responses;
    }
    
// no more
    return bestS<1.0;
}

template<class FeatureType, class Aggregator, class Metric>
template<class R>
void RealDecisionStumpOracle<FeatureType, Aggregator, Metric>::StreamEvaluate_1(const R * h, int size)
{
#ifdef _MULTITHREAD
    if(m_threadCount>1)
    {
        sprint::thread_group thread_pool_;
        for(int k=0; k<m_threadCount; ++k)
        {
            unsigned int i0 = (m_training_set.Size() * k) / m_threadCount;
            unsigned int i1 = (m_training_set.Size() * (k+1)) / m_threadCount;

            thread_pool_.create_thread(sprint::thread_bind(&RealDecisionStumpOracle::template Internal_StreamEvaluate_1<R>, this, h, size, i0, i1));
        }

        thread_pool_.join_all();
    }
    else
#endif
        Internal_StreamEvaluate_1(h, size, 0, m_training_set.Size());
}

template<class FeatureType, class Aggregator, class Metric>
template<class R>
void RealDecisionStumpOracle<FeatureType, Aggregator, Metric>::Internal_StreamEvaluate_1(const R * h, int size, unsigned int i0, unsigned int i1)
{
  ComputeFeaturesResponse(m_store, h, size, m_training_set, i0, i1);
}
#endif



