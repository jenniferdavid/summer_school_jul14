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

/** @file FeatureGenerator.h
  * @brief method to generate Feature
  */

#ifndef _FEATURE_GENERATOR_H
#define _FEATURE_GENERATOR_H

#include "Utility/timer.h"
#include "Utility/random.h"


/** Virtual Feature Generator 
 *  A generic interface class capable of generate a Feature object 
  */
template<class Feature>
class FeatureGenerator {

public:

  /// The feature type generate by this generator
  typedef Feature FeatureType; 
    
public:
  virtual ~FeatureGenerator() { }

  /// Test if the feature generator can be precomputed. In this case Count must be valid.
  virtual bool CanBePrecomputed() const { return false; }
  /// return the count of feature available
  virtual unsigned int Count() const = 0;
  /// return the next feature, or return false
  virtual bool Next(Feature & out) = 0;
  /// reset any interal counters
  virtual void Reset() = 0;
};


////////////////////////////////////////////////////////////////////////////////////

/** random samples a vector of features */
template<class T>
bool RandomSampleFeature(std::vector<T> & out, const std::vector<T> & in, int n)
{
  std::vector<bool> sampled;
  if(in.size()<=n)
  {
    std::cerr << "Error: too few features" << std::endl;
    out = in;
    return false;
  }
  out.clear();
  out.reserve(n);
  for(int i=0;i<n;++i)
    out.push_back( in[ nrand(in.size() )  ] );
  return true;
}

/// Fill vector of FeatureType using a FeatureGenerator object
template<class FeatureGenerator>
void GenerateFeatures(std::vector<typename FeatureGenerator::FeatureType> & features, FeatureGenerator & generator)
{
  Timer t;
  typename FeatureGenerator::FeatureType h;
//   PatternResponse *pr = 0;
  
//   if(prune_weak_classifier)
//     pr = new PatternResponse[m_validate_templates.size()];
// 
  std::cout << "Generating feature from " << generator.Feature() << " haar bases..." << std::endl;

  int count = 0;

  t.Start();
  generator.Reset();
  while(generator.Next(h) ) {
    
    features.push_back(h);
    
    count++;
    if((count & (4096-1))==0)
      {
	std::cout << '.'; 
	std::cout.flush();
      }    
    }	
  std::cout << count << " feature generated in " << t.GetTime() << "s. Keeped " << features.size() << "\n";

//   delete [] pr;
}

/// This generator uses a stored feature list
template<class TFeature>
class StoredFeatureGenerator {
  typename std::vector<TFeature>::const_iterator i;

  public:
  
  std::vector<TFeature> features;
  /// only for cosmetic
  int n_bases;

  public:
  
  typedef TFeature FeatureType; 
  
  public:
    
    StoredFeatureGenerator() : n_bases(0) { }
    ~StoredFeatureGenerator() { }
 
  /// number of bases (cosmetic)
  unsigned int Feature() const { return n_bases; } 
  
  void Reset() {
    i = features.begin();
  }
  bool Next(TFeature & out)
  {
    if (i==features.end()) return false;
    
    out = *i;
    ++i;
    return true;
  }
};

#endif
