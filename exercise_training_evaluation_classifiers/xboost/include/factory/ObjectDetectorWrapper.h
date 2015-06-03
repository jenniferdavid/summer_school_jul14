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

#ifndef _OBJECT_CLASSIFIER_WRAPPER_H
#define _OBJECT_CLASSIFIER_WRAPPER_H


#include "Preprocessor/IntegralChannelImagePreprocessor.h"
#include "Preprocessor/IntegralImagePreprocessors.h"
#include "Traits/IntegralChannelFeature.h"
#include "Traits/HaarFeature.h"

#include "Types.h"
#include "Utility/timer.h"

//NMS
#include "Utility/NonMaximaSuppression.h"

#include "detail/IntegralChannelImage.h"
#include "detail/IntegralImage.h"


#include "ObjectDetector.h"


/** @file ObjectDetectorWrapper.h
 * @brief implements various classifier */

/// Wrapper used as bridge between classifier and preprocessor with the ImageClassifier object. All method are implemente here.
/// Load instance and call detect of inner helper
template<class _Instance, class _Preprocessor>
class ObjectDetectorWrapper: public ObjectDetector {
private:
    /// The real classifier 
    ClassifierDetectHelper< _Preprocessor, _Instance > *pHelper;

public:

    ObjectDetectorWrapper() : pHelper(0) { }
    
    ObjectDetectorWrapper(const char *filename) : pHelper(0) 
    { 
    ObjectDetectorParams dummy;
    addClassifier(filename, dummy);
    }
    virtual ~ObjectDetectorWrapper() {
        delete pHelper;
    }

    virtual int addClassifier(const char* pth_cls, const ObjectDetectorParams & params) {
        std::ifstream fin(pth_cls);

        if(fin)
        {
            template_geometry geom;

            std::string code;

            fin >> code;
            if (code != _Instance::signature())
                {
                  std::cerr << "error in loading " << pth_cls << ": incompatible classifier" << std::endl;
                  return -1;
                }

                
            fin >> geom.width;
            fin >> geom.height;

            if(pHelper == 0)
            {
                pHelper = new  ClassifierDetectHelper< _Preprocessor, _Instance>(fin);
            }
            else
            {
                char pconf[16];
                char conf[16];
                pHelper->GetConf(pconf); // save Conf
                // load new conf
                pHelper->Configure(fin);
                // 
                pHelper->GetConf(conf); // get new Conf
                                
                if(strcmp(pconf,conf)!=0)
                {
                    pHelper->Configure(pconf); // restore previous conf
                    std::cerr <<  "error in loading " << pth_cls << ": incompatible preprocessor" << std::endl;
                    return -1;
                }
            }

            // load classifier:
            _Instance ogg(fin);
            pHelper->m_classifs.push_back(ogg);
            pHelper->m_clsparams.push_back(params);
            pHelper->m_clsparams.back().sz = geom; // set the geometry 

            return pHelper->m_clsparams.size() - 1;
        }
        else {
            std::cerr << "error open "<<pth_cls<<std::endl;
            return -1;
        }
    }

    
    static std::string signature() {
        return _Instance::signature();
    }

    /// virtual
    std::string getSignature() const {
        return _Instance::signature();
    }

    void setParams(const ObjectDetectorGlobalParams & params) {
        pHelper->setParams(params);
    }
    const ObjectDetectorGlobalParams & getParams() const {
      return pHelper->getParams();
    }
    
    /// set per-classifier params
    void setClassifierParams(int index,  const ObjectDetectorParams & params) {
        pHelper->setClassifierParams(index, params);
    }
    /// get per-classifier params
    const ObjectDetectorParams & getClassifierParams(int index) {
        return pHelper->getClassifierParams(index);
    }
    
    
    virtual void Detect(std::vector<Candidates>& out, const ImageHandle & src) {
        pHelper->detect(out, src);
    }
};

#endif
