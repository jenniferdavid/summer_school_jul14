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

#ifndef _FEATURE_GENERATOR_UTILS_H
#define _FEATURE_GENERATOR_UTILS_H

/** @file FeatureGeneratorUtils.h
 * @brief function to handle FeatureGenerator objects
 * */

#include <vector>

/// Extract all features from a feature generator and store them in a vector
template<class FeatureGeneratorType>
void ExtractAllFeatures(std::vector< typename FeatureGeneratorType::FeatureType > & feats, FeatureGeneratorType & feature_generator)
{
        typename FeatureGeneratorType::FeatureType feat;

        feature_generator.Reset();

        // create all features
        while( feature_generator.Next(feat) )
        {
            feats.push_back(feat);
        }
}
        
#endif        