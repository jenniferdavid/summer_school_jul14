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

#ifndef _INTEGRALIMAGE_DETECTOR_H
#define _INTEGRALIMAGE_DETECTOR_H

#include "ClassifierDetectorHelper.h"
#include <factory/ObjectDetector.h>
#include <cmath>

/// an IntegralImage detector, based on the multiple instance allocator with multiscale instance
#ifndef WIN32
template<>
#endif
template <class _Instance>
struct ClassifierDetectHelper<IntegralImagePreprocessor, _Instance>: public ObjectDetectorWrapperBase<IntegralImagePreprocessor, _Instance> {
private:

    typedef typename IntegralImagePreprocessor::ReturnType data_type;

    /// resampling object (UNUSED sometimes)
    IntegralImage<uint32_t> m_img;
    /// source image geometry
    unsigned int m_width, m_height;

    // no-thread safe octave
//    int nOctave;
private:

    void ImportImage(const ImageHandle & src)
    {
        m_width = src.width;
        m_height = src.height;
        m_img.Build(src);
    }
    // esegue il classificatore a una determinata scala
    void scale_object_detector(std::vector<Candidates> * out, double s)
    {
        data_type data;
        detail::CandidateParam nms_params;
        double *response = new double[this->m_width * this->m_height];

        bool have_to_release;
        IntegralImageHandle<uint32_t> resampled;

        // size of resampled image
        int width = this->m_width / s;
        int height = this->m_height / s;

        // create resampled channel image
        if(width == this->m_width && height == this->m_height)
        {
            IntegralImagePreprocessor::Process(data, m_img);
            have_to_release = false;
        }
        else
        {

            Resample(m_img, resampled, rect(0,0,this->m_width,this->m_height), width, height);
            IntegralImagePreprocessor::Process(data, resampled);
            have_to_release = true;
        }

        // estimate maximum octave
        int nOctave = 1;

        for(int n=0; n<this->m_classifs.size(); ++n)
        {
            int cl_width = this->m_clsparams[n].sz.width;
            int cl_height = this->m_clsparams[n].sz.height;

            int nOctave_test = ( (int)log2(std::min(width/cl_width, height/cl_height)) ) + 1; // rounded
            if(nOctave_test > nOctave) 
              nOctave = nOctave_test;
        }

        // for each octave
        for(int o=0; o<nOctave; ++o)
        {
            // for each classifier
            for(int n=0; n<this->m_classifs.size(); ++n)
            {

                // allocate an optimized version of _Instance
                _Instance rInst(this->m_classifs[n], (1<<o), 0, width);

                int cl_width  = this->m_clsparams[n].sz.width * (1<<o);
                int cl_height = this->m_clsparams[n].sz.height * (1<<o);
                // geometry of the response image
                int r_width = width - cl_width;
                int r_height = height - cl_height;

                if(r_width > 3 && r_height > 3)
                {
                    unsigned int step = std::min(cl_width, cl_height)/2; // max 50% occlusion
                    
                    if(step==0) 
                      step = 1;

                    if(this->require_a_mask(n))
                    {
                        unsigned char *mask = new unsigned char [r_width * r_height];
                        rect roi;
                        this->prepare_mask(n, s * (1<<o), mask, r_width, r_height, cl_width, cl_height, roi);
                        ObjectDetectorWrapperBase<IntegralImagePreprocessor, _Instance>::compute_masked_response(rInst, data, response, r_width, roi, mask, this->m_params.concurrent_jobs);
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
                        // ObjectDetectorWrapperBase<IntegralImagePreprocessor, _Instance>::compute_response(this->m_classifs[0], data, response, r_width, roi, 8);
                        ObjectDetectorWrapperBase<IntegralImagePreprocessor, _Instance>::compute_response(rInst, data, response, r_width, roi, this->m_params.concurrent_jobs);
                    }

                    nms_params.scale = s;
                    nms_params.category = this->m_clsparams[n].category;
                    nms_params.cl_geom.width = cl_width;
                    nms_params.cl_geom.height = cl_height;

//             {
//             char buffer[512];
//             static int count = 0;
//             sprintf(buffer, "/tmp/response-%06u.pgm", count);
//             count++;
//             ObjectDetectorWrapperBase<IntegralImagePreprocessor, _Instance>::dump_response_image(response, r_width, r_height, buffer);
//             }

                    // extract candidates
                    NonMaximaSuppression(response, *out, step, r_width, r_width, r_height, this->m_clsparams[0].th, nms_params, 1);

                }

            }

            // s *= 2.0; // next octave
        }

        if(have_to_release)
            data.first.release();

        delete [] response;
    }
public:

    // bridge for preprocessor
    ClassifierDetectHelper(std::istream & in) : ObjectDetectorWrapperBase<IntegralImagePreprocessor, _Instance>(in) { }


    // detect implementa le 3 varianti
    void detect(std::vector<Candidates>& out, const ImageHandle & src) {

#ifdef DEBUG_TIMING
        this->timer_total.Start();
#endif

        data_type data_store;

//         std::cout << "Max octave:" << nOctave << std::endl;

        // scale factor
        double scale_factor = (this->m_params.octave_mode) ? ( exp( log(2.0) / this->m_params.nScales) ) : (1.0 / this->m_params.nScales);

        std::vector< std::vector<Candidates > > tmp_out;

        // import image in the helper
        this->ImportImage(src);

        tmp_out.resize( this->m_params.nScales );

        double s = 1.0;
        /*
        sprint::thread_group thread_pool_;
        // TODO: use concurrency value!
        // ogni thread gestisce una scala
        for(int scale=0; scale<this->m_params.nScales; scale++)
        {
            thread_pool_.create_thread(sprint::thread_bind(&ClassifierDetectHelper<IntegralImagePreprocessor, _Instance>::scale_object_detector,this, &tmp_out[scale], s));
            if(!this->m_params.octave_mode)
                s+=scale_factor;
            else
                s*=scale_factor;
        }
        */
        for(int scale=0; scale<this->m_params.nScales; scale++)
        {
            scale_object_detector(&out, s);
            if(!this->m_params.octave_mode)
                s+=scale_factor;
            else
                s*=scale_factor;
        }
#ifdef DEBUG_TIMING
        this->timer_total.Stop();
#endif
    }
};


#endif
