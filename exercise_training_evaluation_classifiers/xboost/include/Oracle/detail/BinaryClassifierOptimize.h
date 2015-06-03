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

#ifndef _BINARY_CLASSIFIER_OPTIMIZER_H
#define _BINARY_CLASSIFIER_OPTIMIZER_H

/// found the best threshold who maximize weights
/// @param ret the output, first a threshols and second a polarity
/// @param data an ordered vector of pair of data and weight. weights must be premultiplied by polarity of the category (positive for positive class, negative for negative class)
/// @param n number of data elements
/// @param wp,wn precomputed weights of positive and negative elements
/// @todo due to the genericity of the function, move it in library
/// @param strictly_growin guarantee strictly growing parameters
template<class DataType, class ScalarType>
ScalarType Optimize(std::pair<DataType, int> & ret, const std::pair<DataType, ScalarType> *data, int n, ScalarType wp, ScalarType wn, bool strictly_growin)
{
    ScalarType w, curW; // current best w
    ScalarType sumW = wp + wn;

    // initial "bestW" using external border (all NEGs)
    ret.first = data[0].first; // th is the global minimum value
    ret.second = 1; // v < th ? 1 : -1
    curW = wn;

    if(strictly_growin)
    {
        w = wn;
        for(int i=0; i<n-1; i++)
        {
            w += data[i].second; // se prendo un POS alzo w, se prendo un NEG abbasso w
            if(w>curW) // parity +1
            {
                curW = w;
                ret.first = data[i+1].first;
                ret.second = 1;
            }
            if((sumW-w)>curW) // parity -1
            {
                curW = sumW-w;
                ret.first = data[i].first;
                ret.second = -1;
            }
        }
    }
    else
    {
        w = wn;
        for(int i=1; i<n; i++)
        {
            w += data[i-1].second; // se prendo un POS alzo w, se prendo un NEG abbasso w
            // (boundary check) se e' cresicuto value, solo ora faccio il check
            if(data[i].first>data[i-1].first)
            {
                // 1) search the maximum of W in the case v < th
                if(w>curW)
                {
                    curW = w;
                    ret.first = data[i].first;
                    ret.second = 1;
                }
                // 2) opposite
                if((sumW-w)>curW)
                {
                    curW = sumW-w;
                    ret.first = data[i-1].first;
                    ret.second = -1;
                }
            }
        }
    }

    return curW; // best w found
}

/// Optimize a simple scalar @a data
/// @param ret return with the result of optimization, in first the index of the best threshold, in second the polarity
template<class ScalarType>
ScalarType SimpleOptimize(std::pair<int, int> & ret, const ScalarType *data, int n, ScalarType wp, ScalarType wn)
{
    ScalarType w, curW; // current best w
    ScalarType sumW = wp + wn;

    // initial "bestW" using external border (all NEGs)
    ret.first = 0; // th is the global minimum value
    ret.second = 1; // v < th ? 1 : -1
    curW = wn;

    w = wn;
    for(int i=0; i<n-1; i++)
    {
        w += data[i]; // se prendo un POS alzo w, se prendo un NEG abbasso w
        if(w>curW) // parity +1
        {
            curW = w;
            ret.first = i+1;
            ret.second = 1;
        }
        if((sumW-w)>curW) // parity -1
        {
            curW = sumW-w;
            ret.first = i;
            ret.second = -1;
        }
    }
    return curW; // best w found
}

#endif
