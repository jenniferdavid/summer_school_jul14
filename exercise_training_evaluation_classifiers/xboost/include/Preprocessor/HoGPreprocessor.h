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

#ifndef _HOG_PREPROCS_H
#define _HOG_PREPROCS_H

#include "Descriptor/RawData.h"
#include <cmath> // M_PI
#include <iostream> // debug

struct  HoGParams {
  /// dimensione del descrittore
  unsigned int desc_size;
  
  unsigned int width, height;
  
public:
  
  inline bool operator == (const HoGParams & b) const 
  {
    return desc_size == b.desc_size;
  }
};

/// @brief Policy to convert an Image in a HoG descriptor
/// Input: BW Image | Output: RawData n scalar
class HoGPreprocessor {
  unsigned int cell_width, cell_height;
  unsigned int block_width, block_height;
  unsigned int h_step, v_step;
  unsigned int n_bins;
  bool sign; 
public:
  /// numero di celle per blocco
  unsigned int n_cells_per_block;

public:
  
  typedef RawData<double> DataType;
  
  /// Common data provided by this preprocessor
  typedef HoGParams ParamType;

  /// Data provided by this preprocessor
  typedef std::pair<DataType, ParamType> ReturnType;
    
public:
  HoGPreprocessor() : cell_width(8), cell_height(8), block_width(3), block_height(3), h_step(8), v_step(8), n_bins(8), sign(false) { }
  
  void SetCellGeometry(unsigned int width, unsigned int height) { cell_width = width; cell_height = height; }
  void SetBlockGeometry(unsigned int width, unsigned int height) { block_width = width; block_height = height;  n_cells_per_block = block_width * block_height; }
  void SetHystogramBins(int n) { n_bins = n; }
  void SetBlockStep(int h, int v) { h_step = h; v_step = v; }
  void EnableSign(bool s) { sign = s; }
  
  void Configure(std::istream & in) { }
  
  void Configure(const char* str)
    {
    }
    
  void debug_geometry(unsigned int width, unsigned int height) const
  {
  static const unsigned int filter_clearance = 1;     // derivative filter 
  
  unsigned int block_real_width = cell_width * block_width;
  unsigned int block_real_height = cell_height * block_height;
  
   std::cerr << "Cell Geometry: " << cell_width << 'x' << cell_height << " | ";
   std::cerr << "Block Geometry: " << block_real_width << 'x' << block_real_height << " | ";
   std::cerr << "Step: " << h_step << 'x' << v_step << " | ";
  
  /// numero di blocchi per colonna
  unsigned int rows, cols;  
  
  // numero di blocchi per colonna
  cols = 1 + (width - 2*filter_clearance - block_real_width) / h_step;
  rows = 1 + (height - 2*filter_clearance - block_real_height) / v_step;
  
   std::cerr << "Blocks: " << cols << 'x' << rows << " | ";
   std::cerr << "Cells per Block: " << n_cells_per_block << " | ";
   std::cerr << "area: (" << filter_clearance << "," << filter_clearance<<") - (" 
         << filter_clearance + (cols - 1) * h_step + block_real_width << ',' << filter_clearance + (rows - 1) * v_step + block_real_height  << ")" << " | ";
  
  int desc_size = rows * cols * n_cells_per_block * n_bins;
  
   std::cerr << "Descriptor size: " << desc_size << " elements" << std::endl;    
  }
    
template<class T>
bool Process(ReturnType & out, const T *image, unsigned int width, unsigned int height, long stride) const
{
  static const unsigned int filter_clearance = 1;     // derivative filter 
  
  unsigned int block_real_width = cell_width * block_width;
  unsigned int block_real_height = cell_height * block_height;
  
//   std::cerr << "Cell Geometry: " << cell_width << 'x' << cell_height << " | ";
//   std::cerr << "Block Geometry: " << block_real_width << 'x' << block_real_height << " | ";
//   std::cerr << "Step: " << h_step << 'x' << v_step << " | ";
  
  /// numero di blocchi per colonna
  int rows, cols;  
  
  // numero di blocchi per colonna
  cols = 1 + (width - 2*filter_clearance - block_real_width) / h_step;
  rows = 1 + (height - 2*filter_clearance - block_real_height) / v_step;
  
  if(rows<1 || cols<1)
  {
    std::cerr <<"invalid input image:" << width << 'x' << height << std::endl;
    return false;
  }
  
//   std::cerr << "Blocks: " << cols << 'x' << rows << " | ";
//   std::cerr << "Cells per Block: " << n_cells_per_block << " | ";
//   std::cerr << "area: (" << filter_clearance << "," << filter_clearance<<") - (" 
//         << filter_clearance + (cols - 1) * h_step + block_real_width << ',' << filter_clearance + (rows - 1) * v_step + block_real_height  << ")" << " | ";
  
  int desc_size = rows * cols * n_cells_per_block * n_bins;
  
//   std::cerr << "Descriptor size: " << desc_size << " elements" << std::endl;
  
  // precalcola il fattore di conversione da angolo a BIN
  // TODO: generare LUT
  double phase_factor = (sign) ? ((double)n_bins / (2.0*M_PI) ) : ((double)n_bins / (M_PI) );
  
  // riserva il descrittore per HOG
  double *hog = new double[desc_size];
  for(int l=0;l<desc_size;++l)
    hog[l] = 0.0;

  // compute derivate
  
  // calcolo modulo e fase
  float *module = new float [width * height];
  int *bin = new int [width * height];
  
  for(int j=filter_clearance;j<height-filter_clearance;++j)
  {
   int kin  = j * stride;
   int kout = j * width;
   for(int i=filter_clearance;i<width-filter_clearance;++i)
   {
    int dx = (int) image[kin+i+1]      - (int) image[kin+i-1];
    int dy = (int) image[kin+i+stride] - (int) image[kin+i-stride];
    
    module[kout + i] = std::sqrt(dx*dx+dy*dy);
    if (dx!=0 || dy!=0) {
      double phase = std::atan2(dy, dx) + M_PI; // [0, +2pi]
      bin[kout + i] = ((int)(phase * phase_factor)) % n_bins; 
    }
    else
    {
      bin[kout + i]=0;
    }
   }
  }
   
  // TODO: compute integral image to boost the HOG computation
    
    
  // compute Hogs
  
  for(int j=0;j<rows;++j)
    for(int i=0;i<cols;++i)
    {
      const int h0 =  (i + j * cols) * n_cells_per_block*n_bins;
      double factor = 0.0;
      
      for(int j1 = 0;j1<block_height;++j1)
       for(int i1 = 0;i1<block_width;++i1)
        {
        int x0 = filter_clearance + i * h_step + i1 * cell_width;
        int y0 = filter_clearance + j * v_step + j1 * cell_height;
        int h = h0 + (i1 + j1 * block_width) * n_bins;

//         std::cout << '\n' << h << " = ";
        for(int n=y0;n<y0+cell_height;++n)
          for(int m=x0;m<x0+cell_width;++m)
          {
          int k = m + n * width;
//           std::cout << k << ':' << bin[k] << ' ';
//           std::cout.flush();
          hog[h + bin[k]] += module[k];
          factor += module[k];
          }
        }
        
       
    // normalize per block
    for(int k=h0;k<h0+n_bins*n_cells_per_block;++k)
      hog[k] /= factor;
      
    }
  
  delete [] bin;
  delete [] module;
  
  out.first.data = hog;
  out.second.width = width;
  out.second.height = height;
  out.second.desc_size = desc_size;
}

};

#endif
