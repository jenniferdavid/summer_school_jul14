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

#include "Oracle/WeightedPatternResponse.h"

void GenerateBucket(BinaryWeightedPatternResponse<int> *store, int n_bucket, const BinaryWeightedPatternResponse<int> *source, int n_element)
{
    int min_resp, max_resp;
    int bin_size;
    // compute value, min and max:
    min_resp = max_resp = source[0].value;
    for(unsigned int i =1; i < n_element; i++)
    {
          if(min_resp > source[i].value) min_resp = source[i].value;
          if(max_resp < source[i].value) max_resp = source[i].value;
    }
    
    // compute bin_size rounded (max - min + 1) elements
    bin_size = (max_resp - min_resp + 1 + n_bucket - 1) / n_bucket;

    // avoid division by zero:
    if(bin_size == 0)
      bin_size = 1;
    
    // setup store[i] memory
    for(int i = 0;i<n_bucket; ++i)
    {
      store[i].value = min_resp + bin_size * i;
      store[i].d = 0.0;
    }
    
    // fill histogram
    for(unsigned int i =0; i< n_element; i++)
    {
     int bin = (source[i].value - min_resp) / bin_size;
     store[bin].d += source[i].d;
    }    
  
}
