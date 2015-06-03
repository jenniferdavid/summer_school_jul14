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

#include "Feature/HaarFeature.h"

std::istream & operator >> (std::istream & in, HaarNode & d)
  {
   in >> d.sign >> d.x >> d.y;
  return in;
  }

std::ostream & operator << (std::ostream & out, const HaarNode & s)
  {
  out << s.sign << ' ' << s.x << ' ' << s.y;
  return out;
  }

std::istream & operator >> (std::istream & in, HaarFeature & s)
 {
 int size = 0;
 in >> size;
//  std::cout << size << " elements feature\n";
 s.resize(size);
 for(int i = 0; i < size; ++i)
  in >> s[i];
 return in;
 }

 std::ostream & operator << (std::ostream & out, const HaarFeature & s)
 {
 out << s.size() << ' ';
 for(std::vector<HaarNode>::const_iterator i = s.begin(); i!= s.end(); ++i)
  out << *i << ' ';
 return out;
 }


// An optimized evaluatori to unroll-loop
template<int n>
static int _ievaluate(const unsigned int *pIntImage, int stride, const HaarNode *node)
  {
  int sum = 0;
  for(int i=0;i<n;i++)
    sum +=(int)(pIntImage[node[i].x + node[i].y * stride]) * node[i].sign;
  return sum;
  }

// look up table for unrolled loop callback
const Evaluate_t ievaluate[] = 
    {_ievaluate<0>,
     _ievaluate<1>,
     _ievaluate<2>,
     _ievaluate<3>,
     _ievaluate<4>,
     _ievaluate<5>,
     _ievaluate<6>,
     _ievaluate<7>,
     _ievaluate<8>,
     _ievaluate<9>,
     _ievaluate<10>,
     _ievaluate<11>,
     _ievaluate<12>,
     _ievaluate<13>,
     _ievaluate<14>,
     _ievaluate<15>,
     _ievaluate<16>
    };
