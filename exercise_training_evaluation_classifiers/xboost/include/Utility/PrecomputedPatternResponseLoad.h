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

#ifndef _PRECOMPUTED_PATTERN_RESPONSE_LOAD_H
#define _PRECOMPUTED_PATTERN_RESPONSE_LOAD_H

#include "Pattern/PrecomputedPatternResponse.h"
#include <fstream>
#include <sstream>
#include <map>

/** @file PrecomputedPatternResponseLoad.h
  * @brief this file provides some API for Load a PrecomputedPatternResponse
  **/

/** Load Data from a text file.
 * The file format must be
 * [n_samples] [n_features] [s0f0] [s0f1] .... [sNfM]
 * */
template<class ScalarType>
void LoadData(PrecomputedPatternResponse<ScalarType> & out, const char *filename)
{
    std::ifstream in(filename);

    int n_samples, n_features;
    in >> n_samples >> n_features;

    out.allocate(n_samples, n_features);

    for(int i =0; i<n_samples; ++i)
    {
        in >> out.category[i];
        for(int j=0; j<n_features; ++j)
            in >> out.response[i + j * n_samples];
    }
}

/// Load SVM Light file
template<class ScalarType>
void LoadSVMData(PrecomputedPatternResponse<ScalarType> & out, const char *filename)
{
    std::ifstream in(filename);

    std::vector< std::map<int, ScalarType> > m;
    std::vector< int> cat;
    int n_features = 0;

    std::string data;

    while(std::getline(in, data))
    {
        std::istringstream src(data);
        int c;
        src >> c;
        cat.push_back(c);

        std::map<int, ScalarType> f;

        while(src)
        {
            int index;
            char sep;
            ScalarType value;
            src>>index >> sep >> value;

            f[index] = value;
            if(index >= n_features)
                n_features = index+1;
        }

        m.push_back(f);
    }

    int n_samples = m.size();
    out.allocate(n_samples, n_features);

    for(int i =0; i<n_samples; ++i)
    {
        out.category[i] = cat[i];
        for(int j=0; j<n_features; ++j)
        {
            out.response[i + j * n_samples] = m[i][j];
        }
    }

}

#endif