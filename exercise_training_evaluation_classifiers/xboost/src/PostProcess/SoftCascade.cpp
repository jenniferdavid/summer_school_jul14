/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <fstream>

#include <cmath> // abs
#include <cstring>
#include <map>
#include "PostProcessFactory.h"

#include "IO/sdfin.h"

// #include "factory.h"

#ifndef _MSC_VER
# include <stdint.h>
#else
typedef unsigned int uint32_t;
#endif

#include "IO/pnmio.h"
#include "IntegralImage.h"
#include "HaarFeatureTools.h"
#include "Utility/Utils.h"

#include "DataSet.h"
#include "Descriptor/IntegralImageData.h"
#include "SourceDataSet.h"
#include "Pattern/Pattern.h"

#include "HaarClassifiers.h"
#include "IchnClassifiers.h"

#include "Traits/HaarFeature.h"
#include "Traits/IntegralChannelFeature.h"

#include "Classifier/BoostClassifier.h"
#include "Classifier/DecisionTree.h"
#include "Classifier/SoftCascade.h"
#include "Trainer/AdaBoostCommon.h" // AdaBoostDataSetHandle
#include "Trainer/SoftCascade.h"

#ifdef _MULTITHREAD
 #include "Thread/thread.h"
#endif


using namespace std;

static void help()
{
    std::cout << "Usage: process softcascade <validateset> [OPTIONS]\n\n";
    std::cout << "-h\tthis help\n";
    std::cout << "-v\tverbose output\n";
    #ifdef _MULTITHREAD
    std::cout << "-j <t>\tmax number of concurrent jobs (default 1)\n";
    #endif
    std::cout << "-i <file>\tInput boost file (required)\n";
    std::cout << "-o <file>\tOutput softcascade file\n";
    std::cout << "-r <rate>\tRejection rate (0.05 default)\n";
    std::cout << "-a <factor>\tExponential factor (-1.0 default)\n";
    std::cout << "-s <algo>\tFeature selection algorithm (none is the default):\n";
    std::cout << "\t\tnone, weight, edge, mahalanobis\n";
    std::cout << "-n <max>\tLimit number of stages\n";
    std::cout << "-b\tuse all features in the input file\n";
    std::cout << "--dbp\tUse direct backward pruning scheme\n";
    std::cout << "\nSet Options:\n";
    std::cout << "-N <n>\n\tNumber of negative extracted random\n";

    std::cout.flush();
}

static const char *training_set_filename = 0;
static double ratio = 0.05;
static double alpha = -1.0;
static const char *input_file = 0;
static const char *output_file = "softcascade.out";
static bool verbose = false;

static int sort_algo = 0;
static bool dbp = false;
static bool blind_complete = false;
static int max_stages = -1;
static int max_concurrent_jobs = 1;

static int negative_random_samples = 0;

#define check_param(i)  if(i>=argc) { std::cerr << str <<  ": missing parameters" << std::endl; return false; }

static bool parse_cmd_line(int argc, const char *argv[])
{
    // Parse Command Line
    int i = 1;
    while(i<argc)
    {
        const char *str = argv[i];
        if ((!strcmp(str, "-h") ) || (!strcmp(str, "--help") ) )
        {
            help();
            exit(0);
        }
        else if(!strcmp(str, "-v") )
        {
            verbose = true;
        }
        else if(!strcmp(str, "-o") )
        {
            i++;
            output_file = argv[i];
        }
        else if(!strcmp(str, "-i") )
        {
            i++;
            check_param(i);
            input_file = argv[i];
        }
        else if(!strcmp(str, "-r") )
        {
            i++;
            check_param(i);
            ratio = atof(argv[i]);
        }
        else if(!strcmp(str, "--dbp") )
        {
            dbp = true;
        }
        else if(!strcmp(str, "-b") )
        {
            blind_complete = true;
        }
        else if(!strcmp(str, "-s") )
        {
            i++;
            check_param(i);

            if(!strcmp(argv[i], "none"))
                sort_algo = 0;
            else if(!strcmp(argv[i], "weight"))
                sort_algo = 1;
            else if(!strcmp(argv[i], "edge"))
                sort_algo = 2;
            else if(!strcmp(argv[i], "mahalanobis"))
                sort_algo = 3;
            else
            {
                help();
                exit(0);
            }
        }
        else if(!strcmp(str, "-a") )
        {
            i++;
            check_param(i);
            
            alpha = atof(argv[i]);
        }
#ifdef _MULTITHREAD
        else if(!strcmp(str, "-j") )
        {
            i++;
            check_param(i);
            
            if(!strcmp(argv[i], "max"))
              max_concurrent_jobs = sprint::thread::hardware_concurrency();
            else
              max_concurrent_jobs = atof(argv[i]);
        }
#endif
        else if(!strcmp(str, "-N") )
        {
            i++;
            check_param(i);
            
            negative_random_samples = atoi(argv[i]);
        }
        else if(!strcmp(str, "-n") )
        {
            i++;
            check_param(i);
            
            max_stages = atoi(argv[i]);
        }
        else if(training_set_filename == NULL)
            training_set_filename = str;
        else
        {
            help();
            return false;
        }
        i++;
    }
    return true;
}


/// The factory used to collect all soft cascade preprocessors
static PostProcessMap SoftCascadeMapInstance;

/// the jumper function
template<class Preprocessor, class WeakClassifier>
bool ProcessSoftCascade(std::istream & in)
{
    unsigned int width, height;
    char conf[256];
    size samples_geometry;

    in >> width >> height;
    
    std::cout << "SoftCascade on " << WeakClassifier::signature() << ' ' << width <<'x' << height << std::endl;

    samples_geometry.width = width;
    samples_geometry.height = height;

    // istanzio un oggetto che ha gia' incorporati i pesi di AdaBoost:
    SourceDataSet< Preprocessor, typename AdaBoostPatternAggregator< Preprocessor>::AggregatorType  > training_set;
    
    // configure training_set using std::istream
    training_set.Configure(in);
    // recover configuration
    training_set.GetConf(conf);
                
    // load images
    {
        sdfin in(training_set_filename);
        LoadSet(training_set, in, samples_geometry, negative_random_samples);
    }

    std::cout << training_set.Size() << " patterns correctly loaded ("
              << training_set.n_patternP << "+, " <<  training_set.n_patternN << "-)\n";

    BoostClassifier<WeakClassifier> source(in);
    SoftCascadeClassifier<WeakClassifier> dest;
    TrainSoftCascade(training_set, source, dest, max_stages, (SoftCascadeRankingAlgo) sort_algo, dbp, ratio, alpha, blind_complete, max_concurrent_jobs);
    save_classifier(output_file, dest, width, height, conf);
    return true;
}

// register all instances; ////////////////////////////////////////////////////

static bool scp1 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier<HaarClassifier>::signature(), &ProcessSoftCascade< IntegralImagePreprocessor, HaarClassifier >);
static bool scp2 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier<NaiveHaarClassifier>::signature(), &ProcessSoftCascade< IntegralImagePreprocessor, NaiveHaarClassifier >);
// bool scp3 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier<RealHaarClassifier>::signature(), &ProcessSoftCascade< IntegralImagePreprocessor, RealHaarClassifier >);
static bool scp4 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier<BayesianHaarClassifier>::signature(), &ProcessSoftCascade< IntegralImagePreprocessor, BayesianHaarClassifier >);
static bool scp5 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier<SimpleHaarClassifier>::signature(), &ProcessSoftCascade< IntegralImagePreprocessor, SimpleHaarClassifier >);

static bool scp6 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier<IchnClassifier>::signature(), &ProcessSoftCascade< IntegralChannelImagePreprocessor, IchnClassifier >);


// static bool scp7 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier< DecisionTree<HaarClassifier> >::signature(), &ProcessSoftCascade< IntegralChannelImagePreprocessor, DecisionTree<HaarClassifier> >);
// static bool scp8 = RegisterPostProcess(SoftCascadeMapInstance, BoostClassifier< DecisionTree<IchnClassifier> >::signature(), &ProcessSoftCascade< IntegralChannelImagePreprocessor, DecisionTree<IchnClassifier> >);

////////////////////////////////////////////////////

void softcascade(int argc, const char *argv[])
{
    if(!parse_cmd_line(argc, argv))
      return; 

    std::cout << "[command line: ";
    for(int i =0; i<argc; ++i)
        std::cout << ' ' << argv[i];
    std::cout << "]\n";


    if(training_set_filename == NULL)
    {
        help();
        return;
    }

    std::string code;
    std::ifstream f_in;
    f_in.open(input_file);
    if(!f_in.good()) {
        std::cerr << "error open "<< input_file<<std::endl;
        return;
    }
    
    f_in >> code;

    bool ret = CallPostProcess(SoftCascadeMapInstance, code, f_in);

    f_in.close();

    if(!ret)
        std::cerr << "SoftCascade process failed" << std::endl;

}

