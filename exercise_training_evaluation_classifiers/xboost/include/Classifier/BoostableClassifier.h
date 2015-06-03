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

#ifndef _BOOSTABLE_CLASSIFIER_H
#define _BOOSTABLE_CLASSIFIER_H

#include <string>
#include <vector>
#include "Types.h"

/** @file BoostableClassifier.h
 *  @brief a Voting Boostable classifier */

//////////////////////////////////////////////////////////////////////////////////////

/** Expand a binary weak classifier in order to become an ensemble learner.
  *  An ensemple learner with a weight associated to classifier.
  * WeakClassifier must implement a method operator() that returns {-1|+1}
  * @note WeakClassifier must be a Discrete Weak Classifier, Single or MultiClass
  * TODO: change name in BoostedClassifier
  */
template<class WeakClassifier>
struct BoostableClassifier : public WeakClassifier {
  
  /// Weight associated to this classifier
  float alpha;
  
public:

  ///
  typedef WeakClassifier WeakClassifierType;
  
  /// BoostableClassifier convert a DiscreteClassifier in a RealClassifier
  static const ClassifierType Type = RealClassifier;
  
public:
  
  BoostableClassifier() : alpha(0.0f) { }
  BoostableClassifier(const WeakClassifier & weak, float a) : WeakClassifier(weak), alpha(a) { }

// NOTE build and convert costructor with 1 parameter overlapps with previous constructor
  
/// build and convert costructor with 2 parameters
  template<class R, class P1, class P2>
  explicit BoostableClassifier(const R & src, P1 p1, P2 p2) : WeakClassifier(static_cast<const typename R::WeakClassifierType &>(src), p1, p2), alpha(src.alpha)  {  }

/// build and convert costructor with 3 parameters
  template<class R, class P0, class P1, class P2>
  explicit BoostableClassifier(const R & src, P0 p0, P1 p1, P2 p2) : WeakClassifier(static_cast<const typename R::WeakClassifierType &>(src), p0, p1, p2), alpha(src.alpha)  {  }
  
  
  // TODO: boost- here is not correct. Boost is BoostClassifier. This class associate only weight to a Discrete Weak Classifier
  static std::string signature() { return std::string("boost-") + WeakClassifier::signature(); }
  
  /** evaluate integral image and return alpha/-alpha according to binary class */
  template<class TParam1>
  inline float operator()(const unsigned int *pIntImage, TParam1 stride) const;

  /** evaluate integral image and return alpha/-alpha according to binary class */
  inline float operator()(const unsigned int *pIntImage) const;

  template<class TParam1>
  inline void accumulate(float *response, const unsigned int *pIntImage, TParam1 stride, int *work) const;
  
  /// return the maxima response (in absolute value) of this classifier.
  /// @return the inner alpha (used only for debug purpose usually)
  inline float getAlpha() const { return alpha; }
  
};

template<class WeakClassifier>
inline std::istream & operator >> (std::istream & in, BoostableClassifier<WeakClassifier> & s);

template<class WeakClassifier>
inline std::ostream & operator << (std::ostream & out, const BoostableClassifier<WeakClassifier> & s);


//////////////////////////////////////////////////////////////////////////////////////

template<class T>
template<class TParam1>
float BoostableClassifier<T>::operator()(const unsigned int *pIntImage, TParam1 stride) const
  {
    return T::operator()(pIntImage, stride) * alpha; 
  }

template<class T>
float BoostableClassifier<T>::operator()(const unsigned int *pIntImage) const
  {
    return T::operator()(pIntImage) * alpha;
  }
  
template<class T>
template<class TParam1>
void BoostableClassifier<T>::accumulate(float *response, const unsigned int *pIntImage, TParam1 stride, int *work) const
  {
//     T::classify(work, pIntImage, stride);
//     for(int i =0;i<m_nClasses;++i)
//     {
//       response[i] += work[i] * alpha;
//     }
  }
  

template<class T>
inline std::istream & operator >> (std::istream & in, BoostableClassifier<T> & s)
  {
  in >> s.alpha >> static_cast<T&>(s);
  return in;
  }
  
template<class T>
inline std::ostream & operator << (std::ostream & out, const BoostableClassifier<T> & s)
  {
  out << s.alpha << ' '  << static_cast<const T&>(s);
  return out;
  }

  
#endif