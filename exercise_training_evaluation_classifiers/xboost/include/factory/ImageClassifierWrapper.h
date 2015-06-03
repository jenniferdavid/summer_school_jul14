/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 * Copyright (c) 2013-2014 Luca Castangia <l.caio@ce.unipr.it>
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

#ifndef _IMAGE_CLASSIFIER_WRAPPER_H
#define _IMAGE_CLASSIFIER_WRAPPER_H

#include "Types.h"
#include "ImageClassifier.h"

// #include "IO/pnmio.h"

/** @file ImageClassifierWrapper.h
 * @brief implements various classifier */

/// bridge for ImageClassifier
template<class _Instance, class _Preprocessor>
class ImageClassifierWrapper: public ImageClassifier, public _Preprocessor, public _Instance  {
    /// the preprocessed data
    typename _Preprocessor::ReturnType m_clsf_data;      
public:
  
  /// [imageclassifier w,h] [preprocessor if any] [classifier data]
  ImageClassifierWrapper(std::istream & in) : ImageClassifier(in), _Preprocessor(in), _Instance(in) { m_clsf_data.first.data=NULL; }
  
  virtual ~ImageClassifierWrapper() { Release(); }
  

    static std::string signature() {
        return _Instance::signature();
    }
    
    
    std::string getSignature() const {
        return _Instance::signature();
    }
    
    /// initialize m_clsf_data
    virtual void setImage(const unsigned char *ptr, long stride, unsigned int width, unsigned int height) {
      // initialize m_clsf_data
        _Preprocessor::Process(m_clsf_data, ptr, width, height, stride);
    }

    /// release m_clsf_data
    virtual void Release() {
        m_clsf_data.first.release();
    }

    /// process single point of m_clsf_data
    virtual float operator() (int x0, int y0) const {
        return _Instance::operator()(getData1(m_clsf_data.first,m_clsf_data.second,x0,y0),getData2(m_clsf_data.first,m_clsf_data.second));
    }

    /// process a row of m_clsf_data, with a fixed step
    virtual void Process (double *out, int x0, int y0, int n, int step) const {
        for(int i=0; i<n; i+=step)
            *out++=_Instance::operator()(getData1(m_clsf_data.first,m_clsf_data.second,x0+i,y0),getData2(m_clsf_data.first,m_clsf_data.second));
    }

};

#endif
