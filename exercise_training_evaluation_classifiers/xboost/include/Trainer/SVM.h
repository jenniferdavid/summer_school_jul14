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

#ifndef _SVM_H
#define _SVM_H

#include <cmath>
#include <algorithm>
#include <vector>

#include <iostream>

/// dot product
inline double dot(const double *x1, const double *x2, int m)
{
  double s = 0.0;
  for(int i =0;i<m;++i)
    s += x1[i] * x2[i];
  return s;
}

/// evaluate wolfe dual
inline double evaluate(const double *alpha, const int *y, const double **x, int n, int m)
{
  double s = 0.0;
  for(int i =0;i<n;++i)
    s -= alpha[i];
  
  for(int i=0;i<n;++i)
    for(int j=0;j<n;++j)
      s+= y[i] * y[j] * dot(x[i], x[j], m) * alpha[i] * alpha[j];
    
  return s;
}

/// linear kernel u=(w^\top x - b)
inline double linear(const double *x, const double *w, double b, int m)
{
  return dot(x,w,m) - b;
}

/// compute 1/|w|
inline double margin(const double *w, int m)
{
  return 1.0 / std::sqrt(dot(w,w,m));
}

/// dot product
class metric_linear {
  

  /// backup 
  std::vector<double *> m_x;
  
  /// number of samples
  int m_n;	
  /// number of feature (space size)
  int m_m;
 

public:

  metric_linear()
  {
  }
  ~metric_linear()
  {
  }
  
  void computeCache(int n, int m, const std::vector<double *> & x)
  {
    m_n = n;
    m_m = m;
    m_x = x;
    std::cerr << "Cache completed" << std::endl;
  }
  
  inline double f(int i, int j) const
  {
    return dot(m_x[i],m_x[j], m_m);
  }
  
};


/// una metrica che calcola la cache
class metric_cache {
  
  double *m_k;	// cache

  /// number of samples
  int m_n;	
  /// number of feature (space size)
  int m_m;
 

public:

  metric_cache()
  {
    m_k = 0;
  }
  ~metric_cache()
  {
    delete [] m_k;
  }
  
  void computeCache(int n, int m, const std::vector<double *> & x)
  {
    m_n = n;
    m_m = m;
    
    std::cerr << "Expected " << n*n*sizeof(double)/(1024*1024) << " Mb for cache" << std::endl;
    delete [] m_k;
    m_k = new double[n*n];
    
    std::cerr << "Builindg cache..." << std::endl;
    
    for(int j=0;j<n;++j)
      for(int i=0;i<n;++i)
	m_k[i + j * n] = dot(x[i], x[j], m_m);
      
    std::cerr << "Cache completed" << std::endl;
  }
  
  inline double f(int i, int j) const
  {
    return m_k[i + j * m_n];
  }
  
};

/// SVM trainer
template<class Functor>
struct svm_train: public Functor {

  /// number of samples
  int n;	
  /// number of feature (space size)
  int m;
  
  
  /// input patterns
  std::vector<double *> x;
  
  /// alpha weights
  std::vector<double> alpha;

  /// train category {-1,+1}
  std::vector<int> y;
  
  /// hyperplane vector [out]
  double *w;
  /// hyperplane bias [out]
  double bias;
  
    /// C constant SVM
  double C;
  
  /// epsilon
  double epsilon;
  /// tollerance
  double tol;

private:
  
  double *e;	// error cache;

  int minimum;
  int maximum;
  
private:
  
double cache(int i,int j) const {
  return Functor::f(i,j);
}
  
double learnedFunc( int k ) const
{  
   double s = 0.0;  
   long i;  
   for( i = 0; i < n; i++ ) {  
      if( alpha[i] > 0.0 )  
         s += alpha[i]*y[i]*cache(i,k);  
   }  
  s -= bias;  
  return s;  
}  
  
  
void updateErrorCache()
{
  for(int i =0;i<n;++i)
      e[i] = (alpha[i]>0.0 && alpha[i]<C) ? ( linear(x[i], w, bias, m) - y[i] ) : 0.0;
}

/// count the alpha out of BOUND
int nonBoundLagrangeMultipliers() const
{
   int result = 0;

   for (int i = 0; i < n; i++)
   {
      if (alpha[i] > 0.0 && alpha[i] < C)
      {
         result++;
      }
   }

   return result;
}

/// count alpha>0
int nonZeroLagrangeMultipliers() const
{
   int result = 0;

   for (int i = 0; i < n; i++)
   {
      if (alpha[i] > 0.0)
      {
         result++;
      }
   }

   return result;
}


bool takeStep(int i1, int i2)
{
if(i1==i2)
  return false;

// std::cout << "examine " << i1 << ' ' << i2 << std::endl;

double L,H;
double a1 = alpha[i1];
double a2 = alpha[i2];
int y1 = y[i1];
int y2 = y[i2];
int s = y1*y2;

if(y1 != y2)
{
  L = std::max<double>(0, a2 - a1);
  H = std::min<double>(C, C + a2 - a1);
}
else
{
  L = std::max<double>(0, a2 + a1 - C);
  H = std::min<double>(C, a2 + a1);
}
if(L==H)
{
//   std::cout << "\tL:" << L << "==H"<< std::endl;
  return false;
}

// std::cout << "L:" << L << " H:" << H << std::endl;

/* recompute Lagrange multiplier for pattern i2 */
double e1 = (a1 > 0.0 && a1 < C) ? e[i1] : ( learnedFunc(i1) - y1 );
double e2 = (a2 > 0.0 && a2 < C) ? e[i2] : ( learnedFunc(i2) - y2 );
   
double k11 = cache(i1,i1);
double k22 = cache(i2,i2);
double k12 = cache(i1,i2);
double eta = k11 + k22 - 2 * k12;

// std::cout << "e1:" << e1 << " e2:" << e2 << " eta:" << eta << std::endl;

if(eta > 0.0)
  {
  double a2_new = a2 + y2 * (e1 - e2) / eta;
  // clamp inside the box
  a2_new = (a2_new >= H) ? H : (a2_new <= L) ? L : a2_new;

  if (std::abs(a2_new-a2) < epsilon*(a2_new+a2+epsilon))
   {
//      std::cout << "rej1" << std::endl;
      return false;
   }
   

  double a1_new = a1 + s * (a2 - a2_new);

  // update threshold
   double w1   = y1*(a1_new - a1);
   double w2   = y2*(a2_new - a2);
   double b1   = e1 + w1*k11 + w2*k12;
   double b2   = e2 + w1*k12 + w2*k22;
   double bold = bias;
  
    bias += 0.5*(b1 + b2);
  // update weight vector
  
//   w += y1 * (a1_new - a1) * x[i1] + y2 * (a2_new - a2) * x[i2];

  // update error cache
   /* update error cache->*/

   if (std::abs(b1-b2) < epsilon)
   {
      e[i1] = 0.0;
      e[i2] = 0.0;
   }
   else
   {
      if (a1 > 0.0 && a1 < C)
      {
         e[i1] = learnedFunc(i1) - y1;
      }

      if (a2 > 0.0 && a2 < C)
      {
         e[i2] = learnedFunc(i2) - y2;
      }
   }

   if (e[i1] > e[i2])
   {
      minimum = i2;
      maximum = i1;
   }
   else
   {
      minimum = i1;
      maximum = i2;
   }

   for (int i = 0; i < n; i++)
   {
      if (alpha[i] > 0.0 && alpha[i] < C && i != i1 && i != i2)
      {
         e[i] += w1 * cache(i1,i)
              +  w2 * cache(i2,i)
              +  bold - bias;

         if (e[i] > e[maximum])
         {
            maximum = i;
         }

         if (e[i] < e[minimum])
         {
            minimum = i;
         }
      }
   }

   
//  std::cout << "a1:" << a1_new << " a2:" << a2_new << std::endl;
  
  alpha[i1] = a1_new;
  alpha[i2] = a2_new;
  
  return true;
  }
else
// std::cout << "\teta:" << eta << std::endl;
return false;
}

bool examineNonBound(int i2)
{
   int i1 = rand() % n;

   for (int i = 0; i < n; i++)
   {
      if (alpha[i1] > 0.0 && alpha[i1] < C)
      {
         if (takeStep(i1, i2))
         {
            return true;
         }
      }
      i1 = (i1 + 1) % n;
   }

   return false;
}

bool examineBound(int i2)
{
   int i1 = rand() % n;

   for (int i = 0; i < n; i++)
   {
      if (alpha[i1] == 0.0 || alpha[i1] == C)
      {
         if (takeStep(i1, i2))
         {
            return true;
         }
      }
      i1 = (i1 + 1) % n;
   }

   return false;
}


bool examineFirstChoice(int i2)
{
   if (minimum > -1)
   {
     double e2 = e[i2];
      if (std::abs(e2 - e[minimum]) > std::abs(e2 - e[maximum]))
      {
         if (takeStep(minimum, i2))
         {
            return true;
         }
      }
      else
      {
         if (takeStep(maximum, i2))
         {
            return true;
         }
      }
   }

   return false;
}

bool examineExample(int i2)
{
  int y2 = y[i2];
  double a2 = alpha[i2];
  double e2 = e[i2];
  double r2 = e2 * y2;
  
//   std::cout << "examine " << i2 << " e:" << e2 << " r2:" << r2 << std::endl;
  
  if((r2 < - tol && a2 < C) || (r2 > tol && a2 > 0.0))
  {
    
    if(examineFirstChoice(i2))
      return true;
    
    if(examineNonBound(i2))
      return true;

    if(examineBound(i2))
      return true;

  }
 
return false; 
}
  
public:
  
svm_train(int _m) : m(_m)
  {
   n = 0;
   
   w = new double[m];
   for(int i=0;i<m;++i)
     w[i] = 0.0;
   bias = 0.0;
   
   minimum   = -1;
   maximum   = -1;   
   
   epsilon   = 1e-12;
   tol = 1e-3;
   e = 0;
  }

~svm_train()
{
   for(int i=0;i<n;++i)
     delete [] x[i];
  delete [] w;
  delete [] e;
}

void insert(int y, double *x)
{
  this->y.push_back(y);
  this->x.push_back(x);
  this->alpha.push_back(0.0);
  
  ++n;
}

void train()
{
  double examineAll = true;
  int numChanged = 0;
  
  if(n==0)
    return;
  
  delete [] e;
  e = new double [n];
  
  Functor::computeCache(n,m,x);
  
  for (int i = 0; i < n; i++)
   {
//       this->C[i] = c*z[i];

      if (alpha[i] > 0.0 && alpha[i] < C)
      {
	
         // e[i] = learnedFunc(i) - y[i];
      }
      else
      {
      // e[i] = 0.0;
      }
         e[i] = - y[i];
   }
   
  while(numChanged > 0 || examineAll)
  {
	
//     std::cout << "iter...\n";
    numChanged = 0;
    if(examineAll)
    {
      for(int i =0;i<n;++i)
	numChanged += examineExample(i) ? 1 : 0;
    }
    else
    {
      for(int i =0;i<n;++i)
	if (alpha[i] > 0 && alpha[i] < C)
	  numChanged += examineExample(i) ? 1 : 0;
    }
    
   if(examineAll)
     examineAll = false;
   else
     if(numChanged == 0)
       examineAll = true;
  }
  
  
  for(int j =0;j<m;++j)
  {
  w[j]=0.0;
    for(int i =0;i<n;++i)
      w[j] += y[i]*alpha[i] * x[i][j];
  }
  
  std::cerr << numChanged << std::endl;
  
//   int imax = 0; 
//   double amx = alpha[0];
//   
//   for(int i =0;i<n;++i)
//   if(alpha[i]>amx)
//     {
//       amx = alpha[i];
//       imax = i;
//     }
//   for(int i=0;i<m;++i)
//     std::cout << ' ' << w[i];
//   std::cout << std::endl;
//   for(int i=0;i<m;++i)
//     std::cout << ' ' << x[imax][i];
//   std::cout << std::endl;
//   std::cout <<"dit"<<dot(w, x[imax],m)<<std::endl;
  // bias = dot(w, x[imax],m) - y[imax];
}
    
};

#endif
