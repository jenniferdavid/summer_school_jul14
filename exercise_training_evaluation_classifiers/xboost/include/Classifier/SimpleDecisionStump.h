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

#ifndef _SIMPLE_DECISION_STUMP_H
#define _SIMPLE_DECISION_STUMP_H

/** @file SimpleDecisionStump.h
  * @brief A very simple decision stump with no parity
  * Implements the class SimpleDecisionStump for BinaryClassifier
  **/

#include "Types.h"

/// A simpler decision stump without parity
/// TODO: instance it when feature admit inversion
template<class DataType>
struct SimpleDecisionStump {

    /// threshold value
    DataType th;
public:
  typedef int ReturnType;
  
    /// SimpleDecisionStump is a DiscreteClassifier
    static const ClassifierType Type = DicreteClassifier;
  
public:
  

    /// convert the feature value to {-1,+1} using internal threshold
    inline int evaluate_feature(DataType value) const
    {
        return (value > th) ? 1 : -1;
    }
    
    static std::string signature() {
      return "simplestump";
    }
      

};

template<class DataType>
inline std::istream & operator >> (std::istream & in, SimpleDecisionStump<DataType> & s)
{
    in >> s.th;
    return in;
}

template<class DataType>
inline std::ostream & operator << (std::ostream & out, const SimpleDecisionStump<DataType> & s)
{
    out << s.th;
    return out;
}

#endif
