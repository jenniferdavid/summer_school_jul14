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

#ifndef _NORMALIZED_HAAR_FEATURE_H
#define _NORMALIZED_HAAR_FEATURE_H

/** @file NormalizedHaarFeature.h
  * @brief a haar feature with normalized response
  */

#include "HaarFeature.h"

/// A rationale haar feature
struct NormalizedHaarFeature : public HaarFeature {

  /// roi
  int width, height;

public:

  /// type of data requested by this Feature Extractor
  typedef unsigned int InputDataType;
  
//   /// return a float response
//   typedef float OutputDataType;
  
public:  

 NormalizedHaarFeature() { }
 
 /** weak classifier name */
 static std::string signature() { return "normalized-haar"; }

 inline int response(const unsigned int *pIntImage, int stride) const
  {
    int value = HaarFeature::response(pIntImage, stride);
    int factor = pIntImage[-1 - stride] + pIntImage[width + height * stride] - pIntImage[width] - pIntImage[height * stride];
  return (value * 65536) / factor;
  }


  inline int operator() (const unsigned int *pIntImage, int stride) const
  {
    return response(pIntImage, stride);
  }

 /// Rescale current feature using sx,sy scale factor
 /// @note due to particular architecture of this filter, only integral scale factor are alowed now
 ///       use a fHaarFeature in order to take advantage of floating point rescaling
 void rescale(int sx, int sy)
 {
  HaarFeature::rescale(sx, sy);
  width = (width * sx + sx - 1);
  height = (height * sy + sy - 1);
 }
};


 std::istream & operator >> (std::istream & in, NormalizedHaarFeature & s);
 std::ostream & operator << (std::ostream & out, const NormalizedHaarFeature & s);

#endif
