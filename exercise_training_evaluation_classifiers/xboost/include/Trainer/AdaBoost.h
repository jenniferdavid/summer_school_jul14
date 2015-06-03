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


#ifndef _ADABOOST_H
#define _ADABOOST_H

#include "DataSet.h"
#include "AdaBoostCommon.h"
#include <cmath> // exp

#include "Traits/HaarFeature.h" // TODO:

template<class WeakClassifier>
class AdaBoost;

/** AdaBoost Weight Update Rules
 * 
 * This AdaBoost is valid for all AdaBoost variants (Real and Gentle in particular)
 *  Update rule only update weights.
 * */
template<class WeakClassifier>
class AdaBoost {

public:

    AdaBoost() { }

    /** Perform an AdaBoost generic update step.
     * @param H a Generic Boosted hypothesis
     * @param training_set 
     * @param validation_set a training set updated using H
     * @return true if is a good classifier
     * */
    template<class DataSetType>
    static bool Update(const WeakClassifier & H,  DataSetType & training_set, DataSetType & validation_set, bool verbose=true)
    {
        int P = 0;
        int N = 0;
        int TP = 0;
        int TN = 0;
        double wP = 0.0; // sum of weights of positive patterns
        double wN = 0.0; // sum of weights of negative patterns
        double wTP = 0.0; // sum of weight of correctly detected positive patterns
        double wTN = 0.0; // sum of weight of correctly detected negative patterns
        
        double z = 0.0; // sum e^-u_i

        for(int i=0; i<validation_set.Size(); i++)
        {
            double f = H.classify( getData1(validation_set.templates[i], validation_set), getData2(validation_set.templates[i], validation_set) ); // classifier response
            double u = validation_set.templates[i].category * f; // correctness function

            if(verbose)
            {
                if(validation_set.templates[i].category == +1)
                {
                    wP += validation_set.templates[i].d;
                    P++;
                    if(u>0.0) {
                      wTP +=  validation_set.templates[i].d;
                      TP++;
                    }
                }
                else
//                 if(set.templates[i].category == -1)
                {
                    wN += validation_set.templates[i].d;
                    N++;
                    if(u>0.0) {
                      wTN += validation_set.templates[i].d;
                      TN++;
                    }
                }

            }
            
            
            // w' = w * e^{-u_i}
            validation_set.templates[i].d *= exp( - u );
            
            z+= validation_set.templates[i].d;
            
        }
        
        double norm = 1.0/z;
        
        // normalize
        for(int i=0; i<validation_set.Size(); i++)
          validation_set.templates[i].d *= norm;

        if(verbose)
        {
            std::cout << "Best Feature | Z = " << z << " | Correct In Number = " << (TP+TN) * 100 / validation_set.Size() << "% (" << 100*TP/P << "% +, " << 100 * TN/N << "% -)" << 
            " | Correct in Weight = " << (int)( (wTP+wTN) * 100 / (wP+wN)) << "% (" << (int)(100 * wTP/wP) << "% +, " << (int)(100 * wTN/wN) << "% -)" <<         
            std::endl;
        }
        return true;
    }

};

#endif
