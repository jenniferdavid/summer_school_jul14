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

#include "xbversion.h"
#include "Utility/Utils.h"
#include "DataSetUtils.h"

#include "IO/sdfin.h"

#include "HaarClassifiers.h"

// #include <Trainer/SVM.h>

#include "Oracle/BinaryClassifierOracle.h"
#include "Oracle/PrecomputedBinaryClassifierOracle.h"
#include "Oracle/RealDecisionStumpOracle.h"
#include "Oracle/BayesianStumpOracle.h"
#include "Oracle/NaiveDecisionStumpOracle.h"
#include "Oracle/DecisionTree.h"

#include "Traits/HaarFeature.h"
#include "Traits/IntegralChannelFeature.h"

#include "FeatureGenerator/IntegralChannelFeatureGenerators.h"
#include "FeatureGenerator/FeatureGeneratorUtils.h"

#include "Pattern/PrecomputedPatternResponse.h"

#include "Classifier/IndirectDecisionTree.h"
#include "Classifier/SoftCascade.h"

#include "Trainer/SoftCascade.h"
#include "Trainer/AdaBoostTrainer.h"
#include "Trainer/DecisionTreeLearner.h"
#include "Trainer/DiscreteAdaBoost.h"

#ifdef _MULTITHREAD
# include "Thread/thread.h"
#endif

#include <list>
#include <memory>
#include <cstring>
// #include <linear.h> //

// #include "HeatmapColorGenerator.h"
#include "Utility/timer.h"
#include "Utility/Time.h"
#include "Utility/TrainUtils.h"

// fw help
void help();
void help_algo();

/////// global variables (parameters) //////////////

static const char *dataset = NULL;
static const char *outputfile = "output.dat";
static char tmp_outputfile[1024];
static const char *pconf = 0; // use default
static int n_iteration = 100;
static bool bootstrap_increase_iteration = false;
static int n_max_depth = 1; // decision tree depth
static int preload = 512;
// static bool precompute = false; // precompute FEATURE response
static int refresh_rate = 0;    // precompute once
static int minFeatSize = 1;     // TODO: increase a bit
static int minFeatWidth = 1;
// int featBorder = 0;
static int featStep = 1;
static int randNum = -1;
static int nThread = 1;
static float n_samples = -1.0f;
static bool reject_bad_classifier = false;
static bool prune_weak_classifier = false;
static float prune_threshold = 0.5;
static float reject_ratio = 0.5;
static const char *algorithm = NULL;
static const char *inputfile = NULL;
static bool verbose = false;
static bool enable_fast_heuristic = false;
static bool optimize_fast_heuristic = true;
static double positive_weight = -1.0; //
static int adaboost_fast_heuristic_bin = 256;
static bool prune_decision_tree = false;
static const char *generator_algorithm = "bf";
static bool use_madaboost = false;
// int pos_add_noise = 0;
// int neg_add_noise = 0;
static bool reoptimize = false;
static const char *feature_type = "haar";

static int filter_type = 0;

static int negative_random_samples = 0;
static int bootstrap_negatives = -1;
static int bootstrap_negatives_per_frames = -1;
static size samples_geometry(0,0);

static float bootstrap_scale_factor = 1.189207115f; // 4 scales per octave
static int bootstrap_iteration = 0;
static int bstrap_step = -1; // AUTO
static const char *export_folder = 0;

static double thMin=0;
static int nNegImage=0;
static int maxNeg=0;

static bool auto_terminate = false;
static double tpr_goal=1.0;
static double fpr_goal=0.0;
static int    ni_goal=1;
static double margin_goal = -1000000000000000000.0; // a really small value

//// cascades parameters
static double cascade_target_tpr = 0.95;
static double cascade_target_fpr = 0.0;
static int cascade_metric = 0;
static double cascade_tpr_decay = 0.0;



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
        else if(!strcmp(str, "-f") )
        {
            i++;
            check_param(i);
            feature_type = argv[i];
        }
        else if(!strcmp(str, "--auto-terminate") )
        {
            auto_terminate = true;
        }
        else if (!strcmp(str, "--tpr-goal") )
        {
            auto_terminate = true;
            i++;
            check_param(i);
            tpr_goal = atof(argv[i]);
        }
        else if (!strcmp(str, "--fpr-goal") )
        {
            auto_terminate = true;
            i++;
            check_param(i);
            fpr_goal = atof(argv[i]);
        }
        else if (!strcmp(str, "--margin-goal") )
        {
            auto_terminate = true;
            i++;
            check_param(i);
            margin_goal = atof(argv[i]);
        }
        else if (!strcmp(str, "--extra-iterations") )
        {
            auto_terminate = true;
            i++;
            check_param(i);
            ni_goal = atof(argv[i]);
        }
        else if( (!strcmp(str, "--gt")) || (!strcmp(str, "--terminate-condition")))
        {
            auto_terminate = true;
            i++;
            check_param(i);
            tpr_goal = atof(argv[i]);
            i++;
            check_param(i);
            fpr_goal = atof(argv[i]);
            i++;
            check_param(i);
            ni_goal = atoi(argv[i]);

        }
        else if(!strcmp(str, "-n") )
        {
            i++;
            check_param(i);
            if(argv[i][0]=='+')
                bootstrap_increase_iteration = true;
            else
                bootstrap_increase_iteration = false;
            n_iteration = atoi(argv[i]);
        }
        else if(!strcmp(str, "-d") )
        {
            i++;
            check_param(i);
            n_max_depth = atoi(argv[i]);
        }
        else if(!strcmp(str, "-o") )
        {
            i++;
            check_param(i);
            outputfile = argv[i];
        }
        else if(!strcmp(str, "-j") ) // TODO use -j for jobs and use a preload parameters!
        {
            i++;
            check_param(i);
            preload = atoi(argv[i]);
        }
//         else if(!strcmp(str, "--precompute") )
//         {
//             precompute = true;
//         }
        else if(!strcmp(str, "--refresh") )
        {
            i++;
            check_param(i);
            refresh_rate = atoi(argv[i]);
        }
        else if(!strcmp(str, "-i") )
        {
            i++;
            check_param(i);
            inputfile = argv[i];
        }
        else if(!strcmp(str, "-v") )
        {
            verbose = true;
        }
        else if(!strcmp(str, "-p") )
        {
            i++;
            check_param(i);
            pconf = argv[i];
        }
        else if(!strcmp(str, "--good") )
        {
            i++;
            check_param(i);
            reject_bad_classifier = true;
            reject_ratio = atof(argv[i]);
        }
        else if(!strcmp(str, "-a") )
        {
            i++;
            check_param(i);
            algorithm = argv[i];
            if(!strcmp(algorithm,"help"))
            {
                help_algo();
                return false;
            }

        }
        else if(!strcmp(str, "--fsize") )
        {
            i++;
            check_param(i);
            minFeatSize = atoi(argv[i]);
        }
        else if(!strcmp(str, "--fwidth") )
        {
            i++;
            check_param(i);
            minFeatWidth = atoi(argv[i]);
        }
        else if(!strcmp(str, "--fstep") )
        {
            i++;
            check_param(i);
            featStep = atoi(argv[i]);
        }
//         else if(!strcmp(str, "--fborder") )
//         {
//             i++;
//             check_param(i);
//             featBorder = atoi(argv[i]);
//         }
        else if(!strcmp(str, "-N") )
        {
            i++;
            check_param(i);
            negative_random_samples = atoi(argv[i]);
        }
        else if(!strcmp(str, "--bstrap-negatives") )
        {
            i++;
            check_param(i);
            bootstrap_negatives = atoi(argv[i]);
        }
        else if(!strcmp(str, "--cascade-tpr") )
        {
            i++;
            check_param(i);
            cascade_target_tpr = atof(argv[i]);
        }
        else if(!strcmp(str, "--cascade-tpr-decay") )
        {
            i++;
            check_param(i);
            cascade_tpr_decay = atof(argv[i]);
        }
        else if(!strcmp(str, "--bstrap-negatives-per-frame") )
        {
            i++;
            check_param(i);
            bootstrap_negatives_per_frames = atoi(argv[i]);
        }
        else if(!strcmp(str, "--bstrap") )
        {
            i++;
            check_param(i);
            bootstrap_iteration = atoi(argv[i]);
        }
        else if(!strcmp(str, "--bstrap-step") )
        {
            i++;
            check_param(i);
            bstrap_step = atoi(argv[i]);
        }
        else if(!strcmp(str, "--bstrap-scale-factor") )
        {
            i++;
            check_param(i);
            bootstrap_scale_factor = atof(argv[i]);
        }
        else if(!strcmp(str, "-r") )
        {
            i++;
            check_param(i);
            samples_geometry.width = atoi(argv[i]);
            i++;
            check_param(i);
            samples_geometry.height = atoi(argv[i]);
        }
        else if(!strcmp(str, "-g") )
        {
            i++;
            check_param(i);
            generator_algorithm = argv[i];
            i++;
            check_param(i);
            randNum = atoi(argv[i]);
        }
        else if(!strcmp(str, "--reoptimize") )
        {
            reoptimize = true;
        }
        else if(!strcmp(str, "--positive-weight") )
        {
            i++;
            check_param(i);
            positive_weight = atof(argv[i]);
        }
        else if(!strcmp(str, "--prune") )
        {
            prune_decision_tree = true;
        }
        else if(!strcmp(str, "--weak-th") )
        {
            i++;
            check_param(i);
            prune_weak_classifier = true;
            prune_threshold = atof(argv[i]);
        }
        else if(!strcmp(str, "--export-folder") )
        {
            i++;
            check_param(i);
            export_folder = argv[i];
        }
        else if(!strcmp(str, "--avoid-optimize"))
        {
            optimize_fast_heuristic = false;
        }
        else if(!strcmp(str, "--fast-heuristic"))
        {
            enable_fast_heuristic = true;
            i++;
            check_param(i);
            adaboost_fast_heuristic_bin = atoi(argv[i]);
        }
        else if(!strcmp(str, "-t") )
        {
            i++;
            check_param(i);

#ifndef _MULTITHREAD
            std::cerr << "Sorry, but no multithread is available with those build flags" << std::endl;
            nThread = 1;
#else
            if(!strcmp(argv[i], "max"))
            {
                nThread = sprint::thread::hardware_concurrency();
            }
            else
            {
                nThread = atoi(argv[i]);
            }
#endif
        }
        else if(!strcmp(str, "--mada") )
        {
            use_madaboost = true;
        }
        else if(dataset == NULL)
            dataset = str;
        else
        {
            std::cerr << "Invalid parameter: " << str << std::endl;
            help();
            return false;
        }
        i++;
    }

    return true;
}


bool test_terminate_conditions(const ReportTest & rep, int & iteraz_goal_reached)
{
    double fpr = rep.Fpr();
    double tpr = rep.Recall();
    double margin = rep.Margin();
    std::cout << "[AT] tpr:"<<tpr<<"/" << tpr_goal << " | fpr:"<<fpr << "/" << fpr_goal << " | margin: " <<  margin << "/" << margin_goal;
    if((tpr>=tpr_goal) && (fpr <= fpr_goal) && (margin >= margin_goal) )
    {
        iteraz_goal_reached++;
        std:: cout << " : reached, to break "<<ni_goal-iteraz_goal_reached<<std::endl;
    }
    else
    {
        std:: cout << " : not reached "<<std::endl;
        iteraz_goal_reached = 0; // reset iteraz_goal_reached
    }
    if(iteraz_goal_reached >= ni_goal)
    {
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// This is a list of "bootstrap_trainer" object functions: they use the global parameters declared in Trainer.cpp to make a bridge between
////  the user interfaces (command line) and the inner trainer classes.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function Object for bootstrap_trainer using an AdaBoost trainer.
 *  Train using a Trainer, a DataSet, and a FeatureGenerator.
 *  Iteration based (as AdaBoost)
 */
template<class _Traits, class Oracle>
class CAdaBoostTrainer {

/// The Inner Trainer
    AdaBoostTrainer<Oracle> tr;

    /// last ReportTest
    ReportTest m_rep;

public:

    typedef _Traits Traits;
    typedef typename AdaBoostTrainer<Oracle>::ClassifierType ClassifierType;

public:
    CAdaBoostTrainer(  typename Traits::SourceDataSetType & training_set, typename Traits::FeatureGeneratorType & features, int bstrap_iter)
    {
        // increase the number of iteration for any bootstrap cycle
        int n_max_iter =(bootstrap_increase_iteration) ?  ((bstrap_iter+1) * n_iteration) : n_iteration;
        int iteraz_goal_reached =0;
        char conf[256];

        training_set.GetConf(conf);

        if(n_max_depth != 1)
        {
            std::cerr << "Error: AdaBoost on Decision Tree is not provided by this module. Use --depth 1, selecting Decision Stump, or use other algorithms" << std::endl;
            return;
        }

//         if(precompute)
//         {
//             std::cerr << "Warn: --precompute is not yet implemented, sorry." << std::endl;
//             return;
//         }

        tr.EnableMAdaBoost(use_madaboost);

        // import the set
        tr.SetValidationSet(training_set);
        // reset weights
        tr.ResetValidationSet( positive_weight );

        // copy the training set of Trainer inside the Oracle
        // TODO: investigate benefit of using Validation and Training Set in AdaBoost
        tr.SetTrainingSet(tr.GetValidationSet());

        tr.SetFeatureGenerator(features);

#ifdef _MULTITHREAD
        tr.SetNumberOfThreads(nThread);
#endif

        tr.SetPreloadSize(preload);

        // enable the fast heuristic
        tr.SetFastHeuristic(enable_fast_heuristic, optimize_fast_heuristic, adaboost_fast_heuristic_bin);

        // TODO: deprecate inputfile
        if(inputfile)
        {
            // TODO: bstrap e inputfile non vanno molto in accordo
            unsigned int width, height;
            load_classifier(inputfile, tr.Classifier(), width, height ); // TODO: import preprocessor params!!!!

            // TODO: use traits!
            if(width != training_set.Width() || height != training_set.Height() )
                //if(width != dataset.Width() || height != dataset.Height() )
            {
                std::cerr << "Wrong input file geometry: " << width << 'x' << height << " but " << training_set.Width() << 'x' << training_set.Height() << " expected" << std::endl;
            }
            tr.Restart(reoptimize);
        }

        Timer t;
        t.Start();

        int i_iter = 0;

        // se n_iteration == 0 e' un RETRAIN (non fa nulla)
        if(n_iteration>0)
            for(;;) {

                ++i_iter;

                std::cout << " --- " << i_iter << '/' << n_max_iter << " [" << bstrap_iter << "] ---" << std::endl;

                // USES ALL SAMPLES: copy the VALIDATION set (TRAINER) in the TRAINING SET (ORACLE)
                //  Serve solo per MAdaBoost
                tr.SetTrainingSet(tr.GetValidationSet());

                // TODO: if fail, but I am using a subset random, try again
                if(!tr.Train())
                {
                    std::cout << " * Terminated by trainer" << std::endl;
                    break;
                }

                save_classifier(tmp_outputfile, tr.Classifier(), training_set.Width(), training_set.Height(), conf);

                // Evaluate performance on the Training Set
                m_rep = tr.TestAndExportStat(0.0, nThread);

                if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
                {
                    std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
                    break;
                }

                if(n_iteration>0)
                {
                    // TODO: bootstrap_increase_iteration
                    double ETA = (t.GetTime()/(i_iter)) * (n_max_iter - i_iter + n_max_iter * (bootstrap_iteration - bstrap_iter - 1) );
                    std::cout << "ETA: " << human_readable_time(ETA) << std::endl;
                }

                if(i_iter>=n_max_iter)
                {
                    std::cout << "* Maximum number of iteration reached\n";
                    break;
                }

            }
    }

    ~CAdaBoostTrainer() {}

    const ReportTest & Report() const {
        return m_rep;
    }

    const ClassifierType & Classifier() const {
        return tr.Classifier();
    }

};

///////

/** Function Object for bootstrap_trainer using an AdaBoost trainer.
 *  Train using a Trainer, a DataSet, and a FeatureGenerator.
 *  Iteration based (as AdaBoost)
 *
 * TODO: codice comune con AdaBoostTrainer ? codice comune con DecisionTree?
 */
template<class _Traits, class Oracle>
class CFastAdaBoostTrainer {
public:

    typedef _Traits Traits;
    typedef typename Traits::FeatureGeneratorType::FeatureType FeatureType;
    typedef typename Oracle::ClassifierType IndirectClassifierType;
    typedef typename Oracle::template Traits<FeatureType>::ClassifierType OutClassifierType;

private:

    /// last ReportTest
    ReportTest m_rep;

    /// Boosted Classifier
    BoostClassifier<OutClassifierType> m_boost;


public:
    /// Execute a train cycle
    CFastAdaBoostTrainer(  typename Traits::SourceDataSetType & training_set, typename Traits::FeatureGeneratorType & feature_generator, int bstrap_iter)
    {
        int n_max_iter =(bootstrap_increase_iteration) ?  ((bstrap_iter+1) * n_iteration) : n_iteration;
        int iteraz_goal_reached =0;
        int i_iter = 0;
        char conf[256];
        bool *correct_tables;
        double *incremental_response;
        int refresh_countdown;

        // The Inner Trainer
        Oracle tr;

        tr.reoptimize = reoptimize;

        if(!enable_fast_heuristic)
        {
            std::cerr << "Warn: --fast-heuristic is required for this training. It will be automatically activated." << std::endl;
            enable_fast_heuristic = true;
        }

        // NOTE: ignore depth!

        training_set.GetConf(conf);

        // Precompute Feature Response
        PrecomputedPatternResponse<int32_t> response(adaboost_fast_heuristic_bin);

        Timer t2;

        t2.Start();

        std::vector< FeatureType > feats;

        ExtractAllFeatures(feats, feature_generator);

        if(feats.empty())
        {
            std::cerr << "Cannot train: Feature space is empty" << std::endl;
            return;
        }

        std::cout << "Precompute response (" << training_set.Size() <<'x' << feats.size() << " " << training_set.Size() * feats.size() * sizeof(int32_t)*2 / (1024*1024) << "Mb)" << std::endl;

        response.Precompute(training_set, feats, nThread);
        std::cout << "Process tooks " << t2.GetTime() << " seconds" << std::endl;

        // Initialize Weights
        response.InitializeWeights();

        refresh_countdown = refresh_rate;

        correct_tables = new bool[response.n_samples];
        incremental_response = new double [response.n_samples];
        
        for(int i =0;i<response.n_samples;++i) incremental_response[i] = 0.0;

        std::cout << "Iterate up to " << n_max_iter << " features for boosting..." << std::endl;

        Timer t;
        t.Start();

        for(;;)
        {
            if(refresh_rate !=0)
            {
                if(refresh_countdown==0)
                {
                    t2.Start();

                    feats.clear();
                    ExtractAllFeatures(feats, feature_generator);

                    // if training_set is the same, weights are keeped
                    response.Precompute(training_set, feats, nThread);
                    std::cout << training_set.Size() <<'x' << feats.size() << " patterns processed in " << t2.GetTime() << " seconds" << std::endl;

                    refresh_countdown = refresh_rate;
                }
                else
                    refresh_countdown--;
            }
            // BUILD Inverse Matrix response
            typename Oracle::ClassifierType c;

            // Train
            double w = tr.Train(c, response, nThread);

            /*
            std::cout << "best: " << c.feature_index << ' ' << c.th << ' ' << c.parity << ' ' << w << std::endl;
            */

            double dt =  t.GetTime() / (double) (i_iter + 1);
//             std::cout << i_iter << "# Best Tree detected in " << dt << " average secs" << std::endl;
            std::cout << i_iter << "# Oracle tooks " << dt << "s in average. Expected " << human_readable_time((n_max_iter - i_iter) * dt ) << " to the end of this bootstrap cycle\n";

            // compute error and the correct_tables
            double wp, wn;
            int match;

            // run the indirect classifier on the matrix
            response.ExecuteClassifier(c, correct_tables, &wp, &wn, &match);

            // AdaBoost
            //Z Fn
            double Z = 2.0 * sqrt( wn * wp);
            // return alpha
            double alpha =  0.5 * log ( (wp + _epsilon) / (wn + _epsilon) );

            std::cout <<  i_iter << "# Correct:" << match << "(" << (match*100)/(float)response.n_samples << "%) | W+:" << wp << " | W-:" << wn << " | Z:" << Z << " | alpha:" << alpha << std::endl;

            if(alpha < 1e-3)
            {
                std::cout << " * terminated by algorithm" << std::endl;
                break;
            }

            // create final classifier:
            OutClassifierType out;

            // convert classifier TODO function
            static_cast<typename OutClassifierType::FeatureType&>(out) = feats[ c.feature_index ];
            out.parity = c.parity;
            out.th = c.th;

            m_boost.insert(BoostableClassifier<OutClassifierType>(out, alpha) );

            // evaluate TODO: slow! TODO: creare a TestAndExportStat incrementale
//             m_rep = ::TestAndExportStat( m_boost, training_set, 0.0, nThread);
// 
//             if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
//             {
//                 std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
//                 break;
//             }

            // Update Weights /////// TODO in library

            double bw = exp( alpha );  // weight associated to error (>1)
            double gw = 1.0/bw;        // weight associated to correct detection (<1)
            double z = 0.0;

            for(int i =0; i<response.n_samples; ++i)
            {
                response.weights[i] *= ( correct_tables[i] ) ? gw : bw;
                z += std::abs(response.weights[i]);
            }

            std::cout << "\tbw:" << bw << " | gw:" << gw << " | z:" << z <<std::endl;

            // recipr
            z = 1.0/z;

            // normalize
            for(int i =0; i<response.n_samples; ++i)
                response.weights[i] *= z;

            //////////////////////////////////////////////////////////
            
            for(int i =0; i<response.n_samples; ++i)
            {
              bool hyp = (correct_tables[i]&&(response.category[i]==1)) || ((!correct_tables[i])&&(response.category[i]!=1));
              incremental_response[i] += (hyp) ? alpha : (-alpha);
            }
            
            //////////////////////////////////////////////////////////

            m_rep = ::ExportStat( incremental_response, response.category, response.n_samples, 0.0);
 
             if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
             {
                 std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
                 break;
             }

            i_iter++;
            if(i_iter>=n_max_iter)
            {
                std::cout << " * terminated by number of adaboost stages" << std::endl;
                break;
            }
        }

        delete [] correct_tables;
    }

    const ReportTest & Report() const {
        return m_rep;
    }

    const BoostClassifier< OutClassifierType > & Classifier() const {
        return m_boost;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class _Traits, class Oracle>
class CFastSoftCascadeTrainer {
public:

    typedef _Traits Traits;
    typedef typename Traits::FeatureGeneratorType::FeatureType FeatureType;
    typedef typename Oracle::ClassifierType IndirectClassifierType;
    typedef typename Oracle::template Traits<FeatureType>::ClassifierType OutClassifierType;

private:

    /// last ReportTest
    ReportTest m_rep;

    /// Boosted Classifier
    BoostClassifier<OutClassifierType> m_boost;

    /// SoftCascade Classifier
    SoftCascadeClassifier<OutClassifierType> m_soft;

public:
    /// Execute a train cycle
    CFastSoftCascadeTrainer(  typename Traits::SourceDataSetType & training_set, typename Traits::FeatureGeneratorType & feature_generator, int bstrap_iter)
    {
        int n_max_iter =(bootstrap_increase_iteration) ?  ((bstrap_iter+1) * n_iteration) : n_iteration;
        int iteraz_goal_reached =0;
        int i_iter = 0;
        char conf[256];
        bool *correct_tables;
        double *incremental_response;
        int refresh_countdown;

        // The Inner Trainer
        Oracle tr;

        tr.reoptimize = reoptimize;

        if(!enable_fast_heuristic)
        {
            std::cerr << "Warn: --fast-heuristic is required for this training. It will be automatically activated." << std::endl;
            enable_fast_heuristic = true;
        }

        // NOTE: ignore depth!

        training_set.GetConf(conf);

        // Precompute Feature Response
        PrecomputedPatternResponse<int32_t> response(adaboost_fast_heuristic_bin);

        Timer t2;

        t2.Start();

        std::vector< FeatureType > feats;

        ExtractAllFeatures(feats, feature_generator);

        if(feats.empty())
        {
            std::cerr << "Cannot train: Feature space is empty" << std::endl;
            return;
        }

        std::cout << "Precompute response (" << training_set.Size() <<'x' << feats.size() << " " << training_set.Size() * feats.size() * sizeof(int32_t)*2 / (1024*1024) << "Mb)" << std::endl;

        response.Precompute(training_set, feats, nThread);
        std::cout << "Process tooks " << t2.GetTime() << " seconds" << std::endl;

        // Initialize Weights
        response.InitializeWeights();

        refresh_countdown = refresh_rate;

        correct_tables = new bool[response.n_samples];
        incremental_response = new double [response.n_samples];
        
        for(int i =0;i<response.n_samples;++i) incremental_response[i] = 0.0;

        std::cout << "Iterate up to " << n_max_iter << " features for boosting..." << std::endl;

        Timer t;
        t.Start();

        for(;;)
        {
            if(refresh_rate !=0)
            {
                if(refresh_countdown==0)
                {
                    t2.Start();

                    feats.clear();
                    ExtractAllFeatures(feats, feature_generator);

                    // if training_set is the same, weights are keeped
                    response.Precompute(training_set, feats, nThread);
                    std::cout << training_set.Size() <<'x' << feats.size() << " patterns processed in " << t2.GetTime() << " seconds" << std::endl;

                    refresh_countdown = refresh_rate;
                }
                else
                    refresh_countdown--;
            }
            // BUILD Inverse Matrix response
            typename Oracle::ClassifierType c;

            // Train
            double w = tr.Train(c, response, nThread);

            /*
            std::cout << "best: " << c.feature_index << ' ' << c.th << ' ' << c.parity << ' ' << w << std::endl;
            */

            double dt =  t.GetTime() / (double) (i_iter + 1);
//             std::cout << i_iter << "# Best Tree detected in " << dt << " average secs" << std::endl;
            std::cout << i_iter << "# Oracle tooks " << dt << "s in average. Expected " << human_readable_time((n_max_iter - i_iter) * dt ) << " to the end of this bootstrap cycle\n";

            // compute error and the correct_tables
            double wp, wn;
            int match;

            // run the indirect classifier on the matrix
            response.ExecuteClassifier(c, correct_tables, &wp, &wn, &match);

            // AdaBoost
            //Z Fn
            double Z = 2.0 * sqrt( wn * wp);
            // return alpha
            double alpha =  0.5 * log ( (wp + _epsilon) / (wn + _epsilon) );

            std::cout <<  i_iter << "# Correct:" << match << "(" << (match*100)/(float)response.n_samples << "%) | W+:" << wp << " | W-:" << wn << " | Z:" << Z << " | alpha:" << alpha << std::endl;

            if(alpha < 1e-3)
            {
                std::cout << " * terminated by algorithm" << std::endl;
                break;
            }

            // create final classifier:
            OutClassifierType out;

            // convert classifier TODO function
            static_cast<typename OutClassifierType::FeatureType&>(out) = feats[ c.feature_index ];
            out.parity = c.parity;
            out.th = c.th;

            m_boost.insert(BoostableClassifier<OutClassifierType>(out, alpha) );

//             // evaluate TODO: slow! TODO: creare a TestAndExportStat incrementale
//            m_rep = ::TestAndExportStat( m_boost, training_set, 0.0, nThread);
// 
//             if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
//             {
//                 std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
//                 break;
//             }

            // Update Weights /////// TODO in library

            double bw = exp( alpha );  // weight associated to error (>1)
            double gw = 1.0/bw;        // weight associated to correct detection (<1)
            double z = 0.0;

            for(int i =0; i<response.n_samples; ++i)
            {
                response.weights[i] *= ( correct_tables[i] ) ? gw : bw;
                z += std::abs(response.weights[i]);
            }

            std::cout << "\tbw:" << bw << " | gw:" << gw << " | z:" << z <<std::endl;

            // recipr
            z = 1.0/z;

            // normalize
            for(int i =0; i<response.n_samples; ++i)
                response.weights[i] *= z;
            //////////////////////////////////////////////////////////
            
            for(int i =0; i<response.n_samples; ++i)
            {
              bool hyp = (correct_tables[i]&&(response.category[i]==1)) || ((!correct_tables[i])&&(response.category[i]!=1));
              incremental_response[i] += (hyp) ? alpha : (-alpha);
            }
            
            //////////////////////////////////////////////////////////

            m_rep = ::ExportStat( incremental_response, response.category, response.n_samples, 0.0);
 
             if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
             {
                 std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
                 break;
             }


            i_iter++;
            if(i_iter>=n_max_iter)
            {
                std::cout << " * terminated by number of adaboost stages" << std::endl;
                break;
            }
        }

        delete [] correct_tables;
        delete [] incremental_response;
        
        std::cout << " == begin softcascade stage ==" << std::endl;
        // TODO: save boosted!
	//
	// NOTE: training_set is scrambled!
        typename Traits::SourceDataSetType::ViewType tmp = training_set;	
        TrainSoftCascade(tmp, m_boost, m_soft, m_boost.size(), Ranking_Edge, false, 1.0-cascade_target_tpr, cascade_tpr_decay, true, nThread);
        // save_classifier(output_file, dest, width, height, conf);
    
    }

    const ReportTest & Report() const {
        return m_rep;
    }

    const SoftCascadeClassifier< OutClassifierType > & Classifier() const {
        return m_soft;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// object function for bootstrap_trainer: A Trainer for the Decision Tree
template<class _Traits>
class CDecisionTreeTrainer {
    DecisionTree<typename _Traits::FeatureType > root;

    ReportTest m_rep;
public:
    typedef _Traits Traits;
public:
    CDecisionTreeTrainer( typename Traits::SourceDataSetType & training_set, typename Traits::FeatureGeneratorType  & features, int bstrap_iter)
    {
        // Full DecisionTree
        DecisionTreeLearner< typename Traits::AggregatorType > tr;

        if(prune_decision_tree)
        {
            // TODO unimplemented, yet
            std::cerr << "Unimplemented Yet" << std::endl;
        }
        else
        {
            std::cout << "Building the decision tree..." << std::endl;
            tr.SetTrainingSet(training_set);
            tr.depth = n_max_depth;

            tr.BuildDecisionTree<typename Traits::FeatureType >(root, features);

            std::cout << " == Final Test == \n";
            // tr.Test<typename Traits::FeatureType>(root);

            // evaluate TODO: slow! TODO: creare a TestAndExportStat incrementale
            m_rep = ::TestAndExportStat(root, training_set, 0.0, nThread);
        }

    }

    const ReportTest & Report() const {
        return m_rep;
    }

    const DecisionTree<typename Traits::FeatureType > & Classifier() const {
        return root;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// A "random forest" trainer
template<class _Traits>
class CRandomTreeTrainer {
    BoostClassifier< DecisionTree<typename _Traits::FeatureType> > classifier;

    /// last ReportTest
    ReportTest m_rep;

public:
    typedef _Traits Traits;
public:

    CRandomTreeTrainer( typename Traits::SourceDataSetType & training_set, typename Traits::FeatureGeneratorType  & features, int bstrap_iter)
    {
        char conf[256];
        int iteraz_goal_reached = 0;
        training_set.GetConf(conf);

        DecisionTreeLearner< typename Traits::AggregatorType > tr;
        tr.depth = n_max_depth;

        typename DecisionTreeLearner< typename Traits::AggregatorType >::DataSetType::ViewType sampleSet, validationSet;
        int k = std::min(training_set.n_patternP, training_set.n_patternN) * n_samples;

        for(int i =0; i<n_iteration; ++i)
        {
            DecisionTree<typename Traits::FeatureType > root;
            typename DecisionTreeLearner< typename Traits::AggregatorType >::DataSetType::ViewType sampleSet;

            // sample a subset of training set
            RandomSample(sampleSet, reinterpret_cast<typename DecisionTreeLearner< typename Traits::AggregatorType >::DataSetHandleType*>(0), training_set, k,k);

            std::cout << " =========== " << i << " =========== " << std::endl;

            tr.SetTrainingSet(sampleSet);

            // build a decision tree on the subset
            tr.BuildDecisionTree<typename Traits::FeatureType >(root, features);

            classifier.insert(root);

            std::cout << std::endl;

            save_classifier(tmp_outputfile, classifier, tr.Width(), tr.Height(), conf);

            // evaluate TODO: slow! TODO: creare a TestAndExportStat incrementale
            m_rep = ::TestAndExportStat(classifier, training_set, 0.0, nThread);

            if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
            {
                std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
                break;
            }
        }

        std::cout << " == Final Test == \n";
        m_rep = ::TestAndExportStat(classifier, training_set, 0.0, nThread);
    }

    const ReportTest & Report() const {
        return m_rep;
    }

    const BoostClassifier< DecisionTree<typename _Traits::FeatureType> > & Classifier() const {
        return classifier;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// A "Naive" AdaBoost using DecisionTree object function for boostrap_trainer
///  Trees are generated by a subset of the training set and merged together using Ada_Boost
template<class _Traits>
class CNaiveBoostTreeTrainer {
    BoostClassifier< DecisionTree<typename _Traits::FeatureType> > classifier;

    /// Last report
    ReportTest m_rep;

public:
    typedef _Traits Traits;
public:
// buona parte del codice andrebbe migliorata
    CNaiveBoostTreeTrainer( typename Traits::SourceDataSetType & _training_set, typename Traits::FeatureGeneratorType  & features, int bstrap_iter)
    {
        char conf[256];
        int iteraz_goal_reached = 0;

        _training_set.GetConf(conf);

        typedef typename Traits::FeatureType FeatureType;
        AdaBoost< BoostableClassifier< DecisionTree<FeatureType> > > adaboost;

        DataSetHandle< typename AdaBoostPatternAggregator<typename Traits::PreprocessorType>::AggregatorType > training_set ( _training_set );

        ResetWeight(training_set, -1.0);

        DecisionTreeLearner<  typename AdaBoostPatternAggregator<typename Traits::PreprocessorType>::AggregatorType > tr;
        tr.depth = n_max_depth;

        std::list< DecisionTree<FeatureType> > forest;
        int k = std::min(training_set.n_patternP, training_set.n_patternN) * n_samples; // 1/10
        typename DecisionTreeLearner<  typename AdaBoostPatternAggregator<typename Traits::PreprocessorType>::AggregatorType >::DataSetHandleType sampleSet, validationSet;

        std::cout << "Generating a Forest of " << n_iteration*100 << " trees...." << std::endl;
        for(int i=0; i<n_iteration*100; ++i)
        {
            DecisionTree<FeatureType> root;
            DataSetHandle< typename AdaBoostPatternAggregator<typename Traits::PreprocessorType>::AggregatorType >sampleSet;
            RandomSample(sampleSet, reinterpret_cast< DataSetHandle< typename AdaBoostPatternAggregator<typename Traits::PreprocessorType>::AggregatorType > *>(0), training_set, k,k);
            std::cout << i << ':';
            tr.SetTrainingSet(sampleSet);
            try {
                tr.BuildDecisionTree(root, features);
            }
            catch(std::exception & e)
            {
                std::cerr << "build stopped: " << e.what() << std::endl;
            }

            // TODO: reject bad tree (?)

            // AdaBoost requires a DiscreteClassifier: convert from Real to Discrete:
            root.Discrete();
            forest.push_back(root);
            std::cout << '\n';
        }

        std::cout<< "\nBoosting Forest..." << std::endl;
        for(int i =0; i<n_iteration; ++i)
        {
            try {
                typename std::list< DecisionTree<FeatureType> >::iterator best = forest.end();
                double bestAlpha;
                for( typename std::list< DecisionTree<FeatureType> >::iterator j = forest.begin(); j!=forest.end(); ++j)
                {
                    double alpha =  adaboost.ComputeAlpha(*j, training_set);
                    if((j == forest.begin()) || (alpha>bestAlpha))
                    {
                        bestAlpha = alpha;
                        best = j;
                    }
                }

                double alpha = adaboost.ComputeAlpha(*best, training_set);
                if(alpha>0.0)
                {
                    adaboost.UpdateWeights(training_set, alpha);
                    classifier.insert( *best );
                    forest.erase(best);

                    // test the complete classifier
                    // evaluate TODO: slow! TODO: creare a TestAndExportStat incrementale
                    m_rep = ::TestAndExportStat(classifier, training_set, 0.0, nThread);

                    save_classifier(tmp_outputfile, classifier, tr.Width(), tr.Height(), conf);

                    if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
                    {
                        std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
                        break;
                    }

                }
                else
                {
                    // --i; // TRY AGAIN
                    std::cout << "Failed... todo: increase forest" << std::endl;
                    break;
                }
            }
            catch(std::exception & e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        std::cout << " == Final Test == \n";
        m_rep = ::TestAndExportStat(classifier, training_set, 0.0, nThread);
    }

    const BoostClassifier< DecisionTree<typename _Traits::FeatureType> > & Classifier() const {
        return classifier;
    }

    const ReportTest & Report() const {
        return m_rep;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Convert an IndirectDecisionTree to an DecisionTree
template<class DataType, class FeatureType>
void ConvertDecisionTree(DecisionTree<FeatureType> & out, const IndirectDecisionTree<DataType> & src, const std::vector<FeatureType> & feats)
{
    if(src.left)
    {
        out.th = src.th;
        out.classifier = feats[src.classifier];
        out.left = new DecisionTree<FeatureType>;
        out.right = new DecisionTree<FeatureType>;
        ConvertDecisionTree(*out.left, *src.left, feats);
        ConvertDecisionTree(*out.right, *src.right, feats);
    }
    else
    {
        out.category = src.category; // int to float
    }
}

/// AdaBoost + DecisionTree using precompted feature response matrix
template<class _Traits, bool fast_mode>
class CBoostTreeTrainer {
    /// The classifier
    BoostClassifier< DecisionTree<typename _Traits::FeatureType> > root;

    /// Last report
    ReportTest m_rep;
public:
    typedef _Traits Traits;
public:

    CBoostTreeTrainer( typename Traits::SourceDataSetType & training_set, typename Traits::FeatureGeneratorType  & feature_generator, int bstrap_iter)
    {
        int n_max_iter =(bootstrap_increase_iteration) ?  ((bstrap_iter+1) * n_iteration) : n_iteration;
        int iteraz_goal_reached =0;
        int i_iter = 0;
        char conf[256];
        bool *correct_tables;
        int refresh_countdown;

        if(!enable_fast_heuristic)
        {
            std::cerr << "Warn: --fast-heuristic is required for this training. It will be automatically activated." << std::endl;
            enable_fast_heuristic = true;
        }

//         if(!precompute)
//         {
//             std::cerr << "Warn: --precompute is required for this training. It will be automatically activated." << std::endl;
//             precompute = true;
//         }

        training_set.GetConf(conf);

        // Precompute Feature Response
        PrecomputedPatternResponse<int32_t> response(adaboost_fast_heuristic_bin);

        Timer t2;

        t2.Start();

        std::vector< typename Traits::FeatureGeneratorType::FeatureType > feats;

        ExtractAllFeatures(feats, feature_generator);

        if(feats.empty())
        {
            std::cerr << "Cannot train: Feature space is empty" << std::endl;
            return;
        }

        std::cout << "Precompute response (" << training_set.Size() <<'x' << feats.size() << " " << training_set.Size() * feats.size() * sizeof(int32_t)*2 / (1024*1024) << "Mb)" << std::endl;

        response.Precompute(training_set, feats, nThread);
        std::cout << "Process tooks " << t2.GetTime() << " seconds" << std::endl;

        // Initialize Weights
        response.InitializeWeights();

        refresh_countdown = refresh_rate;

        correct_tables = new bool[response.n_samples];

        std::cout << "Iterate up to " << n_max_iter << " features for boosting..." << std::endl;

        std::vector<int> initial_set;
        for(int i =0; i<training_set.Size(); ++i)
            initial_set.push_back(i);

        Timer t;
        t.Start();

        for(;;)
        {
            if(refresh_rate !=0)
            {
                if(refresh_countdown==0)
                {
                    t2.Start();

                    feats.clear();

                    ExtractAllFeatures(feats, feature_generator);

                    // if training_set is the same, weights are keeped
                    response.Precompute(training_set, feats, nThread);
                    std::cout << training_set.Size() <<'x' << feats.size() << " patterns processed in " << t2.GetTime() << " seconds" << std::endl;

                    refresh_countdown = refresh_rate;
                }
                else
                    refresh_countdown--;
            }
            // BUILD Inverse Matrix response
            // QuantizedPatternResponse<uint32_t> work(response, adaboost_fast_heuristic_bin);

            // find the best decision tree on matrix response [work]
            IndirectDecisionTree<int32_t> tree;

            // TODO
            double w = (fast_mode) ? BuildDecisionTreeHeuristic_v2<int32_t>(tree, response, n_max_depth, initial_set, nThread, adaboost_fast_heuristic_bin, optimize_fast_heuristic) : (enable_fast_heuristic) ?  BuildDecisionTreeHeuristic<int32_t>(tree, response, n_max_depth, initial_set, nThread, adaboost_fast_heuristic_bin, optimize_fast_heuristic) :  BuildDecisionTree<int32_t>(tree, response, n_max_depth, initial_set, nThread);


            double dt =  t.GetTime() / (double) (i_iter + 1);
//             std::cout << i_iter << "# Best Tree detected in " << dt << " average secs" << std::endl;
            std::cout << i_iter << "# BuildDecisionStump tooks " << dt << "s in average. Expected " << human_readable_time((n_max_iter - i_iter) * dt ) << " to the end of this bootstrap cycle\n";
//             tree.print(0);

            // compute error
            double wp, wn;
            int match;

            response.ExecuteClassifier(tree, correct_tables, &wp, &wn, &match);

            // AdaBoost
            //Z Fn
            double Z = 2.0 * sqrt( wn * wp);
            // return alpha
            double alpha =  0.5 * log ( (wp + _epsilon) / (wn + _epsilon) );

            std::cout <<  i_iter << "# Correct:" << match << "(" << (match*100)/(float)response.n_samples << "%) | W+:" << wp << " | W-:" << wn << " | Z:" << Z << " | alpha:" << alpha << std::endl;

            if(alpha < 1e-3)
            {
                std::cout << " * terminated by algorithm" << std::endl;
                break;
            }

            DecisionTree<typename _Traits::FeatureType> xtree;

            // convert tree in xtree
            ConvertDecisionTree(xtree, tree, feats);

            // uses alpha as response for the decision tree
            xtree.scale_response(alpha);

            root.insert(xtree);

            // evaluate
            // evaluate TODO: slow! TODO: creare a TestAndExportStat incrementale
            m_rep = ::TestAndExportStat( root, training_set, 0.0, nThread);

            if(auto_terminate && test_terminate_conditions(m_rep, iteraz_goal_reached) )
            {
                std::cout << " * Terminated by goal tpr, fpr and margin"<<std::endl;
                break;
            }

            // Update Weights

            double bw = exp( alpha );  // weight associated to error (>1)
            double gw = 1.0/bw;        // weight associated to correct detection (<1)
            double z = 0.0;

            for(int i =0; i<response.n_samples; ++i)
            {
                response.weights[i] *= ( correct_tables[i] ) ? gw : bw;
                z += std::abs(response.weights[i]);
            }

            std::cout << "\tbw:" << bw << " | gw:" << gw << " | z:" << z <<std::endl;

            // recipr
            z = 1.0/z;

            // normalize
            for(int i =0; i<response.n_samples; ++i)
                response.weights[i] *= z;

            i_iter++;
            if(i_iter>=n_max_iter)
            {
                std::cout << " * terminated by number of adaboost stages" << std::endl;
                break;
            }
        }

        delete [] correct_tables;
    }

    const ReportTest & Report() const {
        return m_rep;
    }

    const BoostClassifier< DecisionTree<typename _Traits::FeatureType> > & Classifier() const {
        return root;
    }

};

//////////////////////////////////////////////////////////////////

/// add BootStrap capabilities to a Trainer object
template<class Trainer>
void bootstrap_trainer(typename Trainer::Traits::SourceDataSetType & training_set, typename Trainer::Traits::FeatureGeneratorType  & features)
{
    char buff[128]; // configuration data

    if(training_set.n_patternP == 0)
    {
      std::cerr << "Fatal Error: no positives in the set. Training cannot be performed" << std::endl;
      return;
    }

    if(training_set.n_patternN == 0)
    {
      std::cerr << "Fatal Error: no negatives in the set. Training cannot be performed" << std::endl;
      return;
    }
    
    training_set.GetConf(buff);

    // iterate up to bootstrap_iteration
    for(int bstrap_iter = 0; bstrap_iter < bootstrap_iteration; ++bstrap_iter)
    {
        std::cout << "=== Bootstrap #" << bstrap_iter + 1 << "/" << bootstrap_iteration << " (" << training_set.n_patternP <<  " positive and " << training_set.n_patternN << " negative samples) ===" << std::endl;
        Trainer tr(training_set, features, bstrap_iter);

        if(tr.Report().Fpr() > 0.0)
            std::cout << "Warn: The training process did not reach 0% on FPR! Bootstrapping could be inaccurate in this case. Try to modify parameters or check the training set" << std::endl;
        // TODO: se non ha raggiunto il 100% dei negativi segnalarlo con un warning!

        // last one:
        if(bstrap_iter == bootstrap_iteration-1)
            save_classifier(outputfile, tr.Classifier(), training_set.Width(), training_set.Height(), buff);
        else
        {
            char buffer[1024];
            sprintf(buffer, "%s-%u", outputfile, bstrap_iter);
            save_classifier(buffer, tr.Classifier(), training_set.Width(), training_set.Height(), buff);
        }


        // except for the last iteration:
        if(bstrap_iter != bootstrap_iteration-1)
        {
            std::cout << "Aggragate negatives..." << std::endl;

            // ADD NEGATIVE using existing classifier
            BootStrapParams params;

            params.negative_random_samples = (bootstrap_negatives<0) ? negative_random_samples : bootstrap_negatives;
            params.thMin = thMin;
            params.nms_step = std::min(training_set.width, training_set.height); // non overlapping box
            params.scale_factor = bootstrap_scale_factor;
            params.downsampling = (bstrap_step>0) ? (bstrap_step) : (std::max<int>(1, (int)(training_set.width * 0.08))); // 1 or 8%

            sdfin in(dataset);
            bool result = BootStrapProcedure(training_set, in, tr.Classifier(), params, nThread);

            if(result)
            {
                // final classifier
                save_classifier(outputfile, tr.Classifier(), training_set.Width(), training_set.Height(), buff);
                std::cout << "* Bootstrap completed. Terminate\n";
                break;
            }
        }

    }
}

//////////////////////////////////////////////////////////////////


/// "factory" for the trainer
struct trainer_list_type {
    /// algorithm name
    const char *algoname;
    /// an helpful description
    const char *description;
    /// the callback for Haar feature trainer
    void (* haar_trainer)( typename traits::HaarFeature::SourceDataSetType &, typename traits::HaarFeature::FeatureGeneratorType &  );
    /// the callback for IntegralChannel feature trainer
    void (* ichn_trainer)( typename traits::IntegralChannelFeature::SourceDataSetType &, typename traits::IntegralChannelFeature::FeatureGeneratorType &  );
};

typedef PatternAggregator<WeightedPattern< IntegralImageData >, IntegralChannelImageParams>  ICFAggregator;

/// haar feature trainer list
static const trainer_list_type trainer_list[] = {
    {   "adaboost-stump",               "AdaBoost using Decision Stump",
        &bootstrap_trainer< CAdaBoostTrainer< traits::HaarFeature, BinaryClassifierOracle< HaarFeatureGenerator, DecisionStump<int>, AdaBoostMetric, HaarAdaBoostAggregator > > >,      &bootstrap_trainer< CAdaBoostTrainer< traits::IntegralChannelFeature, BinaryClassifierOracle< IntegralChannelFeatureGenerator, DecisionStump<int>, AdaBoostMetric, ICFAggregator> > >
    },
    {   "gentleadaboost-stump",         "GentleAdaBoost using Decision Stump",
        &bootstrap_trainer< CAdaBoostTrainer< traits::HaarFeature, RealDecisionStumpOracle< HaarFeatureGenerator, HaarAdaBoostAggregator, GentleAdaBoostMetric > > >,                   &bootstrap_trainer< CAdaBoostTrainer< traits::IntegralChannelFeature, RealDecisionStumpOracle< IntegralChannelFeatureGenerator, ICFAggregator, GentleAdaBoostMetric > > >
    },
    {   "realadaboost-stump",           "RealAdaBoost using Decision Stump",
        &bootstrap_trainer< CAdaBoostTrainer< traits::HaarFeature, RealDecisionStumpOracle< HaarFeatureGenerator, HaarAdaBoostAggregator, RealAdaBoostMetric > > >,                     &bootstrap_trainer< CAdaBoostTrainer< traits::IntegralChannelFeature, RealDecisionStumpOracle< IntegralChannelFeatureGenerator, ICFAggregator, RealAdaBoostMetric > > >
    },
    {   "adaboost-bayesian-stump",      "AdaBoost using Bayesian Stump",
        &bootstrap_trainer< CAdaBoostTrainer< traits::HaarFeature, BayesianStumpOracle< HaarFeatureGenerator, HaarAdaBoostAggregator> > >,                                              &bootstrap_trainer< CAdaBoostTrainer< traits::IntegralChannelFeature, BayesianStumpOracle< IntegralChannelFeatureGenerator, ICFAggregator > > >
    },
    {   "adaboost-naive-decision-stump","AdaBoost using Decision Stump with threshold 0",
        &bootstrap_trainer< CAdaBoostTrainer< traits::HaarFeature, NaiveDecisionStumpOracle< HaarFeatureGenerator, HaarAdaBoostAggregator > >  >,                                       &bootstrap_trainer< CAdaBoostTrainer< traits::IntegralChannelFeature, NaiveDecisionStumpOracle< IntegralChannelFeatureGenerator, ICFAggregator > > >
    },
    {   "fast-adaboost-stump",          "AdaBoost using Decision Stump and precomputing feature response",
        &bootstrap_trainer< CFastAdaBoostTrainer< traits::HaarFeature, PrecomputedPatternBinaryClassifierOracle<  DecisionStump<int> > > >, &bootstrap_trainer< CFastAdaBoostTrainer< traits::IntegralChannelFeature, PrecomputedPatternBinaryClassifierOracle<  DecisionStump<int> > > >
    },
    {   "fast-softcascade-stump",       "SoftCascade AdaBoost using Decision Stump and precomputing feature response",
        &bootstrap_trainer< CFastSoftCascadeTrainer< traits::HaarFeature, PrecomputedPatternBinaryClassifierOracle<  DecisionStump<int> > > >, &bootstrap_trainer< CFastSoftCascadeTrainer< traits::IntegralChannelFeature, PrecomputedPatternBinaryClassifierOracle<  DecisionStump<int> > > >
    },
    {   "decision-tree",                "A large decision Tree",
        &bootstrap_trainer< CDecisionTreeTrainer< traits::HaarFeature > >,        &bootstrap_trainer< CDecisionTreeTrainer< traits::IntegralChannelFeature > >
    },
    {   "random-tree",                  "Random Decision Tree voting per majority",
        &bootstrap_trainer< CRandomTreeTrainer< traits::HaarFeature > >,          &bootstrap_trainer< CRandomTreeTrainer< traits::IntegralChannelFeature > >
    },
    {   "naive-boosted-tree",           "Several \"naive\" decision trees merged together using AdaBoost",
        &bootstrap_trainer< CNaiveBoostTreeTrainer< traits::HaarFeature > >,      &bootstrap_trainer< CNaiveBoostTreeTrainer< traits::IntegralChannelFeature > >
    },
    {   "boosted-tree",                 "Several decision trees merged together using AdaBoost",
        &bootstrap_trainer< CBoostTreeTrainer< traits::HaarFeature, false > >,           &bootstrap_trainer< CBoostTreeTrainer< traits::IntegralChannelFeature, false > >
    },
    {   "boosted-tree-fast",            "Several decision trees merged together using AdaBoost, but using a stronger heuristic",
        &bootstrap_trainer< CBoostTreeTrainer< traits::HaarFeature, true > >,           &bootstrap_trainer< CBoostTreeTrainer< traits::IntegralChannelFeature, true > >
    },

    {0,0,0,0}
};

void help_algo()
{
    std::cout << "Available Trainer are:\n";
    for(const trainer_list_type*ptr = trainer_list; (ptr->algoname); ptr++)
    {
        std::cout << "\n  " << ptr->algoname;
        if(ptr->haar_trainer) std::cout << " [haar]";
        if(ptr->ichn_trainer) std::cout << " [ichn]";
        std::cout << "\n\t  " << ptr->description << "\n";
    }
}

void help()
{
    std::cout << "Usage: trainer [OPTIONS] <training set> [OPTIONS]\n\n"
              " -v\n\tBe More Verbose\n"
              " -i inputFile\n\tContinue training starting from a previous trained file (valid only for *boost algorithm) [deprecated]\n"
              " -o outputFile\n\tOutput file (default is output.dat)\n"
              " -r <w> <h>\n\tSet the geometry of samples. Images will be resampled.\n"

              " -j preload\n\tPreload multiple features and process it (default 512)\n"
//               " --precompute\n\tInstead of evaluate all the features each iteration, precompute the features response and reuse them (requires a lot of memory).\n"
              " --refresh <n>\n\tRefresh keeps the precomputed values up to n stages.\n"
#ifdef _MULTITHREAD
              " -t <thread>\n\tN Thread in MultiThread training\n"
              " -t max\n\tUses automatically all cores available\n"
#endif
              " -f <feature type>\n\tType of feature: haar or integralchannel.\n"

              " -a <algorithm>\n\tSelect the trainer. Use -a help per a list of valid trainer."

              " -g <feature generator> <params>\n\tSelect a feature generator. The default is the BruteForce (bf) algorithm (Haar and ICHN).\n"
              " -g rs <n>\n\tRandom feature sampler generator (Haar and ICHN)\n"
              " -g jsgf <n>\n\tJoint Sparse Granular Features generator (only for haar feature)\n"

              " --fsize minFeatSize\n\tMinimum size of a feature w*h>=th (default 1) generated \n"
              " --fwidth minFeatWidth\n\tMinimum size of a feature w>=th and h>=th (default 1) generated \n"
              " --fstep step\n\tSet the step in the feature spatial quantization\n"
//               " --fborder n\n\tAvoid feature around n pixel from the image border\n"

              "\nOptions for Haar features:\n"
              " -p <pconf>\n\tOne of " << BaseHaarFeatureGenerator::default_pattern<< "\n"
              " -p iso\n\tuse only unbiased pconf " << iso_pattern << "\n"
              " -p std\n\tuse only standard pconf " << standard_pattern << "\n"

              "\nOptions for Integral Channel Features:\n"
              " -p <n>[s][m][l]>\n\tOptions for create ICF. n bins, s use sign, m gradient module, l luminance\n" //nsml

              "\nOptions for *Boost (AdaBoost, GentleBoost, RealBoost) algorith:\n"
              " --positive-weight <w>\n\tSet the sum of weight for positive set, for example 0.5. The default is that any patter have the same weight\n"
              " --reoptimize\n\tWhen using a previous training file reoptimize classifier before append new classifier using the new dataset\n"
              " --mada\n\tUses MAdaBoost updating rule\n"

              "\nOptions for terminate training for *Boost algorithm:\n"
              " -n numIterations\n\tMaximum number of iterations (default 100). With an additional + it is possible to increase the number for any bootstrap cycle.\n"
              " --auto-terminate\n\tTerminate training when there are no errors on training set\n"
              " --terminate-condition <tpr> <fpr> <n>\n\tThreshold to autoterminate after n iterarions that satisfy tpr & fpr\n"
              " --tpr-goal <tpr>\n\tSet the true positive rate goal for auto-terminate\n"
              " --fpr-goal <fpr>\n\tSet the false positive rate goal for auto-terminate\n"
              " --margin-goal <margin>\n\tSet the margin goal for auto-terminate\n"

              "\nOptions for decision tree/decision stump:\n"
//     std::cout << " --prune\n\tPrune Decision Tree\n";
              " -d depth\n\tMaximum depth of Decision Tree algorithms. 1 means Decision Stump. (default 1)\n"
              " --fast-heuristic <n>\n\tInstead of find the exact threshold for decision stump, quantize the response in <n> bins\n"
              " --avoid-optimize\n\tSkip reoptimization step when using the fast heuristic algorithm\n"
              
              "\tOptions for *Cascade:\n"
              " --cascade-tpr <n>\n\tTarget True Positive Rate\n"
              " --cascade-fpr <n>\n\tTarget False Positive Rate\n"
              " --cascade-tpr-decay\n\tSoftCascade exponential factor for spread TPR\n"

              "\nOptions for BootStrapping:\n"
              " -N <n>\n\tSet the number of negative random extraced from any frame of dataset for create the first set.\n"
              " --bstrap-negatives <n>\n\tSet the number of negative extraced from any bootstrapped frame (default is the value set by previous command).\n"
              " --bstrap <n>\n\tSet the number of bootstrap iteration (default 1).\n"
              " --bstrap-scale-factor <f>\n\tSet the multiscale factor (default 1.189207115).\n";

    std::cout.flush();
}

const trainer_list_type *find_trainer(const char *algorithm)
{
    const trainer_list_type *ptr = trainer_list;
    while(ptr->algoname)
    {
        if(!strcmp(algorithm ,ptr->algoname))
        {
            return ptr;
        }
        ptr++;
    }

    std::cerr << algorithm << " not found. Available algorithms are: ";
    ptr = trainer_list;
    while(ptr->algoname)
    {
        if(ptr!=trainer_list) std::cerr << ", ";
        std::cerr << ptr->algoname;
        ptr++;
    }
    std::cerr << std::endl;

    return 0;
}


/// The Haar Feature Trainer collects all the ensemble learning algorithm to train using haar feature descriptor (bruteforce or random)
int haar_feature_trainer()
{
    std::cout << "Train using " << algorithm << " on haar features" << std::endl;

    traits::HaarFeature::SourceDataSetType training_set;

    // eventually store on disk the bootstrapped/resampled image:
    if(export_folder)
        training_set.SetExportFolder(export_folder);

    // load dataset from file
    {
        Timer t;
        sdfin in(dataset);
        LoadSet(training_set, in, samples_geometry, negative_random_samples);
        std::cout << training_set.Size() << " patterns loaded in " << t.GetTime() << " seconds.\n";
        std::cout << training_set.n_patternP <<  " positive and " << training_set.n_patternN << " negative samples\n";
    }

    std::auto_ptr<HaarFeatureGenerator> feature_generator;

    // TODO: those configuration are valid only for "brute_force" and "random sampling"
    // JSGF is a different concept
    if(pconf == 0)
        pconf = BaseHaarFeatureGenerator::default_pattern;
    else if(!strcmp(pconf, "all"))
        pconf = BaseHaarFeatureGenerator::default_pattern;
    else if(!strcmp(pconf, "iso"))
        pconf = iso_pattern;
    else if(!strcmp(pconf, "std"))
        pconf = standard_pattern;
    // factory?
    if(!strcmp(generator_algorithm,"bf"))
    {
        BruteForceFeatureGenerator *f = new BruteForceFeatureGenerator(pconf);
        f->SetFeatureMinArea(minFeatSize);
        f->SetStep(featStep);
        f->SetBorder(0);
        f->SetFeatureMinWidth(minFeatWidth);
        f->SetGeometry(training_set.Width(), training_set.Height());

        feature_generator = std::auto_ptr<HaarFeatureGenerator>(f);
    }
    else if(!strcmp(generator_algorithm,"rs"))
    {
        RandomFeatureGenerator *f = new RandomFeatureGenerator(pconf, randNum);
        f->SetFeatureMinArea(minFeatSize);
        f->SetStep(featStep);
        f->SetBorder(0);
        f->SetFeatureMinWidth(minFeatWidth);
        f->SetGeometry(training_set.Width(), training_set.Height());

        feature_generator = std::auto_ptr<HaarFeatureGenerator>(f);
    }
    else if(!strcmp(generator_algorithm,"jsgf"))
    {
        JointSparseGranularFeatureGenerator *f = new JointSparseGranularFeatureGenerator(randNum);
        f->SetFeatureMinArea(minFeatSize);
        f->SetStep(featStep);
        f->SetBorder(0);
//     f->SetFeatureMinWidth(minFeatWidth);
        f->SetGeometry(training_set.Width(), training_set.Height());

        feature_generator = std::auto_ptr<HaarFeatureGenerator>(f);
    }

    if(feature_generator.get() == 0)
    {
        std::cerr << "Invalid feature generator " << generator_algorithm << std::endl;
        std::cerr << "\tvalid values are: bf rs jsgf" << std::endl;
        return -1;
    }

    const trainer_list_type *ptr = find_trainer(algorithm);
    if(ptr && ptr->haar_trainer)
    {
        try {
            ptr->haar_trainer(training_set, *feature_generator);
        }
        catch(const char *err)
        {
            std::cerr << algorithm << " terminate called after throwing: " << err << std::endl;
        }
    }
    return 0;
}

int integral_channel_feature_trainer()
{
    int orient_bin = 9;
    bool use_luminace = true;
    bool use_magnitude = true;
    bool use_sgn_orientation = false;

    traits::IntegralChannelFeature::SourceDataSetType training_set;

    // eventually store on disk the bootstrapped/resampled image:
    if(export_folder)
        training_set.SetExportFolder(export_folder);

    // parse pconfiguration
    if(pconf == 0)
    {
        // default
        orient_bin = 9;
        use_sgn_orientation = true;
        use_magnitude = true;
        use_luminace = true;
    }
    else
    {
        const char *str = pconf;
        orient_bin = atoi(str);

        if(orient_bin<2)
            orient_bin = 2; // TODO:

        if(strchr(str,'s'))
            use_sgn_orientation = true;
        if(strchr(str,'m'))
            use_magnitude = true;
        if(strchr(str,'l'))
            use_luminace = true;
    }

    std::cout << "Train using " << algorithm << " on integral channel features (" << orient_bin;
    if(use_sgn_orientation) std::cout << 's';
    if(use_magnitude) std::cout << 'm';
    if(use_luminace) std::cout << 'l';
    std::cout << ")" << std::endl;

    training_set.Configure(use_sgn_orientation, orient_bin, use_magnitude, use_luminace,filter_type);

    // load dataset from file
    {
        Timer t;
        sdfin in(dataset);
        LoadSet(training_set, in, samples_geometry, negative_random_samples);
        std::cout << training_set.Size() << " patterns loaded in " << t.GetTime() << " seconds.\n";
        std::cout << training_set.n_patternP <<  " positive and " << training_set.n_patternN << " negative samples\n";
    }


    std::cout << training_set.Size() << " patterns correctly loaded\n";

    typedef PatternAggregator<WeightedPattern< IntegralImageData >, IntegralChannelImageParams>  ICFAggregator;

    std::auto_ptr<IntegralChannelFeatureGenerator> feature_generator;

    // factory?
    if(!strcmp(generator_algorithm,"bf"))
    {
        BruteForceIntegralChannelFeatureGenerator *f = new BruteForceIntegralChannelFeatureGenerator();
        f->SetParams(training_set.NChannels(),training_set.Width(),training_set.Height());
        f->SetFeatureMinArea(minFeatSize);
        f->SetStep(featStep);
        f->SetBorder(1); //
        f->SetFeatureMinWidth(minFeatWidth);
//         f->SetGeometry(training_set.Width(), training_set.Height());

        feature_generator = std::auto_ptr<IntegralChannelFeatureGenerator>(f);
    }
    else if(!strcmp(generator_algorithm,"rs"))
    {
        RandomIntegralChannelFeatureGenerator *f = new RandomIntegralChannelFeatureGenerator(randNum);
        f->SetParams(training_set.NChannels(),training_set.Width(),training_set.Height());
        f->SetFeatureMinArea(minFeatSize);
        f->SetStep(featStep);
        f->SetBorder(1);
        f->SetFeatureMinWidth(minFeatWidth);
//         f->SetGeometry(training_set.Width(), training_set.Height());

        feature_generator = std::auto_ptr<IntegralChannelFeatureGenerator>(f);
    }

    if(feature_generator.get() == 0)
    {
        std::cerr << "Invalid feature generator " << generator_algorithm << std::endl;
        std::cerr << "\tvalid values are: bf rs" << std::endl;
        return -1;
    }

    const trainer_list_type *ptr = find_trainer(algorithm);
    if(ptr && ptr->ichn_trainer)
    {
        try {
            ptr->ichn_trainer(training_set, *feature_generator);
        }
        catch(const char *err)
        {
            std::cerr << algorithm << " terminate called after throwing: " << err << std::endl;
        }
    }
    return 0;
}

///// feature trainer /////

/// 
struct feature_list_type {
    const char *name;
    const char *description;

    int (* procedure)();
};

static const feature_list_type feature_list[] = {
    {"haar", "Haar like Features", haar_feature_trainer},
    {"ichn", "Integral Channel Features", integral_channel_feature_trainer},
    {0,0,0}
};

/////// ////////////////////

int main( int argc, char *argv[])
{
    std::cout << "xBoost Trainer " XBOOST_VERSION << std::endl;

    // default algorithm
    algorithm = trainer_list[0].algoname;

    // parse the command line
    if(!parse_cmd_line(argc, argv))
        return -1;

    // create the tmp_outputfile
    sprintf(tmp_outputfile, "%s.tmp", outputfile);

    std::cout << "[command line: ";
    for(int i =0; i<argc; ++i)
        std::cout << ' ' << argv[i];
    std::cout << "]\n";

    ///////////////// check the parameters //////////

    // at least
    if(bootstrap_iteration<1)
        bootstrap_iteration = 1;

    ////////////////////////////////////////////////

    try {

        // check callback in the feature factory
        for(const feature_list_type *i = feature_list; i->name != 0; ++i)
        {

            if(!strcmp(feature_type, i->name))
                return i->procedure();
        }

        std::cout << "Unknown feature type " << feature_type << std::endl;
        std::cout << "Available feature type are:\n";
        for(const feature_list_type *i = feature_list; i->name != 0; ++i)
        {
            std::cout << i->name << "\n\t" << i->description << std::endl;
        }
    }
    catch(const char *str_error)
    {
        std::cerr << "Program terminate with an exception: " << str_error << std::endl;
    }

    return 0;
}
