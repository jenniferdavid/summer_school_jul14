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

#ifndef _PATTERN_H
#define _PATTERN_H

/** @file Pattern.h
 *  @brief implement the generic pattern object */

/** a Pattern object used in training. Pattern extend Descriptor (Descriptor or Precomputed data) with the category identifier.
 *  The Descriptor contains all "variable" data associated to the pattern.
 *
 * This class is normally used for load pattern before the training (or used inside Decision Tree when weight patterns are not required).
 * Un pattern e' normalmente l'oggetto che viene caricato da File.
 * 
 * @note width, stride, height of descriptor are stored elsewhere to reduce memory consumption (see Descriptor::ParamType)
 *
 * @param Descriptor a Descriptor (for example IntegralImageData object)
 *
 * */
template<class Descriptor>
struct Pattern: public Descriptor {

    /// Pattern category (0..k-1 for multiclass problem, or -1|+1 for binary problem)
    int category;

    /// weight associated to this pattern
    float weight;

public:

    /// Data Type
    typedef Descriptor DataType;

public:

  Pattern() { }
  Pattern(const Descriptor & d, int cat) : Descriptor(d), category(cat) { }
  
};


#endif
