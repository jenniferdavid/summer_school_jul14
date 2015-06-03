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

#ifndef _ADABOOST_TRAINER_H
#define _ADABOOST_TRAINER_H

/** @file AdaBoostTrainer.h
  * @brief AdaBoostTrainer: implements an Adaboost (Discrete,Gentle,Real) algorithm on some weak classifier
  *        Provides some usefull tool to train a classifier using AdaBoost algorithm
  **/

#include "DataSet.h"
#include "Test.h"
#include "AdaBoost.h"
#include "Classifier/BoostClassifier.h"
#include "Classifier/BoostableClassifier.h"
#include "Pattern/Pattern.h"
#include "AdaBoostCommon.h"

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

/// Implements an AdaBoost Discrete/Gentle/Real Train algorithm using a generic Oracle (the hypothesis generator)
///  The Classifier will be BoostClassifier on a WeakClassifier type.
/// It implements also MAdaBoost weight update rule.
/// @param Oracle is an hypotheis generator policy
/// @note true class A positive +1, false class B negative -1
template<class Oracle>
class AdaBoostTrainer: public AdaBoost< typename Oracle::ClassifierType >, public Oracle {
public:

    typedef PatternAggregator<WeightedPattern< typename Oracle::DataType >, typename Oracle::ParamType>  AdaBoostPattern;
    
    /// DataSet required by AdaBoostTrainer
    typedef DataSetHandle<AdaBoostPattern> DataSetType;
    

    /// Weak Classifier Type, reported by Oracle
    typedef typename Oracle::ClassifierType WeakClassifierType;

    /// The Final Strong Classifier as Additive Model
    typedef BoostClassifier<WeakClassifierType> ClassifierType;

    /// The internal list, inside BoostClassifier
    typedef typename ClassifierType::ClassifierListType ClassifierListType;

private:

    /// Final Classifier
    ClassifierType m_classifier;

    /** Validation Set: pattern used to update AdaBoost weights but not find the best feature from oracle
     * @note serve solo per MAdaBoost al momento
     */
    DataSetType  m_validation_set;

    /// uses MAdaBoost variant?
    bool m_useMAdaBoost;

public:
    AdaBoostTrainer() : m_useMAdaBoost(false) { }
    ~AdaBoostTrainer() { }

/// Set MAdaBoost Variant
    void EnableMAdaBoost(bool mada)
    {
        m_useMAdaBoost = mada;
    }

    /// the internal signature of classifier (not the trainer)
    static std::string signature() {
        return ClassifierType::signature();
    }

    template<class SourceDataSetType>
    void SetValidationSet(const SourceDataSetType & set)
    {
        std::cout << "Update with: " << set.n_patternP << "(+), " << set.n_patternN << "(-)\n";

        m_validation_set = set;
        //std::cin.get();
    }

    /// return the validation Set
    const DataSetType & GetValidationSet() const {
        return m_validation_set;
    }


    /// Reset weight of validation set
    /// @param positive_weight set the weight of positive set
    void ResetValidationSet(double positive_weight = -1.0) // se e' random conviene iniziare con -1.0
    {
        ResetWeight(m_validation_set, positive_weight); 
    }

    /// Reset weights and alphas and retrain the classifier
    /// @param reOptimize change classifier params in order to deal with better response
    void Restart(bool reOptimize);

    /// Direct Access to Final Classifier
    ClassifierType &Classifier() {
        return m_classifier;
    }

    /// Direct Access to Final Classifier
    const ClassifierType &Classifier() const {
        return m_classifier;
    }

    /** Performs a single training cycle
      *  Extract the best classifier from Oracle using Training Set and update the Validation Set
      * @note not update the Oracle Training Set: it has been done manually
      * */
    bool Train();

    /** Test current network (all classifiers) on Validation Set
      *  prints the result to output console */
    bool Test();
    
    /** Execute the test and report stats */
    ReportTest TestAndExportStat(double th, int max_concurrent_jobs);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////


template<class Oracle>
bool AdaBoostTrainer<Oracle>::Test()
{
    std::cout << m_classifier.length() << " classifiers:\n";

    return ::Test( m_classifier, m_validation_set);
}
template<class Oracle>
ReportTest AdaBoostTrainer<Oracle>::TestAndExportStat(double th, int max_concurrent_jobs)
{
    std::cout << m_classifier.length() << " classifiers:\n";

    return ::TestAndExportStat( m_classifier, m_validation_set, th, max_concurrent_jobs);
}


template<class Oracle>
void AdaBoostTrainer<Oracle>::Restart(bool reOptimize)
{
    ClassifierListType & list =   m_classifier.list();
// for any Classifier call Post
    for(typename ClassifierListType::iterator i = list.begin(); i != list.end();)
    {
        if(reOptimize)
        {
            // reoptimize (using validation? training?)
            Oracle::Optimize(*i);
        }
        
        // TODO MAdaBoost is missing?

        // Update AdaBoost weights on training/validation set
        bool valid = this->Update(*i, Oracle::GetTrainingSet(), m_validation_set);

        if( !valid )
        {
            std::cout << "Bad classifier removed from list" << std::endl;
            i = list.erase(i);
        }
        else
        {
            ++i;
        }
    }

    Test(); // nono ?

}

template<class Oracle>
bool AdaBoostTrainer<Oracle>::Train()
{
    WeakClassifierType bestH;

    if(m_useMAdaBoost)
    {
      // TODO: probabilmente non funziona
      
        // Modify the "Traning Set" using MAdaBoost variant
        DataSetType & train = Oracle::GetTrainingSet();
        double d = 1.0/m_validation_set.Size(); // MAdaBoost threshold
        int n = 0;
        for(unsigned int i =0; i<train.Size(); i++)
            if( train.templates[i].d > d)
            {
                train.templates[i].d = d;
                ++n;
            }
        std::cout << "[+] MAdaBoost: " << n << " samples of " << train.Size() << " (" << (n*100)/train.Size() << "%) have been limited in weight (" << d <<")" << std::endl;
    }

// get the best Weak Classifier using AdaBoost* metric on Training Set
    if(!Oracle::GetHypothesis(bestH))
        return false;

    // Call AdaBoost Update on validation_set. Eventually improve bestH using validation set.
    // TODO: attenzione che se calcola alpha sul validation set, probabilmente si ottengono degli alpha negativi (anche in Madaboost)
    bool valid = this->Update(bestH, Oracle::GetTrainingSet(), m_validation_set);

    if(valid)
    {
        // store
        m_classifier.insert(bestH);
    }
    else
    {
        std::cout << "[WW] classifier skipped by update rule" << std::endl;
    }

    return true;
}

#endif

