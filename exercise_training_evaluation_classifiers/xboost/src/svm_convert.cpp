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


#include "Descriptor/RawData.h"
#include "HoGDataSet.h"
#include "Trainer/HoGCommon.h"
#include "IO/sdfin.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <iostream>

void help()
{
    std::cout <<
              "svm_train <dataset> [options]\n"
//  "-C <weight>\n\tMargin SVM C parameters\n"
              "-o <file>\n\tOutput filename\n"
              "-f <feature>\n\tFeature Type (default hog)\n"
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

static size samples_geometry(0,0);
static int random_negative =0;

const char *dataset_filename = 0;
const char *output_filename = 0;
double C = 1000000.0;
int cell_width = 8;
int cell_height = 8;
int block_width = 3;
int block_height = 3;
int h_step = 8;
int v_step = 8;

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
        if(!strcmp(str, "-C") )
        {
            i++;
            C = atof(argv[i]);
        }
        else if(!strcmp(str, "-o") )
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
        else if(dataset_filename == NULL)
            dataset_filename = str;
        else
        {
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

    if(dataset_filename == NULL)
    {
        help();
        return 0;
    }

    // 1) LOAD Training Set and Generate Hog Feature
    SourceDataSet< HoGPreprocessor, HogSVMAggregator > train_set;

    train_set.SetCellGeometry(cell_width, cell_height);
    train_set.SetBlockGeometry(block_width, block_height);
    train_set.SetBlockStep(h_step, v_step);

    {
        sdfin in(dataset_filename);
        LoadSet(train_set, in, samples_geometry, random_negative);
    }
    
    if(!train_set.templates.empty())
      train_set.debug_geometry(train_set.GetParams().width, train_set.GetParams().height);
    
    // 3) Train SVM
    std::ostream *out = &std::cout;
    if(output_filename )
        out = new std::ofstream(output_filename);

    //
    int desc_size = train_set.GetParams().desc_size;

//         *out << desc_size << std::endl;

    for(int i =0; i<train_set.templates.size(); ++i)
    {
        *out << train_set.templates[i].category;

        for(int j =0; j<desc_size; ++j)
            *out << '\t' << j+1 << ':'<< train_set.templates[i].data[j];
//         *out << '\t' << train_set.templates[i].data[j];

        *out << std::endl;
    }

    if(output_filename )
        delete out;

#if 0
    svm_train svm(2);
    const char *input_file = 0;

    // Parse Command Line
    {
        int i = 1;
        while(i<argc)
        {
            const char *str = argv[i];

            if(!strcmp(str,"-C"))
            {
                ++i;
                svm.C = atof(argv[i]);
            }
            else
                input_file = argv[i];

            ++i;
        }

    }

    if(!input_file)
        return 0;

    std::ifstream in(input_file);
    while(in)
    {
        int y;
        double *x;
        in >> y;
        if(in)
        {
            x = new double [svm.m];

            for(int j=0; j<svm.m; ++j)
                in >> x[j];

            svm.insert(y, x);
        }
    }

//   std::cout << " .... \n";
//   for(int i=0;i<svm.n;++i)
//     {
//       std::cout << svm.y[i];
//       for(int j=0;j<svm.m;++j)
// 	std::cout << ' ' << svm.x[i][j];
//       std::cout << std::endl;
//     }

    svm.train();

//   for(int i=0;i<svm.n;++i)
//     {
//       std::cout << svm.alpha[i] << ' ';
//     }
//   std::cout << std::endl;


    std::cout << "hyperplane equation:\n";
    for(int i=0; i<svm.m; ++i)
    {
        std::cout << svm.w[i] << ' ';
    }
    std::cout << svm.bias << std::endl;

    std::cout << "Test on training set:\n";

    for(int i=0; i<svm.n; ++i)
    {
        std::cout << svm.y[i] << ' ' << linear(svm.x[i], svm.w, svm.bias, svm.m) << std::endl;
    }
#endif
    return 0;
}
