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

#include "Oracle/WeightedPatternResponse.h"
#include <algorithm>

#include "_stdint.h"

/*
// Bubble Sort 203 feature invece di quasi 3000!
template <typename T>
void BubbleSort(T* v, const int size)
{
  for(int i=0; i!=size-1; i++)
  {
    for(int j=0; j!=size-i-1; j++)
    {
      if (v[j+1]<v[j])
      {
        std::swap(v[j],v[j+1]);
      }
    }
  }
}*/


void sort_pattern(PatternResponse *store, int n)
{
    std::sort(&store[0], &store[n]);
}

// parity=1  : v < th ? 1 : -1
// parity=-1 : v > th ? 1 : -1
double AdaBoostMetric::optimize(DecisionStump<int> & h, const PatternResponse *store, int n, double wp, double wn, bool strictly_growing)
{
    double w, curW; // current best w
    double sumW = wp + wn;

    // initial "bestW" using external border (all NEGs)
    h.th = store[0].value; // th is the global minimum value
    h.parity = 1; // v < th ? 1 : -1
    curW = wn;

    if(strictly_growing)
    {
        w = wn;
        for(int i=0; i<n-1; i++)
        {
            w += store[i].d; // se prendo un POS alzo w, se prendo un NEG abbasso w
            if(w>curW) // parity +1
            {
                curW = w;
                h.th = store[i+1].value;
                h.parity = 1;
            }
            if((sumW-w)>curW) // parity -1
            {
                curW = sumW-w;
                h.th = store[i].value;
                h.parity = -1;
            }
        }
    }
    else
    {
        w = wn;
        for(int i=1; i<n; i++)
        {
            w += store[i-1].d; // se prendo un POS alzo w, se prendo un NEG abbasso w
            // (boundary check) se e' cresicuto value, solo ora faccio il check
            if(store[i].value>store[i-1].value)
            {
                // 1) search the maximum of W in the case v < th
                if(w>curW)
                {
                    curW = w;
                    h.th = store[i].value;
                    h.parity = 1;
                }
                if((sumW-w)>curW)
                {
                    curW = sumW-w;
                    h.th = store[i-1].value;
                    h.parity = -1;
                }
            }
        }
    }
    
    return curW; // best w found
}

// parity=1  : v < th ? 1 : -1
// parity=-1 : v > th ? 1 : -1
double AdaBoostMetric::optimize(DecisionStump<int> & h, const PatternResponse *store, int n_pattern, double m_dB)
{
    double w, curW;

    // initial "bestW" using external border (all NEGs)
    curW = m_dB;
    h.th = store[0].value; // th is the global minimum value
    h.parity = 1; // v < th ? 1 : -1

    // 1) search the maximum of W in the case v < th
    w = m_dB;
    for(int i=1; i<n_pattern; i++)
    {
        w += store[i-1].d; // se prendo un POS alzo w, se prendo un NEG abbasso w
        // (boundary check) se e' cresicuto value, solo ora faccio il check
        if((store[i].value>store[i-1].value)&&(w>curW))
        {
            curW = w;
            h.th = store[i].value;
            h.parity = 1;
        }
    }

    //  Manca il caso di bordo tutti POS, tuttavia non si puo' dire molto (nel caso che la soglia sarebbe comunque un minimo non forte)

    // 2) search the maximum of W in the case v > th
    w = m_dB;
    for(int i=n_pattern-2; i>=0; i--)
    {
        w += store[i+1].d; // d(A)>0, d(B)<0
        if((store[i].value<store[i+1].value)&&(w>curW))
        {
            curW = w;
            h.th = store[i].value;
            h.parity = -1;
        }
    }

//   std::cout << "Threshold: " << h.th << " Parity: " << h.parity << " w:" << curW << std::endl;

    return curW;
}

double RealAdaBoostMetric::optimize(RealDecisionStump<int> & h, const PatternResponse *store, int n, double wp, double wn)
{
    double TP = wp;
    double FP = wn;
    double FN = 0.0;
    double TN = 0.0;
    double z;
    double zbest = 1.0;

    // v > th ? a : b
    // concetto di edge:
    //  esistono dei valori k...k+m con uguale value
    //  i positivi sulla soglia sono FN
    //  i negativi sulla soglia sono TN
    //  TP = sum(v>th) | TN = sum(v<=th)

    for(int i=0; i<n-1; i++)
    {
        if(store[i].d>0.0)
            TP-=store[i].d, FN+=store[i].d;
        else
            FP+=store[i].d, TN-=store[i].d;

        FN = wp - TP;
        FP = wn - TN;

        if((FP>0.0) && (TN>0.0) && (store[i].value != store[i+1].value))
        {
            double z = std::sqrt(TP*FP)+std::sqrt(FN*TN);
            if(z<zbest)
            {
                zbest = z;
                h.th = store[i].value;
                h.pr = 0.5 * std::log(TP/FP);
                h.nr = 0.5 * std::log(FN/TN);
            }
        }

    }

    return zbest;
}


double GentleAdaBoostMetric::optimize(RealDecisionStump<int> & h, const PatternResponse *store, int n, double wp, double wn)
{
    double TP = wp;
    double FP = wn;
    double FN = 0.0;
    double TN = 0.0;
    double s;
    double sbest = 1.0;

    // v > th ? a : b
    // concetto di edge:
    //  esistono dei valori k...k+m con uguale value
    //  i positivi sulla soglia sono FN
    //  i negativi sulla soglia sono TN
    //  TP = sum(v>th) | TN = sum(v<=th)

    for(int i=0; i<n-1; i++)
    {
        if(store[i].d>0.0)
            TP-=store[i].d, FN+=store[i].d;
        else
            FP+=store[i].d, TN-=store[i].d;

        FN = wp - TP;
        FP = wn - TN;

        double l = (TN + FN)*(TP + FP);
        // devo cercare un edge
        if((l>0.0) && (i>0) && (store[i].value != store[i+1].value) )
        {
            s = (TP*FP*TN + TP*TN*FN + FP*TN*FN + TP*FP*FN)/l;
            if(s<sbest)
            {
                sbest = s;
                h.th = store[i].value;
                h.pr = (TP - FP)/(TP + FP);
                h.nr = - (TN - FN)/(TN + FN);
            }
        }

    }

//    ///// CONTROPROVA /////
//
//    TP=FP=FN=TN=0.0;
//    for(int i = 0;i<n;++i)
//    {
//      if(store[i].value > h.th)
//      {
//        // POSITIVE
//        if(store[i].d>0.0) TP+=store[i].d;
//        else
//                           FP-=store[i].d;
//      }
//      else
//      {
//        // NEGATIVE
//        if(store[i].d>0.0) FN+=store[i].d;
//        else
//                           TN-=store[i].d;
//      }
//    }
//
//    double l = (TN + FN)*(TP + FP);
//    s = (TP*FP*TN + TP*TN*FN + FP*TN*FN + TP*FP*FN)/l;
//
//    if( std::abs(s - sbest) > 1e-5)
//    {
//      std::cerr << "WARN: " << s << ' ' << sbest << ' ' << sbest- s  << std::endl;
//    }
//

    return sbest;
}

