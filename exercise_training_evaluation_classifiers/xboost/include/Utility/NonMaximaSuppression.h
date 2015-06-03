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

#ifndef _NON_MAXIMA_SUPPRESSION_H
#define _NON_MAXIMA_SUPPRESSION_H

/** @file NonMaximaSuppression.h
 * @brief Implement a local maxima search algorithm
 * */

#include <vector>

#ifdef _MULTITHREAD
# include "Thread/thread.h"
# include "Thread/thread_group.h"
# include "Thread/bind.h"
#endif

/** Non Maxima Suppression (without mask)
 * This algorithm find local maxima of a map @a src of geometry @a width x @a height
 * Two maxima cannot be near more than @a n and have a value less than a @a threshold
 * @param src an image (int, double, float, etc etc)
 **/
template<class D, class FeatureListType, class Param>
void NonMaximaSuppression(const D *src, FeatureListType &maxima, unsigned int n, long stride, unsigned int width, unsigned int height, D threshold, Param param, int nThreads = sprint::thread::hardware_concurrency() );

/******************** implementation ***************************************/

namespace detail {

/// The internal NM class
template<class D, class FeatureListType, class Param>
struct LocalMaxima {
    /// The buffer
    const D *m_src;
    /// nms step
    int n;
    /// stride
    long stride;
    /// horizontal number of pixel to be processed
    int width;
    /// clamping value
    int height;
    /// optional data used by Set Method
    Param id;
    /// threshold
    D threshold;
public:

/// Internal Non Maxima Suppression
/// @param j0,j1 sono le righe su cui eseguire la NMS
/// @bug omit local maxima in on the right border if width is not a perfect mutipler of n+1
    void *Process(FeatureListType &maxima, int j0, int j1) const
    {
        const D * src = m_src + j0 * stride; // initial row
        typedef typename FeatureListType::value_type FeatureType;
        // i need at least n+1 elements in the pool
        for(int j=j0; j<j1; j+=n+1)
        {
            int j_max = std::min(j+n, height-1); // size of slice

            for(int i=0; i<width; i+=n+1)
            {
                // src punta sempre alla riga (j)
                const D *ptr;
                int pi, pj;
                D pval;
                
                int i_max = std::min(i+n, width-1); // remaing part of cell

                ptr = src; // ptr deve puntare sempre alla riga di inizio, con i invece relativo al tipo di elaborazione
                pval = ptr[i]; // initialized with (i,j)
                pi = i;
                pj = j;
                // per ogni cella n x n cerco il massimo assoluto
                for(int j2=j; j2<=j_max; ++j2)
                {
                    for(int i2=i; i2<=i_max; ++i2)
                    {
                        D cval = ptr[i2];
                        // il punto deve essere maggiore o uguale al precedente per essere valido
                        // POSITIVI
                        if( cval>pval) {
                            pi   = i2;
                            pj   = j2;
                            pval = cval;
                        }
                    }
                    ptr += stride;
                }

                if(pval<threshold)
                    goto pfailed;

                {
                  // clip the search area inside (0,0)-(width-1,height-1)
                    int j2_0 = pj - (int) n;
                    if(j2_0 < 0) j2_0 = 0;
                    int j2_1 = pj + (int) n;
                    if(j2_1 >= height) j2_1 = height-1;

                    int i2_0 = pi - (int) n;
                    if(i2_0 < 0) i2_0 = 0;
                    int i2_1 = pi + (int) n;
                    if(i2_1 >= width) i2_1 = width-1;

                    // siccome src punta sempre a j devo togliere j, se voglio lavorare con pj
                    ptr = src + (j2_0 - j ) * stride;

                    // TODO: split in 4 cluster to avoid internal if
                    for (int32_t j2=j2_0; j2<=j2_1; j2++) {
                        for (int32_t i2=i2_0; i2<=i2_1; i2++) {
                            if (i2<i || i2>i+(int)n || j2<j || j2>j+(int)n) {
                                D cval = ptr[i2];
                                if (cval>pval)
                                    goto pfailed; // abort
                            }
                        }
                        ptr += stride;
                    }
                }

                // POSITIVI
                if(pval>=threshold)
                {
                    FeatureType p;
                    p.Set(pi, pj, pval, id);
                    maxima.push_back( p );
                }

pfailed:
                ;

            }

            src+= (stride * (n+1));
        }

        return 0;
    }

};

} //detail


// multithread implementation:

template<class D, class FeatureListType, class Param>
void NonMaximaSuppression(const D *src, FeatureListType &maxima, unsigned int n, long stride, unsigned int width, unsigned int height, D threshold, Param param, int nThreads )
{
    detail::LocalMaxima<D, FeatureListType, Param> nms;

    nms.m_src = src;
    nms.n = n;
    nms.height = height;
    nms.width = width;
    nms.threshold = threshold;
    nms.id = param;
    nms.stride = stride;

    // number of active "blocks" n+1 size
    int32_t active = (int)height / ((int)n+1);

    // at least 3 chunk per thread TODO
    if((active > 0) && (nThreads>1) && (active>=3*nThreads) )
    {
        sprint::thread_group thread_pool_;
        FeatureListType *outputs = new FeatureListType[nThreads];

        for(int k=0; k<nThreads; ++k)
        {
            uint32_t k0 = (active * k) / nThreads;	 // prima riga da assegnare a questo core
            uint32_t k1 = (active * (k+1) ) / nThreads; // ultima riga (non compresa) da assegnare a questo core

            thread_pool_.create_thread( sprint::thread_bind(  &detail::LocalMaxima<D, FeatureListType, Param>::Process, &nms, &outputs[k], (int) k0 * (n+1), (int) k1 * (n+1)));
        }

        thread_pool_.join_all();

        for(int k =0; k<nThreads; ++k)
        {
            maxima.insert(maxima.end(), outputs[k].begin(),outputs[k].end());
        }

        delete [] outputs;
    }
    else
    {
        nms.Process(maxima, 0, (int) height);
    }
}

#endif
