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

#ifndef _SUM_AREA_FEATURE_H
#define _SUM_AREA_FEATURE_H

/** @file SumAreaFeature.h
  * @brief an haar like feature but the respose is the sum of a rectangular area
  */

#include <vector>
#include <iostream>

/// Sum of a rectangular area, implemented on top of Integral Image
struct SumAreaFeature {
  /// integral image nodes
  /// output = SUM (X0+1 -> X1) (Y0+1 -> Y1)
  int x0,y0,x1,y1;
public:

  /// type of data requested by this Feature Extractor
  typedef unsigned int InputDataType;
  
  /// type of data returned by this feature extractor
  typedef int DescriptorType;
public:  

 SumAreaFeature() { }
 
 /** weak classifier name */
 static std::string signature() { return "sum"; }
 
 inline int response(const unsigned int *pIntImage, int stride) const
  {
  return (int) pIntImage[x1 + y1 * stride] + (int) pIntImage[x0 + y0 * stride] - (int) pIntImage[x1 + y0 * stride] - (int) pIntImage[x0 + y1 * stride];
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
   x0 = (x0 * sx + sx - 1);
   y0 = (y0 * sy + sy - 1);
   x1 = (x1 * sx + sx - 1);
   y1 = (y1 * sy + sy - 1);
 }
};


 std::istream & operator >> (std::istream & in, SumAreaFeature & s);
 std::ostream & operator << (std::ostream & out, const SumAreaFeature & s);

//////////////////////////////////////////////////////////////////////////////////////////////////

/// an optimized SumAreaFeature
struct OptimizedSumAreaFeature {
  long A,B,C,D;
public:

/** compute an optimized version from an HaarFeature */  
OptimizedSumAreaFeature(const SumAreaFeature &src, long offset, long stride)
{
  A = offset + src.x0 + src.y0 * stride;
  B = offset + src.x0 + src.y1 * stride;
  C = offset + src.x1 + src.y0 * stride;
  D = offset + src.x1 + src.y1 * stride;
  
}

/// compute the response
inline int response(const unsigned int *pIntImage) const
{
  return (int) pIntImage[A] + (int) pIntImage[D] - (int) pIntImage[B] - (int) pIntImage[C];
}

};

#endif
