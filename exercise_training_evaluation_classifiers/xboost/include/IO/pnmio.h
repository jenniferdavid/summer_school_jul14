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

#ifndef _PNM_IO_H
#define _PNM_IO_H

/** @file pnmio.h
  * @brief PNM files I/O
  **/

#include "Image.h"

/** Load a PGM file 
  * @return a pointer to image buffer, who have to be freed with delete []
  **/
unsigned char *pgm_load(unsigned int & width, unsigned int &height, unsigned int &max, const char *file);

/** Load a PPM file 
 * @return a pointer to image buffer, who have to be freed with delete []
 **/
unsigned char *ppm_load(unsigned int & width, unsigned int &height, unsigned int &max, const char *file);

/** Dump PGM image */
void pgm_write(const char *filename, const unsigned char *buffer, unsigned int width, unsigned int height, unsigned int max);
void pgm_write(const char *filename, const unsigned char *buffer, unsigned int width, unsigned int height, unsigned int max,long stride); // TODO
/** Dump a PPM image */
bool ppm_write(const char *filename, const unsigned char *buf, unsigned int width, unsigned int height);

/** load a PGM/PPM image in an Image structure */
bool pnm_load(const char *file, Image & out);
/** save an image as PPM/PGM on disk */
bool pnm_write(const ImageHandle & in, const char *file);

#endif
