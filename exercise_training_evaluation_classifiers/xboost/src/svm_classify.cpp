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


// ATTUALMENTE QUESTO CODICE GENERA UN FILE PER la LIBLINEAR/LIBSVM


// #include "Trainer/SVM.h"
#include "Descriptor/RawData.h"
#include "HoGDataSet.h"
#include "Trainer/HoGCommon.h"
#include "Classifier/HyperplaneClassifier.h"
#include "Utility/Utils.h"
#include "IO/sdfin.h"
#include "Test.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <iostream>
#include <set>

void help()
{
    std::cout <<
              "svm_classify <dataset> <model> [options]\n"
              "-o <file>\n\tOutput filename\n"
              "-f <feature>\n\tFeature Type (default hog)\n"
              "-n <n>\n\tRandom negative samples per frame\n"
              "-r <w> <h>\n\tTrain Pattern geometry\n"
              "\n"
              "HoG Parameters:\n"
              "-g <w> <h>\n\tCell geometry (default 8x8)\n"
              "-b <w> <h>\n\tCells per Block (default 3x3)\n"
              "-l <w> <h>\n\tBlock Step (default 8x8)\n"
              "-n <n>\n\tHystogram bins\n"
              "-s\n\tKeep HoG signs\n"
              "-d <algo>\n\tUse a different gradient algorithm\n";
    std::cout.flush();
}

const char *dataset_filename = 0;
const char *model_filename = 0;
const char *output_filename = 0;

static size samples_geometry(0,0); // TODO 
static int random_negative =0;

// TODO this parameters should be imported by HOG-train file 
int cell_width = 8;
int cell_height = 8;
int block_width = 3;
int block_height = 3;
int h_step = 8;
int v_step = 8;
int hog_bin = 8;
bool grad_sign = false;
        
bool parse_cmd_line(int argc, char *argv[])
{
    // Parse Command Line
    if(argc < 3)
    {
        help();
        return false;
    }

    int i = 1;
    while(i<argc)
    {
        const char *str = argv[i];
        if(!strcmp(str, "-o") )
        {
            i++;
            output_filename = argv[i];
        }
        else if(!strcmp(str, "-g") )
        {
            i++;
            cell_width = atoi(argv[i]);
            i++;
            cell_height = atoi(argv[i]);
        }
        else if(!strcmp(str, "-b") )
        {
            i++;
            block_width = atoi(argv[i]);
            i++;
            block_height = atoi(argv[i]);
        }
        else if(!strcmp(str, "-l") )
        {
            i++;
            h_step = atoi(argv[i]);
            i++;
            v_step = atoi(argv[i]);
        }
        else if(!strcmp(str, "-n") )
        {
            i++;
            hog_bin = atoi(argv[i]);
        }
        else if(!strcmp(str, "-u") )
        {
            grad_sign = true;
        }        
        else if(dataset_filename == NULL)
            dataset_filename = str;
        else if(model_filename == NULL)
            model_filename = str;
        else
        {
            std::cout << "Unknown parameter: " << str << std::endl;
            help();
            return false;
        }
        i++;
    }

    return true;
}

int main(int argc, char *argv[])
{

    if(!parse_cmd_line(argc, argv))
        return 0;

    if(dataset_filename == NULL || model_filename == NULL)
    {
        help();
        return 0;
    }

    HyperPlaneClassifier<double> classifier;
    unsigned int width, height;
    
    load_classifier(model_filename, classifier, width, height);
    
    samples_geometry.width = width;
    samples_geometry.height = height;
    
    // 1) LOAD Training Set and Generate Hog Feature
    SourceDataSet< HoGPreprocessor, HogSVMAggregator > train_set;

    train_set.SetCellGeometry(cell_width, cell_height);
    train_set.SetBlockGeometry(block_width, block_height);
    train_set.SetBlockStep(h_step, v_step);
    train_set.SetHystogramBins(hog_bin);
    train_set.EnableSign(grad_sign);
    

    {
        sdfin in(dataset_filename);
        LoadSet(train_set, in, samples_geometry, random_negative);
    }
    
    

    
    if(  train_set.GetParams().desc_size != classifier.n)
    {
      std::cerr << "invalid descriptor size" << std::endl;
      return -1;
    }
        
    if(output_filename)
    {
    std::ofstream out(output_filename);
    DumpRoc(out, classifier, train_set);
    }
    else
    {
    DumpRoc(std::cout, classifier, train_set);
    }
    
    return 0;
}
