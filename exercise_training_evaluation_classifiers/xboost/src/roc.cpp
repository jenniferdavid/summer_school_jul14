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

/** @brief generate ROC/PRC curve.
 *      Examples of ImageClassifier and ObjectDetector classes.
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath> // abs
#include <set>
#include <map>
#include "IO/pnmio.h"
#include <Utility/ImageUtils.h> // Resample
#include <Utility/ImageClassifierOperator.h> // ComputeResponseOperator
#include <Utility/ImageSet.h>
#include <Utility/timer.h>
#include <Utility/Time.h>

#include "IO/sdfin.h"

#include "xbversion.h"

#ifndef _MSC_VER
# include <stdint.h>
#else
typedef unsigned int uint32_t;
#endif

#include "factory/allocate.h"
#include "Utility/GroupRectangles.h"



void help()
{
    std::cout << "Usage: roc <classifier> <validation set> [OPTION]\n\n"
              " -v\n\tVerbose\n"
              " -vv\n\tMore verbose\n"
              " -vvv\n\tReally verbose\n"
              " -o <file>\n\toutput file with ROC/PRC\n"
              " -f type\n\tSet the output format\n"
              " -d\n\tTest Object Detector instead of Image Classifier\n"
              " --search-ranges <file>\n\tDeclare a search range file used to prune the searching window\n"
              " -t <th>\n\tMinimum classification threshold to provide candidates (default 0)\n"
              " -j <n>\n\tNumber of concurrent jobs (default " << sprint::thread::hardware_concurrency() <<")\n"
              "Parameters for Image Classifier:\n"
              " -N <n>\n\tNumber of negative samples extracted randomply by each scale and image\n"
              "Parameters for Object Detector:\n"
              " -s <n>\n\tNumber of scales per octave (default 4)\n"
              " --min-th-step <th>\n\tMinimal differences between two evaluated threshold (default 1e-2)\n"              " --ov <th>\n\tMinimum overlapping threshold (default 0.5)\n"
//                " --dd <file>\n\toutput file to dump detections with score\n";
              " --dump <path>\n\tDump processed image\n";
    std::cout.flush();
}

// global parameters

const char *network_file = NULL, *validation_set = NULL;
const char *output_file = NULL;
const char *dumpdetection_file = NULL;
const char *dump_path = 0;
const char *search_range_file = 0;

std::vector< std::pair<int, int> > search_ranges;

float min_th_step = 1e-2;

// int n_classifiers = -1; // unimplemented now
// bool resort = false; // unimplemented now
int verbose = 0;
bool add_noise = false;
// bool normalize = false; // unimplemented now
bool avoid_upsampling = true;
bool test_object_detector = false;
float min_overlap = 0.5; //pascal criteria 0.5
float th_detector = 0.0;
int scale_per_octaves = 4;
int response_step = 1;
int n_threads = sprint::thread::hardware_concurrency();

bool m_no_curves = false;

int negative_random_samples = 10;

// a check if the argument is valid
#define check_param(i)  if(i>=argc) { std::cerr << str <<  ": missing parameters" << std::endl; return false; }

bool parse_cmd_line(int argc, char *argv[])
{
    // Parse Command Line
    if(argc < 3)
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
        else if(!strcmp(str, "-v") )
        {
            verbose = 1;
        }
        else if(!strcmp(str, "-vv") )
        {
            verbose = 2;
        }
        else if(!strcmp(str, "-vvv") )
        {
            verbose = 3;
        }
//         else if(!strcmp(str, "--normalize") )
//         {
//             normalize = true;
//         }
        else if(!strcmp(str, "-o") )
        {
            i++;
            check_param(i);
            output_file = argv[i];
        }
        else if(!strcmp(str, "--dd"))
        {
            i++;
            check_param(i);
            dumpdetection_file = argv[i];
        }
        else if(!strcmp(str, "-t"))
        {
            i++;
            check_param(i);
            th_detector = atof(argv[i]);
        }
        
        else if(!strcmp(str, "-j"))
        {
            i++;
            check_param(i);
            n_threads = atoi(argv[i]);
        }
        else if(!strcmp(str, "-s"))
        {
            i++;
            check_param(i);
            scale_per_octaves = atoi(argv[i]);
        }
        else if(!strcmp(str, "--ov") )
        {
            i++;
            check_param(i);
            min_overlap = atof(argv[i]);
        }
        else if(!strcmp(str, "--min-th-step") )
        {
            i++;
            check_param(i);
            min_th_step = atof(argv[i]);
        }        
        else if(!strcmp(str, "--step") )
        {
            i++;
            check_param(i);
            response_step = atoi(argv[i]);
        }
        else if(!strcmp(str, "--no-curves") )
        {
            m_no_curves = true;
        }
        else if(!strcmp(str, "--search-ranges") )
        {
            i++;
            check_param(i);
            search_range_file = argv[i];
        }
        else if(!strcmp(str, "--dump") )
        {
            i++;
            check_param(i);
            dump_path = argv[i];
        }
        else if(!strcmp(str, "-N") )
        {
            i++;
            check_param(i);
            negative_random_samples = atoi( argv[i] );
        }
        else if(!strcmp(str, "-d"))
        {
            test_object_detector = true;
        }
        else if(!strcmp(str, "--add-noise") )
        {
            // NOTE: removed
            add_noise = true;
            srand(time(0));
        }
        else if(network_file == NULL)
            network_file = str;
        else if(validation_set == NULL)
            validation_set = str;
        else
        {
            std::cerr << "Unknown options: " << str << std::endl;
            help();
            exit(0);
        }
        i++;
    }
    return true;
}

/// effective size of images involved in training
unsigned int m_width, m_height;

/// Test the ImageClassifier
void compute_resp(const size & sz, ComputeResponseOperator & op,const char* set)
{
    sdfin in(set);

    ProcessSet(in, sz, negative_random_samples, op);
}

/// Test the ImageClassifier
void test_image_classifier(std::ostream & out, ImageClassifier * c)
{
    size sz = c->GetClassifierGeometry();

    // compute response and category
    ComputeResponseOperator op(c);
    compute_resp(sz, op, validation_set);

    if(op.resp.empty())
    {
        std::cerr << "Error: no data to create curves" << std::endl;
        return;
    }

    // TODO: move in library: /////// BEGIN
    std::set<double> steps;
    // sorting:
    for(int i =0; i<op.resp.size(); ++i)
    {
        steps.insert( op.resp[i] );
    }

    if(verbose > 0)
        std::cerr << op.cat.size() << " samples. " <<  steps.size() << " thresholds (from " << *steps.begin() << " to " << *steps.rbegin() <<")..." << std::endl;

    double cd_prev, fp_prev;
    cd_prev = 0.0;
    fp_prev = 0.0;
    double th;
    unsigned int tp, fp;
    unsigned int tn, fn;
    // for any threshold.....
    for(std::set<double>::const_iterator it = steps.begin(); it != steps.end(); ++it)
    {
        //   double th = min + (max - min) * k / (double)step;
        th = *it;
        tn = fn = tp = fp = 0;

        // test the threshold on the response
        for(int i =0; i<op.resp.size(); ++i)
        {
            if(op.cat[i]>0 && op.resp[i]>th)  tp++;
            if(op.cat[i]>0 && op.resp[i]<=th) fn++;
            if(op.cat[i]<0 && op.resp[i]>th)  fp++;
            if(op.cat[i]<0 && op.resp[i]<=th) tn++;
        }

        if(verbose>1)
            std::cout << th << ' ' << tp << ' ' << fp << std::endl;

        // to avoid straight line: both terms should change
        if(tp != cd_prev && fp != fp_prev)
        {
            // [Threshold] [TP] [TN] [FP] [FN] [Recall/TPR] [FPR] [Precision]
            out << th << ' ' << tp << ' ' << tn << ' ' << fp << ' ' << fn << ' ' << (double)tp/(double)(tp+fn) << '\t' << (double)fp/(double)(tn+fp) << '\t' << (double)tp/(double)(tp+fp) <<  '\n';
            cd_prev = tp;
            fp_prev = fp;
        }
    }

    // last
    out << th << ' ' << tp << ' ' << tn << ' ' << fp << ' ' << fn << ' ' << (double)tp/(double)(tp+fn) << '\t' << (double)fp/(double)(tn+fp) << '\t' << (double)tp/(double)(tp+fp) << '\n';

    //// END
}


/// Test the ObjectDetector
/// @todo move resp outside this function
void execute_object_detector(ObjectDetector * c, std::set<float> & resp, std::vector< std::vector<Candidates> >& detections ,std::vector< std::vector<datasetobject> >& gt,const char* set)
{
    sdfin in(set);

    datasetitem item;
    int skipped_frames = 0;

    int count_ig = 0, count_gt = 0;

    /*
    std::ofstream fdd;

    // TODO: unimplemented yet!
    if(dumpdetection_file)
    {
        try {
            fdd.open(dumpdetection_file);
        } catch(std::ifstream::failure e) {
            std::cerr << "failure to open "<<dumpdetection_file << std::endl;
        }
    }
    */

    while(in.next(item))
    {
        Image img_in;
        if(!pnm_load(item.filename.c_str(), img_in))
        {
            std::cerr << item.filename << " failed to load" << std::endl;
        }
        else
        {
            if(item.auto_negative)
            {
                Timer t;

                if(verbose)
                {
                    std::cout << item.filename << ": ";
                    std::cout.flush();
                }

                std::vector<Candidates> ttx;
                t.Start();
                c->Detect(ttx, img_in);
                double dt = t.GetTime();

                // store results for further processing
                detections.push_back(ttx);
                gt.push_back(item.object);

                // update response map
                for(int i=0; i<ttx.size(); i++)
                    resp.insert(ttx[i].response);

                ///// minimalia stats:
                int ngt = 0;
                int nig = 0;
                for(int i = 0; i<item.object.size(); ++i)
                    if(item.object[i].category == 1) ngt++;
                    else nig++;

                if(verbose)
                {
                    std::cout << "detected "<<ttx.size()<< " of " << nig << '+' << ngt << " candidates in " << dt << " secs" << std::endl;
                }

                count_ig += nig;
                count_gt += ngt;
                /////// and of minimalia stats

                if(dump_path)
                {
                    for(int i=0; i<item.object.size(); i++)
                        if(item.object[i].category==1)
                            BlendBox(img_in, item.object[i].roi, 255, 0.5f);

                    for(int i=0; i<item.object.size(); i++)
                        DrawRect(img_in, item.object[i].roi, 255);

                    for(int i=0; i<ttx.size(); i++)
                        DrawRect(img_in, ttx[i].box, 0);

                    char buffer[512];
                    sprintf(buffer, "%s/test-%06u.pgm", dump_path, gt.size() );

                    pnm_write(img_in, buffer);

                }

                img_in.release();
                // TODO: groupRectangle here?
            }
            else
            {
                skipped_frames ++;
            }
        }
    }

    if(skipped_frames>0)
    {
        std::cerr << "Warn: " << skipped_frames << " frames skipped because they are not negative frames" << std::endl;
    }

    if(count_gt == 0)
        std::cerr << "Error: no positive object in the set. Curves cannot be provided" << std::endl;
}

void perform_test_object_detector_no_curves(std::ostream & out, ObjectDetector * c)
{
//     std::set<float> steps;
    std::vector< std::vector<Candidates> > detections;
    std::vector< std::vector<datasetobject> > gt;
    std::vector<int> category;
    int frame = 0;

    {
        ObjectDetectorGlobalParams param = c->getParams();
        param.nScales = scale_per_octaves;
        param.concurrent_jobs = n_threads;
        c->setParams(param);
    }

    {
        ObjectDetectorParams param =  c->getClassifierParams(0);
        size sz = param.sz;
        param.category = 1; //
        param.downsampling = response_step;
        param.th = th_detector; // change the threshold
        if(!search_ranges.empty())
            param.m_srcSearchRanges = search_ranges;
        c->setClassifierParams(0, param);
    }

    // compute response and category
    Timer t;
    t.Start();


    sdfin in(validation_set);

    datasetitem item;
    int skipped_frames = 0;


    while(in.next(item))
    {
        Image img_in;
        if(!pnm_load(item.filename.c_str(), img_in))
        {
            std::cerr << item.filename << " failed to load" << std::endl;
        }
        else
        {
            if(item.auto_negative)
            {
                Timer t;

                if(verbose)
                {
                    std::cout << item.filename << ": ";
                    std::cout.flush();
                }

                std::vector<Candidates> ttx;
                t.Start();
                c->Detect(ttx, img_in);
                double dt = t.GetTime();
                groupRectanglesOverlapped(ttx,min_overlap);//group


                ///////
                std::vector<Candidates> grpd_cdts;
                const std::vector<datasetobject> & gt_frame = item.object;

                // extract grouped
                grpd_cdts.reserve( ttx.size() ); // avoid realloc
                for(int i=0; i<ttx.size(); i++)
                {
                    if(ttx[i].grouped==false)
                        grpd_cdts.push_back(ttx[i]);
                }


                int* gt_list = new int[gt_frame.size()];//gt list
                int* od_list = new int[grpd_cdts.size()];//detection list

                // reset lists
                for(int i=0; i<grpd_cdts.size(); i++)
                    od_list[i]=-1;
                for(int i=0; i<gt_frame.size(); i++)
                    gt_list[i]=-1;

                //associo cercando quello che overlappa di più e poi lo toglie dalla lista
                // e aggiorno le statistiche
                unsigned int c_fp=0, c_tp=0, c_fn=0;
                unsigned int gt_cnt_pos=0;  // number of positive

                // for each element in GT (category could be 1[positive] or 0[do-not-care])

                // firstly look for positive
                for(int i=0; i<gt_frame.size(); i++)
                    if(gt_frame[i].category==1)
                    {
                        float best_ov = min_overlap;
                        int index=-1;

                        gt_cnt_pos++;   //

                        // for each candidates search the best overalap with gt_frame[i]
                        for(int j=0; j<grpd_cdts.size(); j++) {
                            float ov=boxoverlap(grpd_cdts[j].box,gt_frame[i].roi, Overlap_On_Union);
//                                         std::cout << i << ' ' << j << ' ' << ov << std::endl;
                            if(od_list[j]==-1 && ov > best_ov ) {
                                best_ov = ov;
                                index=j;
                            }
                        }

                        if(index != -1)
                        {
                            od_list[index] = i;
                            gt_list[i] = index;

                        }
                        else
                        {
                            // is a miss
                        }
                    }

                ///////////////////// look for ignored regions ////////////////////

                // look for the do-not-care or mark it as FP
                for(int j=0; j<grpd_cdts.size(); j++)
                    if(od_list[j]==-1) // non ancora assegnati dallo stadio precedente
                    {
                        for(int i=0; i<gt_frame.size(); i++)
                            if(gt_frame[i].category==0) //
                            {
                                float ov=boxoverlap(grpd_cdts[j].box, gt_frame[i].roi, Overlap_On_First);
                                if(ov > 0.1f) // 10% overlap consentito
                                {
                                    od_list[j]=i;     // mark for detected (ok. the index is wrong, but it is fine)
                                    break;
                                }
                            }
                    }


                if(dump_path)
                {
                    Image out;
                    out.alloc(img_in.width, img_in.height, 3);
                    Convert(out, img_in); // convert from Gray to RGB

                    for(int i=0; i<item.object.size(); i++)
                        if(item.object[i].category==1)
                        {
                            // TODO
                            //   BlendBox(out, item.object[i].roi, 0x0000FF, 0.5f);
                        }

                    for(int i=0; i<item.object.size(); i++)
                        if(item.object[i].category==1)
                        {
                            if(gt_list[i]!=-1)
                                DrawRect(out, item.object[i].roi, 0x00FF00); // positive detected
                            else
                                DrawRect(out, item.object[i].roi, 0xFF0000); // positive missed
                        }
                        else
                        {
                            DrawRect(out, item.object[i].roi, 0x444477); // omit
                        }

                    /*
                    for(int i=0; i<ttx.size(); i++)
                        if(ttx[i].grouped)
                            DrawRect(out, ttx[i].box, 0x777744);
                    */

                    for(int i=0; i<grpd_cdts.size(); i++)
                    {
                        if(od_list[i]!= -1)
                            DrawRect(out, grpd_cdts[i].box, 0x00FFFF); // candidate hit a positive or omit
                        else
                            DrawRect(out, grpd_cdts[i].box, 0xFFFF00); // false positive
                    }

                    char buffer[512];
                    sprintf(buffer, "%s/test-%06u.ppm", dump_path, frame );

                    pnm_write(out, buffer);

                }

                delete [] gt_list;
                delete [] od_list;
                ///////

                frame++;

                img_in.release();
            }
            else
            {
                skipped_frames ++;
            }
        }
    }

//     if(skipped_frames>0)
//     {
//         std::cerr << "Warn: " << skipped_frames << " frames skipped because they are not negative frames" << std::endl;
//     }

    std::cerr << "DataSet processed in " << human_readable_time(t.GetTime()) << std::endl;

//     if(steps.empty())
//     {
//         std::cerr << "Error: there are no candidates to perform the test" << std::endl;
//         return;
//     }

}

/// Test the ObjectDetector
void perform_test_object_detector(std::ostream & out, ObjectDetector * c)
{
    std::set<float> steps;
    std::vector< std::vector<Candidates> > detections;
    std::vector< std::vector<datasetobject> > gt;
    std::vector<int> category;

    {
        ObjectDetectorGlobalParams param = c->getParams();
        param.nScales = scale_per_octaves;
        param.concurrent_jobs = n_threads;
        c->setParams(param);
    }

    {
        ObjectDetectorParams param =  c->getClassifierParams(0);
        size sz = param.sz;
        param.category = 1; //
        param.th = th_detector; // change the threshold
        param.downsampling = response_step;
        if(!search_ranges.empty())
            param.m_srcSearchRanges = search_ranges;
        c->setClassifierParams(0, param);
    }

    // compute response and category
    Timer t;
    t.Start();
    execute_object_detector(c, steps, detections, gt, validation_set);
    std::cerr << "DataSet processed in " << human_readable_time(t.GetTime()) << ". " << scale_per_octaves <<" scales per octaves" << std::endl;

    if(steps.empty())
    {
        std::cerr << "there are no candidates to perform the test" << std::endl;
        return;
    }

    std::cerr << steps.size() << " thresholds (ranging from " << *steps.begin() << " to " << *steps.rbegin() <<").\n\tGenerating ROC/PRC..." << std::endl;
//     std::cerr << ": "<<gt.size()<< " frames"<<std::endl;
    double th;
    unsigned int fp_prev=0,tp_prev=0,fn_prev=0;

    float previous_threshold;
    
    // for each threshold
    for(std::set<float>::const_iterator it = steps.begin(); it != steps.end(); ++it)
    {
        unsigned int fp=0,tp=0,fn=0;
        th = *it;
        
        // avoid small thresholds
        if( (it != steps.begin()) && (th - previous_threshold < min_th_step) )
          continue;
        previous_threshold = th;

        for(int frame = 0; frame < gt.size(); ++frame)
        {
            std::vector<Candidates> cdts_over_th;       // candidates over threshold
            std::vector<Candidates> grpd_cdts;          // grouped candidates over threshold

            const std::vector<Candidates> & src = detections[frame];

            cdts_over_th.reserve(src.size()); // avoid realloc

            // extract only element over threshold:
            for(int i=0; i<src.size(); i++)
            {
                if(src[i].response>th)
                    cdts_over_th.push_back(src[i]);
            }

            groupRectanglesOverlapped(cdts_over_th,min_overlap);//group

            // extract grouped
            grpd_cdts.reserve( cdts_over_th.size() ); // avoid realloc
            for(int i=0; i<cdts_over_th.size(); i++)
            {
                if(cdts_over_th[i].grouped==false)
                    grpd_cdts.push_back(cdts_over_th[i]);
            }

            const std::vector<datasetobject> & gt_frame = gt[frame];//get current line from ground true

            int* gt_list = new int[gt_frame.size()];//gt list
            int* od_list = new int[grpd_cdts.size()];//detection list

            // reset lists
            for(int i=0; i<grpd_cdts.size(); i++)
                od_list[i]=-1;
            for(int i=0; i<gt_frame.size(); i++)
                gt_list[i]=-1;

            //associo cercando quello che overlappa di più e poi lo toglie dalla lista
            // e aggiorno le statistiche
            unsigned int c_fp=0, c_tp=0, c_fn=0;
            unsigned int gt_cnt_pos=0;  // number of positive

            // for each element in GT (category could be 1[positive] or 0[do-not-care])

            // firstly look for positive
            for(int i=0; i<gt_frame.size(); i++)
                if(gt_frame[i].category==1)
                {
                    float best_ov = min_overlap;
                    int index=-1;

                    gt_cnt_pos++;   //

                    // for each candidates search the best overalap with gt_frame[i]
                    for(int j=0; j<grpd_cdts.size(); j++) {
                        float ov=boxoverlap(grpd_cdts[j].box,gt_frame[i].roi, Overlap_On_Union);
//                                         std::cout << i << ' ' << j << ' ' << ov << std::endl;
                        if(od_list[j]==-1 && ov > best_ov ) {
                            best_ov = ov;
                            index=j;
                        }
                    }

                    if(index != -1)
                    {
                        od_list[index]=i;
                        gt_list[i] = index;

                        c_tp++; // good match!
                    }
                    else
                    {
                        // is a miss
                        if(gt_frame[i].category==1)
                            c_fn++; // only positives are misses
                    }
                }

            // look for the do-not-care or mark it as FP
            for(int j=0; j<grpd_cdts.size(); j++)
                if(od_list[j]==-1)
                {
                    for(int i=0; i<gt_frame.size(); i++)
                        if(gt_frame[i].category==0)
                        {
                            float ov=boxoverlap(grpd_cdts[j].box,gt_frame[i].roi, Overlap_On_First);
                            if(ov > 0.1f) // 10% overlap
                            {
                                od_list[j]=i;     // mark for detected (ok. the index is wrong, but it is fine)
                                break;
                            }
                        }

                    // detected something?
                    if(od_list[j]==-1)
                    {
                        c_fp++;
                    }

                }

            fp+=c_fp;
            tp+=c_tp;
            fn+=c_fn;

            delete [] gt_list;
            delete [] od_list;

        }

        if(verbose>1)
            std::cout << th << '\t' << tp << "\tx\t" << fp << '\t' << fn << '\t' << (double)tp/(double)(tp+fn) << "\tx\t" << (double)tp/(double)(tp+fp) <<  '\t' << (float)fp/(float)gt.size() << '\n';

        if(tp_prev!=tp || fp_prev!=fp || fn_prev!=fn)
        {
            if ((tp+fp > 0) && (tp+fn > 0))
            {
//                 double precision= double(tp)/double(tp+fp);
//                 double recall= double(tp)/double(tp+fn);
                // [Threshold] [TP] [x]  [FP] [FN] [Recall/TPR] [x] [Precision] [FP/Frame]
                out << th << '\t' << tp << "\tx\t" << fp << '\t' << fn << '\t' << (double)tp/(double)(tp+fn) << "\tx\t" << (double)tp/(double)(tp+fp) <<  '\t' << (float)fp/(float)gt.size() << '\n';
            }
            tp_prev=tp;
            fp_prev=fp;
            fn_prev=fn;
        }
    }

    // TODO: last

    std::cerr << "done.\n";
}

/*
        sigset_t none, set, oset;
        sig_t ohandler;
        pid_t pid, cpid;
        int status;

        sigemptyset(&none);
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &set, &oset) == -1)
                err(1, "sigprocmask");
        ohandler = signal(SIGUSR1, sigusr1_handler);
        if (ohandler == SIG_ERR)
                err(1, "signal");
        */
int main(int argc, char *argv[])
{
    std::cout << "xBoost ROC/PRC Generator " XBOOST_VERSION << std::endl;

    if(!parse_cmd_line(argc, argv))
        return -1;

    if(verbose>0)
    {
        std::cout << "[command line: ";
        for(int i =0; i<argc; ++i)
            std::cout << ' ' << argv[i];
        std::cout << "]\n";
    }

    if(search_range_file)
    {
        std::ifstream in(search_range_file);
        while(in)
        {
            std::pair<int,int> v;
            in >> v.first >> v.second;
            if(in)
            {
                search_ranges.push_back(v);
            }
        }
    }

    if(test_object_detector)
    {
        // allocate a classifier (ObjectDetector) using the factory
        ObjectDetector *c = allocate_detector_from_file(network_file);
        if(c)
        {
            template_geometry size = c->getClassifierParams(0).sz;

            std::cout << "loaded classifier " << size.width << ' ' << size.height << std::endl;

            std::ostream *out = &std::cout;
            std::ofstream *fout = 0;
            if(output_file)
            {
                fout = new std::ofstream(output_file);
                out = fout;
            }

            if(m_no_curves)
            {
                perform_test_object_detector_no_curves(*out, c);
            }
            else
            {
                perform_test_object_detector(*out, c);
            }

            delete fout;
            delete c;
        }
    }
    else
    {
        // allocate a classifier (ImageClassifier) using the factory
        ImageClassifier *c = allocate_classifier_from_file(network_file);
        if(c)
        {
            template_geometry size = c->GetClassifierGeometry();

            if(verbose>0)
                std::cout << "loaded classifier " << size.width << ' ' << size.height << std::endl;

            std::ostream *out = &std::cout;
            std::ofstream *fout = 0;
            if(output_file)
            {
                fout = new std::ofstream(output_file);
                out = fout;
            }

            test_image_classifier(*out, c);

            delete fout;
            delete c;
        }


    }

    return 0;
}
