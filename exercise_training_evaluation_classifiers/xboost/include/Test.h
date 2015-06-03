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

#ifndef _XBOOST_TEST_H
#define _XBOOST_TEST_H

/** @file Test.h
 *  @brief some function to test the classifier on the set */

#include <set>
#include <iosfwd>

#ifdef _MULTITHREAD
# include "Thread/thread_group.h"
# include "Thread/bind.h"
#endif

#include <iostream>

/// esporta le statistiche
/// TODO: esportare il margine
struct ReportTest {
    int TP,TN,FP,FN;

    double pmargin, nmargin;

    inline double Fpr() const {
        return double(FP) / double(TN + FP);
    }
    inline double ErrorRate() const {
        return double(FN+FP) / double(TP+TN+FN+FP);
    }
    /// Recall/TruePositiveRate
    inline double Recall() const {
        return double(TP) / double(TP + FN);
    }
    inline double Precision() const {
        return double(TP)/double(FP+TP);
    }
    inline double Accuracy() const {
        return double(TP+TN)/double(TP+TN+FN+FP);
    }
    inline double Margin() const {
        return pmargin - nmargin;
    }

    void print(std::ostream & out)
    {
        out << "TP:" << TP << ", TN:" << TN << ", FN:" << FN <<", FP:" << FP << '\n';
        out << "Correct Detection: " << TP+TN << " (" << (100*(TP+TN))/(TP+TN+FN+FP) <<"%), Errors: " << FN+FP << '\n';
        out << "Accuracy: " << Accuracy()
            << " Error Rate: " << ErrorRate()
            << " Precision: " << Precision()
            << " Recall: " << Recall() << std::endl;

    }



};

/// TODO Va bene per chi ha response Template
template<class ClassifierType, class TrainingSet>
void DumpRoc(std::ostream & out, const ClassifierType & classifier, const TrainingSet & train_set)
{
    std::set<double> th;
    int n = train_set.Size();
    double *response = new double [n];


    for(int i =0; i<n; ++i)
    {
        response[i] = classifier.response( train_set.templates[i] );
        th.insert(response[i]);
//       std::cerr << response[i] << ' ' << train_set.templates[i].category << std::endl;
    }

    for( std::set<double>::const_iterator j = th.begin(); j != th.end(); ++j)
    {
        int TP, TN, FP, FN;
        TP = TN = 0;
        FP = FN = 0;
        for(int i =0; i<n; ++i)
        {
            int test = ((response[i] > (*j)) ? 1 : -1);

            if( train_set.templates[i].category == 1)
            {
                if(test == 1) TP++;
                else FN++;
            }
            else
            {
                if(test == 1) FP++;
                else TN++;
            }

        }
        // [Threshold] [TP] [TN] [FP] [FN] [Recall/TPR] [FPR] [Precision]
        out << (*j) << '\t' << TP << '\t' << TN << '\t' << FP << '\t' << FN << '\t' <<  (double)TP/(double)(TP+FN) << '\t' << (double)FP/(double)(TN+FP) <<'\t' << (double)TP/(double)(TP+FP) <<'\n';
    }

    delete [] response;
}

/** a "simpler" version of test exploiting the "response" method of the classifier */
/// TODO Va bene per chi ha response Template
template<class ClassifierType, class TrainingSet>
void TestSimple(const ClassifierType & classifier, const TrainingSet & train_set, double th = 0.0)
{
    int n = train_set.Size();
    int TP, TN, FP, FN;
    TP = TN = 0;
    FP = FN = 0;

    for(int i =0; i<n; ++i)
    {
        double response = classifier.response( train_set.templates[i] );
        int test = ((response > th) ? 1 : -1);

        if( train_set.templates[i].category == 1)
        {
            if(test == 1) TP++;
            else FN++;
        }
        else
        {
            if(test == 1) FP++;
            else TN++;
        }

    }

    std::cout <<"TP: " <<TP << " | TN: " <<TN << " | FP: " << FP << " | FN: " << FN << " | TPR: " <<  (double)TP/(double)(TP+FN) << " | FPR: " << (double)FP/(double)(TN+FP) << " | Precision: " << (double)TP/(double)(TP+FP) << std::endl;
}

/** Execute test on classifier. Ouput on std::cout */
template<class DataType, class ClassifierType>
bool Test(const ClassifierType & c, const DataType & data, double threshold = 0.0)
{
    unsigned int cd[2],er[2];
// compute margin
// pmargin contiene il valore del piu' piccolo dei positivi, nmargin il valore del piu' grande dei negativi
    double pmargin = 1000000000.0; // a large positive number
    double nmargin = -1000000000.0; // a large negative number
    cd[0] = er[0] = cd[1] = er[1] = 0;

    double pavg,navg;
    pavg=navg = 0.0;

    double *r = new double[ data.Size() ];

    for(int i = 0; i<data.templates.size(); i++)
    {
        // m_templates[i].data punta al byte -1,-1
        double ret = c( getData1(data.templates[i], data), getData2(data.templates[i], data) );
        r[i] = ret;

        if(data.templates[i].category == 1)
        {
            // gt is positive
            pavg += ret;

            if(ret < pmargin)
                pmargin = ret;

            if(ret>threshold)
            {
                cd[0]++;
            }
            else
            {
                er[0]++;
            }
        }
        else
        {
            // gt is negative
            navg += ret;
            if(ret > nmargin)
                nmargin = ret;
            if(ret<threshold)
            {
                cd[1]++;
            }
            else
            {
                er[1]++;
            }
        }
    }

    pavg /= (double) (cd[0]+er[0]);
    navg /= (double) (cd[1]+er[1]);

    std::cout << "TP:" << cd[0] << ", TN:" << cd[1] << ", FN:" << er[0] <<", FP:" << er[1] << '\n';
    std::cout << "Correct Detection: " << cd[0]+cd[1] << " (" << (100*(cd[0]+cd[1]))/(cd[0]+cd[1]+er[0]+er[1]) <<"%), Errors: " << er[0]+er[1] << '\n';
    std::cout << "Accuracy: " << (float)(cd[0]+cd[1])/(float)(cd[0]+cd[1]+er[0]+er[1])
              << " Error Rate: " << (float)(er[0]+er[1])/(float)(cd[0]+cd[1]+er[0]+er[1])
              << " Precision: " << (float)(cd[0])/(float)(er[1]+cd[0])
              << " Recall: " << (float)(cd[0])/(float)(er[0]+cd[0])  << std::endl;

    std::cout << "Average: " << pavg << "(+) " << navg << "(-)\n";

    if(pmargin>nmargin)
        std::cout << "Margin: " << pmargin-nmargin  << '/' << 2*c.max_response() <<  std::endl;
    else
    {
        int pm, nm;
        pm = nm = 0;
        for(int i = 0; i<data.templates.size(); i++)
        {
            if(data.templates[i].category == 1 && r[i]<nmargin)
                pm++;
            if(data.templates[i].category == -1 && r[i]>pmargin)
                nm++;
        }

        std::cout << "No separation: " << pmargin-nmargin << '/' << 2*c.max_response() << " (" << pm+nm << " pattern inside margin: " << pm << " +," << nm << " -)" << std::endl;
    }

    delete [] r;

    return (er[0]+er[1])==0; // no error
}

namespace detail {

/// compute the response of a slice (@a s0 to @a s1) of samples using classifier @a c
/// TODO: move in library
template<class DataType, class ClassifierType>
void compute_response(double *r, const ClassifierType * c, const DataType * data,  int s0, int s1)
{
    for(int i = s0; i<s1; i++)
    {
        // m_templates[i].data punta al byte -1,-1
        double ret = (*c)( getData1(data->templates[i], *data), getData2(data->templates[i], *data) );
        r[i] = ret;
    }
}
}


/** Execute test on classifier. @note Ouput on std::cout
 * @note it is a multithread method
 */
template<class DataType, class ClassifierType>
ReportTest TestAndExportStat(const ClassifierType & c, const DataType & data, double threshold, int max_concurrent_jobs)
{
    unsigned int cd[2],er[2];
    ReportTest report;
// compute margin
// pmargin contiene il valore del piu' piccolo dei positivi, nmargin il valore del piu' grande dei negativi
    double pmargin = 1000000000.0; // a large positive number
    double nmargin = -1000000000.0; // a large negative number
    cd[0] = er[0] = cd[1] = er[1] = 0;

    double pavg,navg;
    pavg=navg = 0.0;

    int n_samples = data.Size();

    // store the response to count the number of pattern inside the margin
    double *r = new double[ n_samples ];

    // TODO: estimate the number of samples useful
#ifdef _MULTITHREAD
    if(max_concurrent_jobs > 1)
    {
        sprint::thread_group thread_pool_;

        for(int k=0; k<max_concurrent_jobs; ++k)
        {
            int s0 = (k * n_samples) / max_concurrent_jobs;
            int s1 = ((k+1) * n_samples) / max_concurrent_jobs;

            thread_pool_.create_thread(sprint::thread_bind(&detail::compute_response<DataType,ClassifierType>, r, &c, &data, s0, s1));

        }
        thread_pool_.join_all();

        for(int i = 0; i<n_samples; i++)
        {
            double ret = r[i];

            if(data.templates[i].category == 1)
            {
                // gt is positive
                pavg += ret;

                if(ret < pmargin)
                    pmargin = ret;

                if(ret>threshold)
                {
                    cd[0]++;
                }
                else
                {
                    er[0]++;
                }
            }
            else
            {
                // gt is negative
                navg += ret;
                if(ret > nmargin)
                    nmargin = ret;
                if(ret<threshold)
                {
                    cd[1]++;
                }
                else
                {
                    er[1]++;
                }
            }

        }
    }
    else
#endif // #ifdef _MULTITHREAD      
    {

        for(int i = 0; i<n_samples; i++)
        {
            // m_templates[i].data punta al byte -1,-1
            double ret = c( getData1(data.templates[i], data), getData2(data.templates[i], data) );
            r[i] = ret;

            if(data.templates[i].category == 1)
            {
                // gt is positive
                pavg += ret;

                if(ret < pmargin)
                    pmargin = ret;

                if(ret>threshold)
                {
                    cd[0]++;
                }
                else
                {
                    er[0]++;
                }
            }
            else
            {
                // gt is negative
                navg += ret;
                if(ret > nmargin)
                    nmargin = ret;
                if(ret<threshold)
                {
                    cd[1]++;
                }
                else
                {
                    er[1]++;
                }
            }
        }

    }

    pavg /= (double) (cd[0]+er[0]);
    navg /= (double) (cd[1]+er[1]);
    report.TP = cd[0];
    report.TN = cd[1];
    report.FN = er[0];
    report.FP = er[1];

    report.pmargin = pmargin;
    report.nmargin = nmargin;

    report.print(std::cout);
    std::cout << "Average: " << pavg << "(+) " << navg << "(-)\n";

    if(pmargin>nmargin)
        std::cout << "Margin: " << pmargin-nmargin  << '/' << 2*c.max_response() <<  std::endl;
    else
    {
        int pm, nm;
        pm = nm = 0;
        for(int i = 0; i<data.templates.size(); i++)
        {
            if(data.templates[i].category == 1 && r[i]<nmargin)
                pm++;
            if(data.templates[i].category == -1 && r[i]>pmargin)
                nm++;
        }

        std::cout << "No separation: " << pmargin-nmargin << '/' << 2*c.max_response() << " (" << pm+nm << " pattern inside margin: " << pm << " +," << nm << " -)" << std::endl;
    }

    delete [] r;
    return report;
}

/** Execute test on classifier. @note Ouput on std::cout
 * @note it is a multithread method
 */
ReportTest ExportStat(const double *r, const int *category, int n_samples, double threshold);

#endif
