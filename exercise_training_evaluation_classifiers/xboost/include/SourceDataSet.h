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

#ifndef _SOURCE_DATA_SET_H
#define _SOURCE_DATA_SET_H


#include "IO/pnmio.h"
#include "IO/datasetin.h"
#include "Image.h"
#include "DataSet.h"
#include "Utility/ImageUtils.h"

#include <stdio.h> // sprintf
#include <stdlib.h> // rand

/** @file SourceDataSet.h
 * @brief declare the SourceDataSet template class */

/** Data Set used to load effectively data.
 *  Il SourceDataSet contains the effective data storage (DataSet<PatternType>) and the Preprocessor used to convert a
 *  sample imagine in Data.
 *
 * @param PreprocessorType a "preprocessor" used to convert an image in the DataType
 * @param PatternType must import a DataType from Preprocessor
 *
 * */
template<class TPreprocessorType, class Aggregator>
class SourceDataSet: public TPreprocessorType, public DataSet<Aggregator> {
public:

    /// the dataset (contains DataType plus additional data)
    typedef DataSetHandle<Aggregator> DataSetType;

    DECLARE_AGGREGATOR

private:
    /// data set parameters are valid?
    bool m_initialized;

    /// export dataset
    bool m_export_data_set;

    /// path where store the imported dataset
    std::string m_export_path;

public:

    SourceDataSet() : m_initialized(false), m_export_data_set(false) { }

    /// Import an Image and transform it using "Preprocessor"
    bool ImportImage(const ImageHandle & image, int category);

    /// Set the folder where store pattern generated in the dataset
    void SetExportFolder(const std::string & folder) {
        m_export_data_set = true;
        m_export_path = folder;
    }
};

//////////////////////////////////////////////////////////////////////////////

extern int frameidx;

// Import a GrayScale Image
template<class TPreprocessorType, class Aggregator>
bool SourceDataSet<TPreprocessorType, Aggregator>::ImportImage(const ImageHandle & image, int category)
{
    typename TPreprocessorType::ReturnType data; // processed data (pair of <DATA, PARAMS> )

    if(m_export_data_set)
    {
        char buffer[256];
        int count = this->templates.size();
        if(category == 1)
            sprintf(buffer, "%s/P%06u_%u.pgm", m_export_path.c_str(), count, frameidx);
        else
            sprintf(buffer, "%s/N%06u_%u.pgm", m_export_path.c_str(), count, frameidx);
        pnm_write(image, buffer);
    }

    // il proceprocessor converte da image a ReturnType (un pair composto dal dato e dalla geometria)
    if(image.bpp == 1)
    {
        TPreprocessorType::Process(data, (unsigned char *) image.data, image.width, image.height, image.stride );
    }
    else if(image.bpp == 2)
    {
        TPreprocessorType::Process(data, (unsigned short *) image.data, image.width, image.height, image.stride );
    }
    else
    {
        std::cerr << "Invalid bpp = " << image.bpp << " for this data set" << std::endl;
        return false;
    }

    if(!m_initialized)
    {
        // set the parameters on DataSet
        this->SetParams(data.second);
        m_initialized = true;
    }
    else
    {
        // verificare che la geometria sia corretta
        if(!( static_cast<const ParamType &>(*this) == data.second))
        {
            std::cerr << "invalid geometry" << std::endl;
            return false;
        }
    }

    // import the data inside the DataSet
    this->Import(data.first, category);
    return true;
}

/// process a single octave/scale and extract @a nRand not colliding object of size @a set_geometry
/// The object are directly imported in the @a set
template<class DataSetType>
bool RandomExtract(DataSetType & set, const ImageHandle & img, const size & set_geometry, int nRand, const std::vector<datasetobject> & object)
{
    if(img.width < set_geometry.width || img.height < set_geometry.height)
    {
        std::cerr << "the image is too small to be used during negative random extraction" << std::endl;
        return false;
    }

    // for any candidate in the set
    for(int k=0; k<nRand; ++k)
    {
        int x0 = rand() % (img.width - set_geometry.width);
        int y0 = rand() % (img.height - set_geometry.height);
        rect roi(x0, y0, x0 + set_geometry.width, y0 + set_geometry.height);

        bool collision = false;
        // Check Collision
        for(int i=0; i<object.size(); ++i)
            if(overlap(roi, object[i].roi))
                collision = true;

        if(!collision)
        {
            if(!set.ImportImage(img.crop(roi), -1)) // only negative
            {
                std::cerr << "Failed to import pattern";
                return false;
            }
        }
    }
    return true;
}

/// some parameters used in LoadSet
// struct LoadSetParams {
//   bool sz_valid;
//   size sz;
//   int nRand;
//   float scale_factor;
// };

/// Load a Set
/// TODO: move in an another file
/// TODO: potrebbe essere utile caricare i dati delle immagini coinvolte (width height) e salvare se e' un potenziale immagine di test per selezionare le immagini comunque utili poi per la fase di bootstrap dei negativi
template<class DataSetType>
bool LoadSet(DataSetType & set, datasetin & in, const size & sz, int nRand)
{
    datasetitem item;
    size set_geometry = sz; //

    if(set_geometry.width != 0 || set_geometry.height!=0)
        std::cout << "LoadSet with geometry " << set_geometry.width << 'x' << set_geometry.height << std::endl;

    // iterate on input data
    while(in.next(item))
    {
        bool have_negative = false;
        Image img;
        if(!pnm_load(item.filename.c_str(), img))
        {
            std::cout << "Warn: \'" << item.filename << "' failed to load: skip." << std::endl;
        }
        else
        {
            // for any candidate in the set (positive/negative)
            for(int i=0; i<item.object.size(); ++i)
                if(item.object[i].category != 0) // DO NOT CARE
                {
                    rect roi = item.object[i].roi;

                    // whole image ROI
                    if(roi.x0 == 0 && roi.y0 == 0 && roi.x1 == 0 && roi.y1 == 0)
                    {
                        roi.x1 = img.width;
                        roi.y1 = img.height;
                    }

                    if(roi.x0 < 0 || roi.y0 < 0 || roi.x1 > img.width || roi.y1 > img.height)
                    {
                        std::cout << "Warn: ROI #" << i << " is out of image " << item.filename << std::endl;
                    }
                    else
                    {

                        if(set_geometry.width == 0 && set_geometry.height==0)
                        {
                            set_geometry.width = roi.width();
                            set_geometry.height = roi.height();
                            std::cout << "Geometry: " << set_geometry.width << 'x' << set_geometry.height << std::endl;
                        }

                        if(set_geometry == roi.extension()) // se sono uguali viene fatto un Crop
                        {
                            if(!set.ImportImage(img.crop(roi), item.object[i].category))
                                std::cerr << "Failed to import image " << item.filename << std::endl;
                        }
                        else
                        {
                            // resize
                            Image img2;
//                     std::cout << item.filename << " convert " << roi.width() << 'x' << roi.height() << " to " << set_geometry.width << 'x' << set_geometry.height << '\n';
                            img2.alloc(set_geometry.width, set_geometry.height, 1);
                            Resample(img2, img.crop(roi), rect(0,0, roi.width(), roi.height()));
                            set.ImportImage(img2, item.object[i].category);
                        }

                    }

                }

            // se e' un file da estrazione automatica di negativi, li usa
            if(item.auto_negative && (nRand>0))
            {
                if(set_geometry.width == 0 || set_geometry.height == 0)
                {
                    std::cerr << "Error: Invalid geometry for random sample extraction. Check -r <w> <h> option." << std::endl;
                }
                else
                {
                    std::cout << "Extract random from " << item.filename << " ID:" << frameidx << std::endl;
                    std::vector<datasetobject> list = item.object;

                    // for all octaves:
                    for(;;) {

                        RandomExtract(set, img, set_geometry, nRand, list);

                        if(img.width > 2 * set_geometry.width && img.height > 2 * set_geometry.height)
                        {
                            Image out;
                            out.alloc(img.width/2, img.height/2, 1);
                            Downsample2X(out, img);

                            for(int i=0; i<list.size(); ++i)
                            {   list[i].roi.x0 /= 2;
                                list[i].roi.y0 /= 2;
                                list[i].roi.x1 = (list[i].roi.x1+1)/2;
                                list[i].roi.y1 = (list[i].roi.y1+1)/2;
                            }

                            std::swap(img, out);
                        }
                        else
                            break;
                    }

                }

                frameidx++;
            }

        }

    }

    frameidx = 0;
    return true;
}

#endif
