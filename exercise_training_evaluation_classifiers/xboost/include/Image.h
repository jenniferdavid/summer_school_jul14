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

#ifndef _IMAGE_H
#define _IMAGE_H


#include "Types.h"
#include <algorithm> // swap
#include <string.h>

/** @file Image.h
  * @brief method to hold an image
  **/

/// a structure to hold image data, without release
///  it is a simply container for image geometry, delegating memory management to others classes
struct ImageHandle {
  /// byte per pixel (1 for grayscale image, 2 for 16bit greyscale image, 3 for RGB/YUV/etc color image)
  unsigned int bpp;
  /// image geometry
  unsigned int width, height;
  /// line stride, the delta offset, in bytes, between two different scanline
  long stride;
  /// initial address of the first pixel. It must be cast to correct format (uint8, uint16, rgb, etc etc)
  unsigned char *data;

public:
  /// return a subpart of the image (without copy)
  ImageHandle crop(int x0, int y0, int x1, int y1) const {
    ImageHandle out;

    out.bpp = bpp;
    out.stride = stride;
    out.width = x1 - x0;
    out.height = y1 - y0;
    out.data = data + x0 * bpp + y0 * stride;

    return out;
  }

  /// return a sub part of the image (without copy)
  ImageHandle crop(const rect & roi) const {
    ImageHandle out;

    out.bpp = bpp;
    out.stride = stride;
    out.width = roi.x1 - roi.x0;
    out.height = roi.y1 - roi.y0;
    out.data = data + roi.x0 * bpp + roi.y0 * stride;

    return out;
  }
};

/// a structure to hold image data (memory)
struct Image: public ImageHandle {

private:

  // uncopiable
  Image(const Image & src) { }

public:

  Image(int w, int h, int b) { data=0; alloc(w,h,b); }
  Image() { data = 0 ; }
  ~Image() { delete [] data; }

  /// method to delegate the destroy of the data to a third player.
  unsigned char *delegate() {
    unsigned char *ptr = data;
    data  =0;
    return ptr;
  }

  /// release manully the memory
  void release() { delete [] data; data = 0; }

  /// reserve memory for the image
  void alloc(unsigned int w, unsigned int h, unsigned int b) { release(); width = w; height = h; bpp = b; stride = width * bpp; data = new unsigned char [width * height * bpp]; }

  /// allocate this image with the same geometry with a source image
  void alloc_as(const ImageHandle & src) { alloc(src.width, src.height, src.bpp); }
  
  /// clone an image 
  void clone(const ImageHandle & src)
  {
    alloc_as(src);
    for(int j=0;j<src.height;++j)
      memcpy(data + j * stride, src.data + j * src.stride, src.width * src.bpp);
  }
  
};

namespace std {

/// swap two images without perform a copy of data
inline void swap(::Image &a, ::Image &b)
{
  std::swap(a.bpp, b.bpp);
  std::swap(a.width, b.width);
  std::swap(a.height, b.height);
  std::swap(a.stride, b.stride);
  std::swap(a.data, b.data);
}
}

#endif
