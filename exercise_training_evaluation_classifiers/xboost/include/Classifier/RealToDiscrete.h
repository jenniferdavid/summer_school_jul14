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

#ifndef _real_to_discrete_h
#define _real_to_discrete_h
// parked here

/** convert a Real Classifier in a Dicrete One using a fixed threshold */
template<class Classifier>
struct RealToDiscrete : public Classifier {

public:
  
  typedef Classifier FeatureType;

public:
  
  RealToDiscrete(const Classifier & x) : Classifier(x) { }
  RealToDiscrete() { }
  ~RealToDiscrete() { }
  
  /// return the "feature" name (for debug purpose)
  const char *str() const {return Classifier::name; }
  
  /// signature name is the name of provider (for simplicity)
  static std::string signature() { return Classifier::signature() + "-discrete"; }

  /// Classify using a Decision Stump on Classifier response
  /// @return {-1,+1)
  /// esempio: true (+1) pedone, -1 non pedone
  inline int classify(const unsigned int *pIntImage, int stride) const
   {
   return ((Classifier::classify(pIntImage, stride) ) > 0.0) ? 1 : -1;
   }

  /// classify wrapper. @see classify
  inline int operator()(const unsigned int *pIntImage, int stride) const
   {
   return classify(pIntImage, stride );
   }

};

#endif
