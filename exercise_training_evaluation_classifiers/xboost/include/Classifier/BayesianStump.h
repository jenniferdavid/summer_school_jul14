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

#ifndef _BAYESIAN_STUMP_H
#define _BAYESIAN_STUMP_H

/** @file BayesianStump.h
  * @brief An hystogram based Bayesian classifier
  * Implements the class BayesianStump for BinaryClassifier.
  **/

#include <iostream>
#include <vector>
#include <string>
#include "_stdint.h"
#include "Types.h"


/// A simpler decision stump without parity
/// TODO: instance it when feature admit inversion
template<class DataType>
struct BayesianStump {

  /// limit to 32bit thruth table
  static const int bin_size = 32;
  
//   int m_binsze;
  
  /// minimum feature value
  DataType m_minFeatValue;
  /// feature range values
  DataType m_featRange;
  
  /// truth table (BinSize bin) 1:+ 0:-
  uint32_t m_truthTable;
  
public:
  typedef int ReturnType;
  
    /// BayesianStump is a DiscreteClassifier
    static const ClassifierType Type = DicreteClassifier;
  
public:
  

  int evaluate_feature(DataType response) const {
   int bin = ((response - m_minFeatValue) * bin_size) / m_featRange;
   if(bin>=bin_size) bin=bin_size-1;
   if(bin<0) bin=0;
   
   return ((m_truthTable & (1<<bin))!=0) ? 1 : -1;
  }

    static std::string signature() {
      return "bayesianstump";
    }
  
};

template<class DataType>
std::istream & operator >> (std::istream & in, BayesianStump<DataType> & s)
 {
 unsigned int tt;
 int bin_size; // TODO check
 in >> bin_size >>  s.m_minFeatValue >> s.m_featRange >> tt;
 s.m_truthTable = tt;
 return in;
 }

template<class DataType>
std::ostream & operator << (std::ostream & out, const BayesianStump<DataType> & s)
 {
 out << s.bin_size << ' ' << s.m_minFeatValue << ' ' << s.m_featRange << ' ' << (unsigned int) s.m_truthTable;
 return out;
 }

#endif
