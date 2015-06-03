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

#ifndef _BINARY_CLASSIFIER_H
#define _BINARY_CLASSIFIER_H

/** @file BinaryClassifier.h
  * @brief A Binary Classifier.
  *  Implements the class BinaryClassifier
  **/

#include <iostream>
#include <vector>
#include <string>
#include "Types.h"

/** @brief A classifier composed by a Feature Extractor and an Evaluation Policy
 * A "Second Level" classifier, merging a feature extractor algorithm with a feature evaluation classifier.
 **/
template<class FeatureExtractor, class Policy>
struct BinaryClassifier : public FeatureExtractor, public Policy {

public:

    typedef FeatureExtractor FeatureType;
    
    typedef Policy PolicyType;

    typedef typename Policy::ReturnType ReturnType;
    
    /// the optimized type
    typedef BinaryClassifier<typename FeatureExtractor::OptimizedType, Policy> OptimizedType;

    /// BinaryClassifier inherits type from Policy
    static const ClassifierType Type = Policy::Type;
    
public:

    BinaryClassifier(const FeatureExtractor & x) : FeatureExtractor(x) { }

    /** special costructor */
    template<class R, class P1>
    explicit BinaryClassifier(const R & x, P1 p1) : FeatureExtractor(x, p1), Policy(x) { }
    /** special costructor */
    template<class R, class P1, class P2>
    explicit BinaryClassifier(const R & x, P1 p1, P2 p2) : FeatureExtractor(x, p1, p2), Policy(x) { }
    /** special costructor */
    template<class R, class P0, class P1, class P2>
    explicit BinaryClassifier(const R & x, P0 p0, P1 p1, P2 p2) : FeatureExtractor(x, p0, p1, p2), Policy(x)
    {
    	Policy::rescale(p0,p0,1);//scale th
    }

    BinaryClassifier() { }
    ~BinaryClassifier() { }

    /// return the "feature" name (for debug purpose)
    const char *str() const {
        return FeatureExtractor::name;
    }

    /// signature name is the name of provider (for simplicity)
    static std::string signature() {
        return Policy::signature() + "-" + FeatureExtractor::signature();
    }

    /** Classify using a Decision Stump on FeatureExtractor response
      * @return {-1,+1), for example: true (+1) pedestrian, -1 non-pedestrian
      * if parity=1  then +1 is returned if v < th, -1 otherwise ( v >= th )
      * if parity=-1 then +1 is returned if v > th, -1 otherwise ( v <= th )
      * */
    template<class D1, class D2>
    inline ReturnType classify(const D1 d1, const D2 d2) const
    {
        return Policy::evaluate_feature( FeatureExtractor::response(d1, d2) );
    }

    /// classify (1 param version)
    /// @see other version
    template<class D1>
    inline ReturnType classify(const D1 d1) const
    {
        return Policy::evaluate_feature( FeatureExtractor::response(d1) );
    }

    /// classify wrapper. @see classify
    template<class D1, class D2>
    inline ReturnType operator()(const D1 d1, const D2 d2) const
    {
        return classify(d1, d2);
    }

    /// classify wrapper. @see classify
    template<class D1>
    inline ReturnType operator()(const D1 d1) const
    {
        return classify(d1);
    }
    /// return both feature responde and classify hypothesis
    template<class D1, class D2>
    void test_and_evalute(int & value, ReturnType & test, const D1 d1, const D2 d2) const
    {
        value = FeatureExtractor::response(d1, d2);
        test = Policy::evaluate_feature(value);
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

// IO operators

template<class FeatureType, class Policy>
std::istream & operator >> (std::istream & in, BinaryClassifier<FeatureType, Policy> & s);

template<class FeatureType, class Policy>
std::ostream & operator << (std::ostream & out, const BinaryClassifier<FeatureType, Policy> & s);

//////////////////////////////////////////////////////////////////////////////////////////////////


template<class FeatureType, class Policy>
std::istream & operator >> (std::istream & in, BinaryClassifier<FeatureType, Policy> & s)
{
    in >> static_cast<Policy&>(s) >> static_cast<FeatureType&>(s);
    return in;
}

template<class FeatureType, class Policy>
std::ostream & operator << (std::ostream & out, const BinaryClassifier<FeatureType, Policy> & s)
{
    out << static_cast<const Policy&>(s) << ' ' << static_cast<const FeatureType&>(s);
    return out;
}


#endif
