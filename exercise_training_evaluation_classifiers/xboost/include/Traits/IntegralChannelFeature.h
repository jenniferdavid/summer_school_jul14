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

#ifndef _INTEGRAL_CHANNEL_FEATURE_TRAITS_H
#define _INTEGRAL_CHANNEL_FEATURE_TRAITS_H

/** @file IntegralChannelFeature.h
 * @brief Traits for ICF
 * */

#include "Feature/IntegralChannelFeature.h"
#include "DataSet.h"
#include "Pattern/Pattern.h"
#include "SourceDataSet.h"
#include "Descriptor/IntegralImageData.h"
#include "Preprocessor/IntegralChannelImagePreprocessor.h"
#include "FeatureGenerator/IntegralChannelFeatureGenerator.h"
#include "Traits.h"

namespace traits {
  class IntegralChannelFeature {
  public:

    /// Preprocessor used to obtain data
    typedef IntegralChannelImagePreprocessor PreprocessorType;

    /// Data provided by this Preprocessor
    typedef IntegralImageData DataType;

    /// complete type loaded by file
    typedef Pattern<IntegralImageData> PatternType;

    /// parameters used to store data and use with feature extractor
    typedef IntegralChannelImageParams ParamType;

    typedef IntegralChannelFeatureGenerator FeatureGeneratorType;

    // additional traits
    DECLARE_BASE_TRAITS

  };
}


// extendend integral image
// param1: PTR (1,1)
inline const uint32_t * getData1(const IntegralImageData & data, const IntegralChannelImageParams & params)
  {
    return data.data + 1 + params.stride1;
  }
inline const uint32_t * getData1(const IntegralImageData & data, const IntegralChannelImageParams & params, int x0, int y0)
  {
    return data.data + 1 + x0 + (1 + y0)* params.stride1;
  }

// param2: STRIDE
inline const std::pair<int32_t,int32_t> getData2(const IntegralImageData & data, const IntegralChannelImageParams & params)
  {
    return std::pair<int32_t,int32_t>(params.stride1, params.stride2);
  }


#endif
