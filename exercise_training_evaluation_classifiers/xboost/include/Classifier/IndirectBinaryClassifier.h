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

#ifndef _INDIRECT_BINARY_CLASSIFIER_H
#define _INDIRECT_BINARY_CLASSIFIER_H

/** @file BinaryClassifier.h
  * @brief A Binary Classifier without feature extractor.
  *  Implements the class BinaryClassifier
  **/

#include <iostream>
#include <vector>
#include <string>
#include "Types.h"

/** @brief A classifier composed by an index of a feature and an Evaluation Policy
 **/
template<class Policy>
struct IndirectBinaryClassifier : public Policy {

public:

    typedef typename Policy::ReturnType ReturnType;
    
    /// no more optimization
    typedef IndirectBinaryClassifier<Policy> OptimizedType;
    
public:
  
    /// the index of the feature used during the classification stage
    int feature_index;

    /// BinaryClassifier inherits type from Policy
    static const ClassifierType Type = Policy::Type;
    
public:

    IndirectBinaryClassifier() { }
    ~IndirectBinaryClassifier() { }

    static std::string signature() {
        return "x-decision-stump";
    }

    /** Classify using a Decision Stump on @a features response
     * @param features a list of features extracted
     * @param stride   the shift between two consecutive features
      * */
    template<class DataType>
    inline int operator() (const DataType *features, long stride) const
    {
        return Policy::evaluate_feature( features[feature_index * stride] );
    }

};

// IO operators

template<class Policy>
std::istream & operator >> (std::istream & in, IndirectBinaryClassifier<Policy> & s);

template<class Policy>
std::ostream & operator << (std::ostream & out, const IndirectBinaryClassifier<Policy> & s);

//////////////////////////////////////////////////////////////////////////////////////////////////


template<class Policy>
std::istream & operator >> (std::istream & in, IndirectBinaryClassifier<Policy> & s)
{
    in >> static_cast<Policy&>(s) >> s.feature_index;
    return in;
}

template<class Policy>
std::ostream & operator << (std::ostream & out, const IndirectBinaryClassifier<Policy> & s)
{
    out << static_cast<const Policy&>(s) << ' ' << s.feature_index;
    return out;
}


#endif
