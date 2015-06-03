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

#ifndef _HAAR_FEATURE_TOOLS_H
#define _HAAR_FEATURE_TOOLS_H

/** @file HaarFeatureTools.h
  * @brief this file implement some debug method usefull for presentation or debugging of Haar Feature
  **/

#include "Feature/HaarFeature.h"

/** Generate a 'Feature Img' from an HaarFeature
  * @param filename an output pgm file
  **/
void DumpFeatureImg(const char *filename, const HaarFeature & out, int w, int h);

/** Generate a 'Stochastic Img' where Haar features are fused according to alpha values */
void AddStochasticImg(double *img, double alpha, const HaarFeature & out, int w, int h);

/** dump on a pgm image the 'stochastic img' generate by subsequent call to AddStochasticImg */
void DumpStochasticImg(const char *filename, const double *img, int w, int h);

#endif
