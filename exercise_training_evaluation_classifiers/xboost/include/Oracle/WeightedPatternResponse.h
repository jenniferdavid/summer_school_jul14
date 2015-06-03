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

#ifndef _WEIGHTED_PATTERN_RESPONSE_H
#define _WEIGHTED_PATTERN_RESPONSE_H

/** @file WeightedPatternResponse.h
 * @brief WeightedPatternResponse methods and utility functions */

#include <cmath>

#include "Classifier/DecisionStump.h"
#include "Classifier/MultiClassDecisionStump.h"

#include "Classifier/RealDecisionStump.h"

/// data request by some trainer associated to any training pattern in cache, for fast element evaluation
/// @note valid only for binary problem
/// @note used internally by Oracles
template<class ResponseType>
struct BinaryWeightedPatternResponse {
  /// Weight associated to this pattern with sign ( >0 = pos +1, <0 =  neg -1)
  double d;

  /// feature response associated with this pattern
  ResponseType value;

public:
  
  /// return current class identifier (see above)
  int getClass() const { return (d>0.0) ? -1 : 1; }
  
  /// return the weight associated to this class
  double getWeight() const { return std::abs(d); }

  /// return weighted category
  float GetWeightedCategory() const { return d; }
  
  /// operator used for sorting
  bool operator < (const BinaryWeightedPatternResponse<ResponseType> & c) const
    {
    return value < c.value;
    }
};

/// A weighted pattern response with integer values
typedef BinaryWeightedPatternResponse<int> PatternResponse;

/// set up @a d field in BinaryWeightedPatternResponse
template<class Set>
void __InitializePatternResponse(BinaryWeightedPatternResponse<int> *store, int n_feature, const Set & set)
{
    int n = set.Size();
    // for each input pattern i0...i1
    for(unsigned int i=0;i<n;++i)
    {
        double d = set.templates[i].category * set.templates[i].d;
        // variation of weight d associated
        for(unsigned int j=0;j<n_feature;j++)
          store[i + j * n].d = d;
    }  
}

/** Used when multiple feature are evaluated at the same time on the same set, in a multi-thread flavor
* @param i0,i1 range of training set evaluated 
* */
template<class FeatureExtractor, class Set>
void ComputeFeaturesResponse(BinaryWeightedPatternResponse<int> *store, const FeatureExtractor * h, int n_feature, const Set & set, int i0, int i1)
{
    int stride = set.Size();
    // for each input pattern i0...i1
    for(unsigned int i=i0; i<i1; i++)
    {
        double d = set.templates[i].category * set.templates[i].d;
        // evaluate any feature on the h block
        for(unsigned int j=0; j<n_feature; ++j)
        {
            // feature value
            store[i+j*stride].value = h[j].response( getData1(set.templates[i], set), getData2(set.templates[i], set) );
            store[i+j*stride].d = d;
        }
    }
}


/// extract from templates feature using h and put in store, and associating the weighted category d
template<class FeatureExtractor, class Set>
void ExtractFeature(BinaryWeightedPatternResponse<int> *store, const FeatureExtractor & h, const Set & set)
{
    for(unsigned int i =0; i<set.Size(); i++)
    {
        // feature value: nota la memoria punta al pixel (-1,-1)
        store[i].value = h.response( getData1(set.templates[i], set), getData2(set.templates[i], set) );
        // variation of weight d associated
        // d, peso associato al pattern, value >0 se e' uno di tipo A, altri < 0
        // note: non gestisce il caso nativo di astensione
        store[i].d = set.templates[i].category * set.templates[i].d;
    }
}

/** Generate only bucket from a precompute response @a value array
 *  @param store an output histogram
 *  @param n_bucket size of the histogram, number of bin/bucket
 *  @param source precomputed response with weight
 *  @param n_element size of precomputed response
 * */
void GenerateBucket(BinaryWeightedPatternResponse<int> *store, int n_bucket, const BinaryWeightedPatternResponse<int> *source, int n_element);

/// extract from templates feature using h and put in store, and associating the weighted category d
/// @param value a working vector set.Size() large
template<class FeatureExtractor, class Set>
void ExtractFeatureAndBucket(BinaryWeightedPatternResponse<int> *store, const FeatureExtractor & h, const Set & set, int n_bucket, int *value)
{
    int min_resp, max_resp;
    int bin_size;
    const int n_element = set.Size();
    // compute value, min and max:
    for(unsigned int i =0; i < n_element; i++)
    {
        value[i] = h.response( getData1(set.templates[i], set), getData2(set.templates[i], set) );
        if(i==0) { min_resp = max_resp = value[0]; }
        else {
          if(min_resp > value[i]) min_resp = value[i];
          if(max_resp < value[i]) max_resp = value[i];
        }
    }
    
    // compute bin_size rounded (max - min + 1) elements
    bin_size = (max_resp - min_resp + 1 + n_bucket - 1) / n_bucket;

    // avoid division by zero:
    if(bin_size == 0)
      bin_size = 1;
    
    // setup store[i] memory
    for(int i = 0;i<n_bucket; ++i)
    {
      store[i].value = min_resp + bin_size * i;
      store[i].d = 0.0;
    }
    
    // fill histogram
    for(unsigned int i =0; i< n_element; i++)
    {
     int bin = (value[i] - min_resp) / bin_size;
     /*
     if(bin<0 || bin>=n_bucket)
       std::cerr << "internal error: bin=" << bin << " | value=" << value[i] << " | min=" << min_resp << " | max=" << max_resp << " | bin_size=" << bin_size << std::endl;
     */
     store[bin].d += set.templates[i].category * set.templates[i].d; // TODO: precompute 
    }    
}


template<class ResponseType>
struct WeightedPatternResponse {
  
  /// category
  int category;

  /// Weight associated to this pattern 
  double d;

  /// feature response associated with this pattern
  ResponseType value;

public:
  
  /// return current class identifier (see above)
  int getClass() const { return category; }
  
  /// return the weight associated to this class
  double getWeight() const { return d; }

  /// operator used for sorting
  bool operator < (const WeightedPatternResponse<ResponseType> & c) const
    {
    return value < c.value;
    }
};


/** Used when multiple feature are evaluated at the same time on the same set, in a multi-thread flavor
* @param i0,i1 range of training set evaluated 
* */
template<class FeatureExtractor, class Set>
void ExtractFeatureSubPart(WeightedPatternResponse<int> *store, const FeatureExtractor * h, int n_feature, const Set & set, int i0, int i1)
{
    long stride = set.Size();
    // for each input pattern i0...i1
    for(unsigned int i=i0; i<i1; i++)
    {
        // evaluate any feature on the h block
        for(unsigned int j=0; j<n_feature; ++j)
        {
            // feature value
            store[i+j*stride].value = h[j].response( getData1( set.templates[i], set), getData2( set.templates[i], set) );
            // variation of weight d associated
            store[i+j*stride].d = set.templates[i].d;
            store[i+j*stride].category = set.templates[i].category;
        }

    }
}

/// extract from templates feature using h and put in store, and associating the weighted category d
template<class FeatureExtractor, class Set>
void ExtractFeature(WeightedPatternResponse<int> *store, const FeatureExtractor & h, const Set & set)
{
    for(unsigned int i =0; i<set.Size(); i++)
    {
        // feature value: nota la memoria punta al pixel (-1,-1)
        store[i].value = h.response( getData1( set.templates[i], set), getData2( set.templates[i], set) );
        // variation of weight d associated
        // d, peso associato al pattern, value >0 se e' uno di tipo A, altri < 0
        // note: non gestisce il caso nativo di astensione
        store[i].d = set.templates[i].d;
        store[i].category = set.templates[i].category;
    }
}

///////// TODO: le metriche di AdaBoost devono stare qua?

/// AdaBoost W+ metric (or any metric based on weighted pattern sum) for DecisionStumpPolicy (in this case only used as container for the param recovered) using PatternResponse
class AdaBoostMetric
{
public:
/* CALLGRIND 04/02/2009
 *  find_threshold 99.96%, (~14.28% self)
 *  sort 54,98% (14,65% self inlined)
 *  eval 30.71% (totally inlined)
 **/
/** This function find the best threshold in a DecisionStump maximing W+ parameters (elements over threshold weighted negatively, element lesser or equal of threshold weighted positively)
  * @param h    a DecisionStumpPolicy (in output the threshold and parity fields are setted)
  * @param store an array of (sorted) value returned by HaarClassifier 
  * @param n    number of stored elements
  * @param sum_d sum of store.d element with sign (precomputed)
  * @return the adaboost W+ parameters and set @a h threshold
  * TODO: change name
  **/
static double optimize(DecisionStump<int> & h, const PatternResponse *store, int n, double sum_d)  __attribute__ ((deprecated));

/** This function find the best threshold in a DecisionStump maximing W+ parameters (elements over threshold weighted negatively, element lesser or equal of threshold weighted positively)
  * @param h    a DecisionStumpPolicy (in output the threshold and parity fields are setted)
  * @param store an array of (sorted) value returned by HaarClassifier 
  * @param n    number of stored elements
  * @param wp   precomputed sum of positive elements
  * @param wn   precomputed sum of negative elements
  * @param strictly_growing suggest that elements are strictly growing, in order to avoid check element transitions
  * @return the adaboost W+ parameters and set @a h threshold
  **/
static double optimize(DecisionStump<int> & h, const PatternResponse *store, int n, double wp, double wn, bool strictly_growing=false);

/// optimize the multiclass problem, single threshold
/// TODO: unimplemented
static double optimize(MultiClassDecisionStumpSingleThreshold<int> & h, const WeightedPatternResponse<int> *store, int n, const double *sum_d);
/// optimize the multiclass problem, multiple threshold
/// TODO: unimplemented
static double optimize(MultiClassDecisionStumpMultiThreshold<int> & h, const WeightedPatternResponse<int> *store, int n, const double *sum_d);

};

/// This metric use the GentleBoost algorithm to minimize feature response.
///  It minimize (y_i - f(x_i)^2)
/// The return value must be minimized.
class GentleAdaBoostMetric {
public:
static double optimize(RealDecisionStump<int> & h, const PatternResponse *store, int n, double wp, double wn);
};

/// This metric use the RealAdaBoost algorithm to minimize feature response.
///  It minimize e^{-y_i f(x_i) }
/// The return value must be minimized (Z_t)
class RealAdaBoostMetric {
public:
static double optimize(RealDecisionStump<int> & h, const PatternResponse *store, int n, double wp, double wn);
};

/** sort pattern with ascending "value". use before find_threshold */
void sort_pattern(PatternResponse *store, int n);
/*@}*/


#endif