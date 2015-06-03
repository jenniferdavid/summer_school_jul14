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

#ifndef _WPATTERN_H
#define _WPATTERN_H

/** @file WeightedPattern.h
 *  @brief extend Pattern object with a weight */

#include "Pattern.h"
#include "DataSet.h"
#include <iostream> // cout debug

// NOTE those data are used inside train algorithm (internal use)
// TODO: convert totally Pattern ?

/** Extend Pattern with a weight.
  *  All information related to input pattern used for AdaBoost training.
  *  contains both static information and dynamic information.
  *  Extend a pattern with data required by AdaBoost or similiar algorithm
  *
  * @see Pattern
  * */
template<class Descriptor>
struct WeightedPattern: public Pattern<Descriptor> {

    /// Current weight associated to this pattern
    double d;
    
    /// Result on the last evaluate (-1,0,1) of Weak Classifier (binary problem)
    int test;

public:

    WeightedPattern() : d(0.0) { }
    WeightedPattern(const Pattern<Descriptor> & src) :  Pattern<Descriptor>(src), d(0.0) { }
    WeightedPattern(const Descriptor & des, int cat) : Pattern<Descriptor>(des, cat), d(0.0) { }

    /// return weighted category (overriding unweighted one)
    /// [Binary Problem] [AdaBoost]
    float GetWeightedCategory() const {
        return d * (float) Pattern<Descriptor>::category ;
    }

    /// test if last weak classifier, classifies correctly this pattern
    /// @note abstension not works
    inline bool correct() const {
        return test == Pattern<Descriptor>::category;
    }
};


/** Reset pattern weights.
 * @param priori_knownledge weight of positive patterns. If it is <0 then weight = 1/Size(), else if >0:  weight_i = 1/n_i;
 * @todo deal with asymmetric adaboost and not use priori_knownledge
 */
template<class DataSet>
void ResetWeight(DataSet & list, double priori_knownledge)
{
    if(priori_knownledge>0.0)
    {
        double A = priori_knownledge;
        double B = 1.0 - priori_knownledge;
        double dA = A/(list.n_patternP);
        double dB = B/(list.n_patternN);
        std::cout << "Initialize weight with " << dA<< " for " << list.n_patternP << " positives, " << dB << " for " << list.n_patternN << " negatives, " << list.Size() << " total patterns." << std::endl;
        for(unsigned int i =0; i<list.Size(); i++)
            list.templates[i].d = (list.templates[i].category==1) ? dA : dB;
    }
    else
    {
// initialize m_d
        double d0 = 1.0 / (double) list.Size();
        std::cout << "Initialize weight with " << d0 << " for " << list.Size() << " patterns" << std::endl;
        for(unsigned int i =0; i<list.Size(); i++)
            list.templates[i].d = d0;
    }

}


#endif
