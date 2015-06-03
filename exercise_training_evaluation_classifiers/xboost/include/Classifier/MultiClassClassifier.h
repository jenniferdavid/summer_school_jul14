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

#ifndef _MULTICLASS_CLASSIFIER_H
#define _MULTICLASS_CLASSIFIER_H

/** @file MultiClassClassifier.h
  * @brief A classifier for Multi-Class Problem
  * Implements the class MultiClassClassifier.
  **/

#include "Types.h"
#include <vector>

/** @brief A Multi-Class Classifier based on a Feature Extractor policy
 **/
template<class FeatureExtractor, class MultiClassPolicy>
struct MultiClassClassifier : public FeatureExtractor, public MultiClassPolicy {

public:

    typedef FeatureExtractor FeatureType;

    /// DecisionStump is a DiscreteClassifier
    static const ClassifierType Type = DicreteClassifier;

public:

    MultiClassClassifier(int n_classes) : MultiClassPolicy(n_classes) { }
    ~MultiClassClassifier() { }

    /// return the "feature" name (for debug purpose)
    const char *str() const {
        return FeatureExtractor::name;
    }

    /// signature name is the name of provider (for simplicity)
    static std::string signature() {
        return MultiClassPolicy::signature() + "-" + FeatureExtractor::signature();
    }

    /** Classify using a Decision Stump on FeatureExtractor response
      * @return {-1,+1), for example: true (+1) pedestrian, -1 non-pedestrian
      * if parity=1  then +1 is returned if v < th, -1 otherwise ( v >= th )
      * if parity=-1 then +1 is returned if v > th, -1 otherwise ( v <= th )
      * */
    template<class D1, class D2>
    inline void classify(int *response, const D1 d1, const D2 d2) const
    {
        evaluate_feature(response,  FeatureExtractor::response(d1, d2) );
    }

    /// classify (1 param version)
    /// @see other version
    template<class D1>
    inline void classify(int *response, const D1 d1) const
    {
        evaluate_feature(response, FeatureExtractor::response(d1) );
    }

    /// classify wrapper. @see classify
    template<class D1, class D2>
    inline void operator()(int *response, const D1 d1, const D2 d2) const
    {
        evaluate_feature(response, FeatureExtractor::response(d1) );
    }

    /// this function permits to change the geometry of haar feature: integral value should be used
    void rescale(float sx, float sy)
    {
        // TODO: rescale th
        FeatureExtractor::rescale(sx,sy);
    }

    template<class FeatureType>
    bool export_features(std::vector<FeatureType> & out) const
    {
        out.push_back( static_cast<const FeatureType&>(*this) );
    }

};

template<class T, class MultiClassPolicy>
std::istream & operator >> (std::istream & in, MultiClassClassifier<T, MultiClassPolicy> & s)
{
    in >> static_cast<MultiClassPolicy &>(s) >> static_cast<T&>(s);
    return in;
}

template<class T, class MultiClassPolicy>
std::ostream & operator << (std::ostream & out, const MultiClassClassifier<T, MultiClassPolicy> & s)
{
    out << static_cast<const MultiClassPolicy &>(s) << ' ' << static_cast<const T&>(s);
    return out;
}


#endif
