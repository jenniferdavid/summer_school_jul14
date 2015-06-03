/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This file is part of AdaBoost Tools
 *
 *  Classify is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Classify is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Classify.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @brief Classify shows how to test classification on a pattern of fixed size, without resampling.\
 *      an example of use of ImageClassifier class (and allocate_classifier_from_file factory).
 */

// TODO: use SDF format
// TODO: set geometry

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath> // abs
#include "IO/pnmio.h"
#include "IO/sdfin.h"
#include "Utility/ImageSet.h"
#include "xbversion.h"

#include "factory/allocate.h"

#include "Utility/ImageClassifierOperator.h"

#include "_stdint.h"

using namespace std;

void help()
{
    std::cout << "usage:\n"
              "\t1) classify <training file> <pos set> <neg set> [OPTIONS]\n\n"
              "\t2) classify <training file> <set description file> [OPTIONS]\n\n"
              " -v\n\tVerbose output\n"
              " -vv\n\tMore Verbose output\n"
              " -vvv\n\tMore Verbose output with response as first element\n"
              " -q\n\tNot print statistics\n"
              " --list\n\tPrint list of available classifier\n"
              " -t threshold\n\tSet separation threshold (default 0)\n";
              " -N <n>\n\tNumber of negative samples extracted randomply by each scale and image\n";
    std::cout.flush();
}

// global parameters

const char *network_file = NULL, *pos_data_set = NULL, *neg_data_set = NULL;
const char *output_file = NULL;
double threshold = 0.0;
bool statistics = true;
int verbose = 0;

int negative_random_samples = 10;

// a check if the argument is valid
#define check_param(i)  if(i>=argc) { std::cerr << str <<  ": missing parameters" << std::endl; return false; }

bool parse_cmd_line(int argc, char *argv[])
{
    // Parse Command Line
    if(argc < 2)
    {
        help();
        exit(0);
    }

    int i = 1;
    while(i<argc)
    {
        const char *str = argv[i];

        if ((!strcmp(str, "-h") ) || (!strcmp(str, "--help") ) )
        {
            help();
            exit(0);
        }
        else if(!strcmp(str, "--list") )
        {
            print_classifier_list(std::cout);
            exit(0);
        }

        else if(!strcmp(str, "-v") )
        {
            verbose = 1;
        }
        else if(!strcmp(str, "-q") )
        {
            statistics = false;
        }
        else if(!strcmp(str, "-vv") )
        {
            verbose = 2;
        }
        else if(!strcmp(str, "-vvv") )
        {
            verbose = 3;
        }
        else if(!strcmp(str, "-o") )
        {
            i++;
            output_file = argv[i];
        }
        else if(!strcmp(str, "-N") )
        {
            i++;
            check_param(i);
            negative_random_samples = atoi( argv[i] );
        }
        else if(!strcmp(str, "-t") )
        {
            i++;
            check_param(i);
            threshold = atof(argv[i]);
        }
        else if(network_file == NULL)
            network_file = str;
        else if(pos_data_set == NULL)
            pos_data_set = str;
        else if(neg_data_set == NULL)
            neg_data_set = str;
        else
        {
            std::cerr << "invalid command line options:" << str << std::endl;
            help();
            exit(0);
        }
        i++;
    }
    
    return true;
}

// version with 2 separates sets, one for positive and one for negative
void evaluate_r(ImageClassifier * c)
{
    ImageClassifierOperator icop(c);
    icop.setThreshold(threshold);
    icop.setVerbosity(verbose);

    // POSITIVE
    std::ifstream f1(pos_data_set);
    std::string str;
    while(f1.good())
    {
        f1 >> str;
        if(!f1.good())
            break;
        Image img_in;
        if(!pnm_load(str.c_str(), img_in))
        {
            std::cout << "Warn: \'" << str << "' failed to load: skip." << std::endl;
        }
        else
        {
            icop(str, img_in, 1);

            img_in.release();
        }
    }

//NEGATIVE
    f1.close();
    f1.open(neg_data_set);
    while(f1.good())
    {
        f1 >> str;
        if(!f1.good())
            break;
        cout << str <<endl;
        Image img_in;
        if(!pnm_load(str.c_str(), img_in))
        {
            std::cout << "Warn: \'" << str << "' failed to load: skip." << std::endl;
        }
        else
        {
            icop(str, img_in, -1);
            img_in.release();
        }
    }

    if(statistics)
        icop.Done();

}


// version with 1 sets (and eventually random extracted samples)
void evaluate_rs(ImageClassifier * c)
{
    ImageClassifierOperator icop(c);
    icop.setThreshold(threshold);
    icop.setVerbosity(verbose);

    size samples_geometry = c->GetClassifierGeometry();

    sdfin in(pos_data_set);
    ProcessSet(in, samples_geometry, negative_random_samples, icop);

    if(statistics)
        icop.Done();
}

int main(int argc, char *argv[])
{
    std::cout << "xBoost Classifier " XBOOST_VERSION << std::endl;

    if(!parse_cmd_line(argc, argv))
      return -1;

    // uses facatory to allocate an ImageClassifier
    ImageClassifier *c = allocate_classifier_from_file(network_file);

    if(c)
    {
        template_geometry size = c->GetClassifierGeometry();
        std::cout << "loaded classifier " << size.width << " "<<size.height << endl;

        if(pos_data_set!=0 && neg_data_set==0)
        {
            evaluate_rs(c);
        }
        else
        {
            evaluate_r(c);
        }
        delete c;
    }
    else
    {
    }

    return 0;
}
