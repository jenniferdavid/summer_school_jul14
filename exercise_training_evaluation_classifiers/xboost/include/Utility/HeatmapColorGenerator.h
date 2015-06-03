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

#ifndef HEATMAPCOLORGENERATOR_H
#define HEATMAPCOLORGENERATOR_H

/** @file HeatmapColorGenerator.h
 * @brief tools for color debug images */

namespace heatmap {
  
  namespace detail {
    
  	struct RGB8{
  		RGB8(){}
  		RGB8(unsigned char i) : R(i), G(i), B(i) {}
  		RGB8(unsigned char r,unsigned char g, unsigned char b):R(r),G(g),B(b){}
  		unsigned char R,G,B;

  	};

    inline RGB8 hsvToRGB(double h, // degrees [0, 360]
			     double s, // [0, 1]
			     double v) { // [0, 1]
      if (s == 0) { // grey
	return RGB8(v*255);
      }
      double h_floor = 0.0,
	    h_frac = modf(h/60,&h_floor);
      double p = v*(1.0-s),
	    q = v*(1.0-s*h_frac),
	    t = v*(1.0-s*(1.0-h_frac));
      p *= 255; q *= 255; t *= 255; v *= 255;
      switch (static_cast<int>(h_floor)) {
	case 0: return RGB8(v,t,p);
	case 1: return RGB8(q,v,p);
	case 2: return RGB8(p,v,t);
	case 3: return RGB8(p,q,v);
	case 4: return RGB8(t,p,v);
	default: // case 5:
	  return RGB8(v,p,q);
      };
    }
    
  }
  
  template <class T>
  class BlueToRed {
    public:
      BlueToRed(const T max, const T min = T()) : m_max(max), m_min(min), m_invRange(1.0/(max-min)) {}
      
      detail::RGB8 operator()(const T value) const {
	double val = (value-m_min)*m_invRange;
	if (val < 0.0) val = 0.0;
	if (val > 1.0) val = 1.0;
	return detail::hsvToRGB(240.0*(1.0-val),1.0,0.2+0.3*val);
      }
      
      T min() const { return m_min; }
      T max() const { return m_max; }
      
    private:
      T m_max, m_min;
      double m_invRange;
  };
  
}

template <class T, template <class> class H = heatmap::BlueToRed>
struct HeatmapColorGenerator : public H<T> {
  HeatmapColorGenerator(const T max, const T min = T()) : H<T>(max,min) {}
};

#endif
