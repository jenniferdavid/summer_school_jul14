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

#ifndef _COLLAPSED_HAAR_FEATURE_H
#define _COLLAPSED_HAAR_FEATURE_H

#include "HaarFeature.h"
#include <Descriptor/IntegralImageData.h>
#include "_stdint.h"

/** @file CollapsedHaarFeature.h
 * @brief an "optimized" haar feature
 * */

/// a precomputed HaarNode in order to boost evaluation performance
struct CollapsedHaarNode {
    /// accumulator
    int sign;
    /// offset
    int offset;

public:

    CollapsedHaarNode() { }
    CollapsedHaarNode(int _sign, long _offset) : sign(_sign), offset(_offset) { }

    inline int operator()(const unsigned int *pIntImage) const {
        return sign * (int) *((const uint32_t *) ( (const uint8_t *)(pIntImage)+offset ) ) ;
    }
};

/// an optimized HaarFeature
struct CollapsedHaarFeature {
    std::vector<CollapsedHaarNode> m_nodes;
    
    /// 
    typedef CollapsedHaarFeature OptimizedType;
public:
  
  // TODO:
  CollapsedHaarFeature() { }

    /** compute an optimized version from an HaarFeature
     * TODO: separate scale and offset+stride
     */
    CollapsedHaarFeature(const HaarFeature &src, int scale, long offset, long stride)
    {
        m_nodes.resize(src.size());
        int j=0;
        for(std::vector<HaarNode>::const_iterator i=src.begin(); i!=src.end(); ++i)
        {
            int x = (int)(i->x * scale + scale - 1);
            int y = (int)(i->y * scale + scale - 1);
            m_nodes[j].sign = i->sign;
            m_nodes[j].offset = ( offset +  x + y * stride )  * sizeof(uint32_t);
            ++j;
        }
    }

    CollapsedHaarFeature(const HaarFeature &src, int offset, const IntegralImageParams & p)
    {
        m_nodes.resize(src.size());
        int j=0;
        for(std::vector<HaarNode>::const_iterator i=src.begin(); i!=src.end(); ++i)
        {
            m_nodes[j].sign = i->sign;
            m_nodes[j].offset = ( offset + i->x + i->y * p.stride )  * sizeof(uint32_t);
            ++j;
        }
    }

    
    /** compute an optimized version from an HaarFeature */
    CollapsedHaarFeature(const HaarFeature &src, long offset, long stride)
    {
        m_nodes.resize(src.size());
        int j=0;
        for(std::vector<HaarNode>::const_iterator i=src.begin(); i!=src.end(); ++i)
        {
            int x = i->x;
            int y = i->y;
            m_nodes[j].sign = i->sign;
            m_nodes[j].offset = ( offset +  x + y * stride )  * sizeof(uint32_t);
            ++j;
        }
    }

    ~CollapsedHaarFeature()
    {
    }

/// invert the sign of haar nodes (in order to simplify the decision stump)
    void invert_polarity()
    {
        for(std::vector<CollapsedHaarNode>::iterator i=m_nodes.begin(); i!=m_nodes.end(); i++)
            i->sign = - i->sign;
    }

    inline int debug(const unsigned int *pIntImage) const
    {
        int sum = 0;
//   std::cout << "[" << m_nodes[0].offset << "]";
        for(std::vector<CollapsedHaarNode>::const_iterator i=m_nodes.begin(); i!=m_nodes.end(); i++)
            sum += (*i)(pIntImage);
        return sum;
    }


/// compute the response
    inline int response(const unsigned int *pIntImage) const
    {
        int sum = 0;
        for(std::vector<CollapsedHaarNode>::const_iterator i=m_nodes.begin(); i!=m_nodes.end(); i++)
            sum += (*i)(pIntImage);
        return sum;
    }

    inline int operator() (const unsigned int *pIntImage) const
    {
        return response(pIntImage);
    }

};

inline std::ostream & operator << (std::ostream & out, const CollapsedHaarFeature & s)
{
    return out;
}

#endif