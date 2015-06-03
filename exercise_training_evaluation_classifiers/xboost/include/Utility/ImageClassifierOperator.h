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

#ifndef _IMAGE_CLASSIFIER_OPERATOR_H
#define _IMAGE_CLASSIFIER_OPERATOR_H

#include <vector>
#include <factory/ImageClassifier.h>

struct ComputeResponseOperator {
  
  ImageClassifier *c;
  // TODO: devlop directly the ROC
  std::vector<int> cat;
  std::vector<float> resp;
public:
   ComputeResponseOperator(ImageClassifier *_c) : c(_c)
   {
   }
   ~ComputeResponseOperator()
   {
   }
   
   bool operator()(const std::string & filename, const ImageHandle & img, int category)
   {
     c->setImage(img);
     float v = (*c)(0,0);
     c->Release();
        
     cat.push_back(category);
     resp.push_back(v);
     
     return true;
   }
   
};

/// an operator used to collect pattern from the ProcessSet operator
class ImageClassifierOperator {
    int TP, TN, FP, FN;
    double mpos, mneg;
    double pavg, navg;
    double pavg2, navg2;
    ImageClassifier *c;

    int verbose;
    double threshold;
    
    // TODO: create ROC/PRC
//     std::map<float, int> m_pos, m_neg;

public:
    ImageClassifierOperator(ImageClassifier *_c) : c(_c)
    {
        pavg = navg = 0.0;
        pavg2 = navg2 = 0.0;
        mpos = 100000000.0;
        mneg = -100000000.0;

        TP=TN=FP=FN = 0;

        verbose = 0;
        threshold = 0.0;
    }

    void setVerbosity(int l) {
        verbose = l;
    }
    void setThreshold(double th) {
        threshold = th;
    }

    bool operator()(const std::string & filename, const ImageHandle & img, int category)
    {
        int w = img.width;
        int h = img.height;
        c->setImage(img);
        double v = (*c)(0,0);
        c->Release();

        if(category == 1)
        {
//             m_pos[v]++;
            pavg += v;
            pavg2 += v*v;
            if(verbose==1)
            {
                std::cout << v << ' ';
                if(v<threshold)
                    std::cout << "FN";
                else
                    std::cout << "TP";
                std::cout << std::endl;
            }
            else if(verbose==2)
            {
                std::cout << filename << ' ' << v << ' ';
                if(v<threshold)
                    std::cout << "FN";
                else
                    std::cout << "TP";
                std::cout << std::endl;
            }
            else if(verbose==3)
            {
                std::cout << v << ' ' << filename << ' ';
                if(v<threshold)
                    std::cout << "FN";
                else
                    std::cout << "TP";
                std::cout << std::endl;
            }

            if(v<mpos) mpos = v;

            if(v<threshold)
                FN++;
            else
                TP++;
        }
        if(category == -1)
        {
//             m_neg[v]++;
            navg += v;
            navg2 += v*v;

            if(verbose==1)
            {
                std::cout << v << ' ';
                if(v>=threshold)
                    std::cout << "FP";
                else
                    std::cout << "TN";
                std::cout << std::endl;
            }
            else if(verbose==2)
            {
                std::cout << filename << ' ' << v << ' ';
                if(v>=threshold)
                    std::cout << "FP";
                else
                    std::cout << "TN";
                std::cout << std::endl;
            }
            else if(verbose==3)
            {
                std::cout << v << ' ' << filename << ' ';
                if(v>=threshold)
                    std::cout << "FP";
                else
                    std::cout << "TN";
                std::cout << std::endl;
            }

            if(v>mneg) mneg = v;

            if(v>=threshold)
                FP++;
            else
                TN++;

        }

        return true;
    }
    
//     void PrintCurve()
//     {
//       float min_th = std::min( m_pos.begin()->first, m_neg.begin()->first);
//       float max_th = std::min( m_pos.rbegin()->first, m_neg.rbegin()->first);
//       
//       float accum[50];
//       for(int i = 0;i<50;++i) accum[i] = 0.0f;
//     }

    // TODO: set a ostream
    void Done()
    {
        pavg /= (double) (FN+TP);
        navg /= (double) (FP+TN);

        std::cout << "Margin: " << mpos - mneg << "(" << mneg << ";" << mpos << ")\n";
        std::cout << "Average: " << pavg << "(+) " << navg << "(-)\n";
        std::cout << "Sqm: " << std::sqrt((pavg2/(double) (FN+TP)) - pavg*pavg) << "(+) " << std::sqrt((navg2/(double) (FP+TN)) - navg*navg) << "(-)\n";
        std::cout << "Statistics with threshold = " << threshold << '\n';
        std::cout << "TP:" << TP << ", TN:" << TN << ", FN:" << FN <<", FP:" << FP << '\n';
        std::cout << "Correct Detection: " << TP+TN << " (" << (100*(TP+TN))/(TP+TN+FN+FP) <<"%), Errors: " << FN+FP << '\n';
        std::cout << "Accuracy: " << (float)(TP+TN)/(float)(TP+TN+FN+FP)
                  << " Error Rate: " << (float)(FN+FP)/(float)(TP+TN+FN+FP)
                  << " Precision: " << (float)(TP)/(float)(FP+TP)
                  << " Recall: " << (float)(TP)/(float)(FN+TP)  << std::endl;

    }

};

#endif