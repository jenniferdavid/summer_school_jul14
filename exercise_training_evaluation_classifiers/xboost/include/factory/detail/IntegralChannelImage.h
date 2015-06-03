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
#ifndef _INTEGRALCHANNELIMAGE_HELPER_H
#define _INTEGRALCHANNELIMAGE_HELPER_H

#include "ClassifierDetectorHelper.h"
#include <factory/ObjectDetector.h>

#include <map>
#include <sstream>
#include <string>
#include <iomanip>

// DEBUG
#include "IO/pnmio.h"

// ICHN is tested with 3 different policy to understand the best performance

//#define PARALLEL_ICHN


/// policy for ICHN
#ifndef WIN32
template<>
#endif
template <class _Instance>
struct ClassifierDetectHelper<IntegralChannelImagePreprocessor, _Instance>: public ObjectDetectorWrapperBase<IntegralChannelImagePreprocessor, _Instance> {
private:

    typedef typename IntegralChannelImagePreprocessor::ReturnType data_type;


public:

    /// execute the classifier on image downsampled to a particular "scale" @a s
    /// @todo missing m_searchRoi
    /// @todo missing
    void scale_object_detector(std::vector<Candidates> * out, const ImageHandle *src, double s, int s_index)
    {
        data_type data;
        detail::CandidateParam nms_params;
        Image scaled_img;
        ImageHandle working_img;

        // allocate the response image (up to width x height)
        double *response = new double[src->width * src->height];

        // estimate maximum octave
        int nOctave = 1;

        for(int n=0; n<this->m_classifs.size(); ++n)
        {
            int cl_width = this->m_clsparams[n].sz.width;
            int cl_height = this->m_clsparams[n].sz.height;

            int nOctave_test = ( (int)log2(std::min(src->width/cl_width, src->height/cl_height)) ) + 1; // rounded
            if(nOctave_test > nOctave)
                nOctave = nOctave_test;
        }

        // per ogni ottava:
        for(int o=0; o<nOctave; ++o)
        {
            // size of resampled image
            int width = src->width / s;
            int height = src->height / s;

#ifdef DEBUG_TIMING
            if(o == 0)
                this->timer_preprocess_scale[s_index].Start();
            else
                this->timer_preprocess_scale[s_index].Resume();
#endif

            /******* */

            /* questa parte calcola la working_img in base alle ottave e alle scale usando il metodo piu' efficiente disponibile */
            if(o == 0)
            {
                // OTTAVA 0: scale source image

                if(width == src->width && height == src->height)
                {
                    // no scaling

                    scaled_img.alloc(src->width, src->height, 1); // reserve memory for subsequent octave
                    working_img = *src;
                }
                else
                {
                    scaled_img.alloc(width, height, 1);  // reserve memory for subsequent octave
                    // for octave=0 a bilinear resampling is used
                    BilinearResample(scaled_img, *src, rect(0,0, src->width, src->height) );

                    working_img = scaled_img;
                }

            }
            else
            {
                // downsample working_img -> scaled
                scaled_img.width = working_img.width/2;
                scaled_img.height = working_img.height/2;
                scaled_img.stride = scaled_img.width;
                Downsample2X(scaled_img, working_img);
                working_img = scaled_img;
            }

#ifdef _DEBUG_RESAMPLING
            {
                static int count = 0;
                char buffer[512];
                sprintf(buffer, "/tmp/test-%06u.pgm", count);
                count++;
                pnm_write(working_img, buffer);
            }
#endif

            /* FINE */

            // compute the preprocessing image
            IntegralChannelImagePreprocessor::Process(data, working_img.data, working_img.width, working_img.height, working_img.stride);

            /****** */

            // compute data from preprocessor
            // ObjectDetectorWrapperBase<IntegralChannelImagePreprocessor, _Instance>::compute_scale(&data, *src, width, height);
#ifdef DEBUG_TIMING
            if(s_index > MAX_SCALES)
                s_index = MAX_SCALES;

            if(o!=nOctave-1)
                this->timer_preprocess_scale[s_index].Pause();
            else
                this->timer_preprocess_scale[s_index].Stop();
#endif

#ifdef DEBUG_TIMING
            if(o == 0)
                this->timer_response_scale[s_index].Start();
            else
                this->timer_response_scale[s_index].Resume();
#endif

            // compute response and NMS for each classifiers
            for(int n=0; n<this->m_classifs.size(); ++n)
            {
                // geometry of the response image
                int rstep = this->m_clsparams[n].downsampling;
                int r_width = (width - this->m_clsparams[n].sz.width)/rstep;
                int r_height = (height - this->m_clsparams[n].sz.height)/rstep;

                if(r_width > 3 && r_height > 3)
                {

                    if(this->require_a_mask(n))
                    {
                        unsigned char *mask = new unsigned char [r_width * r_height];
                        rect roi;
                        this->prepare_mask(n, s, mask, r_width, r_height, this->m_clsparams[n].sz.width, this->m_clsparams[n].sz.height, roi);
                        /*
                        {
                            static int count = 0;
                            char buffer[512];
                            sprintf(buffer, "mask_%06u.pgm", count);
                            count++;
                            pgm_write(buffer, mask, r_width, r_height, 255);
                        }
                        */

                        // TODO: ugly workaround:
                        for(int i =0; i<r_width * r_height; ++i) response[i] = -1.0;
                        ObjectDetectorWrapperBase<IntegralChannelImagePreprocessor, _Instance>::compute_masked_response(this->m_classifs[n], data, response, r_width, roi, mask, this->m_params.concurrent_jobs);
                        delete [] mask;
                    }
                    else
                    {
                        // proces only inside ROI
                        rect roi;
                        roi.x0 = 0;
                        roi.y0 = 0;
                        roi.x1 = r_width;
                        roi.y1 = r_height;

                        // compute response
                        ObjectDetectorWrapperBase<IntegralChannelImagePreprocessor, _Instance>::compute_response(this->m_classifs[n], data, response, r_width, roi, this->m_params.concurrent_jobs);
                    }

#ifdef DEBUG_TIMING
                    if(o!=nOctave-1)
                        this->timer_response_scale[s_index].Pause();
                    else
                        this->timer_response_scale[s_index].Stop();
#endif

                    nms_params.scale = s;
                    nms_params.category = this->m_clsparams[n].category;
                    nms_params.cl_geom.width = this->m_clsparams[n].sz.width;
                    nms_params.cl_geom.height = this->m_clsparams[n].sz.height;

                    unsigned int step = std::min(this->m_clsparams[n].sz.width, this->m_clsparams[n].sz.height)/2; // max 50% occlusion TODO
                    if(step<1) step =1;
                    /*
                    {
                        static int count  = 0;

                        double r_min = *std::min_element(response,response+r_width*r_height);//dbg
                        double r_max = *std::max_element(response,response+r_width*r_height);//dbg

                        std::cout << "\tr_max " << r_max << " | r_min " << r_min;
                        // print RESPONSE to debug
                        Image test;
                        test.alloc(r_width, r_height, 3);
                        for(int j =0; j<r_height; ++j)
                            for(int i =0; i<r_width; ++i)
                            {
                                double r = response[i + j * r_width];
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
                        char buffer[256];
                        sprintf(buffer, "r%06u.ppm", count);
                        count++;
                        pnm_write(test, buffer);
                    }
                    */

#ifdef DEBUG_TIMING
                    this->timer_nms_scale[s_index].Start();
#endif
                    // extract candidates. step ? (max on a step area)
                    NonMaximaSuppression(response, *out, step, r_width, r_width, r_height, this->m_clsparams[n].th, nms_params, 1); // no multi-thread
#ifdef DEBUG_TIMING
                    this->timer_nms_scale[s_index].Stop();
#endif

                } // r_width>3
            } // for classifier

            data.first.release();

            s *= 2.0; // next octave
        }

        delete [] response;
    }
public:

    // bridge for preprocessor
    ClassifierDetectHelper(std::istream & in) :  ObjectDetectorWrapperBase<IntegralChannelImagePreprocessor, _Instance>(in) { }


    // detect implementa le 3 varianti
    void detect(std::vector<Candidates>& out, const ImageHandle & src) {

#ifdef DEBUG_TIMING
        this->timer_total.Start();
#endif

        data_type data_store;

        // scale factor
        if(!this->m_params.octave_mode)
        {
            std::cerr << "Linear Mode is unimplemented for Ichn feature" << std::endl;
            return;
        }
        double scale_factor = (this->m_params.octave_mode) ? ( exp( log(2.0) / this->m_params.nScales) ) : (1.0 / this->m_params.nScales);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // import image in the helper
        // this->ImportImage(src);
#ifdef PARALLEL_ICHN
        std::vector< std::vector<Candidates > > tmp_out;
        tmp_out.resize( this->m_params.nScales );
        sprint::thread_group thread_pool_;
#endif

        double s = 1.0;

        // iterate on "scales"
        for(int scale=0; scale<this->m_params.nScales; scale++)
        {
#ifdef PARALLEL_ICHN
            // ogni thread gestisce una scala
            thread_pool_.create_thread(sprint::thread_bind(&ClassifierDetectHelper<IntegralChannelImagePreprocessor, _Instance>::scale_object_detector,this, &tmp_out[scale], &src, s, scale));
#else
            scale_object_detector(&out, &src, s, scale);
#endif
            if(!this->m_params.octave_mode)
                s+=scale_factor;
            else
                s*=scale_factor;
        }

#ifdef PARALLEL_ICHN
        thread_pool_.join_all();
        for(int scale=0; scale<this->m_params.nScales; scale++)
            out.insert(out.end(), tmp_out[scale].begin(), tmp_out[scale].end());
#endif
#ifdef DEBUG_TIMING
        this->timer_total.Stop();
#endif
    }
};

#endif
