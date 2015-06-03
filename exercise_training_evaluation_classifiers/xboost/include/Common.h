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

#ifndef _COMMON_TOOLS_H
#define _COMMON_TOOLS_H

/** @file Common.h
 *  @brief some common function to deal with images and training set */

#include "IntegralImage.h"
#include <vector>
#include <string>
// #include "factory.h"
#include <Descriptor/IntegralImageData.h>

/// vector of test images
typedef std::vector< std::pair<std::string,unsigned int *> > IntegralImagesType;

/// load a training set using a data_set file. fill the vector with (extended) integral image and return width amd height of images
void load_images(IntegralImagesType & out, const char *data_set, unsigned int &width, unsigned int &height);

/// load a training set using a data_set file. fill the vector with (extended) integral image and return width amd height of images
void load_images2(IntegralImagesType & out, const char *data_set, unsigned int &width, unsigned int &height);


/// Load a single image
/// @return true if file exist
bool load_image(IntegralImageData & img, IntegralImageParams & params, const char *filename, bool normalize = false);

/** create a new integral image list with a precomputed offset */
void add_offset(IntegralImagesType & out, const IntegralImagesType & src, long offset);

/** batch compute response of a Classifier on all pattern */
// void compute_responses(const Classifier & c, std::vector<double> & response, const IntegralImagesType & set, long stride, bool extra_border);

/// release correctly an integral image (manually... or use shared_ptr/auto_ptr)
void release_images(IntegralImagesType & out);


#endif
