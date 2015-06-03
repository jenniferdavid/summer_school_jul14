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

#ifndef _INFORMATION_METRICS_H
#define _INFORMATION_METRICS_H

/** @file InformationMetrics.h
 *  @brief some metrics used when train decision trees */

// Metriche usate nella teoria dell'informazione
// TODO: unused now

struct Entropy { };

struct GiniIndex { };

struct ClassificationError { };

/// compute entropy of n-classes distribution
double entropy(const double *p, int n)
{
double acc=0.0;
static const double norm_factor = 1.0/log(2.0);
for(int i =0;i<n;++i)
 acc+= p[i] * log(p[i]);
return -acc * norm_factor;
}

/// Entropy in binary case (p0=p, p1=1.0-p)
double entropy(double p)
{
static const double norm_factor = 1.0/log(2.0);
return -(p*log(p)+(1.0-p)*log(1.0-p)) * norm_factor;
}

/// gini index in a n-classes distributions
double gini_index(const double *p, int n)
{
double acc=0.0;
for(int i =0;i<n;++i)
 acc+= p[i]*p[i];
return 1.0 - acc;
}

/// Gini Index in binary case (p0=p, p1=1.0-p)
double gini_index(double p)
{
// return -2*p*p + 2*p ;
return 2*p*(1-p);
}

/// classification error in n-classes distribution
double classification_error(const double *p, int n)
{
double acc=0.0;
for(int i =0;i<n;++i)
 if(p[i]>acc) acc = p[i];
return 1.0 - acc;
}

/// classification error in binary case
double classification_error(double p)
{
//	return 1.0 -std::max(p,1.0-p);
	return (p<0.5) ? (1.0-p) : p;
}


#endif