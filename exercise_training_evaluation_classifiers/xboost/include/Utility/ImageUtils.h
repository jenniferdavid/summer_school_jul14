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

#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H

#include "Image.h"
#include "Types.h"
#include "IntegralImage.h"

/** @file ImageUtils.h
  * @brief method to process image
  **/


/// a simple box filter for image resampling
/// @bug first row/column bug, please fix it!
class ImageResampler {
  IntegralImage<uint32_t> img;
public:
  ImageResampler();
  ~ImageResampler();
  
  void ImportImage(const ImageHandle & in);
  void ExportImage(ImageHandle & out, const rect & area);
  void ExportImage(ImageHandle & out);
};

/// convert color to greyscale and viceversa
void Convert(ImageHandle & out, const ImageHandle & in);

/// Crop and Resample an Image using IntegralImage algorithm
void Resample(ImageHandle & out, const ImageHandle & in, const rect & area);

/// Crop and Resample an Image using Bilinear Interpolation algorithm
void BilinearResample(ImageHandle & out, const ImageHandle & in, const rect & area);

/// Draw a rectangle over an image
void DrawRect(ImageHandle & out, rect r, unsigned int color);

/// Draw a Box over an image
void DrawBox(ImageHandle & out, rect r, unsigned int color);

/// Draw a Transparent Box over an image
void BlendBox(ImageHandle & out, rect r, unsigned int color, float factor);

/// Downsample (a factor of 2) the whole image
void Downsample2X(ImageHandle & out, const ImageHandle & in);

/// Downsample a factor of 1.5 the whole image
void Downsample1_5X(ImageHandle & out, const ImageHandle & in);

/// Crop and Downsample
void Downsample2X(ImageHandle & out, const ImageHandle & in, const rect & area);

#endif