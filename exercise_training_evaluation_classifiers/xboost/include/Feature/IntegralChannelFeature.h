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

#ifndef _INTEGRAL_CHANNEL_FEATURE_H
#define _INTEGRAL_CHANNEL_FEATURE_H

/** @file IntegralChannelFeature.h
  * @brief an haar like feature but the respose is the sum of a rectangular area on a separate channels 
  */

#include <vector>
#include <iostream>
#include "Preprocessor/IntegralChannelImagePreprocessor.h"

#ifdef __SSE4_1__
# include <smmintrin.h>
# include <emmintrin.h>
# include <mmintrin.h>
#endif

// FWD
struct OptimizedIntegralChannelFeature;

/// Sum of a rectangular area, implemented on top of Integral Image
struct IntegralChannelFeature {
  /// integral image nodes
  /// output = SUM (X0+1, X1) (Y0+1, Y1)
  int x0,y0,x1,y1;
  /// channel identifier
  int channel;
public:

  /// type of data requested by this Feature Extractor
  typedef unsigned int InputDataType;
  
  /// The feature type used during fast execution:
  typedef OptimizedIntegralChannelFeature OptimizedType;
  
  /// type of data returned by this feature extractor
  typedef int DescriptorType;

  enum { InvokeParam = 2 };
public:  

 IntegralChannelFeature() { }

 /** HaarFeature agg*/
 IntegralChannelFeature(const IntegralChannelFeature &src, int scale, long offset, long stride)
 {
	 std::cerr << "cannot allocate ichn @ scale"<<std::endl;
 }
 void invert_polarity()
 {
	 std::cerr << "cannot invert polarity"<<std::endl;
 }
 
 const char *debug_name() { 
   // TODO: use a global object with real-names
   static const char *channel_name[]={"c0","c1","c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "c10", "c11", "c12", "c13", "c14", "c15", "c16" };
   return channel_name[channel]; 
 }
 static void debug_name(const char *str) { }
 
 /** weak classifier name */
 static std::string signature() { return "icf"; }

 // params: first stride1 second stride2
 inline int response(const unsigned int *pIntImage, const std::pair<int32_t,int32_t> & params) const
 {
   pIntImage += channel * params.second;
   return (int) pIntImage[x1 + y1 * params.first] +
	          (int) pIntImage[x0 + y0 * params.first] -
	          (int) pIntImage[x1 + y0 * params.first] -
	          (int) pIntImage[x0 + y1 * params.first];
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


 std::istream & operator >> (std::istream & in, IntegralChannelFeature & s);
 std::ostream & operator << (std::ostream & out, const IntegralChannelFeature & s);

//////////////////////////////////////////////////////////////////////////////////////////////////

/// an optimized SumAreaFeature
struct OptimizedIntegralChannelFeature {
  int A,B,C,D;
  
  typedef OptimizedIntegralChannelFeature OptimizedType;
  
public:
  
  // TODO
  OptimizedIntegralChannelFeature() {}

  /** HaarFeature agg*/
OptimizedIntegralChannelFeature(const OptimizedIntegralChannelFeature &src, int scale, long offset, long stride)
  {
 	 std::cerr << "cannot allocate ichn @ scale"<<std::endl;
  }
  
  void invert_polarity()
  {
 	 std::cerr << "cannot invert polarity"<<std::endl;
  }
/** compute an optimized version from an IntegralChannelFeature 
 */  
OptimizedIntegralChannelFeature(const IntegralChannelFeature &src, long offset, long stride1, long stride2)
{
  A = offset + src.x0 + src.y0 * stride1 + src.channel * stride2;
  B = offset + src.x0 + src.y1 * stride1 + src.channel * stride2;
  C = offset + src.x1 + src.y0 * stride1 + src.channel * stride2;
  D = offset + src.x1 + src.y1 * stride1 + src.channel * stride2;
}

OptimizedIntegralChannelFeature(const IntegralChannelFeature &src, int offset, const IntegralChannelImageParams &p)
{
  A = offset + src.x0 + src.y0 * p.stride1 + src.channel * p.stride2;
  B = offset + src.x0 + src.y1 * p.stride1 + src.channel * p.stride2;
  C = offset + src.x1 + src.y0 * p.stride1 + src.channel * p.stride2;
  D = offset + src.x1 + src.y1 * p.stride1 + src.channel * p.stride2;
}

/// compute the response
inline int response(const unsigned int *pIntImage) const
{
  return (int) pIntImage[A] + (int) pIntImage[D] - (int) pIntImage[B] - (int) pIntImage[C];
}

#ifdef __SSE4_1__
/// compute the response
inline __m128i response4(const unsigned int *pIntImage) const
{
  __m128i p1 = _mm_load_si128((__m128i *)( (long) (pIntImage + A) ) );
  __m128i p2 = _mm_load_si128((__m128i *)( (long) (pIntImage + D) ) );
  __m128i p3 = _mm_load_si128((__m128i *)( (long) (pIntImage + B) ) );
  __m128i p4 = _mm_load_si128((__m128i *)( (long) (pIntImage + C) ) );
  p1 = _mm_add_epi32(p1, p2);
  p3 = _mm_add_epi32(p3, p4);
  return _mm_sub_epi32(p1, p3);
}
#endif

};

#endif
