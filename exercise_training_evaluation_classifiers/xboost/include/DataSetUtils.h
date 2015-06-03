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

#ifndef _DATA_SET_UTILS_H
#define _DATA_SET_UTILS_H

/** @file DataSetUtils.h
 *  @brief Utility to operate on DataSet */

#include <vector>
#include <_stdint.h>
#include <fstream>
#include <iostream>
#include <map>      // multimap
#include <cmath>
#include <stdlib.h> // rand
#include "DataSet.h"

#include "Utility/bitset.h"


/** Split the Set in positive and negative samples */
template<class AggTypeDst, class AggTypeSrc>
void Split(DataSetHandle<AggTypeDst> & p,DataSetHandle<AggTypeDst> & n, const DataSetHandle<AggTypeSrc> & src);

/// Extract worst @a m elements
/// @note requires a d field in pattern
template<class AggTypeDst, class AggTypeSrc>
void ExtractWorst(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m);

/// Extract @a m best elements
/// @note requires a d field in pattern
template<class AggTypeDst, class AggTypeSrc>
void ExtractBest(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m);

/** Extract random @a m elements usingg distribution.
 * uses .d field to extract randomply samples from distribution.
 * @note requires d field
 */
template<class AggTypeDst, class AggTypeSrc>
void RandomSampleWithDistribution(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m);

/// Extract @a perc of energy random field
/// @note requires a d field in pattern
template<class AggTypeDst, class AggTypeSrc>
void ExtractReweighingEnergy(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, float perc);

/** Extract random samples from the DataSet, splitting in output and validation */
template<class AggTypeDst, class AggTypeSrc>
void RandomSample(DataSetHandle<AggTypeDst> & dst, DataSetHandle<AggTypeDst> * validation, const DataSetHandle<AggTypeSrc> & src, int nA, int nB);

/** Random Sample, with check uniqueness
 */
template<class AggTypeDst, class AggTypeSrc>
void RandomUniqueSample(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m);

//////////////////////////////////////////////////// implementations ////////////////////////////////////

template<class AggTypeDst, class AggTypeSrc>
void Split(DataSetHandle<AggTypeDst> & p,DataSetHandle<AggTypeDst> & n, const DataSetHandle<AggTypeSrc> & src)
{
    p.Clear();
    p.width = src.width;
    p.height = src.height;

    n.Clear();
    n.width = src.width;
    n.height = src.height;

    for(typename std::vector<typename AggTypeSrc::PatternType >::const_iterator i = src.templates.begin(); i!= src.templates.end(); ++i)
        if(i->category == 1)
            p.Insert(*i);
        else
            n.Insert(*i);

}

template<class AggTypeDst, class AggTypeSrc>
void ExtractWorst(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m)
{
    if(m >= src.Size())
    {
        //
        for(int i=0; i<src.Size(); ++i)
            dst.Insert( src.templates[ i ] );
    }
    else
    {
        std::multimap<double,int> d;

        for(int i=0; i<src.Size(); ++i)
            d.insert(std::pair<double,int>(src.templates[i].d, i) );

        std::multimap<double,int>::reverse_iterator rit = d.rbegin();
        for(int i=0; i<m; ++i)
        {
            dst.Insert( src.templates[ rit->second ] );
            ++rit;
        }

    }

}

template<class AggTypeDst, class AggTypeSrc>
void ExtractBest(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m)
{
    if(m >= src.Size())
    {
        //
        for(int i=0; i<src.Size(); ++i)
            dst.Insert( src.templates[ i ] );
    }
    else
    {
        std::multimap<double,int> d;

        for(int i=0; i<src.Size(); ++i)
            d.insert(std::pair<double,int>(src.templates[i].d, i) );

        std::multimap<double,int>::iterator rit = d.begin();
        for(int i=0; i<m; ++i)
        {
            dst.Insert( src.templates[ rit->second ] );
            ++rit;
        }

    }

}

template<class AggTypeDst, class AggTypeSrc>
void ExtractReweighingEnergy(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, float perc)
{
    // sum of weight
    double wP, wN;
    wP = wN = 0.0;
    for(int i=0; i<src.Size(); ++i)
    {
        if(src.templates[i].category == 1)
            wP += src.templates[i].d;
        else
            wN += src.templates[i].d;
    }

    std::cout << "Ammount of Energy: " << wP << "(+) " << wN << "(-)" << std::endl;
    float th = perc * std::min(wP,wN);
    std::cout << "\tthreshold = " << th << std::endl;

    random_bit_set rnd(src.Size());
    // accumulators
    double aP, aN;
    aP = aN = 0.0;
    while(aP < th || aN < th)
    {
        int i = rnd();
        if((src.templates[i].category == 1) && (aP < th))
        {
            aP += src.templates[i].d;
            dst.Insert(src.templates[i]);
        }
        else if((src.templates[i].category == -1) && (aN < th))
        {
            aN += src.templates[i].d;
            dst.Insert(src.templates[i]);
        }
    }
}

template<class AggTypeDst, class AggTypeSrc>
void RandomSampleWithDistribution(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m)
{
    if(m >= src.Size())
    {
        //
        for(int i=0; i<src.Size(); ++i)
            dst.Insert( src.templates[ i ] );
    }
    else
    {
        double *d = new double [src.Size()];
        // TODO: use an equation to convert d in pdf
        // sqrt have no meaning, but seems to work
        double dsum = 0.0;
        for(int i =0; i<src.Size(); ++i)
            dsum += sqrt(src.templates[i].d);

        double daux = 0.0;
        for(int i =0; i<src.Size(); ++i)
        {
            daux += std::sqrt(src.templates[i].d) / dsum;
            d[i] = daux;
        }

//     double dtest = 0.0;
        for(int i=0; i<m; ++i)
        {
            int j = randslot(d, src.Size());
//       dtest += src.templates[j].d;
            dst.Insert( src.templates[ j ] );
        }

//     std::cout << "Input Weight: " << dsum / src.Size() << ". Output Weight: " << dtest / m << std::endl;

        delete [] d;
    }
}

template<class AggTypeDst, class AggTypeSrc>
void RandomUniqueSample(DataSetHandle<AggTypeDst> & dst, const DataSetHandle<AggTypeSrc> & src, int m)
{
    random_bit_set rnd(src.Size());

    for(int i=0; i<m; ++i)
        dst.Insert( src.templates[ rnd() ] );
}

template<class AggTypeDst, class AggTypeSrc>
void RandomSample(DataSetHandle<AggTypeDst> & dst, DataSetHandle<AggTypeDst> * validation, const DataSetHandle<AggTypeSrc> & src, int nA, int nB)
{
    int m = src.Size();
    std::vector<bool> bitset(m);

    dst.Clear();
    dst.templates.reserve(nA+nB);
    dst.width = src.width;
    dst.height = src.height;

    if(validation)
    {
        validation->Clear();
        validation->templates.reserve(m-(nA+nB));
        validation->width = src.width;
        validation->height = src.height;
    }

    for(int i =0; i<m; ++i) bitset[i]=false;
    for(int i =0; i<nA+nB; ++i)
    {
        int n;
        bool failed;
        do {
            do {
                n = rand() % m;
            } while(bitset[n]);

            failed = false;
            if( src.templates[n].category == 1 && nA==0)
                failed = true;
            if( src.templates[n].category == -1 && nB==0)
                failed = true;

        } while(failed);

        if( src.templates[n].category == 1)
        {
            nA--;
        }
        if( src.templates[n].category == -1)
        {
            nB--;
        }

        bitset[n] = true;

        dst.Insert(src.templates[n]);
    }

    if(validation)
    {
        for(int i=0; i<m; ++i)
            if(!bitset[i])
                validation->Insert(src.templates[i]);
    }
}


#endif

