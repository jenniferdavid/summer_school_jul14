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

#ifndef _DATA_SET_H
#define _DATA_SET_H

/** @file DataSet.h
 *  @brief declare a DataSet */

#include <vector>
#include "Types.h"
#include "Pattern/Aggregator.h"

// fwd
template<class Aggregator>
struct DataSet;

/** A collection of "Patterns", preprocessed data.
  *
  * Pattern contains all data usefull for train (algorithm dependent data).
  * A dataset that collect pattern but not remove them in the destroyer automatically
  * @note only for binary classifier, for multiclass problem @see MultiClassDataSetHandle
  *
  * Il DataSet deve contenere un vettore di Dati+DatiExtraAlgoritmo oltre ai Parametri per poter estrarre le feature dai Dati.
  *  Permette la conversione tra DataSetHandle, copiando solo il descrittore.
  *
  * @a TPatternType the type of pattern. Must have at least category member.
  * @a TParamType parameters of DataType (inside Pattern) to declare the data (for example width,height). Parameters common to all the DataType
  *
  * [Training]
  * */
template<class Aggregator >
struct DataSetHandle: public Aggregator::ParamType {

public:
    /// a soft copy of the set
    typedef DataSetHandle<Aggregator> ViewType;

    /// an hard copy of the set
    typedef DataSet<Aggregator> StoreType;

    // additional Typedef
    DECLARE_AGGREGATOR

public:

    /// Number of patterns in two dataset (P: Positive, N: Negative)
    /// @see MultiClassDataSetHandle
    /// TODO: use an array n_pattern[2] ?
    unsigned int n_patternP, n_patternN;

    /// a collection of Pattern used in this dataset
     std::vector<PatternType> templates;

public:

    DataSetHandle() : n_patternP(0), n_patternN(0) { }

    /// DataSet conversion costructor.
    /// Used to convert a Patern type to an extended Pattern type (for example AdaBoostPattern)
    template<class SrcAggregator>
    explicit DataSetHandle( const DataSetHandle<SrcAggregator> & src) :n_patternP(src.n_patternP), n_patternN(src.n_patternN)
    { 
        static_cast<ParamType&>(*this) = static_cast<const ParamType&>(src);
        templates.reserve(src.templates.size() );
        for( typename std::vector<typename SrcAggregator::PatternType>::const_iterator i = src.templates.begin(); i!=src.templates.end(); ++i)
            templates.push_back( PatternType ( *i ) ); // convert      
    }

    // do nothing
    ~DataSetHandle() { }

    /// Reset (but not release memory)
    void Clear()
    {
        n_patternP = n_patternN = 0;
        templates.clear();
    }

    /// Return number of allocated samples (complete size of DataSet)
    inline unsigned int Size() const {
        return templates.size();
    }

    /** Set the geometry of patterns.
      * All pattern must have the same geometry. */
    void SetParams(const ParamType & src) {
        static_cast<ParamType &>(*this) = src;
    }

    /**  */
    const ParamType & GetParams() const {
        return static_cast<const ParamType &>(*this);
    }

    /// add a new pattern to the list
    void Insert(const PatternType & p)
    {
        if(p.category == 1)
            n_patternP++;
        else
            n_patternN++;
        templates.push_back(p);
    }

    /// Force, manually, to release memory in data
    void Release()
    {
        // release pattern data memory
        for(typename std::vector<PatternType>::iterator i = templates.begin(); i!=templates.end(); ++i)
            i->release();
    }

    /// copy operator (copy and convert)
    /// @note do not release old memory since it is inside the DataSetHandle
    template<class SrcAggregator>
    void operator=( const DataSetHandle<SrcAggregator> & src) {
        static_cast<ParamType&>(*this) = static_cast<const ParamType&>(src);
        n_patternP=src.n_patternP;
        n_patternN=src.n_patternN;
        templates.clear();
        templates.reserve(src.templates.size() );
        for( typename std::vector<typename SrcAggregator::PatternType>::const_iterator i = src.templates.begin(); i!=src.templates.end(); ++i)
            templates.push_back( PatternType ( *i ) ); // convert
    }

    /// Import data inside the set
    ///   @note buf will be freed inside this library at release
    void Import(const DataType & data, int category);

};

/** an auto release DataSetHandle.
 *   DataSetHandle can be freely copied, splitted, merged and modified inside the training process.
 *   The DataSet object instead contains the original set that will be freed at the end of training.
 */
template<class Aggregator>
struct DataSet: public DataSetHandle<Aggregator> {
public:

    typedef DataSetHandle<Aggregator> ViewType;
    typedef DataSet<Aggregator> StoreType;

private:

    // uncopiable
    DataSet(const DataSet & src) { }

public:

    DataSet() { }

    ~DataSet()
    {
        // on destruction, memory is released:
        this->Release();
    }

};


//////////////////////////////////////////////////////////////////////////////////////////////

template<class Aggregator >
void DataSetHandle<Aggregator>::Import(const DataType & data, int category)
{
    // update stats:
    if(category == 1)
        n_patternP++;
    else
        n_patternN++;

    // add the data to the set
    templates.push_back(PatternType(data, category));
}


#endif

