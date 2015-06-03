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

#ifndef _INTEGRAL_IMAGE_PREPROCS_H
#define _INTEGRAL_IMAGE_PREPROCS_H

/** @file IntegralImagePreprocessors.h
  * @brief method to convert an image sample in integral image preprocessors
  **/

#include <Descriptor/IntegralImageData.h>
#include "IntegralImage.h"

#include <string.h>

/// Policy to convert an image in a IntegralImage
/// TODO: datatype using template (?)
/// Input: BW Image | Output: RawData < uint32 >
class IntegralImagePreprocessor {
public:

    /// Data provided by this Preprocessor
    typedef IntegralImageData DataType;

    /// parameters used to store data and use with feature extractor
    typedef IntegralImageParams ParamType;

    /// Data provided by this preprocessor
    typedef std::pair<DataType, ParamType> ReturnType;

public:

    IntegralImagePreprocessor() { }

    IntegralImagePreprocessor(std::istream & in) {

    }

    ///return null string configuration
    void GetConf(char* strconf) const
    {
      if(strconf)
        strconf[0] = 0;
    }

    ///in haar preprocessor nothing to do
    void Configure(char* strconf) const
    {
      // NOTHING
    }
    
    void Configure(std::istream & in) {
      // NOTHING
    }

    /// compute memory required to serialize/deserialize the preprocessed image
    unsigned int DataSize(unsigned int width, unsigned int height) const
    {
        return sizeof(uint32_t) * (width + 1) * (height + 1);
    }

    /// to store on disk or use with MPI
    void Serialize(unsigned char *buf, const IntegralImageData & src, int n) const
    {
        memcpy(buf, src.data, n);
    }

    /// to store on disk or use with MPI
    void DeSerialize(IntegralImageData & dst, const unsigned char *buf, int n) const
    {
        dst.data = new uint32_t[n/4];
        memcpy(dst.data, buf, n);
    }
    
    /// compute the params required to addess a image of width x height pixels
    static void GetParam(IntegralImageParams & params, unsigned int width, unsigned int height)
    {
      params.stride = width+1;//width
      params.width = width;
      params.height = height;
    }

    /// the operator used to convert an image to an integral image
    template<class T>
    static bool Process(std::pair<IntegralImageData, IntegralImageParams> & out, const T *image, unsigned int width, unsigned int height, long stride)
    {
        IntegralImageHandle<uint32_t> integral_image;
        integral_image.Build(image, width, height, stride, true); // NOTE: costruisco una EXTENDEND integral image, il punto (1,1) corrisponde al punto (0,0)

        out.first.data = integral_image.data;
        
        out.second.width = width; // pattern geometry (not integral_image)
        out.second.height = height; // pattern geometry
        out.second.stride = integral_image.width; // stride for integral image (uint32_t)
		return true;
    }
    
    /// the operator used to convert an integral image to the data
    static bool Process(std::pair<IntegralImageData, IntegralImageParams> & out, const IntegralImageHandle<uint32_t> & src)
    {
        out.first.data = src.data;
        
        out.second.width = src.width-1; // pattern geometry (not integral_image)
        out.second.height = src.height-1; // pattern geometry
        out.second.stride = src.width; // stride for integral image (uint32_t)
		return true;
    }    

};


#endif
