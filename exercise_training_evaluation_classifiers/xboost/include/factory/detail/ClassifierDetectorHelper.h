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


#ifndef _CLASSIFIER_DETECTOR_HELPER_H
#define _CLASSIFIER_DETECTOR_HELPER_H

#include <Utility/timer.h>
#include <Thread/thread.h>
#include <Utility/ImageUtils.h>
/*
#include "IO/pnmio.h"
*/
#define MAX_SCALES      24

// TODO
// #define DEBUG_TIMING

/* Motivazioni:
 *  - HAAR ammette il ricampionamento delle ottave attraverso l'istanziazione di un oggetto ottimizzato.
 *    Il processing pertanto viene svolto per ogni scala.
 *  - ICF non ammette ricampionamento. In questo caso e' vantaggioso fare il ricampionamento delle scale e poi
 *    le ottave applicare un downsample2X per ridurre al minimo l'aliasing.
 *
 *  in entrambi i casi, per esprimere al meglio le capacita' multithread e il consumo della memoria, si possono mettere in campo diverse strategie
 * */

/// compute the classifier response on a ROI
/// @todo Move in Library
/// @todo step
template<class _Instance, class _DataType>
void inner_compute_response(const _Instance * inst, const _DataType * data, double * response, int stride, rect roi)
{
    for(int j=roi.y0; j<roi.y1; j++)
        for(int i=roi.x0; i<roi.x1; i++)
            response[i+j*stride]= (*inst)(getData1(data->first,data->second,i,j),getData2(data->first,data->second));
}

/// compute the classifier response using a complex ROI
/// @todo Move in Library
/// @todo step
template<class _Instance, class _DataType>
void inner_compute_masked_response(const _Instance * inst, const _DataType * data, double * response, int stride, int y0, int y1, const std::pair<int, int> *roi)
{
    for(int j=y0; j<y1; j++)
        for(int i=roi[j].first; i<roi[j].second; i++)
            response[i+j*stride]= (*inst)(getData1(data->first,data->second,i,j),getData2(data->first,data->second));
}

/// compute the classifier response using a complex ROI
/// @todo Move in Library
/// @todo step
template<class _Instance, class _DataType>
void inner_compute_masked_response(const _Instance * inst, const _DataType * data, double * response, int stride, const unsigned char *mask, rect roi)
{
//   std::cout << "ROI: " << roi.x0 << '\t' << roi.y0 << '\t' << roi.x1 << '\t' << roi.y1 << std::endl;
    for(int j=roi.y0; j<roi.y1; j++)
        for(int i=roi.x0; i<roi.x1; i++)
            if(mask[i+j*stride])
                response[i+j*stride]= (*inst)(getData1(data->first,data->second,i,j),getData2(data->first,data->second));
            else
                response[i+j*stride]= -1.0; // TODO
}

/// some common method that can be used to inner detector
template<class _Preprocessor, class _Instance>
class ObjectDetectorWrapperBase: public _Preprocessor
{

public:

    typedef typename _Preprocessor::ReturnType data_type;

    /// a vector of classifier instance
    std::vector<_Instance> m_classifs;
    /// per classifier params
    std::vector<ObjectDetectorParams > m_clsparams;
    ///
    ObjectDetectorGlobalParams m_params;

#ifdef DEBUG_TIMING
    ///// DEBUG
    Statistic timer_preprocess, timer_total, timer_response, timer_nms;
    Statistic timer_preprocess_scale[MAX_SCALES];
    Statistic timer_response_scale[MAX_SCALES];
    Statistic timer_nms_scale[MAX_SCALES];
#endif

public:

    ObjectDetectorWrapperBase(std::istream & in) : _Preprocessor(in) { }

    ~ObjectDetectorWrapperBase()
    {
#ifdef DEBUG_TIMING
        std::cout << "Timing Debug:\n";
        std::cout << "Total Total: " << timer_total  << std::endl;
        if(timer_preprocess)
            std::cout << "Total Preprocess: " << timer_preprocess  << std::endl;
        for(int i =0; i<MAX_SCALES; ++i)
        {
            if(timer_preprocess_scale[i])
                std::cout << "Scale " << i << " preprocess: " << timer_preprocess_scale[i] << std::endl;
            if(timer_response_scale[i])
                std::cout << "Scale " << i << " response: " << timer_response_scale[i] << std::endl;
            if(timer_nms_scale[i])
                std::cout << "Scale " << i << " nms: " << timer_nms_scale[i] << std::endl;
        }
#endif
    }


    void setParams(const ObjectDetectorGlobalParams & params) {
        m_params = params;
    }
    const ObjectDetectorGlobalParams & getParams() const {
        return m_params;
    }

    /// set per-classifier params
    void setClassifierParams(int index,  const ObjectDetectorParams & params) {
        m_clsparams.at(index) = params;
    }
    /// get per-classifier params
    const ObjectDetectorParams & getClassifierParams(int index) {
        return m_clsparams.at(index);
    }

    /** test if the classifier index @a n need a mask to be processed.
      *  a classifier requires a mask when it has on the parameters the search ranges o ROIs. */
    bool require_a_mask(int n) const
    {
        return !m_clsparams[n].m_srcSearchRanges.empty() || ! m_clsparams[n].m_searchRoi.empty();
    }

    /** this method convert a search ranges and the roi of classifier @a cl_index in a bitmask
     * @param out an output ROI that contains the mask
     */
    void prepare_mask(int cl_index, float s, unsigned char *mask, unsigned int r_width, unsigned int r_height, int cl_width, int cl_height, rect & roi)
    {
        std::vector<std::pair<int,int> > searchRanges;
        std::vector<rect> rois;
        this->get_ranges(searchRanges, rois, cl_index, s);

        // proces only inside ROI (TODO: compute precise ROI)
        roi.x0 = r_width;
        roi.y0 = r_height;
        roi.x1 = 0;
        roi.y1 = 0;

//         std::cout << "ROIs: " << rois.size() << '\t' << searchRanges.size() << std::endl;

        if(rois.empty())
        {
            // only search ranges
            int n = std::min<int>(r_height, searchRanges.size()-cl_height); // up to
            roi.x0 = 0;
            roi.x1 = r_width;
            if(n>0)
            {
                for(int i = 0; i<n; ++i)
                {
//               std::cout << i << '\t' << searchRanges[i+cl_height].first << '\t' << searchRanges[i+cl_height].second << std::endl;
                    // the search range is on the object base
                    if(searchRanges[i+cl_height].first <= cl_width && searchRanges[i+cl_height].second >= cl_width)
                    {
                        if(roi.y0 > i) roi.y0 = i;
                        if(roi.y1 < i) roi.y1 = i;

                        memset(mask + i * r_width, 255, r_width);
                    }
                    else
                        memset(mask + i * r_width, 0, r_width);
                }
            }
            else
            {
                roi.y0 = roi.y1 = 0;
                memset(mask, 0, r_width * r_height);
                // clean UP
//               std::cout << "ERROR:" <<  r_height << '\t' <<  rois.size() << "\t" <<  cl_height << std::endl;
            }

        }
        else if(searchRanges.empty())
        {
            memset(mask, 0, r_width * r_height);

            for(int i = 0; i<rois.size(); ++i)
            {
                rect cur = rois[i];

                // clip cur:
                if(cur.x0 < 0) cur.x0 = 0;
                if(cur.y0 < 0) cur.y0 = 0;
                if(cur.x1 > r_width) cur.x1 = r_width;
                if(cur.y1 > r_height) cur.y1 = r_height;

                // update roi
                if(roi.x0 > cur.x0) roi.x0 = cur.x0;
                if(roi.x1 < cur.x1) roi.x1 = cur.x1;
                if(roi.y0 > cur.y0) roi.y0 = cur.y0;
                if(roi.y1 < cur.y1) roi.y1 = cur.y1;

                if(cur.x0<cur.x1)
                    for(int j = cur.y0; j<cur.y1; ++j)
                        memset(mask + j * r_width + cur.x0, 255, cur.width() );
            }

        }
        else
        {
            int n = std::min<int>(r_height, searchRanges.size()-cl_height); // up to

            // UNION
            memset(mask, 0, r_width * r_height);

            for(int i = 0; i<rois.size(); ++i)
            {
                rect cur = rois[i];

                // clip cur:
                if(cur.x0 < 0) cur.x0 = 0;
                if(cur.y0 < 0) cur.y0 = 0;
                if(cur.x1 > r_width) cur.x1 = r_width;
                if(cur.y1 > n) cur.y1 = n; // clip using {n}

                // update roi
                if(roi.x0 > cur.x0) roi.x0 = cur.x0;
                if(roi.x1 < cur.x1) roi.x1 = cur.x1;
                if(roi.y0 > cur.y0) roi.y0 = cur.y0;
                if(roi.y1 < cur.y1) roi.y1 = cur.y1;

                if(cur.x0<cur.x1)
                    for(int j = cur.y0; j<cur.y1; ++j)
                    {
                        if(searchRanges[j+cl_height].first < cl_width && searchRanges[j+cl_height].second > cl_width)
                        {
                            memset(mask + j * r_width + cur.x0, 255, cur.width() );
                        }
                    }
            }

        }

    }

    /// convert the internal ranges of the classifier of index @a n
    void get_ranges(std::vector<std::pair<int,int> > & searchRanges, std::vector<rect> & roi, int n, float s) const
    {
        // #1 converts search ranges
        {
            const std::vector<std::pair<int,int> > & src = m_clsparams[n].m_srcSearchRanges;
            int h = src.size()/s;
            searchRanges.resize(h);

            // search range bilinear interpolation:
            for(int i=0; i<h; ++i)
            {
                float j = i * s;
                int ij = (int) j;
                float dj = j - (float) ij;

                searchRanges[i].first = ((ij+1 < src.size()) ? (src[ij].first * (1.0f - dj) + src[ij+1].first * dj) : src[ij].first)/s;
                searchRanges[i].second = ((ij+1 < src.size()) ? (src[ij].second * (1.0f - dj) + src[ij+1].second * dj) : src[ij].second)/s;
            }
        }
        // #2 converts ROIs
        {
            const std::vector<rect> & src = m_clsparams[n].m_searchRoi;
            roi.resize(src.size());

            for(int i=0; i<src.size(); ++i)
            {
                roi[i].x0 = (src[i].x0 / s)+0.5f;
                roi[i].x1 = (src[i].x1 / s)+0.5f;
                roi[i].y0 = (src[i].y0 / s)+0.5f;
                roi[i].y1 = (src[i].y1 / s)+0.5f;
            }

        }

    }

    /// compute_response
    /// compute the response image using the instance object on a rectangular ROI
    /// TODO: valutare se calcolare contemporanemnete piu' response image.
    /// TODO: response image in float?
    /// TODO: search ranges and ROI
    /// @param inst an instance to execute
    /// @param data a preprocessor data
    /// @param response response to generate
    /// @param stride response stride
    /// @param roi a ROI
    /// TODO: octave/step
    /// TODO: move outside
    static void compute_response(const _Instance & inst, const data_type & data, double * response, int stride, const rect & roi, int nThread)
    {
        if(nThread>1)
        {
            sprint::thread_group thread_pool_;
            int nTasks = roi.height();
            for(int i =0; i<nThread; ++i)
            {
                rect ir;
                ir.x0 = roi.x0;
                ir.x1 = roi.x1;
                ir.y0 = roi.y0 + (i * roi.height())/nThread;
                ir.y1 = roi.y0 + ((i+1) * roi.height())/nThread;
                thread_pool_.create_thread(sprint::thread_bind(&inner_compute_response<_Instance, data_type>, &inst, &data, response, stride, ir));
            }
            thread_pool_.join_all();
        }
        else
        {
            inner_compute_response(&inst, &data, response, stride, roi);
        }
    }

    static void compute_masked_response(const _Instance & inst, const data_type & data, double * response, int stride, const rect & roi, const unsigned char *mask, int nThread)
    {
        nThread = 1;
        if(nThread>1)
        {
            sprint::thread_group thread_pool_;
            int nTasks = roi.height();
            for(int i =0; i<nThread; ++i)
            {
                rect ir;
                ir.x0 = roi.x0;
                ir.x1 = roi.x1;
                ir.y0 = roi.y0 + (i * roi.height())/nThread;
                ir.y1 = roi.y0 + ((i+1) * roi.height())/nThread;
                thread_pool_.create_thread(sprint::thread_bind(&inner_compute_masked_response<_Instance, data_type>, &inst, &data, response, stride, mask, ir));
            }
            thread_pool_.join_all();
        }
        else
        {
            inner_compute_masked_response(&inst, &data, response, stride, mask, roi);
        }
    }

    /// compute scaled preprocessor
    /// le scale vengono valutate in parallelo
    /// TODO: do not rescale scale=1.0
//     void compute_scale(data_type * out, int width, int height)
//     {
//         Image scaled_img;
//         scaled_img.alloc(width, height, 1);
//
//         m_img.ExportImage(scaled_img);
//
//         _Preprocessor::Process(*out, scaled_img.data, scaled_img.width, scaled_img.height, scaled_img.stride);
//     }

    /// compute scaled preprocessor, without using m_img resampler!
//     void compute_scale(data_type * out, const ImageHandle & src, int width, int height)
//     {
//         if(src.width == width && src.height == height)
//         {
//           _Preprocessor::Process(*out, src.data, src.width, src.height, src.stride);
//         }
//         else
//         {
//           Image scaled_img;
//           scaled_img.alloc(width, height, 1);
//           BilinearResample(scaled_img, src, rect(0,0, width, height) );
//           _Preprocessor::Process(*out, scaled_img.data, scaled_img.width, scaled_img.height, scaled_img.stride);
//         }
//     }

    /// compute scaled preprocessor
    /// le scale vengono valutate in parallelo
//     void compute_scale(data_type * out, double scale)
//     {
//         compute_scale(out, m_width / scale, m_height / scale);
//     }

    /// compute scaled preprocessor
    /// Ogni thread gestisce una scala
//     void compute_scale_octaves(data_type * out, float scale, int octaves)
//     {
//         Image scaled_img;
//         for(int i =0; i<octaves; ++i)
//         {
//             // FIRST OCTAVE
//             if(i == 0)
//             {
//             int width = m_width / scale;
//             int height = m_height / scale;
//
//             scaled_img.alloc(width, height, 1);
//
//             m_img.ExportImage(scaled_img);
//             }
//             else
//             {
//               // OTHERS octaves
//               ImageHandle downsampled = scaled_img;
//               downsampled.width /=2;
//               downsampled.height /=2;
//               downsampled.stride = downsampled.width;
//               Downsample2X(downsampled, scaled_img);
//               scaled_img.width = downsampled.width;
//               scaled_img.height = downsampled.height;
//               scaled_img.stride = downsampled.stride;
//             }
//
//
// //    static int count = 0;
// //    char buffer[256];
// //    sprintf(buffer, "/tmp/R%u.pgm", count);
// //    count++;
// //    pnm_write(scaled_img, buffer);
//
//             _Preprocessor::Process(*out, scaled_img.data, scaled_img.width, scaled_img.height, scaled_img.stride);
//
//             scale *= 2.0;
//             out++;
//         }
//     }

    void precompute_scales(std::vector< data_type > & preprocess_images, double scale_factor)
    {
#ifdef DEBUG_TIMING
        timer_preprocess.Start();
#endif

        sprint::thread_group thread_pool_;

        preprocess_images.resize(m_params.nScales);

        double s=1;
        this->timer_parallel_ichn.Start();
        // ogni thread gestisce una scala
        for(int scale=0; scale<m_params.nScales; scale++)
        {
            thread_pool_.create_thread(sprint::thread_bind(&ObjectDetectorWrapperBase<_Preprocessor, _Instance>::compute_scale,this, &preprocess_images[scale], s));
            if(!m_params.octave_mode)
                s+=scale_factor;
            else
                s*=scale_factor;
        }
        thread_pool_.join_all();
#ifdef DEBUG_TIMING
        timer_preprocess.Stop();
#endif
    }

    /// calcola sia le scale che le ottave
    void precompute_scales_octaves(std::vector< data_type > & preprocess_images, double scale_factor, int k_max)
    {
#ifdef DEBUG_TIMING
        timer_preprocess.Start();
#endif
        sprint::thread_group thread_pool_;

        preprocess_images.resize(m_params.nScales * k_max);

        double s=1;
        this->timer_parallel_ichn.Start();
        for(int scale=0; scale<m_params.nScales; scale++)
        {
            thread_pool_.create_thread(sprint::thread_bind(&ObjectDetectorWrapperBase<_Preprocessor, _Instance>::compute_scale_octaves,this, &preprocess_images[scale * k_max], s, k_max));
            if(!m_params.octave_mode)
                s+=scale_factor;
            else
                s*=scale_factor;
        }
        thread_pool_.join_all();
#ifdef DEBUG_TIMING
        timer_preprocess.Stop();
#endif
    }

    template<class T>
    static void dump_response_image(const T *src, unsigned int w, unsigned int h, const char *filename)
    {
        Image out(w,h,1);
        T max = src[0];
        T min = src[0];
        std::cout << "Dump " << w << 'x'<< h << ':';
        for(int i=0; i<w*h; ++i)
        {
            if (src[i] > max) max = src[i];
            if (src[i] < min) min = src[i];
        }
        std::cout << "min: " << min << "max: " << max << std::endl;

        if(-min > max) max = -min;

        for(int i=0; i<w*h; ++i)
            out.data[i] = 128 + ((src[i] * T(128)) / max);
        pnm_write(out, filename);
    }
};

// a policy
template <class _Instance, class _Preprocessor>
struct ClassifierDetectHelper {};

#endif
