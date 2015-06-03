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


#ifndef _DISCRETE_ADABOOST_H
#define _DISCRETE_ADABOOST_H

#include "AdaBoost.h"
#include "Classifier/BoostableClassifier.h"

#undef _epsilon
/// a small value
/// Shapire & Singer use _epsilon < 1/TrainingExamples
#define _epsilon (1E-10)

// TODO: 
#include "../DataSet.h"


/// Policy Class for DiscreteAdaBoost.
///  Common Part, within template
class DiscreteAdaBoostCommon {
public:
    /** Update Weights of @a set using @a alpha
    *  AdaBoost base update algorithm
    */
    template<class DataSetType>
    static void Update(DataSetType & set, double alpha)
    {
        double bw = exp( alpha );  // weight associated to error (>1)
        double gw = 1.0/bw;        // weight associated to correct detection (<1)
        double z = 0.0;
        double dmin,dmax;
// ricalcolo z per evitare errori di arrotondamento (capitano anche per i double)
        for(unsigned int i =0; i<set.Size(); i++)
        {
            set.templates[i].d *= (set.templates[i].correct() ? gw : bw);
            z+= set.templates[i].d;
        }
        std::cout << "z:" << z <<" bw:" << bw << " gw:" << gw << std::endl;
        // normalize (TODO: capire se serva)
        z = 1.0/z;
        for(unsigned int i =0; i<set.Size(); i++)
        {
            set.templates[i].d *= z;
        }

// calc some stats
        dmin = dmax = set.templates[0].d;
        // davg = set.templates[0].d; // average e' sempre uguale in adaboost
        for(unsigned int i =1; i<set.Size(); i++)
        {
            // davg += set.templates[i].d;
            if(set.templates[i].d>dmax)
                dmax = set.templates[i].d;
            if(set.templates[i].d<dmin)
                dmin = set.templates[i].d;
        }
        std::cout << "Distribution weights range from " << dmin << " to " << dmax << "." << std::endl;
        
    }


    /** Compute sum of error weights
    */
    template<class DataSetType>
    static double CalculateError(const DataSetType & set)
    {
        double e = 0.0;
        for(unsigned int i =0; i<set.Size(); i++)
            if( !set.templates[i].correct() )
                e += set.templates[i].d;
        return e;
    }


    /** Compute Z AdaBoost factor
    */
    template<class DataSetType>
    static double Z(const DataSetType & set, float alpha)
    {
        double Z = 0.0;
        double gw = exp( -alpha); // correct detection
        double bw = exp( alpha);  // error
        for(unsigned int i =0; i<set.Size(); i++)
            Z +=  set.templates[i].d * ( set.templates[i].correct() ? gw : bw );
        return Z;
    }


    /** Update AdaBoost AdaBoostPattern weight using alpha and precompute test field */
    template<class DataSetType>
    static void UpdateWeights(DataSetType & train, double alpha)
    {
        double bw = exp( alpha );  // weight associated to error (>1)
        double gw = 1.0/bw;        // weight associated to correct detection (<1)
        double z = 0.0;
        double dmin,dmax;
// ricalcolo z per evitare errori di arrotondamento (capitano anche per i double)
        for(unsigned int i =0; i<train.Size(); i++)
        {
            train.templates[i].d *= (train.templates[i].correct() ? gw : bw);
            z+= train.templates[i].d;
        }
        std::cout << "z:" << z <<" bw:" << bw << " gw:" << gw << std::endl;
// normalize
        z = 1.0/z;
        for(unsigned int i =0; i<train.Size(); i++)
        {
            train.templates[i].d *= z;
        }

// calc some stats
        dmin = dmax = train.templates[0].d;
        for(unsigned int i =1; i<train.Size(); i++)
        {
            if(train.templates[i].d>dmax)
                dmax = train.templates[i].d;
            if(train.templates[i].d<dmin)
                dmin = train.templates[i].d;
        }
        std::cout << "Distribution weights range from " << dmin << " to " << dmax << std::endl;
    }


    /** compute w coefficient and update test field */
    template<class Classifier, class DataSetType>
    static double ComputeW(const Classifier & c, DataSetType & train)
    {
        double wp, wn;
        Evaluate(train, c);
        return wp;
    }

};


/** Policy DiscreteAdaBoost (AdaBoost.M1)
 *
 * Extended AdaBoost policy with DiscreteDecisionStump that require alpha computation.
 * */
template<class WeakClassifier>
class AdaBoost< BoostableClassifier<WeakClassifier> >: public DiscreteAdaBoostCommon {
public:

    typedef BoostableClassifier<WeakClassifier> Classifier;

public:

    AdaBoost() { }

    /** Evaluate the inner @a h classifier and return @a w value (w+), the sum of weight associated to the correct detection.
      * @param h a Classifier. must have classify method
      * */
    template<class Classifier, class DataSetType>
    static void Evaluate(DataSetType & set, const Classifier & h, double & wp, double & wn)
    {
        wp = wn = 0.0;
        for(unsigned int i =0; i<set.Size(); i++)
        {
            set.templates[i].test = h.classify(getData1(set.templates[i], set), getData2(set.templates[i], set));
            if( set.templates[i].correct() )
                wp += set.templates[i].d;
            else
                wn += set.templates[i].d;
        }
    }



    /** Evaluate @a h classifier on the training set @a set.
      * set the test field in the pattern list and
      * compute @a wp, @a Z and the @a alpha that minimize @a Z function
      * @note adaboost algorithm
      * */
    template<class R, class DataSetType>
    static void Evaluate(DataSetType & set, const R & h, double & alpha, double & Z, double & wp)
    {
        double wn;
        Evaluate(set, h, wp, wn);
        //Z Fn
        Z = 2.0 * sqrt( wn * wp);
        // return alpha
        alpha =  0.5 * log ( (wp + _epsilon) / (wn + _epsilon) );
    }

    /** Perform an AdaBoost.M1 step.
     * @param H a Boostable hypothesis (usually the best found). In output alpha will be set
     * @param training_set the training set used to choose alpha
     * @param validation_set a validation set used to show benchmark
     * @return true if alpha is positive.
     *
     * @note Compute alpha and change the weights
     * 
     * @todo qui c'e' un problema di fondo. Ovvero alpha lo calcola AdaBoost massimizzandolo sul training set. Tuttavia in MAdaBoost alpha e' calcolato con un algoritmo diverso da quello di AdaBoost
     *       in quando il training set ha un peso differente dal set che viene passato a questa funzione.
     *       Alpha va calcolato sul training_set, e aggiornato sul validation
     */
    template<class DataSetType>
    static bool Update(BoostableClassifier<WeakClassifier> & H, DataSetType & training_set,  DataSetType & validation_set, bool verbose=true)
    {
        double bestAlpha1, bestZ1, bestW1;
        double bestAlpha, bestZ, bestW;

        // compute bestAlpha, bestZ, bestW
        // nota: Evaluate modifica set.templates.test
        Evaluate(training_set, H, bestAlpha1, bestZ1, bestW1); // re-compute stats and alpha
        Evaluate(validation_set, H, bestAlpha, bestZ, bestW);  // compute stats
        
        // alpha should be the best alpha associated to training_set. Elsewhere could be negative.
        H.alpha = bestAlpha1; // as side effect, alpha is (always) recomputed here. 

        if(verbose)
        {
            int cdp = 0;
            int cdn = 0;
            std::cout << "[best] V(T) ";

            if(H.debug_name() )
                std::cout << H.debug_name() << ' ';

            std::cout << "Z:" << bestZ << "(" << bestZ1 << "), Alpha:" << bestAlpha << "(" << bestAlpha1 <<"), W:" << bestW << "(" << bestW1 << "). ";

            for(int i=0; i<validation_set.Size(); i++)
            {
                if( validation_set.templates[i].correct() )
                {
                    if(validation_set.templates[i].category == 1)
                        cdp++;
                    else
                        cdn++;
                }
            }
            std::cout << cdp << "(+) " << cdn <<"(-) correct detection (" << ((cdp+cdn)*100)/validation_set.Size() << "%)\n";
        }

        if(H.alpha>0.0)
        {
            // update weight (su entrambi)
            DiscreteAdaBoostCommon::Update(training_set, H.alpha);
            DiscreteAdaBoostCommon::Update(validation_set, H.alpha);
            return true;
        }
        else
        {
            std::cerr << "Trying to use a negative classifier! alpha=" << H.alpha << std::endl;
            return false;
        }
    }

    /** compute alpha coefficient and update test field */
    template<class Classifier, class DataSetType>
    static double ComputeAlpha(const Classifier & c, DataSetType & train)
    {
        double alpha, Z, wp;
        Evaluate(train, c, alpha, Z, wp);
        return alpha;
    }


};


#endif