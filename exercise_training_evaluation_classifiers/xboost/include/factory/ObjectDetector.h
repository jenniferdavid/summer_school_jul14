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

#ifndef _OBJECT_DETECTOR_H
#define _OBJECT_DETECTOR_H

#include <vector>
#include <iostream>
#include "Types.h"
#include "Image.h"
#include "Thread/thread.h" // hardware_concurrency

/** @file ObjectDetector.h
 * @brief virtual classes to work on classifier. 
 *        ObjectDetector exploits all the performance of classifier, reporting directly best candidates 
 * */

namespace detail {

/// parameters used in the NMS step
struct CandidateParam {
  /// detection scale
    float scale;
  /// classifier geometry
    size cl_geom;
  /// category
    int category;

};

}

/// the object reported by classifier
struct Candidates
{
    bool grouped;           ///< used internally by Group Rectangles
    
    int category;
    
    unsigned int flags;     ///< user flags

    double response;	    ///< response value

    int xr,yr;             ///< top left corner in response image coordinates

    float scale;           ///< detection scale

    rect box;              ///< the object area at full scale

public:
    void Set(int x, int y, double val, const detail::CandidateParam & param)
    {
        response = val;
        xr = x;
        yr = y;
        scale = param.scale;
        category = param.category;
        box.x0 = xr*scale;
        box.y0 = yr*scale;
        box.x1 = (xr+param.cl_geom.width)*scale;
        box.y1 = (yr+param.cl_geom.height)*scale;
        grouped = false;
        flags=0;
    }

};
//

/// some parameters for internal algorithm
struct ObjectDetectorParams {
  /// class associated with classifier
  int category;
  /// algorithm threshold
  double th;
  /// how many step per octave are used 
  int step_mode;
  /// response precision
  int downsampling;
  /// search ranges
  std::vector<std::pair<int,int> > m_srcSearchRanges;
  /// subregions ROI(s)
  std::vector<rect> m_searchRoi;

  /// (used internally) (read only) classifier geometry
  size sz;
  
  ObjectDetectorParams() : category(0), th(0.0), step_mode(1), downsampling(1), sz(0,0) { }
};

/// Over-Classifier params
struct ObjectDetectorGlobalParams {
  /// use octave or linear search mode (deprecated)
  int octave_mode;
  /// Number of Scales per Octave
  int nScales; 
  /// number of concurrent jobs
  int concurrent_jobs;

  
  ObjectDetectorGlobalParams() : octave_mode(1), nScales(4), concurrent_jobs(sprint::thread::hardware_concurrency() ) { }
};

/// "Binary" Classifier, virtual class, detect "object".
///  the class support multiple instance of the same classifier object.
class ObjectDetector {

public:

    virtual ~ObjectDetector();

    /// return the classifier signature
    virtual std::string getSignature() const = 0;

    /// Load and append an additional classifier to the internal processing pipeline
    /// @return the index associated with the classifier
    virtual int addClassifier(const char* pth_cls, const ObjectDetectorParams & params) = 0;

    /// set per-classifier params
    virtual void setClassifierParams(int index,  const ObjectDetectorParams & params) = 0;
    /// get per-classifier params
    virtual const ObjectDetectorParams & getClassifierParams(int index) = 0;

    /// Set the Global Params
    virtual void setParams(const ObjectDetectorGlobalParams & params) = 0;
    virtual const ObjectDetectorGlobalParams & getParams() const = 0;

    /// detect object 
    virtual void Detect(std::vector<Candidates>& out, const ImageHandle & src) = 0;

};


#endif
