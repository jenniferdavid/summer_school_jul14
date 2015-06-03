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

/* Part of this file is Copyright 2004-2008 by Paul Hsieh  */

#include "Utility/random.h"

#include <stdlib.h>

#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

/* A non-overflowing average function */
#define average2scomplement(x,y) ((x) & (y)) + (((x) ^ (y))/2)

int nrand(int n)
{
  return
  (n>RAND_MAX) ?
    (( rand() * RAND_MAX + rand() ) % n) :
    (rand() % n);
}


double drand (void) {
    double d;
    do {
       d = (((rand () * RS_SCALE) + rand ()) * RS_SCALE + rand ()) * RS_SCALE;
    } while (d >= 1); /* Round off */
    return d;
}

int randbiased (double x) {
    for (;;) {
        double p = rand () * RS_SCALE;
        if (p >= x) return 0;
        if (p+RS_SCALE <= x) return 1;
        /* p < x < p+RS_SCALE */
        x = (x - p) * (1.0 + RAND_MAX);
    }
}

int randslot (const double *slots, int n) {
    double xhi;

    /* Select a random range [x,x+RS_SCALE) */
    double x = rand () * RS_SCALE;

    /* Perform binary search to find the intersecting slot */
    int hi = n-2, lo = 0, mi, li;
    while (hi > lo) {
        mi = average2scomplement (lo, hi);
        if (x >= slots[mi]) lo = mi+1; else hi = mi;
    }

    /* Taking slots[-1]=0.0, this is now true: slots[lo-1] <= x < slots[lo] */

    /* If slots[lo-1] <= x < x+RS_SCALE <= slots[lo] then
       any point in [x,x+RS_SCALE) is in [slots[lo-1],slots[lo]) */

    if ((xhi = x+RS_SCALE) <= slots[lo]) return lo;

    /* Otherwise x < slots[lo] < x+RS_SCALE */

    for (;;) {
        /* x < slots[lo] < xhi */
        if (randbiased ((slots[lo] - x) / (xhi - x))) return lo;
        x = slots[lo];
        lo++;
        if (lo >= n-1) return n-1;

        if (xhi <= slots[lo]) {
            /* slots[lo-1] = x <= xhi <= slots[lo] */
            return lo;
        }
    }
}
