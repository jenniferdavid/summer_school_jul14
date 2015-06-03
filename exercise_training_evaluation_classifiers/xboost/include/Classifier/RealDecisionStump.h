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

#ifndef _REAL_DECISION_STUMP_H
#define _REAL_DECISION_STUMP_H

/** @file RealRealDecisionStump.h
  * @brief A (really) weak classifier with a real, and asymmetric, value response
  * Implements the class RealRealDecisionStump.
  **/

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "Types.h"


/** @brief A Weak Classifier based on a Feature Extractor policy.
 * 
 *  The FeatureExtractor have to implement a response method that returns an integer scalar quantity
 *  @note value=th is an ambiguos case
 **/
template<class DataType>
struct RealDecisionStump {

  /// threshold value
  DataType th;
 
  /// weight associated to positive response (pr) and negative response (nr) 
  float pr, nr;
public:
    
  typedef float ReturnType;
  
    /// NaiveDecisionStump is a DiscreteClassifier
    static const ClassifierType Type = RealClassifier;
  
public:
  
  static std::string signature() { return "realdecisionstump"; }
  
  /// convert the feature value to {p,n} using internal parity and threshold
  inline float evaluate_feature(DataType value) const
   {
   return (value > th) ? pr : nr;
   }
   
   
    /// this function permits to change the th for rescaling
    inline void rescale(int sx, int sy,int sgn)
    {
        //std::cout << "th "<<th<<std::endl;
        th = (int) (th*sx*sy)*sgn;
        //std::cout << "th*sx*sy "<<th<<std::endl;
    }
   
   /// return the maximum range of data returned
  float getAlpha() const {
    return std::max(std::abs(pr), std::abs(nr));
  }
   
};


template<class DataType>
inline std::istream & operator >> (std::istream & in, RealDecisionStump<DataType> & s)
 {
 in >> s.th >> s.pr >> s.nr;
 return in;
 }
 
template<class DataType> 
inline std::ostream & operator << (std::ostream & out, const RealDecisionStump<DataType> & s)
 {
 out << s.th << ' ' << s.pr << ' ' << s.nr;
 return out;
 }

#endif
