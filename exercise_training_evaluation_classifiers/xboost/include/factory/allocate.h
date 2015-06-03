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

#ifndef _XBOOST_ALLOCATE_H
#define _XBOOST_ALLOCATE_H

/** @file allocate.h
  * @brief Allocator for classifier
  **/

#include <iosfwd>
#include <Types.h>
#include "ImageClassifier.h"
#include "ObjectDetector.h"

/** allocate an ImageClassifier, from a factory name and a stream */
ImageClassifier *allocate_classifier(const char *name, std::istream &in);

/** allocate an ImageClassifier, from a filename 
 */
ImageClassifier *allocate_classifier_from_file(const char *filename);

/** allocate an ObjectDetector, from a factory name and a stream */
ObjectDetector *allocate_detector(const char *name, const char *filename);

/** allocate an ObjectDetector, from a filename 
 */
ObjectDetector *allocate_detector_from_file(const char *filename);


/** debug  */
void print_classifier_list(std::ostream & out);


#endif
