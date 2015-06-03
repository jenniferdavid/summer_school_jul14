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

#ifndef _HYPERPLANE_CLASSIFIER_H
#define _HYPERPLANE_CLASSIFIER_H

#include "Descriptor/RawData.h"

template<class ScalarType>
class HyperPlaneClassifier {
public:
  int n;
  ScalarType *w;
  ScalarType bias;

public:
  
  HyperPlaneClassifier() : w(0) { }
  ~HyperPlaneClassifier() { delete [] w; }
  
  static const char *signature() { return "linear-hog"; }
  
  // w should be n+1 element long
  template<class SrcScalarType>
  void import(int _n, const SrcScalarType *_w)
  {
    delete [] w;
    n = _n;
    w = new ScalarType[n];
    for(int i =0;i<n;++i)
      w[i] =_w[i];
    bias = _w[n];
  }
  
  template<class T>
  ScalarType response(const RawData<T> & src) const 
  {
    ScalarType r = bias;
    for(int i =0;i<n; i++)
      r += src.data[i] * w[i];
    
    return r;
  }
  
};

template<class DataType>
inline std::istream & operator >> (std::istream & in, HyperPlaneClassifier<DataType> & s)
{
    in >> s.n;
    s.w = new DataType[s.n];
    for(int i=0;i<s.n;++i)
    {
      in >> s.w[i];
//       std::cerr << s.w[i] << ' ';
    }
    in >> s.bias;
    
//     std::cerr << std::endl;
    
    return in;
}

template<class DataType>
inline std::ostream & operator << (std::ostream & out, const HyperPlaneClassifier<DataType> & s)
{
    out << s.n;
    for(int i=0;i<s.n;++i)
      out << ' ' << s.w[i];
    out << ' ' << s.bias;
    return out;
}


#endif
