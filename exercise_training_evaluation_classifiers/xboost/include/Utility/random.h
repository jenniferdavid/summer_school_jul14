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

#ifndef _RANDOM_H
#define _RANDOM_H

/** @file random.h
 *  @brief some methods about random number generation */

/** return a random number between 0 and 1 */
double drand (void);

/** return 0 or 1 with probability according to x */
int randbiased (double x);

/** extract a random slot according to slot PDF distribution.
 \code
  double test[n];
  test[0] = 0.25;
  test[1] = 0.5;
  test[2] = 0.75;
  test[3] = 1.0;
  
  int slot = randslot(test, 4);
 \endcode
 */
int randslot (const double *slots, int n);

int nrand(int n);

#endif