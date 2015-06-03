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

#ifndef _PATTERN_AGGREGATOR_H
#define _PATTERN_AGGREGATOR_H

/** @file Aggregator.h
 * @brief macro used to declare a PatternAggregator
 * */

/// Help declaring some traits
template<class TPatternType, class TParamType>
struct PatternAggregator {
  typedef TPatternType PatternType;
  typedef TParamType ParamType;
  typedef typename TPatternType::DataType DataType;
};

#define DECLARE_AGGREGATOR \
    typedef typename Aggregator::PatternType PatternType; \
    typedef typename Aggregator::ParamType ParamType; \
    typedef typename Aggregator::DataType DataType; 
    

#endif
