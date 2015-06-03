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

#include "xbversion.h"
#include "Pattern/PrecomputedPatternResponse.h"
#include "Oracle/PrecomputedBinaryClassifierOracle.h"
#include "Classifier/DecisionStump.h"
#include "Classifier/BoostClassifier.h"
#include "Utility/Utils.h" // save_classifier
#include "Utility/PrecomputedPatternResponseLoad.h"

#include <iostream>
#include <string.h>

typedef float ScalarType;

static const double _epsilon = 1e-8;

void help()
{
    std::cout << "Usage: ptrainer [OPTIONS] <training set> [OPTIONS]\n\n"
              " -v\n\tBe More Verbose\n"
              " -o outputFile\n\tOutput file (default is output.dat)\n"
              " -n n_max_iter\n\tSet the number of AdaBoost iterations\n";
  
}

int n_max_iter = 100;
int n_heuristic_bin = 256;
const char *output_file = "output.dat";
const char *input_data = 0;

// a check if the argument is valid
#define check_param(i)  if(i>=argc) { std::cerr << str <<  ": missing parameters" << std::endl; return false; }

static bool parse_cmd_line(int argc, char *argv[])
{
    // Parse Command Line
    if(argc < 2)
    {
        help();
        return false;
    }

    int i = 1;
    while(i<argc)
    {
        const char *str = argv[i];
        if ((!strcmp(str, "--help") ) || (!strcmp(str, "-h") ) )
        {
            help();
            return false;
        }
        else if (!strcmp(str, "-n") )
        {
            i++;
            check_param(i);
            n_max_iter = atoi(argv[i]);
        }
        else if (!strcmp(str, "-o") )
        {
            i++;
            check_param(i);
            output_file = argv[i];
        }
        else
        if(input_data == 0)
          input_data = str;
        else
        {
          help();
          return false;
        }
        ++i;
    }
    return true;
}

int main(int argc, char *argv[])
{
    std::cout << "Precomputed Feature xBoost Trainer " XBOOST_VERSION << std::endl;

    if(!parse_cmd_line(argc, argv))
        return -1;

    PrecomputedPatternResponse<ScalarType> training_set(n_heuristic_bin);

    // load SVM-like data
    LoadSVMData(training_set, input_data);

    if(training_set.n_features == 0 || training_set.n_samples == 0)
    {
        std::cerr << "not enough data to run a trainer" << std::endl;
        return -1;
    }

    training_set.PrepareBins();

    // decision-stump

    // Initialize Weights
    training_set.InitializeWeights();

    bool * correct_tables = new bool[training_set.n_samples];

    PrecomputedPatternBinaryClassifierOracle< DecisionStump<ScalarType> > tr;
    tr.reoptimize = true;


    // the final classifier;
    BoostClassifier< IndirectBinaryClassifier< DecisionStump<ScalarType> > > classifier;


    for(int i_iter = 0; i_iter<n_max_iter; ++i_iter)
    {
        // BUILD Inverse Matrix response
        IndirectBinaryClassifier< DecisionStump<ScalarType> > c;

        c.parity = 0;
        c.feature_index = -1;

        // Train
        double w = tr.Train(c, training_set, 1);

        // compute error and the correct_tables
        double wp, wn;
        int match;

        training_set.ExecuteClassifier(c, correct_tables, &wp, &wn, &match);

        // AdaBoost
        //Z Fn
        double Z = 2.0 * sqrt( wn * wp);
        // return alpha
        double alpha =  0.5 * log ( (wp + _epsilon) / (wn + _epsilon) );

        std::cout << i_iter << '\t' << alpha << '\t' << c.feature_index << '\t' << c.parity << '\t' << c.th << std::endl;

        if(alpha < 1e-3)
        {
            std::cout << " * terminated by algorithm" << std::endl;
            break;
        }

        // insert the classifier c inside the adaboost classifier classifier
        classifier.insert( BoostableClassifier< IndirectBinaryClassifier< DecisionStump<ScalarType> > > (c, alpha )  );

        // Update Weights /////// TODO in library

        double bw = exp( alpha );  // weight associated to error (>1)
        double gw = 1.0/bw;        // weight associated to correct detection (<1)
        double z = 0.0;

        for(int i =0; i<training_set.n_samples; ++i)
        {
            training_set.weights[i] *= ( correct_tables[i] ) ? gw : bw;
            z += std::abs(training_set.weights[i]);
        }

        // recipr
        z = 1.0/z;

        // normalize
        for(int i =0; i<training_set.n_samples; ++i)
            training_set.weights[i] *= z;
        //////////////////////////////////////////////////////////
    }

    delete [] correct_tables;

    save_classifier(output_file, classifier, 0,0,0);

    return 0;
}