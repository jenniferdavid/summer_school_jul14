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

#ifndef _DECISION_TREE_LEARNER_H
#define _DECISION_TREE_LEARNER_H

/** @file DecisionTreeLearner.h
 *  @brief method to train a DecisionTree */

#include "Pattern/Pattern.h"
#include "DataSet.h"
#include "InformationMetrics.h"
#include "Classifier/DecisionTree.h"
#include "Test.h"

struct DTResponse {
  int response;
  int category;
};

bool operator < (const DTResponse &a, const DTResponse & b)
{
  return a.response < b.response;
}

/// discrete policy
class IntegerPolicy {
  int A, B;
};

/// weighted policy
class WeightedPolicy {
  double A,B;
};

template<class T>
std::pair<int, float> FindThreshold(const DTResponse *data, int n, int A, int B);

// void grow(X, y)
// {
// se tutti i valodi di Y sono uguali, ritornare Y
// se tutti i valori di X sono uguali (ovvero X.row()<2), ritornare LeafNode per maggioranza su Y.
// altrimenti trovare un attributo con il piu' elevato Info Gain (IG)
// 
// scegliere m feature a caso.
// for j=1 a m:
// IG = f(Y|Xj)
// 
// 
// 
// }

// TODO: extend to AdaBoost weighted samples
template<>
std::pair<int, float> FindThreshold<GiniIndex>(const DTResponse *data, int n, int A, int B)
{
int a1,b1; // left count
int a2,b2; // right count
std::pair<int, float> out;
// trovare threshold e parity che massimizzino l'indice di Gini
// max(a1*b1/(a1+b1) + a2*b2/(a2*b2));

// value > th ? RIGHT : LEFT

// con la soglia minima, tutti gli elementi sono nella RIGHT.
a2=A;
b2=B;
// e nessun elemento nella LEFT
a1=0;
b1=0;
/*
out.second = (float)(a1*b1)/(float)(a1+b1);
out.first = data[0].response;*/

out.second = 2.0 * (float)(a2*b2)/(float)(a2+b2);


for(int i=0;i<n-1;++i)
  {
//   std::cout << data[i].response << ',' << data[i].category << ' ';

  // look for transition
  if(i!=0 && data[i-1].response != data[i].response)
    {
    float index = (float)(a1*b1)/(float)(a1+b1) + (float)(a2*b2)/(float)(a2+b2);
    
  //   std::cout << i << ' ' << index << ';';
  //   std::cout << index << ' ' << a1 << ' ' << b1 << ' ' << a2 << ' ' << b2  << '\n';
    
    if(index <= out.second) // keep last
      {
//       std::cout << index << ' ' << a1 << ' ' << b1 << ' ' << a2 << ' ' << b2  << '\n';
      out.second = index;
      out.first = ( data[i-1].response +  data[i].response ) / 2;
      }
    }
    
  if(data[i].category==-1) // B
  { b1++,b2--; }	// TODO: extend to AdaBoost case
  else
  { a1++,a2--; } // TODO: extend to AdaBoost case
    
  
  }
// exit(1);
return out;
}

template<class FeatureExtractor, class PatternType>
float TestThreshold(const std::vector<PatternType> & patterns, long stride, const FeatureExtractor & h, int th)
{
  int a1,a2,b1,b2;
  a1=a2=b1=b2 = 0;
  for(int i = 0;i<patterns.size(); ++i)
  {
    int resp = h(patterns[i].data+stride+1, stride);
    if(resp > th)
    {
      if(patterns[i].category == 1) 
	a2++;
      else
	b2++;
    }
    else
    {
      if(patterns[i].category == 1) 
	a1++;
      else
	b1++;
    }
  }
  
  std::cout << "Expected: " << a1 << ',' << b1 << ' ' << a2 << ',' << b2 << std::endl;
  
  return (float)(a1*b1)/(float)(a1+b1) + (float)(a2*b2)/(float)(a2+b2);
}

template<class FeatureExtractor, class PatternType, class Param>
void build_response(DTResponse *resp, const PatternType * patterns, Param param, int n, const FeatureExtractor & op)
{
  for(int i =0;i<n;++i)
  {
    resp[i].response = op.response(getData1(patterns[i], param), getData2(patterns[i], param) );
    resp[i].category = patterns[i].category;
  }
  
  // sorting
  std::sort(&resp[0], &resp[n]);
}

// static int min_tree_size = 4;

/** Build a Recursive Decision Tree */
template<class FeatureExtractor, class FeatureGenerator, class PatternType, class Param1>
bool BuildDecisionTree(DecisionTree<FeatureExtractor> &root, const std::vector<PatternType> & patterns, Param1 param, FeatureGenerator & f, int max_depth)
{
  std::pair<int, float> out, best;
  int n = patterns.size();
  int A,B;
//   float wA, wB;
  A=B=0;
//   wA = wB = 0.0f;
  
  f.Reset();
  
  // check
  for(int i =0;i<n;++i)
  {
    if(patterns[i].category == 1)
    {
      A++;
//       wA += patterns[i].GetWeightedCategory();
    }
    else
    if(patterns[i].category == -1)
    {
      B++;
//       wB += patterns[i].GetWeightedCategory();
    }
    else
      throw std::runtime_error("wrong category label");
  }
  
//   std::cout << "A:" << A << " B:" << B << std::endl;
  float initial_index;
  
  if(max_depth == 0 || (A==0) || (B==0) /* || (A+B < min_tree_size) */ )
  {
    if(A==0 && B==0)
      throw std::runtime_error("wrong splitting");

    std::cout << 'x';
    std::cout.flush();
    
//     std::cout << "LEAF" << std::endl;
      // root->category = (A>B) ? 1 : -1;
      root.category = (float)(A-B)/(float)(A+B);
//     root.category = (wA+wB)/(wA-wB);
    return true;
  }
  else
  {
  
  DTResponse *resp = new DTResponse[ n ] ;
  typename FeatureGenerator::FeatureType h, bestH;
  best.second = -1.0f;
  
  initial_index = (float)(A*B)/(float)(A+B);
  
  int iter = 0;
 //   std::cout << "Evaluate " << src.size() << " features" << std::endl;
  while(f.Next(h))
  {
   
    build_response(resp, &patterns[0], param, n, h);
    out = FindThreshold<GiniIndex>(resp, n, A, B);
/*    
     float tst = TestThreshold<typename FeatureGenerator::FeatureType>(patterns, stride, h, out.first);
      if(tst != out.second)
        throw std::runtime_error("test failed");
     */
    if(iter == 0 || out.second < best.second)
    {
      best = out;
      bestH = h;
    }
    ++iter;
  }
  
  if(iter == 0)
    throw std::runtime_error("Feature Generator missconfigured");
  
  if(best.second > initial_index)
  {
    std::cout << '?';
    std::cout.flush();
    
//     std::cout << "WEAKLEAF" << std::endl;
      // root->category = (A>B) ? 1 : -1;
    root.category = (float)(A-B)/(float)(A+B);
    // root.category = (wA+wB)/(wA-wB);
    return true;
  }
  
//    std::cout << "from: " << initial_index << '\n';
//    std::cout << "index:" << best.second << " th:" << best.first << std::endl;
//    float tst = TestThreshold<typename FeatureGenerator::FeatureType>(patterns, stride, bestH, best.first);
//    std::cout << "\t" << tst << std::endl;
//   
  std::vector<PatternType> left, right;
  
  for(int i =0;i<n;++i)
  {
    // int resp = bestH(patterns[i].data+stride+1, stride);
    int resp = bestH.response(getData1(patterns[i], param), getData2(patterns[i], param) );
    
    if(resp > best.first)
      right.push_back(patterns[i]);
    else
      left.push_back(patterns[i]);
      
  }
  
  delete [] resp;
  
  root.classifier = bestH;
  
  root.th = best.first;
//   std::cout << "{\n";
  root.left = new DecisionTree<FeatureExtractor>();
  root.right = new DecisionTree<FeatureExtractor>();
  BuildDecisionTree<FeatureExtractor>(*root.left, left, param, f, max_depth-1);
//   std::cout << ",\n";
  BuildDecisionTree<FeatureExtractor>(*root.right, right, param, f, max_depth-1);
//   std::cout << "}\n";

    std::cout << '.';
    std::cout.flush();
  
  return true;
  }
}

/// A Decision Tree builder
template<class Aggregator>
struct DecisionTreeLearner: public DataSetHandle<Aggregator > {
public:
  
  typedef DataSet< Aggregator > DataSetType;
  typedef DataSetHandle< Aggregator > DataSetHandleType;

  DECLARE_AGGREGATOR
  
public:
  
  /// max depth 
  int depth;
  
public:
  
  DecisionTreeLearner() : depth(10) { }
  
  /// import a training set
  void SetTrainingSet(const DataSetHandleType & src)
  {
    static_cast< DataSetHandleType &>(*this) = src;
  }
  
  /// Build a tree
  template<class FeatureExtractor, class FeatureGenerator>
  bool BuildDecisionTree(DecisionTree<FeatureExtractor> & root, FeatureGenerator & f) const
  {
    return ::BuildDecisionTree<FeatureExtractor>(root, DataSetHandle<Aggregator>::templates, this->GetParams(), f, depth);
  }
  
  /// Test tree performance on the training set
  template<class FeatureType>
  void Test(const DecisionTree<FeatureType> & root)
  {
    ::Test(root, static_cast<const DataSetHandle< Aggregator > &>(*this) );
  }
};

#if 0
template<class DecisionTreeType, class DataSetType>
bool Prune(DecisionTreeType * root, const DataSetType & data, float threshold)
{
  if(left!=0)
  {
    Prune(left, data, threshold);
    Prune(right, data, threshold);
  }
  else
  {
    // is a leaf
    return true;
  }
  
  for(int i = 0;i<data.templates.size();i++)
  {
    float ret = (*root)(data.templates[i].data+data.width+1+1,data.width+1);
  }
}
#endif

  
#endif