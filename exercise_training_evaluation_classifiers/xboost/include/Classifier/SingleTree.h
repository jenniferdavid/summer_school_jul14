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

#ifndef _SINGLE_TREE_H
#define _SINGLE_TREE_H

////// TODO: remove this file, is obsolete

/** @file SingleTree.h
  * @brief a single branch trainer of HaarTreeClassifier
  */

#include "BaseTrainer.h"
#include "BoostClassifier.h"

/// Trainer per 2 classi implements AdaBoost.M1
/// true: class A, false: class B
class SingleTree: public BoostClassifier<HaarTreeClassifier>, public BaseTrainer {

  typedef BoostClassifier<HaarTreeClassifier>::Classifier Classifier;
  typedef std::vector<Classifier> ClassifierCollection;

   std::vector<Pattern> m_backup;
   int m_backup_pattern;

public:
  SingleTree() 
    { 
    }

  /// Initialize Trainer
  template<class F>
  void InitTrainer(F & f, int preload)
  {
    BaseTrainer::InitTrainer(preload);

    // Initialize m_count
    TestFeatureGeneration(f, false);
  }

  /// Freed temporary memory before next InitTrainer
  void ShutdownTrainer() { }

  /// Reset weight and bootstrap from classifier
  void Restart() { }

  /// Esegue un singolo ciclo di training
  template<class FeatureGenerator>
  bool Train(FeatureGenerator &f, bool preload) {

  std::cout << "SingleTree" << std::endl;

  Classifier bestH;
  
  // uso il BaseTrainer per ottenere comunque il migliore base Classifier
  if(!BaseTrainer::Train(bestH.root, f, preload))
      return false;

  // ora bisogna massimizzare localmente i corretti e gli sbagliati
  BaseTrainer::EvaluateAndSet(bestH.root);

  std::swap(m_backup, m_templates);
  m_backup_pattern = n_pattern;

  m_templates.clear();
  n_pattern = 0;

  for(int i =0;i<m_backup_pattern;++i)
   {
   if( m_backup[i].test == 1 )
     {
     m_templates.push_back(m_backup[i]);
     n_pattern++;
     }
   }

  std::cout << " === 1: " << n_pattern << " === " << std::endl;
//   InitWeight();

  BaseTrainer::Train(bestH.pos, f, preload);

  m_templates.clear();
  n_pattern = 0;

  for(int i =0;i<m_backup_pattern;++i)
   {
   if( m_backup[i].test == -1 )
     {
     m_templates.push_back(m_backup[i]);
     n_pattern++;
     }
   }

  std::cout << " === -1: " << n_pattern << " === " << std::endl;
//   InitWeight();

  BaseTrainer::Train(bestH.neg, f, preload);
  
  std::swap(m_templates, m_backup);
  n_pattern = m_backup_pattern;
  InitWeight();

  Post(bestH);

  // store
  this->m_weak_classifiers.push_back(bestH);
   
  return true;
  }

  /// Test current network
  bool Test();
  
};

#endif
