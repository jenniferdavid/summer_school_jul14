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

#ifndef _DECISION_STUMP_H
#define _DECISION_STUMP_H

/** @file DecisionStump.h
  * @brief A (really) weak classifier.
  * Implements the policy DecisionStump for BinaryClassifier.
  **/

#include <string>
#include <iosfwd>
#include "Types.h"

#include <iostream>

/** @brief A Weak Classifier based on a Feature Extractor policy.
 *
 *  The FeatureExtractor have to implement a response method that returns an integer scalar quantity
 *  parity=+1 ->  class = (value < th) ? 1 : -1;
 *  parity=-1 ->  class = (value > th) ? 1 : -1;
 *  @note parity=1 and parity=-1 are not exact opposite cases due to missing of =
 *  @note value=th is an ambiguos case
 *  @todo try to compact BoostableClassifier with DecisionStump in order to reduce number of multiplication (parity * alpha) (e.g. using RealDecisionStump classes)
 **/
template<class DataType>
struct DecisionStump {
    /// parity for this classifier (-1, 1)
    int parity;

    /// threshold value
    DataType th;
public:
    typedef int ReturnType;

    /// DecisionStump is a Discrete Classifier
    static const ClassifierType Type = DicreteClassifier;

public:

    /// convert the feature value to {-1,+1} using internal parity and threshold
    /// parity=1  -> value < th ? 1 : -1
    /// parity=-1  -> value > th ? 1 : -1
    inline int evaluate_feature(DataType value) const
    {
        return (parity * value < parity * th) ? 1 : -1;
    }

    static std::string signature() {
        return "decisionstump";
    }

    /// this function permits to change the th for rescaling
    inline void rescale(int sx, int sy,int sgn)
    {
    	th = (int) (th*sx*sy)*sgn;
    }

};

template<class DataType>
inline std::istream & operator >> (std::istream & in, DecisionStump<DataType> & s)
{
    in >> s.parity >> s.th;
    return in;
}

template<class DataType>
inline std::ostream & operator << (std::ostream & out, const DecisionStump<DataType> & s)
{
    out << s.parity << ' ' << s.th;
    return out;
}

#endif
