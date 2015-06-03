#ifndef _IMAGESET_UTILS_H
#define _IMAGESET_UTILS_H

#include "IO/datasetin.h"
#include "Utility/ImageUtils.h"

/// process a single octave/scale and extract @a nRand not colliding object of size @a set_geometry
/// The object are directly imported in the @a set
template<class Operator>
bool RandomProcess(const std::string & filename, Operator & op, const ImageHandle & img, const size & set_geometry, int nRand, const std::vector<datasetobject> & object)
{
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
            if(!op(filename, img.crop(roi), -1)) // only negative
            {
              std::cerr << "Failed to import pattern";
              return false;
            }
        }
    }
    return true;
}

template<class Operator>
bool ProcessSet(datasetin & in, const size & sz, int nRand, Operator & op)
{
    datasetitem item;
    size set_geometry = sz; //

    if(set_geometry.width != 0 || set_geometry.height!=0)
        std::cout << "ProcessSet with geometry " << set_geometry.width << 'x' << set_geometry.height << std::endl;

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


                    if(set_geometry.width == 0 && set_geometry.height==0)
                    {
                        set_geometry.width = roi.width();
                        set_geometry.height = roi.height();
                        std::cout << "Geometry: " << set_geometry.width << 'x' << set_geometry.height << std::endl;
                    }

                    if(set_geometry == roi.extension()) // se sono uguali viene fatto un Crop
                    {
                      
                        if(!op(item.filename, img.crop(roi), item.object[i].category))
                          std::cerr << "Failed to import image " << item.filename << std::endl;
                    }
                    else
                    {
                        // resize
                        Image img2;
                        
                        // TODO: avoid upsampling?

                        img2.alloc(set_geometry.width, set_geometry.height, 1);
                        Resample(img2, img.crop(roi), rect(0,0, roi.width(), roi.height()));
                        op(item.filename, img2, item.object[i].category);
                    }

                }

            // se e' un file da estrazione automatica di negativi, li usa
            if(item.auto_negative && (nRand>0))
            {
                if(set_geometry.width == 0 || set_geometry.height == 0)
                    std::cerr << "Invalid geometry for random sample extraction" << std::endl;

                std::cout << "Extract random from " << item.filename << std::endl;
                std::vector<datasetobject> list = item.object;

                // for all octaves:
                for(;;) {

                    RandomProcess(item.filename, op, img, set_geometry, nRand, list);

                    // TODO: 
                    if(img.width > 2 * set_geometry.width && img.height > 2 * set_geometry.height)
                    {
                        Image out;
                        out.alloc(img.width/2, img.height/2, 1);
                        
                        // TODO: use downsampling factor different from 2X
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

        }

    }

    return true;
}

#endif
