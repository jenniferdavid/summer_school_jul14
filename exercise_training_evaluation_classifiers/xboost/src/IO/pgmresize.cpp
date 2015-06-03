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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "boxfilter.h"

inline unsigned char SwapEndian(unsigned char c) { return c;}
inline unsigned short SwapEndian(unsigned short c) { return ((c&0xFF00)>>8) | ((c&0x00FF)<<8);}


static void pnm_read_comments(std::istream & i)
{
 while (isspace(i.peek()))
    {
    while (isspace(i.peek()))
        i.get();
    if (i.peek() == '#')
      while (i.peek()!='\r' && i.peek()!='\n')
           { i.get(); }
       }
}

static char pnm_read_header(unsigned int & width, unsigned int &height, unsigned int & max_val, std::istream &iss)
{
  char h, t; 

  // check pnm header
  h = iss.get();
  t = iss.get();
  if(!((h=='P') && ((t=='5')||(t=='6'))))
    return '\0';

  pnm_read_comments(iss);
  iss>>width;
  pnm_read_comments(iss);
  iss>>height;
  pnm_read_comments(iss);
  iss>>max_val;
  iss.get(); // Eat the last whitespace (TODO: getline)
  return t;
}

unsigned char *pnm_load(unsigned int & width, unsigned int &height, unsigned int &max, const char *file, int *format)
{
  std::ifstream istr(file);
  char id = pnm_read_header(width,height,max, istr);
  
  if(id=='6')
    {
    if(format)
      *format = 3;
    unsigned char *buffer = new unsigned char [width * height*3];
    istr.read(reinterpret_cast<char *>(buffer),  width * height*3);
    return buffer;
    }
  
  if(id!='5')
  {
    if(format)
      *format = 0;
    
    return NULL;
  }

    if(format)
      *format = 1;
  
  if(max < 256)
  {
  // buffer size
  unsigned char *buffer = new unsigned char [width * height];
  istr.read(reinterpret_cast<char *>(buffer),  width * height);
  return buffer;
  }
  else
  {
  // buffer size
  unsigned char *buffer = new unsigned char [width * height * 2];
  istr.read(reinterpret_cast<char *>(buffer),  width * height * 2);
  for(int i =0;i<width * height * 2;i+=2)
  {
    // BIG_ENDIAN -> LITTLE_ENDIAN
    std::swap(buffer[i], buffer[i+1]);
  }
  return buffer;
  }
}

unsigned char *ppm_load(unsigned int & width, unsigned int &height, unsigned int &max, const char *file)
{
  std::ifstream istr(file);
  if(pnm_read_header(width,height,max,istr)!='6')
    return NULL;
  // buffer size
  unsigned char *buffer = new unsigned char [width * height*3];

  istr.read(reinterpret_cast<char *>(buffer),  width * height*3);

  return buffer;
}

template<class T>
void pgm_write(const char *filename, unsigned int width, unsigned int height, unsigned int max, const T *img)
{
std::ofstream out(filename);
out << "P5\n" << width << ' ' << height << ' ' << max << '\n';
out.write((const char *)img, width*height*sizeof(T));
}

template<class T>
void downsample(const T *src, int width, int height, T *dst, int dwidth, int dheight, int xscale, int yscale)
{
  std::cout << "downsample " << width << 'x' << height << " in " << dwidth << 'x' << dheight << " scale " << xscale <<',' << yscale << std::endl;
  
  for(int j=0;j<dheight;j++)
   for(int i=0;i<dwidth;i++)
   {
     unsigned int accum  = 0;
     unsigned int norm = 0;
     for(int m = 0;m<yscale;m++)
      for(int n = 0;n<xscale;n++)
      {
	if((n+i*xscale) < width && (m + j * yscale) < height)
	{
	accum += src[i*xscale + n + (m + j*yscale) * width];
	norm++;
	}
      }
      
      dst[i + j * dwidth] = (norm > 0) ? (accum / norm) : 0;
   }
}

template<class T>
void downsample_nearest(const T *src, int width, int height, T *dst, int dwidth, int dheight)
{
  float xscale = (float) width / (float) dwidth;
  float yscale = (float) height / (float) dheight;

  std::cout << "downsample " << width << 'x' << height << " in " << dwidth << 'x' << dheight << " scale " << xscale <<',' << yscale << std::endl;
  
  for(int j=0;j<dheight;j++)
   for(int i=0;i<dwidth;i++)
   {
     unsigned int accum  = 0;
     unsigned int norm = 0;
     int i0 = round( i * xscale );
     int i1 = round( (i+1) * xscale );
     int j0 = round( j * yscale );
     int j1 = round( (j+1) * yscale );
     
     for(int m = j0;m<j1;m++)
      for(int n = i0;n<i1;n++)
      {
	if(i0 < width && j0 < height)
	{
	accum += src[n + m * width];
	norm++;
	}
      }
      
      dst[i + j * dwidth] = (norm > 0) ? (accum / norm) : 0;
   }
}

template<class T>
void downsample_interpol(const T *src, int width, int height, T *dst, int dwidth, int dheight)
{
  /*
  float xscale = (float) dwidth / (float) width;
  float yscale = (float) dheight / (float) height;
  int ixscale = (int) xscale;
  int iyscale = (int) yscale;
  
  
  for(int j=0;j<dheight;j++)
   for(int i=0;i<dwidth;i++)
   {
     unsigned int accum  = 0;
     unsigned int norm = 0;
     for(int m = 0;m<yscale;m++)
      for(int n = 0;n<xscale;n++)
      {
	if((n+i*xscale) < width && (m + j * yscale) < height)
	{
	accum += src[i*xscale + n + (m + j*yscale) * width];
	norm++;
	}
      }
      
      dst[i + j * dwidth] = (norm > 0) ? (accum / norm) : 0;
   }
   */
}

template<class T>
void best_resize(const T *src, int src_width, int src_height, T *dst, int dst_width, int dst_height)
{
  float xscale = (float) src_width / (float) dst_width;
  float yscale = (float) src_height / (float) dst_height;
  
  int ax = (int) trunc(xscale);
  int ay = (int) trunc(yscale);
  
  if(ax < 1)
    ax=1;
  if(ay < 1)
    ay=1;
  
  float dx = xscale - ax;
  float dy = yscale - ay;
  
  int bx = ax + 1;
  int by = ay + 1;

  if(bx < 1)
    bx=1;
  if(by < 1)
    by=1;
  
  int wa = (src_width + ax - 1) / ax;
  int ha = (src_height + ay - 1) / ay;
  int wb = (src_width + bx - 1) / bx;
  int hb = (src_height + by - 1) / by;
  
  T * pa = new T[ wa * ha ];
  T * pb = new T[ wb * hb ];
  
  std::cout << "resample " << src_width << 'x' << src_height << " in:\n";
  
  std::cout << "A:" << wa << 'x' << ha << std::endl;
  std::cout << "B:" << wb << 'x' << hb << std::endl;
  
  downsample(src, src_width, src_height, pa, wa, ha, ax, ay);
  downsample(src, src_width, src_height, pb, wb, hb, bx, by);
  
  float wax = (float) wa / (float) dst_width;
  float way = (float) ha / (float) dst_height;
  float wbx = (float) wb / (float) dst_width;
  float wby = (float) hb / (float) dst_height;
  
  std::cout << "a:" <<wax << ',' << way << std::endl;
  std::cout << "b:" <<wbx << ',' << wby << std::endl;
  
  std::cout << "i:" << dx << ',' << dy << std::endl;
  
   for(int j=0;j<dst_height;j++)
    for(int i=0;i<dst_width;i++)
   {
     T a = pa[(int)round(wax * i) + (int)round(way * j) * wa];
     T b = pb[(int)round(wbx * i) + (int)round(wby * j) * wb];
     
     dst[i + j * dst_width] = a * (1.0 - dx) + b * dx;
     // dst[i + j * dst_width] = a;
   }
  
  
  delete [] pa;
  delete [] pb;
}

template<class T>
void trilinear_filtering(const T *src, int src_width, int src_height, T *dst, int dst_width, int dst_height)
{
  float xscale = (float) src_width / (float) dst_width;
  float yscale = (float) src_height / (float) dst_height;
  
  float x2 = log(xscale) / log(2.0);
  float y2 = log(xscale) / log(2.0);
  // potenza di 2 inferiore
  int ax = 1 << ( (int) x2 );
  int ay = 1 << ( (int) y2 );
  
  if(ax < 1)
    ax=1;
  if(ay < 1)
    ay=1;
  
  
  float dx = x2 - (int)(x2);
  float dy = y2 - (int)(y2);
  
  int bx = ax*2;
  int by = ay*2;

  if(bx < 1)
    bx=1;
  if(by < 1)
    by=1;
  
  int wa = (src_width + ax - 1) / ax;
  int ha = (src_height + ay - 1) / ay;
  int wb = (src_width + bx - 1) / bx;
  int hb = (src_height + by - 1) / by;
  
  T * pa = new T[ wa * ha ];
  T * pb = new T[ wb * hb ];
  
  std::cout << "resample " << src_width << 'x' << src_height << " in:\n";
  
  std::cout << "A:" << wa << 'x' << ha << std::endl;
  std::cout << "B:" << wb << 'x' << hb << std::endl;
  
  downsample(src, src_width, src_height, pa, wa, ha, ax, ay);
  downsample(src, src_width, src_height, pb, wb, hb, bx, by);
  
  float wax = (float) wa / (float) dst_width;
  float way = (float) ha / (float) dst_height;
  float wbx = (float) wb / (float) dst_width;
  float wby = (float) hb / (float) dst_height;
  
  std::cout << "a:" <<wax << ',' << way << std::endl;
  std::cout << "b:" <<wbx << ',' << wby << std::endl;
  
  std::cout << "i:" << dx << ',' << dy << std::endl;
  
   for(int j=0;j<dst_height;j++)
    for(int i=0;i<dst_width;i++)
   {
     T a, b;
     {
     int ix = wax * i;
     int iy = way * j;
     float dx = (wax * i) - ix;
     float dy = (way * j) - iy;
     a =  (pa[ix + iy * wa]) * (1.0 - dx) * (1.0 - dy) + 
     (pa[(ix+1) + iy * wa]) * dx * (1.0 - dy) + 
     (pa[ix + (iy+1) * wa]) * (1.0 - dx) * dy +
     (pa[(ix+1) + (iy+1) * wa]) * dx * dy;
     }
     {
     int ix = wbx * i;
     int iy = wby * j;
     float dx = (wbx * i) - ix;
     float dy = (wby * j) - iy;
     b =  (pb[ix + iy * wb]) * (1.0 - dx) * (1.0 - dy) + 
     (pb[(ix+1) + iy * wb]) * dx * (1.0 - dy) + 
     (pb[ix + (iy+1) * wb]) * (1.0 - dx) * dy +
     (pb[(ix+1) + (iy+1) * wb]) * dx * dy;
     }
  
  
      dst[i + j * dst_width] = a * (1.0 - dx) + b * dx;
     
     /*
     T a = pa[(int)round(wax * i) + (int)round(way * j) * wa];
     T b = pb[(int)round(wbx * i) + (int)round(wby * j) * wb];
     
     dst[i + j * dst_width] = a * (1.0 - dx) + b * dx;
     */
     // dst[i + j * dst_width] = a;
   }
  
  
  delete [] pa;
  delete [] pb;
}

template<class T>
void nearest(const T *src, int src_width, int src_height, T *dst, int dst_width, int dst_height)
{
  float xscale = (float) src_width / (float) dst_width;
  float yscale = (float) src_height / (float) dst_height;
  
   for(int j=0;j<dst_height;j++)
    for(int i=0;i<dst_width;i++)
     dst[i + j * dst_width] =  src[(int)round(xscale* i) + (int)round(yscale * j) * src_width];
}

template<class T>
void bilinear(const T *src, int src_width, int src_height, T *dst, int dst_width, int dst_height)
{
  float xscale = (float) src_width / (float) dst_width;
  float yscale = (float) src_height / (float) dst_height;
  
   for(int j=0;j<dst_height;j++)
    for(int i=0;i<dst_width;i++)
    {
     int ix = xscale * i;
     int iy = yscale * j;
     float dx = (xscale * i) - ix;
     float dy = (yscale * j) - iy;
     dst[i + j * dst_width] =  (src[ix + iy * src_width]) * (1.0 - dx) * (1.0 - dy) + 
     (src[(ix+1) + iy * src_width]) * dx * (1.0 - dy) + 
     (src[ix + (iy+1) * src_width]) * (1.0 - dx) * dy +
     (src[(ix+1) + (iy+1) * src_width]) * dx * dy;
    }
}

template<class T>
void resize(const T *src, int src_width, int src_height, T *dst, int dst_width, int dst_height, int algo)
{
  if(src_width == dst_width && src_height == dst_height)
  {
    memcpy(dst, src, src_width * src_height * sizeof(T));
    return;
  }
  
  switch(algo)
  {
    case 0:
      best_resize(src, src_width, src_height, dst, dst_width, dst_height);
      break;
    case 1:
      nearest(src, src_width, src_height, dst, dst_width, dst_height);
      break;
    case 2:
      bilinear(src, src_width, src_height, dst, dst_width, dst_height);
      break;
    case 3:
      trilinear_filtering(src, src_width, src_height, dst, dst_width, dst_height);
      break;
    case 4:
      downsample(src, src_width, src_height, dst, dst_width, dst_height, (src_width + dst_width -1)/ dst_width, (src_height + dst_height -1) / dst_height);
      break;  
    case 5:
      downsample_nearest(src, src_width, src_height, dst, dst_width, dst_height);
      break;
    case 6:
      boxfilter(src, src_width, src_height, dst, dst_width, dst_height);
      break;
    
  }
}


void help()
{
     std::cout << "pgmresize <input image> <out width> <out height> <out image> [options]\n\n";
     std::cout << "-a <algo>\n\tSet the resampling algorithm\n";
     std::cout << "-w r,g,b\n\tSet the RGBtoGrey weights\n";
     std::cout << "-v\n\tVerbose\n";
     
     std::cout.flush();
}


const char *input_image = 0;
const char *output_image = 0;
int out_width = -1;
int out_height = -1;
int algo = 0;
float r = 1.0/3.0;
float g = 1.0/3.0;
float b = 1.0/3.0;
bool verbose = false;
int split_channels = 0;
int min_width = 0;
bool avoid_upsample = false;

void parse_cmd_line(int argc, char *argv[])
{
  // Parse Command Line
  if(argc < 3)
    {
    help();
    exit(0);
    }

  int i = 1;
  while(i<argc)
  {
    const char *str = argv[i];
    if(str[0]=='-')
    {
      
      if(!strcmp(str,"--no-upsample"))
      {
	avoid_upsample = true;
      }
      else
      if(!strcmp(str,"--split-rgb"))
      {
	split_channels = 1;
      }
      else
      if(!strcmp(str,"--split-cccr"))
      {
	split_channels = 2;
      }
      else
      if(!strcmp(str,"-v"))
      {
	verbose = true;
      }
      else
      if(!strcmp(str,"-a"))
      {
	++i;
	algo =atoi (argv[i]);
      }
      else
      if(!strcmp(str,"--min-width"))
      {
        ++i;
        min_width =atoi (argv[i]);
      }
      else
      if(!strcmp(str,"-w"))
      {
	++i;
	r = atof(argv[i]);
	++i;
	g = atof(argv[i]);
	++i;
	b = atof(argv[i]);
      }
      else
      {
	help();
	exit(1);
      }
    }
    else
    {
      if(input_image == 0)
	input_image = str;
      else
	if(out_width == -1)
	out_width = atoi(str);
      else
	if(out_height == -1)
	out_height = atoi(str);
      else
	if(output_image == 0)
	  output_image = str;
	else
	{
	  help();
	  exit(1);
	}
    }
    
   ++i;
  }
}

void rgb_to_gray(unsigned char *out, const unsigned char *p, unsigned int width, unsigned int height)
{
    for(int i =0;i<width*height;++i)
    {
      float v = p[i*3+0]*r + p[i*3+1]*g + p[i*3+2]*b;
      out[i] = (v<0.0f) ? 0 : (v>255.0f) ? 255 : (int)trunc(v+0.5f);
    }
}

void rgb_channel_to_gray(unsigned char *out, const unsigned char *p, unsigned int width, unsigned int height, int idx)
{
    for(int i =0;i<width*height;++i)
      out[i] = p[i*3+idx];
}

int main(int argc, char *argv[])
{
  unsigned int width, height, max;
  int format = -1;
  
  parse_cmd_line(argc, argv);
  
  if(output_image == 0)
  {
    help();
    return 1;
  }
  
  unsigned char *p = pnm_load(width, height, max, input_image, &format);
  unsigned int target_width = out_width;
  unsigned int target_height = out_height;
  
  if(verbose)
  {
    std::cout << input_image << ": " << width << 'x' << height << 'x' << max << " -> " << output_image << ": " << target_width <<'x'<<target_height << std::endl;
  }

  if(avoid_upsample && ((width < target_width) || (height < target_height) ) )
  {
    std::cout << "[Avoid Upsample] skip | size = " << width << 'x' << height << " | target = " << target_width << 'x' << target_height << std::endl;
    return -1;
  }
  
  if(width < min_width)
  {
    std::cout << "[Min Width] skip | size = " << width << 'x' << height << " | min width = " << min_width << std::endl;
    return -1;
  }
  
  
  if(format == 3)
  {
    if(split_channels==1)
    {
    unsigned char *dst = new unsigned char [target_width * target_height * 3];
    unsigned char *out = new unsigned char [width * height];
    
    rgb_channel_to_gray(out, p, width, height, 0);
    resize(out, width, height, dst, target_width, target_height, algo);
    rgb_channel_to_gray(out, p, width, height, 1);
    resize(out, width, height, dst + target_width*target_height, target_width, target_height, algo);
    rgb_channel_to_gray(out, p, width, height, 2);
    resize(out, width, height, dst + target_width*target_height*2, target_width, target_height, algo);
    
    pgm_write(output_image, target_width, target_height*3, max, dst);
    delete [] out;
    delete [] dst;
    delete [] p;
    return 0;
    }
    else
    if(split_channels==2)
    {
    unsigned char *dst = new unsigned char [target_width * target_height * 2];
    unsigned char *out = new unsigned char [width * height];
    
    rgb_to_gray(out, p, width, height);
    resize(out, width, height, dst, target_width, target_height, algo);
    rgb_channel_to_gray(out, p, width, height, 0); // RED
    resize(out, width, height, dst + target_width*target_height, target_width, target_height, algo);
    
    pgm_write(output_image, target_width, target_height*2, max, dst);
    delete [] out;
    delete [] dst;
    delete [] p;
    return 0;
    }
    else
    {
    unsigned char *out = new unsigned char [width * height];
    rgb_to_gray(out, p, width, height);
    std::swap(p,out);
    
    delete [] out;
    }
  }
    
  if(max<256)
  {
    unsigned char *dst = new unsigned char [target_width * target_height];

    resize(p, width, height, dst, target_width, target_height, algo);
    
    pgm_write(output_image, target_width, target_height, max, dst);
    delete [] dst;
  }
  else
  {
    unsigned short *dst = new unsigned short [target_width * target_height];
    
    resize((unsigned short *)p, width, height, dst, target_width, target_height, algo);
    
     for(int i =0;i<target_width*target_height;++i)
	  {
	    dst[i] = SwapEndian(dst[i]);
	  }
	  
    pgm_write(output_image, target_width, target_height, max, dst);
    delete [] dst;
  }
  
  delete [] p;
return 0;
}
