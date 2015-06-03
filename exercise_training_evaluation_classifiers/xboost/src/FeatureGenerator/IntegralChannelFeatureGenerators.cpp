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

#include "FeatureGenerator/IntegralChannelFeatureGenerators.h"

using namespace std;

BaseIntegralChannelFeatureGenerator::~BaseIntegralChannelFeatureGenerator()
{
}

BruteForceIntegralChannelFeatureGenerator::~BruteForceIntegralChannelFeatureGenerator() { }

void BruteForceIntegralChannelFeatureGenerator::Reset()
{
  m_featCount = 0;
  m_rects.clear();
  
  unsigned int idx;
  int i,j;
  int w,h;
  int min_width = std::max(m_step, m_minWidth);
  
  idx = 0;
  i = j = m_border; 
  w = h = min_width; 
  std::cout << "Precompute features...." << std::endl;
  
  for(;;)
  {
  
  if((w*h>=m_minFeatArea) /* && (w>=m_minWidth) && (h>=m_minWidth) */)
  {
    rect out;
    // subtract (-1,-1)
    // NOTE: l'area della feature sara' (x0+1,y0+1,x1,y1) compresi (x0+1,y0+1) per una dimensione w x h
    out.x0 = i - 1;
    out.y0 = j - 1;
    out.x1 = i + w  - 1;
    out.y1 = j + h  - 1;
    m_rects.push_back(out);
  }

    w+=m_step;
    if(i+w>m_mw - m_border)
      {
      w = min_width;
      h+=m_step;
      if(j+h>m_mh - m_border)
        {
        h = min_width;
        i+=m_step;
        if(i + min_width > m_mw - m_border)
          {
          i = m_border;
          j+=m_step;
          if(j + min_width > m_mh - m_border)
            {
            j = m_border;
            std::cout << m_rects.size() << " rects precomputed" << std::endl;
            return;
            }
          }
        
        }
    }
  }
    
}

bool BruteForceIntegralChannelFeatureGenerator::Next(IntegralChannelFeature & out)
{
  int n_channel = m_featCount % m_nchannel;
  int n_rect = m_featCount / m_nchannel;
  
  // check if end is reached
  if(n_rect >= m_rects.size()) return false;
  
  // create "feature"
  out.channel = n_channel;
  out.x0 = m_rects[n_rect].x0;
  out.x1 = m_rects[n_rect].x1;
  out.y0 = m_rects[n_rect].y0;
  out.y1 = m_rects[n_rect].y1;
  
  // incrrment counter
  ++m_featCount;
  
  return true;
}

bool RandomIntegralChannelFeatureGenerator::Next(IntegralChannelFeature & out)
{
  if(m_cur >= m_featRand)
  {
	  return false;
  }

  out.channel = rand() % m_nchannel;
  
  // number of quantizzed cells:
  int qmh = (m_mh - 2 * m_border + 1) / m_step;
  int qmw = (m_mw - 2 * m_border + 1) / m_step;

  do {
    
  // [x0,y0,x1,y1]: SUM (X0+1, X1) (Y0+1, Y1) included
  // x:0..w -> -1..w-1 -> 0..w-1 included
  
  out.y0 = m_border + (rand() % qmh) * m_step - 1;
  out.y1 = m_border + (rand() % qmh) * m_step - 1;
  out.x0 = m_border + (rand() % qmw) * m_step - 1;
  out.x1 = m_border + (rand() % qmw) * m_step - 1;

  if(out.y1 < out.y0) std::swap(out.y0, out.y1);
  if(out.x1 < out.x0) std::swap(out.x0, out.x1);

  } while( (out.x1-out.x0)*(out.y1-out.y0) < m_minFeatArea );

  m_cur++;
  return true;
}

void RandomIntegralChannelFeatureGenerator::Reset()
{
  m_cur = 0;
}


RandomIntegralChannelFeatureGenerator::~RandomIntegralChannelFeatureGenerator()
{
}
