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

#ifndef _RANDOM_BIT_SET_H
#define _RANDOM_BIT_SET_H

/** @file bitset.h
 * @brief some usefull bit operator function
 * */

#include "random.h"
#include <vector>

/// an exclusive random generator
class random_bit_set {
  std::vector<bool> m_set;
  int m_tick;
public:
  /// define the range 0..n-1
  random_bit_set(int n) : m_set(n), m_tick(0) { clear(); } 
  
  /// reset generator
  void clear()
  {
   for(std::vector<bool>::iterator i = m_set.begin(); i != m_set.end(); ++i)
     *i = false;
  }
  
  /// extract one more number between 0..n-1
  int operator()(void) {
    int n;
    do {
      n = nrand( m_set.size() );
    } while( m_set[n] );
    
    m_tick++;
    m_set[n] = true;
    
    return n;
    
  }
};


#endif

