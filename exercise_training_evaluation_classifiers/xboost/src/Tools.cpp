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

#include "HaarFeatureTools.h"
#include <cstring>
#include <fstream>
#include <cmath>

void DumpFeatureImg(const char *filename, const HaarFeature & out, int w, int h)
{
  int *b = new int [w*h];
  unsigned char *img = new unsigned char [w*h];
  memset(b,0,sizeof(int)*w*h);
  for(std::vector<HaarNode>::const_iterator i = out.begin(); i!= out.end(); ++i)
      {
	for(int y=0;y<i->y;y++)
	 for(int x=0;x<i->x;x++)
	  b[x+y*w]+=i->sign;
      }

  for(unsigned int y=0;y<h;y++)
   for(unsigned int x=0;x<w;x++)
   {
     int value =  127 + b[x+y*w]*126; // 1 shade to detect overflow
     if(value > 255) value = 255;
     if(value < 0) value = 0;
     img[x+y*w] = value;
   }

  std::ofstream outf(filename);
  outf << "P5\n" << w << ' ' << h << "\n255\n";
  outf.write(reinterpret_cast<const char *>(img), w*h);

  delete [] img;
  delete [] b;
}


void AddStochasticImg(double *img, double alpha, const HaarFeature & out, int w, int h)
{
  for(std::vector<HaarNode>::const_iterator i = out.begin(); i!= out.end(); ++i)
      {
// 	std::cout << i->x << 'x' << i->y << std::endl;
	for(int y=0;y<i->y;y++)
	 for(int x=0;x<i->x;x++)
	  img[x+y*w]+=alpha * i->sign;
      }
}

#ifdef _MSC_VER
# define round( x )              floor((x) + 0.5)
#endif

void DumpStochasticImg(const char *filename, const double *b, int w, int h)
{
  unsigned char *img = new unsigned char [w*h];
  double mx;

  mx = fabs(b[0]);

  for(unsigned int i=0;i<w*h;i++)
     if(fabs(b[i])>mx)
	mx = fabs(b[i]);
  for(unsigned int i=0;i<w*h;i++)
      img[i] = 127 + round( (b[i]*127.0)/mx );

  std::ofstream out(filename);
  out << "P5\n" << w << ' ' << h << "\n255\n";
  out.write(reinterpret_cast<const char *>(img), w*h);

  delete [] img;
}
