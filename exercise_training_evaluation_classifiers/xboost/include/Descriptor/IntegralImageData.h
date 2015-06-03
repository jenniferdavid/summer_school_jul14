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

#ifndef _INTEGRAL_IMAGE_DATA_H
#define _INTEGRAL_IMAGE_DATA_H

#include <_stdint.h>

/** @file IntegralImageData.h
  * @brief a descritor based on a integral image
  **/

/// parameters required to access to integral image and to create features
struct IntegralImageParams {
  long stride; /// probably an offset is better than stride
  unsigned int width, height;


    /// training pattern geometry: width
    inline unsigned int Width() const  {
        return width;
    }
    /// training pattern geometry: height
    inline unsigned int Height() const {
        return height;
    }
    
  /// to check if parameters are related
  bool operator ==(const IntegralImageParams & b) const {
    return (b.width == width) && (b.height == height);
  }
};

/// il dato IntegralImage e' pressoche' equivalente a quello di RawData < uint32_t >
struct IntegralImageData {

public:
  /// the parameters associated to this data
  typedef IntegralImageParams ParamType;

public:
  
  uint32_t *data;

public:  
  /// release operator
  void release() { delete [] data; data = 0; }
};

// l'accoppiata Data+Params permette di operare sull'immagine


#endif
