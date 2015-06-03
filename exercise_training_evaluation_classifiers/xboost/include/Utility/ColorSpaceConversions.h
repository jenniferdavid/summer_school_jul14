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

#ifndef _COLOR_SPACE_CONVERSIONS_H
#define _COLOR_SPACE_CONVERSIONS_H

#include "Image.h"

/// convert an RGB image to grey
bool rgb_to_grey(const ImageHandle & in, ImageHandle & out);
/// convert an RGB image to YUV
bool rgb_to_yuv(const ImageHandle & in, ImageHandle & out);
/// convert an RGB image to LUV
bool rgb_to_luv(const ImageHandle & in, ImageHandle & out);
/// convert an RGB image to YCbCr
bool rgb_to_YCbCr(const ImageHandle & in, ImageHandle & out);

/// extract a channel from an image
bool extract_channel(const ImageHandle & in, ImageHandle & out, int nchannel);

#endif
