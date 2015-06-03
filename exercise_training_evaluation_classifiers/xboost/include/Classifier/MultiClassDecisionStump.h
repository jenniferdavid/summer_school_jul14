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

#ifndef _MULTICLASS_DECISIONSTUMP_H
#define _MULTICLASS_DECISIONSTUMP_H

/** @file MultiClassDecisionStump.h
  * @brief A (really) weak classifier for Multi-Class Problem
  * Implements some MultiClassDecisionStump.
  **/

#include <string>
#include <iostream>
#include <vector>

/// A simple decision stump for multiclass problem using only one threshold for all problems
template<class DataType>
struct MultiClassDecisionStumpSingleThreshold {

    /// threshold value
    DataType th;

    /// positive or negative response to the class
    /// TODO: use a bitset
    std::vector<int> responses;
public:
  
    /// initialize the vector memory
    MultiClassDecisionStumpSingleThreshold(int n_classes) : responses(n_classes) { }

    static std::string signature() {
      return "mdecisionstumpst";
    }
    
    /// convert the feature value to {-1,+1} using internal threshold
    inline void evaluate_feature(int *response, DataType value) const
    {
        if(value > th)
        {
            for(int k =0; k<responses.size(); ++k)
                response[k] = responses[k];
        }
        else
        {
            for(int k =0; k<responses.size(); ++k)
                response[k] = -responses[k];
        }
    }

};

/// A simple decision stump for multiclass problem using only one threshold for each class
/// NOTE: really hard to implement
template<class DataType>
struct MultiClassDecisionStumpMultiThreshold {

    /// positive or negative response to the class and associated threshold
    /// NOTE: if feature extractor implements "parity" this part can be simplified
    std::vector<std::pair<DataType, int> > responses;
public:
  
    /// initialize the vector memory
    MultiClassDecisionStumpMultiThreshold(int n_classes) : responses(n_classes) { }
  
    static std::string signature() {
      return "mdecisionstumpmt";
    }
  
    /// convert the feature value to {-1,+1} using internal threshold
    inline void evaluate_feature(int *response, DataType value) const
    {
        for(int k =0; k<responses.size(); ++k)
           if(value > responses[k].first) // TODO: parity
            {
                response[k] = responses[k].second;
            }
            else
            {
                response[k] = -responses[k].second;
            }
    }

};


template<class DataType>
inline std::istream & operator >> (std::istream & in, MultiClassDecisionStumpSingleThreshold<DataType> & s)
{
    in >> s.th;
    for(int i = 0;i<s.responses.size(); ++i)
      in >> s.responses[i];
    
    return in;
}

template<class DataType>
inline std::ostream & operator << (std::ostream & out, const MultiClassDecisionStumpSingleThreshold<DataType> & s)
{
    out << s.th;
    
    for(int i = 0;i<s.responses.size(); ++i)
      out << ' ' << s.responses[i];
    
    return out;
}

template<class DataType>
inline std::istream & operator >> (std::istream & in, MultiClassDecisionStumpMultiThreshold<DataType> & s)
{
    for(int i = 0;i<s.responses.size(); ++i)
      in >> s.responses[i].first >>  s.responses[i].second;
    
    return in;
}

template<class DataType>
inline std::ostream & operator << (std::ostream & out, const MultiClassDecisionStumpMultiThreshold<DataType> & s)
{
    for(int i = 0;i<s.responses.size(); ++i)
    {
      if(i!=0)
        out << ' ';
      out << s.responses[i].first << ' ' << s.responses[i].second;
    }
    
    return out;
}

#endif
