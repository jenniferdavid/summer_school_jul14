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

#ifndef _XBOOST_REGISTER_H
#define _XBOOST_REGISTER_H

/** @file register.h
  * @brief builder for classifier
  **/

#include <string>
#include "ImageClassifier.h"
#include "ObjectDetector.h"

/** the classifier factories */
typedef ImageClassifier *(* ClassifierFactory)(std::istream &);
typedef ObjectDetector *(* DetectorFactory)(const char *filename);


/// register a ClassifierFactory in the factory pool
bool register_classifier_factory(const std::string & name, ClassifierFactory, DetectorFactory);

namespace detail {

  /// stub method
  template<class T>
  ImageClassifier *ic_builder(std::istream & in)
  {
    return new T(in);
  }
  template<class T>
  ObjectDetector *od_builder(const char *filename)
  {
    return new T(filename);
  }
  
}

/** a wrapper used to register T object
 * \code
 * register_classifier_factory_wrapper< ClassifierWrapper< BoostClassifier<HaarClassifier> > >();
 * \endcode
 * */
template<class A, class B>
bool register_classifier_factory_wrapper()
{
  return register_classifier_factory(A::signature(), &detail::ic_builder<A>, &detail::od_builder<B>);
}

#endif
