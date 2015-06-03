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

#ifndef _TRAIN_UTILS_H
#define _TRAIN_UTILS_H

/** @file TrainUtils.h
 *  @brief methods to evaluate classifier on images and detect false positives
 * */

#include "NonMaximaSuppression.h"
#include "Types.h"
#include "IO/datasetin.h"
#include "Utility/ResponseUtils.h"

#ifdef _MULTITHREAD
# include "Thread/thread.h"
# include "Thread/thread_group.h"
# include "Thread/bind.h"
# include "Thread/ref.h"
#endif  // #ifdef _MULTITHREADING


//NMS
namespace detail {

/// NMS Parameters for Candidates object. TODO: miss initial offset
struct CandidateParam {
    int width, height;  ///< size of object
    int scale;          ///< scale factor of the response image for (x,y) coordinates
};

}

/// a candidate
struct Candidates
{
    double response;    ///< response value
    int xo,yo;          ///< original coordinate on response image
    rect roi;           ///< the roi on source image

public:
    void Set(int x, int y, double val, const detail::CandidateParam & param)
    {
        response = val;

        roi.x0 = x * param.scale;
        roi.y0 = y * param.scale;
        roi.x1 = roi.x0 + param.width;
        roi.y1 = roi.y0 + param.height;

        xo = x;
        yo = y;
    }

    /// used for std::sort
    inline bool operator < (const Candidates & b) const
    {
        return response > b.response; // invert
    }

};
//

/// Parameters for BootStrapProcedure
struct BootStrapParams {
    /// scale factor between two images
    float scale_factor;
    /// max number of negative extract per frame or per scale
    int negative_random_samples;
    /// non maxima suppression step
    int nms_step;
    /// downsampling factor for computing response
    int downsampling;
    /// threshold
    double thMin;

    BootStrapParams() : scale_factor(1.0f), negative_random_samples(1), nms_step(1), downsampling(1), thMin(0.0) { }
};


extern int frameidx;

/** select best negative greater then thMin in list of image in filename
  * TODO nNegImage
  * @param training_set
  * @param in un dataset
  * @param cl a classifier
  * @param thMin minimum threshold to accept negative
  * @param nNegImage number of negatives per frame
  **/
template<class SourceDatasetType, class ClassifierType>
bool BootStrapProcedure( SourceDatasetType & training_set,  datasetin & in, const ClassifierType& cl, const BootStrapParams & params, int num_thread)
{
    int new_pattern = 0;
    datasetitem item;
    Timer t_elab;
    size sz;
    sz.width = training_set.width;
    sz.height = training_set.height;

    while(in.next(item))
    {
        Image img;

#ifdef LIMIT_PER_FRAME
        // store
        std::vector< ImageHandle > image_scales;
        std::vector< Candidates > candidates;
#endif

        // processo per il bootstrap solo le immagini con auto_negative
        if(item.auto_negative && pnm_load(item.filename.c_str(), img))
        {

            int src_width = img.width;
            int src_height = img.height;
            std::cout << "Testing classifier on " << item.filename << " (" << src_width << 'x' << src_height << ") with step " << params.downsampling << " [ID:" << frameidx << "]" << std::endl;
            std::vector<datasetobject> list = item.object;
            float scale = 1.0f;
            ImageResampler rsmp;
            bool first_scale = true;
            int n_negatives = 0;
            Timer t0;

            t0.Start();

            // iterate for all the octaves
            for(;;) {
                // the response is smaller than img because the window outside image are not considered
                int w_r = (img.width- sz.width)/params.downsampling;
                int h_r = (img.height- sz.height)/params.downsampling;

                std::cout << "\t" << img.width << 'x' << img.height << " (" << w_r << 'x' << h_r << ")... ";
                std::cout.flush();

                std::vector<Candidates>   elements;
                elements.reserve(params.negative_random_samples);
                typename SourceDatasetType::ReturnType Out;
                
                // terminate check (if 3x3 image is provided)
                if(h_r < 3 || w_r < 3)
                {
                  std::cout << "\tWarn: response image too small\n";
                  break;
                }

                
                double* response = new double[w_r*h_r];

                Timer t;

                t.Start();

                // precompute the preprocess image
                training_set.Process(Out, img.data, img.width, img.height, img.stride);

                // create the optimized version for the classifier:

                typename ClassifierType::OptimizedType _Inst(cl, 0, Out.second);

                double ret,r_max,r_min;

#ifdef _MULTITHREAD
                if(num_thread > 1)
                {
                    sprint::thread_group thread_pool_;
                    for(int ii=0; ii<num_thread; ii++)
                    {
                        int startH = (ii*h_r)/num_thread;
                        int endH =   ((ii+1)*h_r)/num_thread;

                        // about 20% increase in performances
                        // thread_pool_.create_thread(sprint::thread_bind(&workerResponse<typename SourceDatasetType::ReturnType,ClassifierType>,response,startH,endH,w_r, &cl, &Out ));
                        thread_pool_.create_thread(sprint::thread_bind(&optimizedWorkerResponse<typename SourceDatasetType::ReturnType, typename ClassifierType::OptimizedType>,response,startH,endH,w_r, sprint::c_ref(_Inst), sprint::c_ref(Out), params.downsampling ));
                    }

                    thread_pool_.join_all();
                }
                else
#endif
                {
                    // generate the response image
                    // about 20% increase in performances
                    // workerResponse(response, 0, h_r, w_r, &cl, &Out);
                    optimizedWorkerResponse(response, 0, h_r, w_r, _Inst, Out, params.downsampling);
                }

                double dt = t.GetTime();

                r_min = *std::min_element(response,response+w_r*h_r);//dbg
                r_max = *std::max_element(response,response+w_r*h_r);//dbg

                std::cout << "\tr_max " << r_max << " | r_min " << r_min;
                std::cout.flush();
                // print RESPONSE to debug
                /* {
                Image test;
                test.alloc(w_r, h_r, 3);
                for(int j =0;j<h_r;++j)
                for(int i =0;i<w_r;++i)
                {
                  double r = response[i + j * w_r];
                  if(r>0.0)
                  {
                  unsigned char p = r * 255.0 / r_max;

                  test.data[3*i + test.stride *j+0] = 0;
                  test.data[3*i + test.stride *j+1] = p;
                  test.data[3*i + test.stride *j+2] = 0;
                  }
                  else
                  {
                  unsigned char p = -r * 255.0 / r_min;

                  test.data[3*i + test.stride *j+0] = 0;
                  test.data[3*i + test.stride *j+1] = 0;
                  test.data[3*i + test.stride *j+2] = p;
                  }
                }
                static int count = 0;
                char buffer[256];
                sprintf(buffer, "/tmp/r%06u.ppm", count);
                    count++;
                    pnm_write(test, buffer);
                } */

                std::vector<Candidates> tmpOut;
                detail::CandidateParam param;

                param.width = training_set.width;
                param.height = training_set.height;
                param.scale = params.downsampling;

                // search non overlapping box TODO: this could be a parameter
                int step = params.nms_step / params.downsampling;
                NonMaximaSuppression(response, tmpOut, step, w_r, w_r, h_r, params.thMin, param, num_thread);

                std::cout << " | detected: "<< tmpOut.size() << " in " << dt << "s";
                std::cout.flush();

                // TODO: check existance of NEGATIVE AREAs
                /*  {
                  static int count = 0;
                  char buffer[256];
                  Image out;
                  out.clone(img);
                    for(int i=0; i<tmpOut.size(); i++)
                      DrawRect(out, rect( tmpOut[i].roi.x0, tmpOut[i].roi.y0, tmpOut[i].roi.x1-1, tmpOut[i].roi.y1-1), 255);
                  sprintf(buffer, "/tmp/test%06u.pgm", count);
                  count++;
                  pnm_write(out, buffer);
                  }  */

                for(int i=0; i<tmpOut.size(); i++)
                {
                    bool is_overlap = false;
                    // puo' solo overlappare dei negativi (non dovrebbero essercene comunque)
                    for(int j = 0; j<list.size(); ++j)
                        if(list[j].category >= 0 && overlap(tmpOut[i].roi, list[j].roi))
                            is_overlap = true;

                    if(!is_overlap)
                    {
                        elements.push_back(tmpOut[i]);

                    }
                }


                // this code extract at least params.negative_random_samples elements per each scale
#ifndef LIMIT_PER_FRAME
                std::cout << " | " << elements.size() << "/" << params.negative_random_samples <<" false positives found" << std::endl;


                // SORT (TODO or random sampling?)
                if(elements.size() > params.negative_random_samples)
                {
                    std::sort(elements.begin(), elements.end()); // sort from higher to lower
                    elements.resize(params.negative_random_samples); // erase
                }

                for(std::vector<Candidates>::const_iterator i = elements.begin(); i != elements.end(); ++i)
                {
                    training_set.ImportImage(img.crop(i->roi),-1);
                    n_negatives++;
                }

                new_pattern += elements.size();
#else

                // this code extract at least params.negative_random_samples elements per each frame
                std::cout << " | " << elements.size() << " false positives found" << std::endl;

                // crop all elements in candidates and store response and image, for further processing
                for(std::vector<Candidates>::const_iterator i = elements.begin(); i != elements.end(); ++i)
                {
                    std::pair<float, Image *> d;
                    d.first = i->response;
                    d.second = new Image;
                    d.second->clone( img.crop(i->roi) );
                    candidates.push_back(d);
                }

#endif

                delete [] response;
                Out.first.release(); // manually release preprocessed data

                scale *= params.scale_factor;

                // size of the response image for the next scale:
                w_r = (int(src_width/scale)  - sz.width) /params.downsampling;
                h_r = (int(src_height/scale) - sz.height)/params.downsampling;
                
                // it is possible to search for additional scale?
                // at least 4x4 response image need to be computed to make any sense
                if(w_r > 3 && h_r > 3)
                {
                  
                    if(first_scale)
                    {
                        rsmp.ImportImage(img);
                        first_scale = false;
                    }

                    Image out;
                    out.alloc( (int) ((float)src_width/scale), (int) ((float)src_height/scale), 1);
                    rsmp.ExportImage(out);

                    for(int i=0; i<list.size(); ++i)
                    {   list[i].roi.x0 = floor((float)item.object[i].roi.x0 / scale);
                        list[i].roi.y0 = floor((float)item.object[i].roi.y0 / scale);
                        list[i].roi.x1 = ceil((float)item.object[i].roi.x1 / scale);
                        list[i].roi.y1 = ceil((float)item.object[i].roi.y1 / scale);
                    }

                    std::swap(img, out);

                    /* {
                    static int count = 0;
                    char buffer[256];
                    sprintf(buffer, "/tmp/downsample%06u.pgm", count);
                    count++;
                    pnm_write(img, buffer);
                    } */

                }
                else
                    break;
            }

#ifdef LIMIT_PER_FRAME
            std::cout << "\t" << candidates.size() << "/" << params.negative_random_samples <<" total false positives found" << std::endl;

            // TODO: GroupRectangle?

            if(candidates.size() > params.negative_random_samples)
            {
                std::sort(candidates.begin(), candidates.end());
            }

            int n = 0;
            for(std::vector<std::pair<float, Image *> >::const_iterator i = candidates.begin(); i != candidates.end(); ++i)
            {
                if(n<params.negative_random_samples)
                    training_set.ImportImage(*i->second,-1);

                delete i->second; // release pointer and associated memory
                ++n;
            }

            new_pattern += candidates.size();
#else
            std::cout << "\tFrame processed in " << t0.GetTime() << "s. " << n_negatives << " total new negatives imported.\n";
#endif


            frameidx++;
        }
    }

    std::cout << "Bootstrap completed. " << new_pattern << " added to the negative pool.\n";

    return new_pattern == 0;
}

#endif
