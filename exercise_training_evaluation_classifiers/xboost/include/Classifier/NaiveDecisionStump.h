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

#ifndef _NAIVE_DECISIONSTUMP_H
#define _NAIVE_DECISIONSTUMP_H

/** @file NaiveDecisionStump.h
  * @brief A very simple decision stump with threshold 0 (could be a more luminance insesitive)
  * Implements the class NaiveDecisionStump for BinaryClassifier
  **/

struct NaiveDecisionStump {
public:
  typedef int ReturnType;
  
    /// NaiveDecisionStump is a DiscreteClassifier
    static const ClassifierType Type = DicreteClassifier;
  
public:
  
  
    /// convert the feature value to {-1,+1} using 0 as threshold
    template<class DataType>
    static int evaluate_feature(DataType value)
    {
        return (value > DataType(0) ) ? 1 : -1;
    }
    
    static std::string signature() {
      return "naivestump";
    }
    
};

inline std::istream & operator >> (std::istream & in, NaiveDecisionStump & s)
{
    return in;
}

inline std::ostream & operator << (std::ostream & out, const NaiveDecisionStump & s)
{
    return out;
}

#endif
