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

#include "IO/pnmio.h"
#include <iostream>
#include <fstream>

static bool is_gz(const char *filename)
{
  int len = strlen(filename);
  if(len>3)
  {
    if(!memcmp(filename + len - 3, ".gz", 3))
      return true;
    else
       return false;
  }
  else
    return false;
}

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

bool pnm_load(const char *file, Image & out)
{
  std::ifstream istr(file);
  if(!istr)
    return false;
  unsigned int max;

  out.release();
  
  char header = pnm_read_header(out.width, out.height,max, istr);

  if(header =='5')
  {

  if(max < 256)
  {
  // buffer size
  unsigned char *buffer = new unsigned char [out.width * out.height];
  istr.read(reinterpret_cast<char *>(buffer),  out.width * out.height);
  out.bpp = 1;
  out.stride = out.width;
  out.data = buffer;
  return true;
  }
  else
  {
  // buffer size
  unsigned char *buffer = new unsigned char [out.width * out.height * 2];
  istr.read(reinterpret_cast<char *>(buffer),  out.width * out.height * 2);
  // BYTESWAP
  for(int i =0;i<out.width * out.height * 2;i+=2)
  {
    // BIG_ENDIAN -> LITTLE_ENDIAN
    std::swap(buffer[i], buffer[i+1]);
  }
  out.bpp = 2;
  out.stride = out.width * 2;
  out.data = buffer;
  return true;
  }

  }
  else
  if(header=='6')
  {
  unsigned char *buffer = new unsigned char [out.width * out.height*3];

  istr.read(reinterpret_cast<char *>(buffer),  out.width * out.height*3);
  out.data =buffer;
  out.stride = out.width * 3;
  out.bpp = 3;
  }
  else
    return false;

}

unsigned char *pgm_load(unsigned int & width, unsigned int &height, unsigned int &max, const char *file)
{
  if(is_gz(file))
  {
    std::cerr << "gzipped file: not implemented yet" << std::endl;
    return 0;
  }
  else
  {
  
  std::ifstream istr(file);
  if(pnm_read_header(width,height,max, istr)!='5')
    return NULL;
  
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
  // BYTESWAP
  for(int i =0;i<width * height * 2;i+=2)
  {
    // BIG_ENDIAN -> LITTLE_ENDIAN
    std::swap(buffer[i], buffer[i+1]);
  }
  return buffer;
  }

  }
  
}

unsigned char *ppm_load(unsigned int & width, unsigned int &height, unsigned int &max, const char *file)
{
  if(is_gz(file))
  {
    std::cerr << "gzipped file: not implemented yet" << std::endl;
    return 0;
  }
  else
  {
  std::ifstream istr(file);
  if(pnm_read_header(width,height,max, istr)!='6')
    return NULL;
  // buffer size
  unsigned char *buffer = new unsigned char [width * height*3];

  istr.read(reinterpret_cast<char *>(buffer),  width * height*3);

  return buffer;
  }
}


void pgm_write(const char *filename, const unsigned char *buffer, unsigned int width, unsigned int height, unsigned int max)
{
    std::ofstream out(filename);
    out << "P5\n" << width << ' ' << height << ' ' << max << '\n';
    if(max>255)
    {
    unsigned char *tmp = new unsigned char [width * height * 2];
    // BYTESWAP
    for(int i =0;i<width * height * 2;i+=2)
    {
      // BIG_ENDIAN -> LITTLE_ENDIAN
      tmp[i] = buffer[i+1];
      tmp[i+1] = buffer[i];
    }
    
    out.write((const char *)tmp, width*height*2);
    }
    else
    {
    out.write((const char *)buffer, width*height);      
    }
	  
}

void pgm_write(const char *filename, const unsigned char *buffer, unsigned int width, unsigned int height, unsigned int max,long stride)
{
    std::ofstream out(filename);
    out << "P5\n" << width << ' ' << height << ' ' << max << '\n';
    if(max>255)
    {
    unsigned char *tmp = new unsigned char [width * height * 2];
    // BYTESWAP
    for(int i =0;i<width * height * 2;i+=2)
    {
      // BIG_ENDIAN -> LITTLE_ENDIAN
      tmp[i] = buffer[i+1];
      tmp[i+1] = buffer[i];
    }

    //out.write((const char *)tmp, width*height*2);
	for(int r=0;r<height;r++, tmp+=stride)
		out.write((const char *)tmp,width*2);

    }
    else
    {
    //out.write((const char *)buffer, width*height);
    	for(int r=0;r<height;r++, buffer+=stride)
    	{
    		//std::cout << "r: "<<r << std::endl;
    		out.write((const char *)buffer,width);
    	}
    }

}

bool ppm_write(const char *filename, const unsigned char *buf, unsigned int width, unsigned int height)
{
		std::ofstream out(filename, std::ios::out | std::ios::binary);
		if(out)
		{
			out << "P6\n" << width << ' ' << height << "\n255\n";
			out.write(reinterpret_cast<const char *>(buf), width*height*3);
			return true;
		}
		else
			return false;


}


bool pnm_write(const ImageHandle & in, const char *file)
{
  if(in.bpp==1)
    pgm_write(file, in.data, in.width, in.height, 255, in.stride);
  if(in.bpp==3)
    ppm_write(file, in.data, in.width, in.height);
  return true;
}
