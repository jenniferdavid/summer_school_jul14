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

#ifndef _INTEGRAL_CHANNEL_FEATURE_GENERATORS_H
#define _INTEGRAL_CHANNEL_FEATURE_GENERATORS_H

/** @file IntegralChannelFeatureGenerators.h
  * @brief this file generates all the possible ichn feature
  **/

#include "IntegralChannelFeatureGenerator.h"
#include "Types.h"

/// Base Feature Generator for IntegralChannelFeatureGenerators
struct BaseIntegralChannelFeatureGenerator: public IntegralChannelFeatureGenerator {
public:

    /// rectangle geometry
    unsigned int m_mw, m_mh;

    /// number of channel
    unsigned int m_nchannel;

    /// dimensione minima
    unsigned int m_minFeatArea;

    /// min Width/Height
    unsigned int m_minWidth;

    /// Search Step
    unsigned int m_step;
    
    /// a Border where skip elements
    unsigned int m_border;

public:

    typedef IntegralChannelFeature FeatureType;

public:
    BaseIntegralChannelFeatureGenerator() : m_border(0) { }
    ~BaseIntegralChannelFeatureGenerator();

    /// Width Height Channels
    void SetGeometry(unsigned int mw, unsigned int mh, unsigned int n);

/// A constraint on minimum area of a feature (width x height > minArea)
    void SetFeatureMinArea(int minArea) {
        m_minFeatArea = minArea;
    }
    /// Set the minimum feature size
    void SetFeatureMinWidth(int minWidth) {
        m_minWidth = minWidth;
    }
    /// Set the search step
    void SetStep(int step) {
        m_step = step;
    }
    
    /// Set a skip border
    void SetBorder(int border) { 
      m_border = border; 
    }

/// Return the generator geometry
    unsigned int W() const {
        return m_mw;
    }
    unsigned int H() const {
        return m_mh;
    }
    unsigned int N() const {
        return m_nchannel;
    }
    unsigned int Size() const {
        return m_mw*m_mh*m_nchannel;
    }

    void SetParams(int nchn, int w,int h)
    {
        m_nchannel = nchn;
        m_mw = w;
        m_mh = h;

    }


};

/// A library able to generate any IC Feature
class BruteForceIntegralChannelFeatureGenerator: public BaseIntegralChannelFeatureGenerator {
    unsigned int m_featCount; ///< current feature count

    /// precomputed features
    std::vector< rect> m_rects;

public:

    BruteForceIntegralChannelFeatureGenerator() : BaseIntegralChannelFeatureGenerator(), m_featCount(0) { }
    virtual ~BruteForceIntegralChannelFeatureGenerator();

    void Reset();

    unsigned int Count() const {
        return m_rects.size() * m_nchannel;
    }



/// Provide a new feature, or return false
    bool Next(IntegralChannelFeature & out);
};

/// This generator, instead of exhaustive, use a Random sampling approach.
class RandomIntegralChannelFeatureGenerator: public BaseIntegralChannelFeatureGenerator {
    /// number of feature to be generated
    int m_featRand;

    /// current count
    int m_cur;

public:
    RandomIntegralChannelFeatureGenerator(int nFeat) : BaseIntegralChannelFeatureGenerator(), m_cur(0), m_featRand(nFeat) { }
    virtual ~RandomIntegralChannelFeatureGenerator();

    unsigned int Count() const {
        return m_featRand;
    }
    bool Next(IntegralChannelFeature & out);
    void Reset();

};

#endif
