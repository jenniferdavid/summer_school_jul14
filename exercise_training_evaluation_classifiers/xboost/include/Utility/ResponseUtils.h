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

#ifndef _RESPONSE_UTILS_H
#define _RESPONSE_UTILS_H

/** @file ResponseUtils.h
 *  @brief helping method used during resposne computations
 * */


/** Compute the response of a classifier on a whole area
  * TODO: move to library
  * @param r output response
  * @param startH initial row (output image, response)
  * @param endH final row (output image, response)
  *
  * @param cl a Classifier object
  * @param out some parameters
  * @param step downsampling precision [both horizontal and vertical]
  * */
template<class ReturnType, class ClassifierType>
void workerResponse(double *r, int startH, int endH, int W,const ClassifierType & cl,const ReturnType & out, int step )
{
    for(int j=startH; j<endH; j++)
        for(int i=0; i<W; i++)
        {
            r[j*W+i] = cl(getData1(out.first,out.second,i*step,j*step),getData2(out.first,out.second));
        }
}

/** Compute the response of a classifier on a whole area
  * TODO: move to library
  * @param r output response
  * @param startH initial row
  * @param endH final row
  * @param cl a Classifier object
  * @param out some parameters
  * @param step downsampling precision [both horizontal and vertical]
  * */
template<class ReturnType, class ClassifierType>
void optimizedWorkerResponse(double *r, int startH, int endH, int W, const ClassifierType & cl,const ReturnType & out, int step )
{
    for(int j=startH; j<endH; j++)
        for(int i=0; i<W; i++)
        {
            r[j*W+i] = cl(getData1(out.first,out.second,i*step,j*step));
        }
}


#endif
