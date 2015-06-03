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

#ifndef _HAAR_FEATURE_GENERATORS_H
#define _HAAR_FEATURE_GENERATORS_H

/** @file HaarFeatureGenerators.h
  * @brief this file generates all the possible haar feature
  **/

#include "HaarFeatureGenerator.h"

/** haar feature generator callback */
typedef bool (* FeatureGenerator_t)(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph);

/** feature description (name) and generator */
struct FeatureDescription {
   const char *name;
   FeatureGenerator_t proc;
 };  

/// a list of feature generator for Haar Feature
extern const FeatureDescription featureDescription[];

/// Default pattern with all iso energetic pattern
extern const char iso_pattern[];

/** the usual default 2h2v3H3V4q */
extern const char standard_pattern[];

/// convert from a 2char string to Feature generator
const FeatureDescription *findFeature(const char *str);

/// Base Feature Generator for Haar Bases using FeatureGenerator_t
struct BaseHaarFeatureGenerator {
  public:
    
  /// pattern size
  unsigned int m_mw,m_mh;
  
  /// dimensione minima
  unsigned int m_minFeatArea;
  
  /// min Width/Height
  unsigned int m_minWidth;
  
  /// Step for feature search
  unsigned int m_step;

  /// lista di feature generator allocati
  std::vector<FeatureGenerator_t> m_feats;

  public:
  /// a default pattern for all features in library
  static const char *default_pattern;

  public:
    BaseHaarFeatureGenerator(const char *pattern);

  /// Width Height
  void SetGeometry(int mw, int mh);
      
/// A constraint on minimum area of a feature (width x height > minArea)
  void SetFeatureMinArea(int minArea) { m_minFeatArea = minArea; }
  /// Set the minimum feature size
  void SetFeatureMinWidth(int minWidth) { m_minWidth = minWidth; }
  
  /// searching step
  void SetStep(int step) { m_step = step; }
  
  /// set a border around patch (in Haar normally have not meaning)
  void SetBorder(int border) { } 
  
/// Generate a classifier able to analize a width x height area from order idx
/// @param out an Haar Feature
/// @param x,y,w,h coverage of feature
/// @param idx feature index (see table)
 bool generate(HaarFeature & f, int x, int y, int w, int h, int idx);

 /// Return the generator geometry
  unsigned int W() const { return m_mw; }
  unsigned int H() const { return m_mh; }
  unsigned int Size() const { return m_mw*m_mh; }

/// Return the number of feature in the vector
  unsigned int Feature() const { return m_feats.size(); }

};

/// A library able to generate any Haar Feature
class BruteForceFeatureGenerator: public BaseHaarFeatureGenerator, public HaarFeatureGenerator {
  unsigned int m_featCount; ///< current feature count
  
  /// Stored Feature
  std::vector<HaarFeature> m_features;  
  
public:
  
  BruteForceFeatureGenerator(const char *pattern) : BaseHaarFeatureGenerator(pattern), m_featCount(0) { }
  virtual ~BruteForceFeatureGenerator();
  
  void SetGeometry(int mw, int mh)
  {
    BaseHaarFeatureGenerator::SetGeometry(mw,mh);
    m_features.clear();
  }
  
   void SetFeatureMinArea(int minArea) { 
    BaseHaarFeatureGenerator::SetFeatureMinArea(minArea);
    m_features.clear();
   }
  
  void Reset();

  unsigned int Count() const { return m_features.size(); }
  
 /// Provide a new feature, or return false
  bool Next(HaarFeature & out);
};

/// This generator, instead of exhaustive, use a Random sampling approach.
class RandomFeatureGenerator: public BaseHaarFeatureGenerator, public HaarFeatureGenerator {
  /// number of feature to be generated
  int m_featRand;
  
  /// current count
  int m_cur;

  public:
  
  typedef HaarFeature FeatureType; 
  
  public:
  RandomFeatureGenerator(const char *pattern, int nFeat) : BaseHaarFeatureGenerator(pattern), m_cur(0), m_featRand(nFeat) { } 
  virtual ~RandomFeatureGenerator();
  
  unsigned int Count() const { return m_featRand; }
  bool Next(HaarFeature & out);
  void Reset();
};

/// This generator, instead of exhaustive, use a Random sampling approach
class JointSparseGranularFeatureGenerator: public HaarFeatureGenerator {

  /// number of feature to be generated
  int m_featRand;
  
  /// current count
  int m_cur;
  
  int m_width;
  int m_height;
  
  int m_minArea;
  
  int m_step;
  
  public:
  
  typedef HaarFeature FeatureType; 
  
  public:
    
  JointSparseGranularFeatureGenerator(int n) : m_minArea(0), m_featRand(n), m_cur(0), m_step(1) { }
  virtual ~JointSparseGranularFeatureGenerator();

  unsigned int Count() const { return m_featRand; }
  
  void SetGeometry(int mw, int mh) { m_width = mw; m_height = mh; }
      
/// A constraint on minimum area of a feature (width x height > minArea)
  void SetFeatureMinArea(int minArea) { m_minArea = minArea; }
  
  /// searching step
  void SetStep(int step) { m_step = step; }  

  /// set a border around patch (in Haar normally have not meaning)
  void SetBorder(int border) { } 
  
 /// Return the generator geometry
  unsigned int W() const { return m_width; }
  unsigned int H() const { return m_height; }
  
  /// generate a Random Feature
  bool Next(HaarFeature & out);
  void Reset();  
};



#endif
