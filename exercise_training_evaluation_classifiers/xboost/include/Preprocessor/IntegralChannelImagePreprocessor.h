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

#ifndef _INTEGRAL_CHANNEL_IMAGE_PREPROCS_H
#define _INTEGRAL_CHANNEL_IMAGE_PREPROCS_H

/** @file IntegralChannelImagePreprocessor.h
  * @brief method to convert an image sample in integral Channels Image
  **/

#include <Descriptor/IntegralImageData.h>
#include "IntegralImage.h"
#include <string.h>
// #include <IO/pnmio.h>
// #include <stdio.h>


/// parameters required to address a channel params
struct IntegralChannelImageParams {
    long stride1, stride2;
    unsigned int width, height, nchannels;

    inline unsigned int Width() const {
        return width;
    }
    inline unsigned int Height() const {
        return height;
    }
    inline unsigned int NChannels() const {
        return nchannels;
    }



    /// to check if parameters are related
    bool operator ==(const IntegralChannelImageParams & b) const {
        return (b.width == width) && (b.height == height) && (b.nchannels == nchannels);
    }


};

std::ostream& operator<< (std::ostream& out, const IntegralChannelImageParams& param);
std::istream& operator>> (std::istream& in,  IntegralChannelImageParams& feat);
/*

*/

#define ICF_GENERATE_ATAN2_TABLE

/// Policy to convert an image in a IntegralChannelImage
/// Input BW/RGB Image, Output: Tensor Data
class IntegralChannelImagePreprocessor {
private:

    bool m_useOrientationSign;
    unsigned int m_nOrientationBin;
    bool m_magnitudeChannel;
    bool m_luminanceChannel;
    int m_derivativeFilter; // 0: derivative, 1: sobel 3x3
    int nChannels;

    #ifdef ICF_GENERATE_ATAN2_TABLE
    unsigned char *table_atan2_module;
    #endif

    
    
public:


    /// Data provided by this Preprocessor
    typedef IntegralImageData DataType;

    /// Common data provided by this preprocessor
    typedef IntegralChannelImageParams ParamType;

    /// Data provided by this preprocessor
    typedef std::pair<DataType, ParamType> ReturnType;

public:

    IntegralChannelImagePreprocessor();

    /// Read the configuration string from @a in stream
    IntegralChannelImagePreprocessor(std::istream & in);

    /// initialize using configuration string
    IntegralChannelImagePreprocessor(const char *conf);
    
    ~IntegralChannelImagePreprocessor();
    
    void Configure(bool orientationSign, int nOrientationBin, bool magnitudeChannel, bool luminanceChannel,int derivativeFilter);

    /// Configure using a string or a stream
    void Configure(const char* str);
    void Configure(std::istream & in);

    void GetConf(char* strconf) const
    {
    	char strPconf[10];
        strPconf[0] = m_nOrientationBin + 0x30;
        int i=1;
        if(m_useOrientationSign) strPconf[i++]='s';
        if(m_magnitudeChannel) strPconf[i++]='m';
        if(m_luminanceChannel) strPconf[i++]='l';
        strPconf[i++]=0;
    	strcpy(strconf,strPconf);
    }

    void GetParam(ParamType & params, unsigned int width, unsigned int height) const
    {
        params.width = width;
        params.height = height;
        params.nchannels = nChannels;
        params.stride1 = width+1;
        params.stride2 = (width+1)*(height);
    }

    /// compute memory required to serialize/deserialize the preprocessed image
    unsigned int DataSize(unsigned int width, unsigned int height) const
    {
        return sizeof(uint32_t) * (width + 1) * (height * nChannels + 1);
    }

    void Serialize(unsigned char *buf, const IntegralImageData & src, int n) const
    {
        memcpy(buf, src.data, n);
    }

    void DeSerialize(IntegralImageData & dst, const unsigned char *buf, int n) const
    {
        dst.data = new uint32_t[n/4];
        memcpy(dst.data, buf, n);
    }

    template<class T, bool magnitude, bool luminance>
    bool core(unsigned char *buffer, const T *image, unsigned int width, unsigned int height, unsigned int j0, unsigned int j1, long stride) const
    {
        unsigned int channel_size = width * height;
        const int magnitude_offset = m_nOrientationBin * channel_size;
        const int luminance_offset = (m_nOrientationBin + (magnitude ? 1 : 0) ) * channel_size;
        
#ifndef ICF_GENERATE_ATAN2_TABLE
          static const float scale = 1.0f / std::sqrt(2.0f);
#endif
        image  += j0 * stride;
        buffer += j0 * width;
        
        // 1..N Orientation
        for(int j=j0; j<j1; ++j)
        {
            for(int i=1; i<width-1; ++i)
            {
                int dx,dy;

                // DERIVATIVE FILTER:
                dx = (int)image[i + 1] - (int)image[i - 1];
                dy = (int)image[i + stride] - (int)image[i - stride];

#ifndef ICF_GENERATE_ATAN2_TABLE
                unsigned char m = std::sqrt(dx*dx + dy*dy) *scale;
                float p = std::atan2( (float) dy, (float)  dx);
                int bin = (m_useOrientationSign) ? ( (int) ( ( p + M_PI) * (double) m_nOrientationBin / (2.0f*M_PI) ) ) : ( (int) ( (M_PI+p)  * (float) m_nOrientationBin / (M_PI) ) );
                bin = (bin) % m_nOrientationBin;
#else
                unsigned char bin = table_atan2_module[((dx + 255) + (dy + 255) * 512)*2+0];
                unsigned char m = table_atan2_module[((dx + 255) + (dy + 255) * 512)*2+1];
#endif

                buffer[bin * channel_size + i] = m;

                if(magnitude)
                {
                    buffer[i + magnitude_offset] = m;
                }
                if(luminance)
                {
                    buffer[i + luminance_offset] = image[i];
                }

            }
            
            image += stride;
            buffer += width;
        }
		return true;
    }
    
    /// compute the ICH
    template<class T>
    bool Process(std::pair<IntegralImageData, IntegralChannelImageParams> & out, const T *image, unsigned int width, unsigned int height, long stride) const
    {
        unsigned char *buffer = new unsigned char [width * height * nChannels];

        out.second.width = width;
        out.second.height = height;
        out.second.stride1 = width+1;
        out.second.stride2 = (width+1)*(height);
        out.second.nchannels = nChannels;

        // TODO:
        memset(buffer,0,width*height*nChannels);
        
        // avoid using of if()
        if(!m_magnitudeChannel && !m_luminanceChannel)
          core<T, false, false>(buffer, image, width, height, 1, height-1, stride);
        else
        if(m_magnitudeChannel && !m_luminanceChannel)
          core<T, true, false>(buffer, image, width, height, 1, height-1, stride);
        else
        if(!m_magnitudeChannel && m_luminanceChannel)
          core<T, false, true>(buffer, image, width, height, 1, height-1, stride);
        else
        if(m_magnitudeChannel && m_luminanceChannel)
          core<T, true, true>(buffer, image, width, height, 1, height-1, stride);

        IntegralImageHandle<uint32_t> integral_image;
        integral_image.Build(buffer, width, height * nChannels, width, true);
        delete [] buffer;

        out.first.data = integral_image.data;
		return true;
    }
};

#endif
