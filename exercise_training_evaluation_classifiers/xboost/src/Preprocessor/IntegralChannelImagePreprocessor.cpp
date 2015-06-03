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

#include "Preprocessor/IntegralChannelImagePreprocessor.h"

IntegralChannelImagePreprocessor::IntegralChannelImagePreprocessor(): m_useOrientationSign(false), m_nOrientationBin(8), m_magnitudeChannel(true), m_luminanceChannel(true),m_derivativeFilter(0)
#ifdef ICF_GENERATE_ATAN2_TABLE
    , table_atan2_module(0)
#endif
{
    nChannels = 10;
}

/// Read the configuration string from @a in stream
IntegralChannelImagePreprocessor::IntegralChannelImagePreprocessor(std::istream & in) :m_useOrientationSign(false), m_nOrientationBin(8), m_magnitudeChannel(true), m_luminanceChannel(true),m_derivativeFilter(0)
#ifdef ICF_GENERATE_ATAN2_TABLE
    , table_atan2_module(0)
#endif
{
    std::string conf;
    in >> conf;

    Configure(conf.c_str());
}

/// initialize using configuration string
IntegralChannelImagePreprocessor::IntegralChannelImagePreprocessor(const char *conf) : m_useOrientationSign(false), m_nOrientationBin(8), m_magnitudeChannel(true), m_luminanceChannel(true),m_derivativeFilter(0)
#ifdef ICF_GENERATE_ATAN2_TABLE
    , table_atan2_module(0)
#endif
{
    Configure(conf);
}

IntegralChannelImagePreprocessor::~IntegralChannelImagePreprocessor()
{
  #ifdef ICF_GENERATE_ATAN2_TABLE
    delete [] table_atan2_module;
  #endif    
}

void IntegralChannelImagePreprocessor::Configure(bool orientationSign, int nOrientationBin, bool magnitudeChannel, bool luminanceChannel,int derivativeFilter)
{
    m_useOrientationSign = orientationSign;
    m_nOrientationBin = nOrientationBin;
    m_magnitudeChannel = magnitudeChannel;
    m_luminanceChannel = luminanceChannel;
    m_derivativeFilter = derivativeFilter;

    nChannels = m_nOrientationBin + ((m_magnitudeChannel) ? 1 : 0) + ((m_luminanceChannel) ? 1 : 0);

#ifdef ICF_GENERATE_ATAN2_TABLE
    // TODO: use a cache!
    delete [] table_atan2_module;
    table_atan2_module = new unsigned char[512*512*2];

    float scale = 1.0f / std::sqrt(2.0f);
    for(int i=0; i<512; ++i)
        for(int j=0; j<512; ++j)
        {
            int dx = i - 255;
            int dy = j - 255;
            float p = std::atan2((float)dy,(float)dx); // -PI +PI
            unsigned char m = std::sqrt(dx*dx + dy*dy) * scale;
            //unsigned char bin = (int) ( ( p + M_PI) * (float) (8.0f / (2.0f*M_PI) ) );
            //bin = bin & 0x0F;
            int bin = (m_useOrientationSign) ? ( (int) ( ( p + M_PI) * (double) m_nOrientationBin / (2.0f*M_PI) ) ) : ( (int) ( (M_PI+p)  * (float) m_nOrientationBin / (M_PI) ) );
            bin = (bin) % m_nOrientationBin;

            table_atan2_module[(i + j * 512)*2+0] = bin;
            table_atan2_module[(i + j * 512)*2+1] = m;
        }
#endif
}

void IntegralChannelImagePreprocessor::Configure(const char* str)
{
    m_nOrientationBin = str[0] & 0x0F;
    if(strpbrk(str,"s"))
        m_useOrientationSign = true;
    else
        m_useOrientationSign = false;
    if(strpbrk(str,"m"))
        m_magnitudeChannel = true;
    else
        m_magnitudeChannel = false;
    if(strpbrk(str,"l"))
        m_luminanceChannel = true;
    else
        m_luminanceChannel = false;
    /*
    if(strpbrk(str,"di"))
            m_derivativeFilter = 0;
    else
            if(strpbrk(str,"so"))
                    m_derivativeFilter = 1;
            */
    m_derivativeFilter = 0; //TODO sobel

    nChannels = m_nOrientationBin + ((m_magnitudeChannel) ? 1 : 0) + ((m_luminanceChannel) ? 1 : 0);

#ifdef ICF_GENERATE_ATAN2_TABLE
    delete [] table_atan2_module;
    table_atan2_module = new unsigned char[512*512*2];

    float scale = 1.0f / std::sqrt(2.0f);
    for(int i=0; i<512; ++i)
        for(int j=0; j<512; ++j)
        {
            int dx = i - 255;
            int dy = j - 255;
            float p = std::atan2((float)dy,(float)dx); // -PI +PI
            unsigned char m = std::sqrt(dx*dx + dy*dy) * scale;
            //unsigned char bin = (int) ( ( p + M_PI) * (float) (8.0f / (2.0f*M_PI) ) );
            //bin = bin & 0x0F;
            int bin = (m_useOrientationSign) ? ( (int) ( ( p + M_PI) * (double) m_nOrientationBin / (2.0f*M_PI) ) ) : ( (int) ( (M_PI+p)  * (float) m_nOrientationBin / (M_PI) ) );
            bin = (bin) % m_nOrientationBin;

            table_atan2_module[(i + j * 512)*2+0] = bin;
            table_atan2_module[(i + j * 512)*2+1] = m;
        }
#endif
}

void IntegralChannelImagePreprocessor::Configure(std::istream & in)
{
  std::string param;
  in >> param;
  Configure(param.c_str());
}