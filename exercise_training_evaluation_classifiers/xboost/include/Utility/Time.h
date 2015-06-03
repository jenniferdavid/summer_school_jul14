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

#ifndef _TIME_UTILITY_H
#define _TIME_UTILITY_H

#include <iostream>

class human_readable_time {
  int s;
public:
  human_readable_time(int _s) : s(_s) { }
  
  int getTime() const { return s; }
};

std::ostream & operator << (std::ostream & o, const human_readable_time & t)
{
  if(t.getTime() < 60)
    o << t.getTime() << " secs";
  else
  if(t.getTime() < 3600)
    o << t.getTime()/60 << " mins and " << t.getTime()%60 << " secs";
  else  
    o << t.getTime()/3600 << " hours and " << (t.getTime()/60)%60 << " mins";
   
  return o;
}

#endif