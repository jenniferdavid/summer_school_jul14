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

#ifndef _BOOST_CLASSIFIER_H
#define _BOOST_CLASSIFIER_H

/** @file BoostClassifier.h
 *  @brief a voting for majority classifier */

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>// sort

#include "BoostableClassifier.h"

namespace detail {
 
  /// a traits to change the inner classifier form, to use or not a weighted one 
  template<ClassifierType type, class T>
  struct traits;

  /// DiscreteClassifiers uses BoostableClassifier object to convert the binary decision-stump (for example) to provide a floating-point response
  template<class T>
  struct traits<DicreteClassifier, T>
  {
  typedef BoostableClassifier<T> Boosted;  
  };
  
  /// Other Classifier usually reports directly a floating-point response
  template<class T>
  struct traits<RealClassifier, T>
  {
  typedef T Boosted;  
  };
  
}

/// A classifier that classify with majoirty voting of N (weak) classifier.
/// Classifier could be any kind of weak classifier (Discrete or Real)
/// Result are voted for majority
template<class Classifier >
struct BoostClassifier {

public:

/// A Boosted Classifier (must be a "Real" Classifier)
typedef typename detail::traits<Classifier::Type, Classifier>::Boosted BoostedClassifierType;

/// List of Boosted classifier
typedef std::vector< BoostedClassifierType > ClassifierListType;

/// An Optimized Classifier
typedef BoostClassifier< typename Classifier::OptimizedType > OptimizedType;

private:
  
/// A che collection of HaarCascadeClassifier or similar
ClassifierListType m_weak_classifiers;

public:

/// ctor
BoostClassifier();
/// ctor with file with data
BoostClassifier(std::istream & in);

  /// genera un BoostClassifier partendo da un altro BoostClassifier, ed eseguendo un rescaling di tutti i classificatori
template<class R, class P0, class P1, class P2>
explicit BoostClassifier(const BoostClassifier<R> & src, P0 scale, P1 p1, P2 p2)
{
  m_weak_classifiers.reserve(src.length() );
  for(typename BoostClassifier<R>::ClassifierListType::const_iterator i = src.list().begin(); i!= src.list().end(); ++i)
  {
    m_weak_classifiers.push_back( BoostedClassifierType(*i, scale, p1, p2) );
  }

}

/// create and convert a classifier
template<class R, class P1, class P2>
explicit BoostClassifier(const BoostClassifier<R> & src, P1 p1, P2 p2)
{
  m_weak_classifiers.reserve(src.length() );
  for(typename BoostClassifier<R>::ClassifierListType::const_iterator i = src.list().begin(); i!= src.list().end(); ++i)
    m_weak_classifiers.push_back( BoostedClassifierType(*i, p1, p2) );
}

/// create and convert a classifier
template<class R, class P1>
explicit BoostClassifier(const BoostClassifier<R> & src, const P1 & p1)
{
  m_weak_classifiers.reserve(src.length() );
  for(typename BoostClassifier<R>::ClassifierListType::const_iterator i = src.list().begin(); i!= src.list().end(); ++i)
    m_weak_classifiers.push_back( BoostedClassifierType(*i, p1) );
}


/// dtor
~BoostClassifier();

/// Return the number of classifiers
inline unsigned int length() const  { return m_weak_classifiers.size(); }

/// Return the inner list of classifier
inline ClassifierListType & list() { return m_weak_classifiers; }
inline const ClassifierListType & list() const { return m_weak_classifiers; }

/// propagate signature
static std::string signature() { return "additive-" + BoostedClassifierType::signature(); }

/// the real computer (binary problem)
/// @param pIntImage integral image
/// @param stride integral image stride
/// @param classifier an array of Boosted Classifier (classifier + alpha)
/// @param n number of Classifier in the Chain
template<class Param1>
static float compute(const unsigned int *pIntImage, Param1 stride, const BoostedClassifierType *classifier, int n);

/// compute for 1 params version
static float compute(const unsigned int *pIntImage, const BoostedClassifierType *classifier, int n);

/// the real computer (multiclass problem)
template<class Param1>
static void compute(float *responses, const unsigned int *pIntImage, Param1 stride, const BoostedClassifierType *classifier, int n);

/// A generalized version of Ada-Boost
/// @return the sum of single classifiers multiplied by weights
template<class Param1>
inline float operator()(const unsigned int *pIntImage, Param1 stride) const {
  return compute(pIntImage, stride, &m_weak_classifiers[0], m_weak_classifiers.size());
}

/// A generalized version of Ada-Boost
/// @return the sum of single classifiers multiplied by weights
inline float operator()(const unsigned int *pIntImage) const {
  // return compute(pIntImage, &m_weak_classifiers[0], m_weak_classifiers.size());
  float acc = 0.0;
  for(typename ClassifierListType::const_iterator i = m_weak_classifiers.begin(); i != m_weak_classifiers.end();i++)
      acc += (*i)(pIntImage);
  /*
  for(int i = 0; i< m_weak_classifiers.size();i++)
      acc += m_weak_classifiers[i](pIntImage);
  */
  return acc;
}

/// A generalized version of Ada-Boost
/// @return the sum of single classifiers multiplied by weights
inline float operator()(const unsigned int *pIntImage, int stride, int n) const {
  return compute(pIntImage, stride, &m_weak_classifiers[0], n);
}

/// sort classifier with alpha descending (@todo check)
void sort();

/// export all features;
template<class FeatureType>
bool export_features(std::vector<FeatureType> & out) const 
{
  for(typename ClassifierListType::const_iterator i = m_weak_classifiers.begin(); i != m_weak_classifiers.end(); i++)
    i->export_features(out);
}

/** load weak classifier from a stream */
void load(std::istream & in);

/// Reduce number of classifiers (remove the last or, after sort(), the low influence ones)
void resize(int size);

/// return the numeber of weak classifiers
int size() const { 
  return m_weak_classifiers.size(); 
};

/// return the sum alphas associated to weak classifiers
float max_response() const {
 float sum=0.0f;
 for(typename ClassifierListType::const_iterator i = m_weak_classifiers.begin(); i != m_weak_classifiers.end(); i++)
   sum+=i->getAlpha();
 return sum;
}

/// normalize the overall response in order to range the return values from -1 to 1
// void normalize() {
//  float f = 1.0f/max_response();
//  for(typename ClassifierListType::iterator i = m_weak_classifiers.begin(); i != m_weak_classifiers.end(); i++)
//   i->alpha *= f;
// }

inline void insert(const BoostedClassifierType & classifier)
{
  m_weak_classifiers.push_back(classifier);
}

// insert a classifier, with a weight alpha, in the chain
// inline void insert(const Classifier & weak, float alpha)
// {
//   insert( BoostedClassifierType(weak, alpha) );
// }

};

template<class T>
std::istream & operator >>(std::istream & in, BoostClassifier<T> & dst);

template<class T>
std::ostream & operator << (std::ostream & out, const BoostClassifier<T> & src);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// IO
template<class T>
std::istream & operator >>(std::istream & in, BoostClassifier<T> & dst)
 {
 dst.load(in);
 return in;
 }

// IO
template<class T>
std::ostream & operator << (std::ostream & out, const BoostClassifier<T> & src)
 {
 for(typename  BoostClassifier<T>::ClassifierListType::const_iterator i = src.list().begin(); i != src.list().end(); i++)
  out << *i << '\n';
 return out;
 }


template<class T>
BoostClassifier<T>::BoostClassifier() { }

template<class T>
BoostClassifier<T>::~BoostClassifier() { }

template<class T>
BoostClassifier<T>::BoostClassifier(std::istream & in)
{
  load(in);
  
//   std::cout << m_weak_classifiers.size() << " weak classifier loaded" << std::endl;
}

template<class T>
void BoostClassifier<T>::load(std::istream & in)
{
 while(in)
  {
  BoostedClassifierType h;
  in >> h;
  //std::cout << "c: "<< h << std::endl;
  //std::cin.get();
  if(in)
    {
    // std::cout << h << std::endl;
    m_weak_classifiers.push_back(h);
    }
  }
}

template<class T>
template<class Param1>
float BoostClassifier<T>::compute(const unsigned int *pIntImage, Param1 stride, const BoostedClassifierType *bc, int n)
{
  float acc = 0.0;
  for(int i = 0; i<n;i++)
      acc += bc[i](pIntImage, stride);
  return acc;
}

template<class T>
float BoostClassifier<T>::compute(const unsigned int *pIntImage, const BoostedClassifierType *bc, int n)
{
  float acc = 0.0;
  for(int i = 0; i<n;i++)
      acc += bc[i](pIntImage);
  return acc;
}


template<class T>
template<class Param1>
void BoostClassifier<T>::compute(float *responses, const unsigned int *pIntImage, Param1 stride, const BoostedClassifierType *bc, int n)
{
//   for(int i = 0; i<m_nClasses;++i)
//       responses[i] = 0.0f;
//   
//   for(int i = 0; i<n;i++)
//   {
//       bc[i].accumulate(responses, pIntImage, stride);
//   }
//   
}


// detail: sort policy
template<class T>
inline bool sort_by_alpha(const BoostableClassifier<T> & a, const BoostableClassifier<T> & b)
{
return a.alpha > b.alpha;
}

template<class T>
void BoostClassifier<T>::sort()
{
std::sort(m_weak_classifiers.begin(), m_weak_classifiers.end(), sort_by_alpha<T> );
}

template<class T>
void BoostClassifier<T>::resize(int size)
{
m_weak_classifiers.resize(size);
}


#endif
