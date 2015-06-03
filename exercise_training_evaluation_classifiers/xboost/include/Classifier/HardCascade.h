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

#ifndef _HARD_CASCADE_CLASSIFIER_H
#define _HARD_CASCADE_CLASSIFIER_H

/** @file HardCascade.h
  * @brief A boosted hard cascade classifier
  **/

/// A single classifier based on a single HaarFeature
/// @note il nome non mi convince, non e' simmetrico con BoostableClassifier
/// @param WeakClassifier a Weak Classifier type.
template<class WeakClassifier >
struct HardCascadeStage  {

    /// A stage
    BoostClassifier< WeakClassifier > m_classifier;
    
    /// rejection threshold
    float rejection;

public:

    HardCascadeStage() : rejection(0.0f) { }
    HardCascadeStage(const WeakClassifier & weak, float a, float r) : BoostableClassifier<WeakClassifier>(weak, a), rejection(r) { }
};

// IO operators

template<class T>
std::istream & operator >> (std::istream & in, HardCascadeStage<T> & s);

template<class T>
std::ostream & operator << (std::ostream & out, const HardCascadeStage<T> & s);

template<class WeakClassifier >
struct HardCascade  {
  std::vector< HardCascadeStage< WeakClassifier> > m_stages;

   static std::string signature() {
        return "cascade-" + BoostableClassifier<WeakClassifier>::signature();
    }
  
};
#endif
