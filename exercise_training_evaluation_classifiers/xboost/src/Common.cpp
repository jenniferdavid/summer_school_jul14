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

#include "Common.h"
#include <stdint.h>
#include <fstream>
#include "HaarFeatureTools.h"
#include "IO/pnmio.h"
#include "Preprocessor/IntegralImagePreprocessors.h"

bool load_image(IntegralImageData & data, IntegralImageParams & params, const char *filename, bool normalize)
{
    Image img;

    if(pnm_load(filename, img))
    {
        IntegralImagePreprocessor filter;
        std::pair<IntegralImageData, IntegralImageParams> out;

        if(img.bpp == 1)
            filter.Process(out, img.data, img.width, img.height, img.stride);
        else if(img.bpp == 2)
            filter.Process(out, (unsigned short*)img.data, img.width, img.height, img.stride);
        else
            std::cerr << filename << ": invalid bpp" << std::endl;

        data = out.first;
        params = out.second;

        return true;
    }
    else
    {
        return false;
    }
}

/// load all images
void load_images(IntegralImagesType & out, const char *data_set, unsigned int &m_width, unsigned int &m_height)
{
    char buf[512];
    std::ifstream a(data_set);
    IntegralImagePreprocessor filter;

    if(!a)
    {
        std::cerr << data_set << " is not valid/readable" << std::endl;
        return;
    }

    while(a.getline(buf, 512))
    {
        Image img;

        if(pnm_load(buf, img))
        {
            std::pair<IntegralImageData, IntegralImageParams> r;

            if(img.bpp == 1)
                filter.Process(r, img.data, img.width, img.height, img.stride);
            else if(img.bpp == 2)
                filter.Process(r, (unsigned short*)img.data, img.width, img.height, img.stride);
            else
                std::cerr << buf << ": invalid bpp" << std::endl;

            if(out.empty())
            {
                // first pattern, initialize variables
                m_width = img.width;
                m_height = img.height;
            }
            else if ((img.width != m_width)||(img.height != m_height))
            {
                std::cerr << "Wrong Image size: " << img.width << 'x' << img.height << ": expected " << m_width << 'x' << m_height << std::endl;
            }

            out.push_back(std::pair<std::string, unsigned int *>(buf,r.first.data));
        }
        else
            std::cerr << "Failed to load " << buf << std::endl;
    }
}


/// load all images
void load_images2(IntegralImagesType & out, const char *data_set, unsigned int &m_width, unsigned int &m_height)
{
    char buf[512];
    std::ifstream a(data_set);
    IntegralImagePreprocessor filter;

    if(!a)
    {
        std::cerr << data_set << " is not valid/readable" << std::endl;
        return;
    }

    while(a.getline(buf, 512))
    {
        Image img;

        if(pnm_load(buf, img))
        {
            std::pair<IntegralImageData, IntegralImageParams> r;

            if(img.bpp == 1)
                filter.Process(r, img.data, img.width, img.height, img.stride);
            else if(img.bpp == 2)
                filter.Process(r, (unsigned short*)img.data, img.width, img.height, img.stride);
            else
                std::cerr << buf << ": invalid bpp" << std::endl;

            if(out.empty())
            {
                // first pattern, initialize variables
                m_width = img.width;
                m_height = img.height;
            }
            else if ((img.width != m_width)||(img.height != m_height))
            {
                std::cerr << "Wrong Image size: " << img.width << 'x' << img.height << ": expected " << m_width << 'x' << m_height << std::endl;
            }

            out.push_back(std::pair<std::string, unsigned int *>(buf,r.first.data));
        }
        else
            std::cerr << "Failed to load " << buf << std::endl;
    }
}


void release_images(IntegralImagesType & out)
{
    for(IntegralImagesType::iterator i = out.begin(); i != out.end(); ++i)
        delete [] i->second;
    out.clear();
}

void add_offset(IntegralImagesType & out, const IntegralImagesType & set, long offset)
{
    out.clear();
    for(IntegralImagesType::const_iterator i = set.begin(); i != set.end(); ++i)
    {
        out.push_back(std::pair<std::string,unsigned int *>(i->first, i->second + offset) ) ;
    }
}

// void compute_responses(const Classifier & c, std::vector<double> & response, const IntegralImagesType & set, long stride, bool additional_border)
// {
//     response.clear();
//     response.reserve(set.size());
// 
// 
//     for(IntegralImagesType::const_iterator i = set.begin(); i != set.end(); ++i)
//     {
//         double v =
//             // v > 0 (+); v < 0 (-)
//             (additional_border) ?
//             // (*i) is memory pointer at -1,-1 in un immagine m_width+1
//             c(i->second+stride+1,stride) :
//             c(i->second,stride);
// 
//         response.push_back(v);
//     }
// }
