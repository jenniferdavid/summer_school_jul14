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

#ifndef _HAAR_FEATURE_H
#define _HAAR_FEATURE_H

/** @file HaarFeature.h
  * @brief the haar feature 
  */

#include <vector>
#include <iostream>


/** The single node in the Haar Feature
  * Any Haar Feature can be decomposed in a weighed read.
  **/
struct HaarNode {
  int sign;	// multiplier	
  int x,y;	// posizion of feature

public:  

  HaarNode() { }
  HaarNode(int _sign, int _x, int _y) : sign(_sign), x(_x), y(_y) 
  { 
  }

  inline int operator()(const unsigned int *pIntImage, long stride) const
	{
    	return (int)(pIntImage[x + y * stride]) * sign;
	}
};

std::istream & operator >> (std::istream & in, HaarNode & d);
std::ostream & operator << (std::ostream & out, const HaarNode & s);

typedef int (* Evaluate_t)(const unsigned int *pIntImage, int stride, const HaarNode *node);

/// some precomputed node (TODO: move to JIT)
extern const Evaluate_t ievaluate[];

/// An haar Feature: a collection of weighted HaarNode
struct HaarFeature : public std::vector<HaarNode> {

  const char *name; // for debug purpose (remember to remove in final version)

public:

  /// type of data requested by this Feature Extractor
  typedef unsigned int InputDataType;
  
  /// type of data returned by this feature extractor
  typedef int DescriptorType;

  enum { InvokeParam = 2 };
  
public:  

 HaarFeature() : name(NULL) { }

 inline const char *debug_name() const { return name; }
 inline void debug_name(const char *str) { name = str; }
 
 /** weak classifier name */
 static std::string signature() { return "haar"; }
  
/// invert the sign of haar nodes (in order to simplify the decision stump)  
void invert_polarity()
{
  for(std::vector<HaarNode>::iterator i=this->begin();i!=this->end();i++)
    i->sign = - i->sign;
}
  
#if 0
 int response(const unsigned int *pIntImage, int stride) const
  {
  for(std::vector<HaarNode> i=m_nodes.begin();i!=m_nodes.end();i++)
    sum += node[i](pIntImage, stride);
  }
#else
 /// 4527.99feat/sec, 4552.72feat/sec, 4722.2feat/sec
 /// Evaluate a single feature starting from pIntImage
 inline int response(const unsigned int *pIntImage, int stride) const
  {
  return ievaluate[size()](pIntImage, stride, &(*this)[0]);
  }
#endif

  inline int operator() (const unsigned int *pIntImage, int stride) const
  {
    return response(pIntImage, stride);
  }

 /// Rescale current feature using sx,sy scale factor
 /// @note due to particular architecture of this filter, only integral scale factor are alowed now
 ///       use a fHaarFeature in order to take advantage of floating point rescaling
 void rescale(int sx, int sy)
 {
  for(std::vector<HaarNode>::iterator i = this->begin(); i!= this->end(); ++i)
         i->x = (i->x * sx + sx - 1), i->y = (i->y * sy + sy - 1);
 }
};


 std::istream & operator >> (std::istream & in, HaarFeature & s);
 std::ostream & operator << (std::ostream & out, const HaarFeature & s);


 
//////////////////////////////////////////////////////////////////////////////////////////////////

 // a precomputed HaarNode in order to boost evaluating performance
// struct OptimizedHaarNode {
//   /// accumulator
//   int sign;
//   /// offset
//   long offset;
//   
//   OptimizedHaarNode(int _sign, long _offset) : sign(_sign), offset(_offset) { }
// 
//   int operator()(const unsigned int *pIntImage) const {
// 	return sign * (int) pIntImage[offset];
// 	}
// };
// 
// /// an optimized HaarFeature
// struct OptimizedHaarFeature: public std::vector<OptimizedHaarNode> {
// 
// public:
// 
// /** compute an optimized version from an HaarFeature */  
// OptimizedHaarFeature(const HaarFeature &src, long offset, long stride)
// {
//  reserve( src.size() );
//  for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
//    this->push_back(OptimizedHaarNode(i->sign, offset +  i->x + i->y * stride) );
//   
// }
//   
// /// invert the sign of haar nodes (in order to simplify the decision stump)  
// void invert_polarity()
// {
//   for(std::vector<OptimizedHaarNode>::iterator i=begin();i!=end();i++)
//     i->sign = - i->sign;
// }
// 
// /// compute the response
// inline int response(const unsigned int *pIntImage) const
// {
//   int sum = 0;
//   for(std::vector<OptimizedHaarNode>::const_iterator i=begin();i!=end();i++)
//     sum += (*i)(pIntImage);
//   return sum;
// }
// 
// };

#endif
