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

#ifndef _ADABOOST_COMMON_H
#define _ADABOOST_COMMON_H

/** @file AdaBoostCommon.h
 * @brief a file containing misc type */

#include <Pattern/Pattern.h>
#include <Pattern/WeightedPattern.h>
#include <Pattern/Aggregator.h>

#include <Preprocessor/IntegralImagePreprocessors.h>
#include <Preprocessor/IntegralChannelImagePreprocessor.h>

/// an Aggregagtor specific for AdaBoost, with a WeightedPattern PatternType
template<class PreprocessorType>
struct AdaBoostPatternAggregator {
  
  /// The inner Aggregator:
  typedef PatternAggregator< WeightedPattern< typename PreprocessorType::DataType >, typename PreprocessorType::ParamType  > AggregatorType;
};

/// Pattern involved in Haar+AdaBoost training procedure
// typedef WeightedPattern< IntegralImageData > HaarAdaBoostPattern;

/// Haar+AdaBoost Weight 
typedef AdaBoostPatternAggregator< IntegralImagePreprocessor >::AggregatorType HaarAdaBoostAggregator;

/// ICF+AdaBoost Weight 
typedef AdaBoostPatternAggregator< IntegralChannelImagePreprocessor >::AggregatorType ICFAdaboostAggregator;
//

/// DataSet usend internally in Haar+AdaBoost
typedef DataSetHandle<HaarAdaBoostAggregator> HaarAdaBoostDataSetHandle;
  
#endif
