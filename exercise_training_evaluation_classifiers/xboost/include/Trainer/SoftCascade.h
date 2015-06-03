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

#ifndef _SOFTCASCADE_TRAINER_H
#define _SOFTCASCADE_TRAINER_H

/** @file SoftCascade.h
 * @brief this header contains a lot of functions and methods implementing some SomeCascade training algorithms */

#ifdef _MULTITHREAD
# include "Thread/thread_group.h"
# include "Thread/bind.h"
# include "Thread/ref.h"
#endif

#include <vector>
#include <iostream>

#include "Classifier/BoostClassifier.h"
#include "Classifier/SoftCascade.h"

/// calcola una distribuzione v tale che la sua somma sia r.
void computeRejectionDistribution(std::vector<double> & v, double alpha, double r, int n)
{
    double acc = 0.0;
    v.clear();
    v.reserve(n);

    for(int i =0; i<n; ++i)
    {
        double tau = (double)i/(double)n;
        double y = (alpha>=0.0) ? exp( alpha * tau) : exp ( -alpha * (1.0 - tau) );
        acc += y;
        v.push_back( y  );
    }

    for(int i =0; i<n; ++i)
    {
        v[i] *= r / acc;
    }

}

// precision of IO
// #define eps  0.00001

/// find the rejection threshold
///  find the minimum threshold that keep at least target positives
/// @param target positive target
template<class DataSetType>
double findBestRejectionThreshold(const DataSetType & data, const std::vector<double> & response, int target)
{
    std::map<double, int> h; // positive hypothesis map
    double psum = 0.0, nsum = 0.0;
    int pcnt = 0, ncnt = 0;

    if(target <= 0)
    {
        // NOTE non ha senso: errore
    }

    // precompute response and sort it. h contains positive count
    for(int i =0; i<data.templates.size(); ++i)
    {
        double v = response[i];
        if(data.templates[i].category == 1)
        {
            h[v] += 1;
            psum += v;
            pcnt ++;
        }
        else
        {
            h[v] += 0; // dummy: only for insert the step in the map
            nsum += v;
            ncnt ++;
        }
    }

    int accum = 0;
    int count = 0;

    std::cout << "\tPositive Response Average = " << psum / (double) pcnt << " | Negative Response Average = " << nsum / (double) ncnt << '\n';
    std::cout << "\tResponses range from " << h.begin()->first << " to " << h.rbegin()->first << std::endl;

    // in ordine decrescente (TODO: si poteva cambiare la funzione sort della mappa)
    for(std::map<double, int>::const_reverse_iterator i = h.rbegin(); i != h.rend(); ++i)
    {
        accum += i->second;
        count++;
        if(accum >= target)
        {
            double th1 = i->first;
            ++i; // next threshold
            if(i==h.rend())
            {
                return th1 - std::abs(th1) * 0.01;  // 1% overhead if reach the end
            }
            double th2 = i->first;
            if(0)
                std::cout << "\tThreshold found: " << th1 << " | accum: " << accum << " | count: " << count << std::endl;
            return (th1 + th2)/2.0; // in the middle
        }
    }

    // nessuna reiezione (out of threshold)
    return h.begin()->first - std::abs(h.begin()->first) * 0.01;
}

/// find the rejection threshold
///  find the minimum threshold that keep at least target positives
/// @param target positive target
template<class DataSetType>
double findMinRejectionThreshold(const DataSetType & data, const std::vector<double> & response)
{
    double vmin = 100000000000000.0;

    // precompute response and sort it. h contains positive count
    for(int i =0; i<data.templates.size(); ++i)
    {
        double v = response[i];
        if(data.templates[i].category == 1)
        {
            if(v<vmin)
                vmin = v;
        }
    }

    return vmin - 0.00001;
}


// keeps only pattern with "response" over threshold
//template<class DataSetType>
//static void rejectSamples(DataSetType & data, std::vector<double> & response, double th)
//{
//    DataSetType out;
//    out.templates.reserve(data.templates.size()); // TODO
//    // use SetParams
//    //out.width = data.width;
//    //out.height = data.height;
//    //
//    out.SetParams(data.GetParams());
//    char buff[10];
//    data.GetConf(buff);
//    out.Configure(buff);
//
//    for(int i = 0; i<data.templates.size(); i++)
//    {
//        if(response[i]>=th)
//            out.Insert(data.templates[i]); //
//    }
//
//    data = out;
//    cout << data.n_patternP << " "<<data.n_patternN<<endl;
//    cin.get();
//}

/// Reject all samples (positive/negative) and modify response vector consequentely
template<class DataSetType>
void rejectSamples(DataSetType & data, std::vector<double> & response, double th)
{
    // I use support vector instead of erase in order to improve performance
    std::vector<typename DataSetType::PatternType> newList;
    std::vector<double> newResponse;

    newResponse.reserve(response.size());
    newList.reserve(data.templates.size());

    for(int i = 0; i<data.templates.size(); i++)
    {
        if(response[i]<th)
        {
            if(data.templates[i].category==1)
                data.n_patternP--;
            else
                data.n_patternN--;
            // do not copy
        }
        else
        {
            // copy in the newlist only elements above threshold
            newList.push_back(data.templates[i]);
            newResponse.push_back(response[i]);
        }
    }

    // swap vectors
    std::swap(data.templates, newList);
    std::swap(response, newResponse);

}


/// Reject only positive samples.
/// @note response vector is not modified so it became
template<class DataSetType>
void rejectPositiveSamples(DataSetType & data, const std::vector<double> & response, double th)
{
    std::vector<typename DataSetType::PatternType> newList;
    newList.reserve(data.templates.size()); // TODO

    for(int i = 0; i<data.templates.size(); i++)
    {
        if((response[i]>=th) || (data.templates[i].category==-1))
            newList.push_back(data.templates[i]); //
    }

    std::swap(data.templates, newList);

}


/// calcola il margine (ok, non e' un vero margine, ma e' la  distanza statistica) medio (tipo LDA), pero' non considera la varianza
template<class DataSetType>
double computeEdge(const DataSetType & data, const std::vector<double> & response)
{
    double pos = 0.0;
    double neg = 0.0;
    int npos = 0;
    int nneg = 0;
    for(int i = 0; i<data.templates.size(); i++)
    {
        if(data.templates[i].category == 1)
        {
            pos += response[i];
            npos++;
        }
        else
        {
            neg += response[i];
            nneg++;
        }
    }

    if(npos>0 && nneg>0)
        return (pos/npos) - (neg/nneg);
    else
    {
        return 0.0;
    }
}

/// quad :D
inline double quad(double x)
{
    return x*x;
}

/// compute a real statistic margin
template<class DataSetType>
double computeEdge2(const DataSetType & data, const std::vector<double> & response)
{
    double pos = 0.0;
    double neg = 0.0;
    double pos2 = 0.0;
    double neg2 = 0.0;
    int npos = 0;
    int nneg = 0;
    for(int i = 0; i<data.templates.size(); i++)
    {
        if(data.templates[i].category == 1)
        {
            pos += response[i];
            pos2 += response[i]*response[i];
            npos++;
        }
        else
        {
            neg += response[i];
            neg2 += response[i]*response[i];
            nneg++;
        }
    }

    if(npos>0 && nneg>0)
    {
        double var = 0.0;


//     for(int i = 0; i<data.templates.size(); i++)
//     {
//         if(data.templates[i].category == 1)
//         {
//             var += quad(response[i] - pos);
//         }
//         else
//         {
//             var += quad(response[i] - neg);
//         }
//     }
        pos/=(double)npos;
        neg/=(double)nneg;

        // compute the mahalanobis distance
        double sigma = std::sqrt( (pos2 + neg2 - pos*pos*npos - neg*neg*nneg) / (double) data.templates.size() );

        return (pos - neg) / sigma;
    }
    else
    {
        return 0.0;
    }
}

/// Compute the "RAW" response for SoftCascade Classifier (disable the "softcascade" architecture)
template<class Classifier, class DataSetType>
void inner_compute_raw_response(const DataSetType & data, double* response, const Classifier & classifier, int s0, int s1)
{
    for(int i = s0; i<s1; i++)
    {
        response[i] = classifier.raw(getData1(data.templates[i],data),getData2(data.templates[i],data));
    }
}

template<class Classifier, class DataSetType>
void inner_compute_response(const DataSetType & data, double * response, const Classifier & classifier, int s0, int s1)
{
    for(int i = s0; i<s1; i++)
    {
        response[i] = classifier(getData1(data.templates[i],data),getData2(data.templates[i],data));
    }
}

template<class Classifier, class DataSetType>
void inner_update_response(const DataSetType & data, double * response, const Classifier & classifier, int s0, int s1)
{
    for(int i = s0; i<s1; i++)
    {
        response[i] += classifier(getData1(data.templates[i],data),getData2(data.templates[i],data));
    }
}

// TOOD: move in library?
/// Compute the response for Boost Classifier
// template<class WeakClassifier, class DataSetType>
// void inner_compute_response(const DataSetType & data, std::vector<double> & response, const BoostClassifier<WeakClassifier> & classifier, int s0, int s1)
// {
//     for(int i = s0; i< s1; i++)
//     {
//         response[i] = classifier(getData1(data.templates[i],data),getData2(data.templates[i],data));
//     }
// }


// TOOD: move in library?
/// Compute the "RAW" response for SoftCascade Classifier
template<class WeakClassifier, class DataSetType>
void computeResponse(const DataSetType & data, std::vector<double> & response, const SoftCascadeClassifier<WeakClassifier> & classifier, int max_concurrent_jobs)
{
    response.resize( data.Size() );

#ifdef _MULTITHREAD
    if(max_concurrent_jobs>1)
    {
        sprint::thread_group thread_pool_;
        int n_jobs = data.Size();
        for(int ii=0; ii<max_concurrent_jobs; ii++)
        {
            int s0 = (ii*n_jobs)/max_concurrent_jobs;
            int s1 = ((ii+1)*n_jobs)/max_concurrent_jobs;

            thread_pool_.create_thread(sprint::thread_bind(&inner_compute_raw_response< SoftCascadeClassifier<WeakClassifier>, DataSetType>, sprint::c_ref(data), &response[0], sprint::c_ref(classifier), s0, s1));
        }

        thread_pool_.join_all();
    }
    else
#endif
    {
        inner_compute_raw_response(data, &response[0], classifier, 0, data.Size());
    }
}


// TOOD: move in library?
/// Compute the response for Boost Classifier
template<class WeakClassifier, class DataSetType>
static void computeResponse(const DataSetType & data, std::vector<double> & response, const BoostClassifier<WeakClassifier> & classifier, int max_concurrent_jobs)
{
    response.resize( data.Size() );

#ifdef _MULTITHREAD
    if(max_concurrent_jobs>1)
    {
        sprint::thread_group thread_pool_;
        int n_jobs = data.Size();
        for(int ii=0; ii<max_concurrent_jobs; ii++)
        {
            int s0 = (ii*n_jobs)/max_concurrent_jobs;
            int s1 = ((ii+1)*n_jobs)/max_concurrent_jobs;

            thread_pool_.create_thread(sprint::thread_bind(&inner_compute_response< BoostClassifier<WeakClassifier>, DataSetType>, sprint::c_ref(data), &response[0], sprint::c_ref(classifier), s0, s1));
        }

        thread_pool_.join_all();
    }
    else
#endif
    {
        inner_compute_response(data, &response[0], classifier, 0, data.Size());
    }
}


/// reset response to 0
template<class DataSetType>
static void resetResponse(const DataSetType & data, std::vector<double> & response)
{
    response.resize( data.Size() );

    for(int i = 0; i<data.templates.size(); i++)
    {
        response[i] = 0.0;
    }
}

// TOOD: move in library?
/// Update the response with a new classifier
template<class WeakClassifier, class DataSetType>
static void updateResponse(const DataSetType & data, std::vector<double> & response, const WeakClassifier & classifier, int max_concurrent_jobs)
{

#ifdef _MULTITHREAD
    if(max_concurrent_jobs>1)
    {
        sprint::thread_group thread_pool_;
        int n_jobs = data.Size();
        for(int ii=0; ii<max_concurrent_jobs; ii++)
        {
            int s0 = (ii*n_jobs)/max_concurrent_jobs;
            int s1 = ((ii+1)*n_jobs)/max_concurrent_jobs;

            thread_pool_.create_thread(sprint::thread_bind(&inner_update_response< WeakClassifier, DataSetType>, sprint::c_ref(data), &response[0], sprint::c_ref(classifier), s0, s1));
        }

        thread_pool_.join_all();
    }
    else
#endif
    {
        inner_update_response(data, &response[0], classifier, 0, data.Size());
    }
}


/// softcascade sort_algo strings
static const char *str_metric[] = {"none", "weight", "edge", "mahalanobis"};

/// List of SoftCascade metrics
enum SoftCascadeRankingAlgo {
    Ranking_None,
    Ranking_Weight,
    Ranking_Edge,
    Ranking_Mahalanobis
};

/** Convert a BoostClassifier @a source in a SoftCascadeClassifier @dest using the @a training_set
 * @param training_set a training set
 * @param _source a boost classifier
 * @param dest the output softcascade
 * @param max_stages limit the maximum number of stages or -1
 * @param sort_algo one of the SoftCascadeRankingAlgo ranking algorithms
 * @param dbp Direct backward pruning scheme
 * */
template<class WeakClassifier, class DataSetType>
void TrainSoftCascade(DataSetType & training_set, const BoostClassifier<WeakClassifier> & _source, SoftCascadeClassifier<WeakClassifier> & dest, int max_stages, SoftCascadeRankingAlgo sort_algo, bool dbp, double ratio, double alpha, bool blind_complete, int max_concurrent_jobs, bool verbose = true)
{
    BoostClassifier<WeakClassifier> source = _source; // working on a copy (destructive training)
    double p = training_set.n_patternP; // numero di positivi (in floating point) richiesti
    std::vector<double> v;

    int n = (max_stages < 1) ? (source.length()) : ( std::min<int>(source.length(), max_stages) );

    int sumNegative = 0;
    int initialNegative = training_set.n_patternN;
    int initialPositive = training_set.n_patternP;

    std::vector<double> response;

    if(verbose)
        std::cout << "[+] using " << n << " features of " << source.length() << " | metric = " << sort_algo << " ("<< str_metric[sort_algo] << ")" << std::endl;

    // Use direct backward pruning scheme
    if(dbp)
    {
        // reject ratio * training_set.n_patternP patterns

        computeResponse(training_set, response, source, max_concurrent_jobs);

        int p0 = (1.0-ratio) * training_set.n_patternP;
        double r = findBestRejectionThreshold(training_set, response, p0);
        if(verbose)
            std::cout << "Using threshold " << r << " to remove too difficult patterns (try to keep " << p0 << " patterns)" << std::endl;

        // reject alla samples
        rejectPositiveSamples(training_set, response, r);
        // NOTE response ora e' invalido. va chiamato computeResponse

        if(verbose)
            std::cout << training_set.n_patternP <<"/" << initialPositive <<" patterns survived" << std::endl;

    }
    else
    {
        // calcola il profilo di rejection
        computeRejectionDistribution(v, alpha, ratio * training_set.n_patternP, n );
    }

    // at maximum a chain long <n> classifiers
    for(int t =0; t<n; ++t)
    {
        int curPositive, curNegative;

        // positive/negative before rejection
        curPositive = training_set.n_patternP;
        curNegative = training_set.n_patternN;

        if(dbp)
        {
            if(verbose)
                std::cout << '#' << t << std::endl;
        }
        else
        {
            // rimuovo la quantita' da regettare allo stadio t
            p -= v[t];

            if(verbose)
                std::cout << '#' << t << " | Positive requested = " << p << "/" << training_set.n_patternP << std::endl;
        }

        // calcolo la risposta fino a questo momento del 'dest' di tutti i pattern del training_set
        //  e' necessario farla a ogni giro piu' che altro perche' la rimozione dei pattern incasina l'ordine dei pattern.
        // TODO: really slow!
        computeResponse(training_set, response, dest, max_concurrent_jobs);

        double bestEdge = 0.0;
        // without resorting jbest is the first of the list
        typename BoostClassifier<WeakClassifier>::ClassifierListType::iterator jbest = source.list().begin();
        std::vector<double> bestResponse;

        switch(sort_algo)
        {
        case Ranking_Weight:
        {
            // ALGO: WEIGHT
            // prendo il maggiore alpha rimanente (si poteva fare un sorting + algo:none)
            bestResponse = response;

            for (typename BoostClassifier<WeakClassifier>::ClassifierListType::iterator j = source.list().begin(); j!=source.list().end(); ++j)
                if(j->alpha > jbest->alpha) // NOTE TODO: gli alberi di decisione non hanno alpha
                    jbest = j;


            // update response
            updateResponse(training_set, bestResponse, *jbest, max_concurrent_jobs);
        }
        break;

        case Ranking_Edge:
        {
            // ALGO: EDGE (original softascade)

            // for each classifier this function sarches the one that maximize a statistic
            for (typename BoostClassifier<WeakClassifier>::ClassifierListType::iterator j = source.list().begin(); j!=source.list().end(); ++j) {
                std::vector<double> test = response; // copy the original response

                // update test with (*j)
                updateResponse(training_set, test, *j, max_concurrent_jobs);
                // TODO: multithread
//                 for(int i = 0; i<training_set.templates.size(); i++)
//                 {
//                     test[i] += (*j)( getData1(training_set.templates[i], training_set), getData2(training_set.templates[i], training_set) );
//                 }

                // compute the edge
                double edge = computeEdge(training_set, test);
                if(j == source.list().begin() || (edge > bestEdge))
                {
                    jbest = j;
                    bestEdge = edge;
                    bestResponse = test;
                }

            }

            if(verbose)
                std::cout << "\tavg edge = " << bestEdge << '\n';
        }
        break;
        case Ranking_Mahalanobis:
        {
            // ALGO: MAHALANOBIS

            // per ognuno dei weak classifier rimasti
            //  cerco quello che massimizza la separazione statistica
            for (typename BoostClassifier<WeakClassifier>::ClassifierListType::iterator j = source.list().begin(); j!=source.list().end(); ++j) {
                std::vector<double> test = response;

                // update test with (*j)
                updateResponse(training_set, test, *j, max_concurrent_jobs);
                /*
                for(int i = 0; i<training_set.templates.size(); i++)
                {
                    test[i] += (*j)(getData1(  training_set.templates[i], training_set), getData2(training_set.templates[i], training_set) );
                }
                */

                // compute edge
                double edge = computeEdge2(training_set, test);
                if(j == source.list().begin() || (edge > bestEdge))
                {
                    jbest = j;
                    bestEdge = edge;
                    bestResponse = test;
                }

            }

            if(verbose)
                std::cout << "\tavg edge = " << bestEdge << '\n';
        }
        break;
        case Ranking_None:
        {
            // ALGO: NONE
            // prendo il successivo in lista
            bestResponse = response;

            updateResponse(training_set, bestResponse, *jbest, max_concurrent_jobs);
            // aggiorno test
            // for(int i = 0; i<training_set.templates.size(); i++)
            //                bestResponse[i] += (*jbest)(training_set.templates[i].data+training_set.width+1+1,training_set.width+1);
        }
        break;
        }
        /*      NOTE: gli alberi di decisione non hanno alpha TODO */
        if(verbose)
            std::cout << "\talpha = " << jbest->alpha << std::endl;

        // no need to compute new response (response is alredy updated in bestResponse)

        double r;
        if(dbp)
        {
            // find the trehsold r that keep all positive patterns
            r = findMinRejectionThreshold(training_set, bestResponse);

        }
        else
        {
            // find the trehsold r that keep at least p patterns
            r = findBestRejectionThreshold(training_set, bestResponse, (int) p);
        }

        if(verbose)
            std::cout <<  "\tthreshold = " << r << std::endl;

        // add jbest to output pipeline, with threshold r
        dest.insert(*jbest, jbest->alpha, r); // NOTE TODO gli aberi decisione non hanno alpha

        // rimuovo jbest dai sorgenti
        source.list().erase(jbest);

        // rimuovo tutti i sample (positivi o negativi che siano) che non hanno soddisfatto the r threshold
        rejectSamples(training_set, bestResponse, r);
        // NOTE bestResponse viene modificata di conseguenza (si potrebbe swap su response ed evitare
        //      cosi' invocare computeResponse continuamente)

        if(verbose)
            std::cout <<  "\tpositive = " << training_set.n_patternP << " (" << (100 * training_set.n_patternP) / initialPositive << "%) " << (int) training_set.n_patternP - curPositive
                      <<  " | negative = " << training_set.n_patternN << " (" << (100 * training_set.n_patternN) / initialNegative << "%) " << (int) training_set.n_patternN - curNegative << std::endl;

        // statistiche dei negativi
        sumNegative += training_set.n_patternN;
        if(training_set.n_patternN == 0 || training_set.n_patternP == 0)
        {
            if(verbose)
                std::cout << "no pattern lefts. terminated." << std::endl;
            break;
        }

    }


////// se e' negativo non verra' considerato mai e la risposta e' fissa (-1). Se e' positivo la soglia che ritorna e' quella effettiva di AdaBoost.
    if(blind_complete)
    {
        if(verbose)
            std::cout << "Insert additional " << source.list().size() << " features..." << std::endl;
        // MERGE REMAINING CLASSIFIER in order to use AdaBoost as original in the last part
        for (typename BoostClassifier<WeakClassifier>::ClassifierListType::iterator j = source.list().begin(); j!=source.list().end(); ++j)
            dest.insert(*j, j->alpha, -100000.0); // TODO lower bound of worst pattern
    }

//   std::cout << "Average negative samples evaluated:" << (double) sumNegative / (double) dest.length() <<  "/" << initialNegative << std::endl;
//   std::cout << "[+] Average number of stages evaluated for negative samples = " << (double) initialNegative * (double) dest.length()  / (double) sumNegative << std::endl;
    if(verbose)
        std::cout << "[+] Average number of stages evaluated for negative samples = " << (double) sumNegative / (double) initialNegative << std::endl;


}

#endif
