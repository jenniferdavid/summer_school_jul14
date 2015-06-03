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

#ifndef _XBOOST_TYPES_H
#define _XBOOST_TYPES_H


/** @file Types.h
 *  @brief Types involved in boosting */

///
enum ClassifierType {
    DicreteClassifier,	///< a dicrete classifier returns {-1,+1}
    DiscreteClassifierWithAbstention, ///< a dicrete classifier with abstention returns {-1,0,+1}
    RealClassifier,	///< a real classifier return a number between -1,+1
    FeatureExtractor	///< a FeatureExtractor return a scalar number without relationship with classification
};

/// image/size TODO namespace
struct size {
    unsigned int width, height;

    size() : width(0), height(0) { }
    size(unsigned int _w, unsigned int _h) : width(_w), height(_h) { }
    
    inline bool operator == (const size & s) const 
    {
      return width == s.width && height == s.height;
    }
};

/// normally referred as template_geometry
typedef size template_geometry;

/// a rectangle structure
struct rect {
    int x0,y0,x1,y1;

    rect() {}
    rect(int _x0,int _y0,int _x1, int _y1) : x0(_x0), y0(_y0), x1(_x1), y1(_y1) {}
    
    inline int width() const { return x1 - x0; }
    inline int height() const { return y1 - y0; } 
    inline size extension() const { return size(x1 - x0, y1 - y0); }
    
    inline bool operator == (const rect & r) const 
    {
      return x0 == r.x0 && y0 == r.y0 && x1 == r.x1 && y1 == r.y1;
    }


};

/// test if 2 rect are overlapped
bool overlap(const rect &a, const rect & b);

enum OverlapCriterion {
  Overlap_On_Union,
  Overlap_On_First,
  Overlap_On_Second,
  Overlap_On_Min,
  Overlap_On_Max
};

/// get overlapped area
double boxoverlap(rect a, rect b, OverlapCriterion criterion = Overlap_On_Union);

#endif
