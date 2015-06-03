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

#ifndef _DATASET_IN_H
#define _DATASET_IN_H

/** @file datasetin.h
 * @brief virtual class to import images */

#include <vector>
#include <string>

#include "Types.h"
#include "Image.h"

/// item for positive/multiclass
struct datasetobject {
  /// category of positive/negative/donotcare class
  int category;
  /// an initial weight (or multiplication factor)
  float weight;
  /// subregion of img where positive lives
  rect roi;
};

/// an item used for positive
struct datasetitem {
  /// filename
  std::string filename;
  /// size of image (for precomputed set)
  size sz;
  /// if the rest of the blob can be used as source for negative
  bool auto_negative;
  /// List of object
  std::vector<datasetobject> object;
};

/// a virtual pure pattern list reader
class datasetin {
public:

  virtual ~datasetin();

  virtual bool next(datasetitem & item) = 0;
};


/// factory to allocate a generic in2
// datasetin *allocate_in2(const char *filename, int category);

#endif
