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

#ifndef _ABS_DECISIONSTUMP_H    
#define _ABS_DECISIONSTUMP_H

/** @file AbsDecisionStump.h
  * @brief A Decision Stump with Abstention
  * Implements the class AbsDecisionStump.
  **/

#include <iostream>
#include <vector>
#include <string>
#include "Types.h"

/** @brief A Weak Classifier based on a Feature Extractor policy.
 * @note not handle case of wrong parity
 **/
template<class DataType>
struct AbsDecisionStump {
  /// neg, pos threshold
  DataType thn, thp;
public:
  
  typedef int ReturnType;
  
    /// AbsDecisionStump is a DiscreteClassifier
    static const ClassifierType Type = DicreteClassifier;
  
public:
  
  /// convert the feature value to {-1,0,+1} using thresholds
  inline int evaluate_feature(DataType value) const
   {
   return (value < thn) ? -1 : (value > thp) ? 1 : 0;
   }
   
    static std::string signature() {
      return "absdecisionstump";
    }   
};

template<class DataType>
inline std::istream & operator >> (std::istream & in, AbsDecisionStump<DataType> & s)
 {
 in >> s.thn >> s.thp;
 return in;
 }

template<class DataType>
inline std::ostream & operator << (std::ostream & out, const AbsDecisionStump<DataType> & s)
 {
 out << s.thn << ' ' << s.thp;
 return out;
 }
 
#endif
