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
#include "Classifier/HyperplaneClassifier.h"
#include <linear.h>
#include "Test.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <map>
#include <iostream>

void help()
{
    std::cout <<
              "svm_train <dataset> [options]\n"
//  "-C <weight>\n\tMargin SVM C parameters\n"
              "-o <file>\n\tOutput filename\n"
              "-N <n>\n\tNumber of Random Negative per frame\n"
              "-r <w> <h>\n\tGeometry of samples\n"
              "--bstrap <n>\n\tNumber of bootstrap iterations\n"
              "-f <feature>\n\tFeature Type (default hog)\n"
              "-C <C>\n\tC SVM parameters (default 1.0)\n"
              "-s type\n\tset type of solver (default 1)\n"
              "\n"
              "HoG Parameters:\n"
              "-g <w> <h>\n\tCell geometry (default 8x8)\n"
              "-b <w> <h>\n\tCells per Block (default 3x3)\n"
              "-l <w> <h>\n\tBlock Step (default 8x8)\n"
              "-n <n>\n\tHystogram bins\n"
              "-u\n\tKeep HoG signs\n"
              "-d <algo>\n\tUse a different gradient algorithm\n";
    std::cout.flush();
}

static size samples_geometry(0,0);
static int random_negative =0;

const char *dataset_filename = 0;
const char *output_filename = "train.linear";
double C = 1.0;
int solver = 1;
int cell_width = 8;
int cell_height = 8;
int block_width = 3;
int block_height = 3;
int h_step = 8;
int v_step = 8;
int bootstrap_max_iterations = 0;
int hog_bin = 8;
float bootstrap_scale_factor = 2.0f;
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
        if(!strcmp(str, "-C") )
        {
            i++;
            C = atof(argv[i]);
        }
        else if(!strcmp(str, "-N") )
        {
            i++;
            random_negative = atoi(argv[i]);
        }
        else if(!strcmp(str, "-s") )
        {
            i++;
            solver = atoi(argv[i]);
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
        else if(!strcmp(str, "--bstrap") )
        {
            i++;
            bootstrap_max_iterations = atoi(argv[i]);
        }
        else if(!strcmp(str, "--bstrap-scale-factor") )
        {
            i++;
            bootstrap_scale_factor = atof(argv[i]);
        }        
        else if(!strcmp(str, "-r") )
        {
            i++;
            samples_geometry.width = atoi(argv[i]);
            i++;
            samples_geometry.height = atoi(argv[i]);
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

template<class DataSetType>
void RandomExtractAndEvaluate(HyperPlaneClassifier<double> & c, DataSetType & set, const ImageHandle & img, const size & set_geometry, int nRand, const std::vector<datasetobject> & object)
{
    std::map< double, rect> objs;
    
    // for any candidate in the set
    for(int k=0; k<nRand * 10; ++k)
    {
        int x0 = rand() % (img.width - set_geometry.width);
        int y0 = rand() % (img.height - set_geometry.height);
        rect roi(x0, y0, x0 + set_geometry.width, y0 + set_geometry.height);

        bool collision = false;
        // Check Collision
        for(int i=0; i<object.size(); ++i)
            if(overlap(roi, object[i].roi))
                collision = true;

        if(!collision)
        {
          ImageHandle image = img.crop(roi);
          typename DataSetType::ReturnType data;
          
          set.Process(data, (unsigned char *) image.data, image.width, image.height, image.stride );
          
          double response = c.response( data.first );
          
          if(response > 0.0)
            objs.insert( std::make_pair(response, roi) );
        }
    }

    int count = 0;
    for(std::map< double, rect>::const_reverse_iterator i = objs.rbegin(); i != objs.rend(); ++i)
    {
    set.ImportImage(img.crop(i->second), -1); // only negative
    count ++;
    if(count > nRand) // max nRand features
      break;
    }
    
    std::cout << count << ' ';
    
}

template<class DataSetType>
bool CollectNegatives(HyperPlaneClassifier<double> & c, DataSetType & set, datasetin & in, const size & sz, int nRand)
{
    datasetitem item;
    size set_geometry = sz; //

    if(set_geometry.width != 0 || set_geometry.height!=0)
        std::cout << "LoadSet with geometry " << set_geometry.width << 'x' << set_geometry.height << std::endl;

    // iterate on input data
    while(in.next(item))
    {
        bool have_negative = false;
        Image img;
        if(!pnm_load(item.filename.c_str(), img))
        {
        }
        else
        {
            // se e' un file da estrazione automatica di negativi, li usa
            if(item.auto_negative && (nRand>0))
            {
                if(set_geometry.width == 0 || set_geometry.height == 0)
                    std::cerr << "Invalid geometry for random sample extraction" << std::endl;

                int src_width = img.width;
                int src_height = img.height;
                
                std::cout << "Extract and Evaluate random from " << item.filename << " (" << src_width << 'x' << src_height << ")" << std::endl;
                std::vector<datasetobject> list = item.object;
                float scale = 1.0f;
                ImageResampler rsmp;
                rsmp.ImportImage(img);
            
                // for all octaves:
                for(;;) {

                    RandomExtractAndEvaluate(c, set, img, set_geometry, nRand, list);

                    scale *= bootstrap_scale_factor;
                    
                    if(img.width > 3 * set_geometry.width && img.height > 3 * set_geometry.height)
                    {
                    Image out;
                    out.alloc( (int) ((float)src_width/scale), (int) ((float)src_height/scale), 1);
                    rsmp.ExportImage(out);

                    for(int i=0; i<list.size(); ++i)
                    {   list[i].roi.x0 = floor((float)item.object[i].roi.x0 / scale);
                        list[i].roi.y0 = floor((float)item.object[i].roi.y0 / scale);
                        list[i].roi.x1 = ceil((float)item.object[i].roi.x1 / scale);
                        list[i].roi.y1 = ceil((float)item.object[i].roi.y1 / scale);
                    }

                        std::swap(img, out);
                    }
                    else
                        break;
                }
                
                std::cout << std::endl;
            }

        }

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
    train_set.SetHystogramBins(hog_bin);
    train_set.EnableSign(grad_sign);
  
    {
        sdfin in(dataset_filename);
        LoadSet(train_set, in, samples_geometry, random_negative);
    }

    if(!train_set.templates.empty())
        train_set.debug_geometry(train_set.GetParams().width, train_set.GetParams().height);

    for(int bstrap_iter=0; bstrap_iter<bootstrap_max_iterations; ++bstrap_iter)
    {
        // 3) Train SVM
        //
        int desc_size = train_set.GetParams().desc_size;

        problem pr;

        pr.l = train_set.templates.size();
        pr.n = desc_size + 1;
        pr.bias = 1.0;
        pr.y = new int [train_set.templates.size()];
        pr.x = (feature_node**) malloc(train_set.templates.size() * sizeof(feature_node*));

        for(int i =0; i<train_set.templates.size(); ++i)
        {
            pr.y[i] = train_set.templates[i].category;

            feature_node *ptr = new feature_node[desc_size + 2];

            pr.x[i] = ptr;

            for(int j =0; j<desc_size; ++j)
            {
                ptr[j].index = j+1;
                ptr[j].value = train_set.templates[i].data[j];
            }

            ptr[desc_size].index = desc_size+1;
            ptr[desc_size].value = 1.0; // biasq

            ptr[desc_size+1].index = -1; // EOP
            ptr[desc_size+1].value = 0.0;
        }

        parameter param;
        param.solver_type = solver;
        param.C = C;
        param.nr_weight = 0;
        param.weight_label = 0;
        param.weight = 0;

        const char *rep = check_parameter(&pr, &param);
        if(rep)
        {
            std::cerr << "Parameters Error: " << rep << std::endl;
            return -1;
        }

        std::cout << "SVM train " << pr.l << 'x' << pr.n << std::endl;
        
        model *m = train(&pr, &param);

        std::cout << "nr_class: " << m->nr_class << " | nr_feature: " << m->nr_feature << " | bias: " << m->bias << " | classes:";
        for(int i=-0; i<m->nr_class; ++i)
            std::cout << ' ' << m->label[i];
        std::cout << std::endl;

        HyperPlaneClassifier<double> cl;

        // fix for -1 as label(0)
        for(int i =0; i<desc_size+1; ++i)
            m->w[i] *= m->label[0];

        cl.import(desc_size, m->w); // 2 class problem only one vector

        TestSimple(cl, train_set);

        std::ofstream out(output_filename);
        out << "linear-hog " << train_set.width << ' ' << train_set.height << '\n';
        out << cl << '\n';
        out.close();

        free_model_content(m);

        if(bstrap_iter+1 != bootstrap_max_iterations)
        {
            // BootStrap
          sdfin in(dataset_filename);
          
          CollectNegatives(cl, train_set, in, samples_geometry, random_negative);
        }

    }


    return 0;
}
