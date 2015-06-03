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

#ifndef _MULTICLASS_DATA_SET_H
#define _MULTICLASS_DATA_SET_H

/** @file MultiClassDataSet.h
 *  @brief declare a DataSet for Multi-Class problem */

#include <vector>
#include <_stdint.h>
#include <fstream>
#include <iostream>
#include <map>      // multimap
#include <cmath>
#include <stdlib.h> // rand

#include "bitset.h"

// fwd
template<class PatternType>
struct MultiClassDataSet;

/** A collection of "Patterns", preprocessed data.
  * 
  * Pattern contains all data useful for train (algorithm dependent data).
  * A dataset that collect pattern but not remove them in the destroyer automatically
  *
  * TODO: non e' aggiornata: ripartire da DataSet e riscriverla nuovamente
  * */
template<class _PatternType, class DataParams>
struct MultiClassDataSetHandle: public DataParams {

public:

    /// Inner DataType
    typedef typename _PatternType::DataType DataType;

    /// the pattern type
    typedef _PatternType PatternType;
    /// a soft copy
    typedef DataSetHandle<PatternType> ViewType;
    /// an hard copy
    typedef DataSet<PatternType> StoreType;

public:
    /// Size of Template (width and height have meaning only for images)
    /// TODO: channel feature for example have an additional dimension
    unsigned int width, height;

    /// Number of patterns in two dataset (Positive +1, Negative -1)
    /// TODO: in case of multiple classes that not have senses
    std::vector<unsigned int> n_pattern;

    /// a collection of Pattern used in this dataset
    std::vector<PatternType> templates;

public:

    MultiClassDataSetHandle(int n_classes) : width(0), height(0), n_pattern(n_classes) { }

    /// DataSet conversion
    template<class R>
    explicit MultiClassDataSetHandle( const MultiClassDataSetHandle<R> & src) : width(src.width), height(src.height),  n_pattern(src.n_pattern), templates(src.templates) { }

    // do nothing
    ~MultiClassDataSetHandle() { }

    /// Reset (but not release memory)
    void Clear()
    {
        width = height = 0;
        for(int i =0;i<n_pattern.size();++i)
          n_pattern[i] = 0;
        templates.clear();
    }

    /// training pattern geometry: width
    inline unsigned int Width() const  {
        return width;
    }
    /// training pattern geometry: height
    inline unsigned int Height() const {
        return height;
    }
    
    /// Return number of allocated samples (complete size of DataSet)
    inline unsigned int Size() const {
        return templates.size();
    }

    /** Set the geometry of patterns.
      * All pattern must have the same geometry. */
    void SetGeometry(unsigned int width, unsigned int height) {
        this->width = width;
        this->height = height;
    }

    /// add a new pattern to the list
    void Insert(const PatternType & p)
    {
      // NOTE: not boundary check
      n_pattern[p.category] ++;
      templates.push_back(p);
    }

    /// Force, manually, to release memory in data
    void Release()
    {
        // release pattern data memory
        for(typename std::vector<PatternType>::iterator i = templates.begin(); i!=templates.end(); ++i)
            i->release(); 
    }

    /// copy operator (do not release old memory)
    template<class R>
    void operator=( const DataSetHandle<R> & src) {
        width=src.width;
        height=src.height;
        n_pattern=src.n_pattern;
        templates.clear();
        templates.reserve(src.templates.size() );
        for(typename  std::vector<R>::const_iterator i = src.templates.begin(); i!=src.templates.end(); ++i)
            templates.push_back( *i );
    }

    /// Import a datatype+category
    ///   @note buf will be freed inside this library at release
    void Import(const DataType & data, int category);

};

/** an auto release DataSetHandle */
template<class PatternType>
struct MultiClassDataSet: public MultiClassDataSetHandle<PatternType> {
public:

    typedef MultiClassDataSetHandle<PatternType> ViewType;
    typedef MultiClassDataSet<PatternType> StoreType;

public:
    ~MultiClassDataSet()
    {
        this->Release();
    }

};


//////////////////////////////////////////////////////////////////////////////////////////////

template<class PatternType>
void MultiClassDataSetHandle<PatternType>::Import(const DataType & data, int sign)
{
    Insert( PatternType(data,sign) );
}


#endif

