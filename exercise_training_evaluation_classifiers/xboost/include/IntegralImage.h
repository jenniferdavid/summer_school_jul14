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

#ifndef _INTEGRAL_IMAGE_H
#define _INTEGRAL_IMAGE_H

/** @file IntegralImage.h
  * @brief method to handle an Integral Image
  **/


#include <algorithm>
#include <cmath>
#include <iostream>
#include "Image.h"
#include "Types.h"
#include "_stdint.h"

/////////////////////////////
#include <cmath>
#include <limits>
#include <stdexcept>

/** the integral image.
 *  The use of handle is required in order to remove an eventual pointer dereferencing during apply and improve performance.
  */
template<class T>
struct IntegralImageHandle {
public:
    /** the origin of integral image */
    T *data;
    /** Integral image geometry */
    unsigned int width, height;

public:
    IntegralImageHandle() : data(0), width(0), height(0) { }

    /** copy: only handle integral image can receive copy */
    IntegralImageHandle(const IntegralImageHandle<T> & src)
    {
        data = src.data;
        width = src.width;
        height = src.height;
    }

    void operator=(const IntegralImageHandle<T> & src)
    {
        data = src.data;
        width = src.width;
        height = src.height;
    }

    ~IntegralImageHandle() {
    }

    /** Force the memory release */
    void Release()
    {
        delete [] data;
        data = 0;
    }

    /// Resize (destructive) of buffer
    void Resize(unsigned int width, unsigned int height)
    {
        delete [] data;
        this->width = width;
        this->height = height;
        data = new T[this->width * this->height]; // reserve new memory
    }

    T & operator() (int i, int j) {
        return data[i + j * width];
    }
    const T & operator() (int i, int j) const {
        return data[i + j * width];
    }

    /**convert the integral image in a buffer. The buffer must be width x height
     * @return pixel(x,y) = I(x,y) + I(x-1,y-1) - I(x,y-1) - I(x-1,y)
     */
    T pixel_at(int x, int y) const
    { 
        // TODO: NDEBUG
        if(x<0 || y < 0 || x >=(int)this->width || y>=(int)this->height)
        {
            std::cerr << "pixel_at(" << x << ',' << y << ")" << std::endl;
            throw std::runtime_error("pixel_at called with invalid parameters");
        }
        uint32_t acc = data[x + y * width];
        if(x>0 && y>0) acc += data[(x-1) + (y-1) * width];
        if(x>0) acc -= data[(x-1) + y * width];
        if(y>0) acc -= data[x + (y-1) * width];
        return acc;
    }

    /// recover using interpolation integral-image value in x,y
    double interpolated_value_at(double x, double y) const {
        int ix = (int) (x);
        double mx = x - (double) ix;
        int iy = (int) y;
        double my = y - (double) iy;

        // clamping
        if(ix<0) { ix=0; mx = 0.0; }
        if(iy<0) { iy=0; my = 0.0; }

        double acc = data[(ix+1)+(iy+1)*width] * mx * my;
        if(ix>0 && iy> 0)
            acc += data[ix + iy * width] * (1.0 - mx) * (1.0 - my);
        if(ix>0)
            acc += data[ix + (iy+1) * width] * (1.0 - mx) * my;
        if(iy>0)
            acc += data[(ix+1) + iy * width] * mx * (1.0 - my);

        return acc;
    }


    /** convert the integral image in a buffer of the same size.
     * \code
     * auxIntImage.Export(buf.Buffer(), 1,1, width, height);
     * \endcode
     * 
     * @todo handle cropping better
     * */
    template<class D>
    void Export(D * buf, int x0, int y0, int dst_width, int dst_height) const
    {
        for(int j = 0; j<dst_height; ++j)
            for(int i = 0; i<dst_width; ++i)
            {
                T val = pixel_at(x0 + i, y0 + j); // non molto ottimizzato al momento
                if(val > std::numeric_limits<D>::max() )
                    val = std::numeric_limits<D>::max();
                if(val < std::numeric_limits<D>::min() )
                    val = std::numeric_limits<D>::min();
                buf[i + j * dst_width] = val;
            }
    }

    /** Resample and export an integral image
     * @param buf an output buffer
     * @param x0,y0,x1,y1 a rectangle
     * @param dst_width,dst_height size of @a buf buffer 
     * */
    template<class DataType>
    void ResampleAndExport(DataType * buf, double x0, double y0, double x1, double y1, int dst_width, int dst_height) const
    {
        const double dx = (x1 - x0) / (double) dst_width;
        const double dy = (y1 - y0) / (double) dst_height;
        const double norm = 1.0 / (dx*dy);
 
        // TODO: se non e' un extended_integral_image il codice e' leggermente differente.
//         x0 += dx - 1.0;
//         y0 += dy - 1.0;
        
//         std::cout << x0 << ' ' << y0 << ' ' << dx << ' ' << dy << ":" << std::endl;
        
        for(int j = 0; j<dst_height; ++j)
            for(int i = 0; i<dst_width; ++i)
            {
              // +1 is added due to extended_integral_image
              // the point (dx*i,dy*i) is generated by the pixel formed by the previous dx*dy area
                double A = interpolated_value_at(x0 + i * dx + 1.0,     y0 + j * dy + 1.0);
                double B = interpolated_value_at(x0 + (i-1) * dx + 1.0, y0 + j * dy + 1.0);
                double C = interpolated_value_at(x0 + i * dx + 1.0,     y0 + (j-1) * dy + 1.0);
                double D = interpolated_value_at(x0 + (i-1) * dx + 1.0, y0 + (j-1) * dy + 1.0);

                buf[i + j * dst_width] = norm*(A+D-B-C);
            }
    }

/// Build integral image from buffer
    template<class _S>
    void Build(const _S *src, unsigned int width, unsigned int height, long stride, bool extend = false)
    {
        delete [] data; // release previous image

        if(extend)
        {
            this->width = width + 1;
            this->height = height + 1;
        }
        else
        {
            this->width = width;
            this->height = height;
        }

        data = new T[this->width * this->height]; // reserve new memory

        T *i_data = data;
        // first row only
        T rs = T(0);

        if(extend) {
            // [first row] and [first element] of second row are uninitialized and placed to 0.
            for(unsigned int j=0; j<width+2; j++)
                *i_data++ = T(0);
        }

        // first row
        for(unsigned int j=0; j<width; j++)
        {
            rs += src[j];
            *i_data = rs;
            i_data++;
        }

        src += stride;
        long prev_row = - (long) this->width;

        // remaining cells are sum above and to the left
        for(unsigned int i=1; i<height; ++i)
        {
            rs = T(0);

            if(extend) {
                *i_data++ = T(0);
            }

            for(unsigned int j=0; j<width; ++j)
            {
                rs += src[j];
                *i_data = rs + i_data[prev_row];
                i_data++;
            }
            
            src += stride;
        }
    }

    /// Build the integral image using an ImageHandle object
    void Build(const ImageHandle & src, bool extend = false)
    {
      if(src.bpp == 1)
        Build(src.data, src.width, src.height, src.stride, extend);
    }

};


/** internal version of Resample.
  * du,dv per aumentare la precisione possono venire dall'esterno.
  *  attenzione a non prendere punti fuori dalla immagine sorgente (non c'e' il controllo).
  * @todo valgrind notice a read outside memory
  **/
template<class T>
void Resample(IntegralImageHandle<T> & dst, unsigned int dstWidth, unsigned int dstHeight, const IntegralImageHandle<T> & src, double u0, double v0, double du, double dv)
{
    dst.Resize(dstWidth, dstHeight);

    double norm = 1.0 / (du*dv);
    
//     std::cout << "resample " << dstWidth << 'x' << dstHeight << ' ' << u0 << ' ' << v0 << ' ' <<du << ' ' << dv << std::endl;

    // equazione magica (believe, funziona!) leggere la documentazione dell'immagine integrale
    u0 += du - 1.0;
    v0 += dv - 1.0;

    const double w0 = 0.5; // TODO

    // check nel caso in cui du,dv < 1.0
    int i0 = 0;
    if(u0 < 0.0)
    {
        i0 = (int) ceil(-u0 / du);
        u0 += i0 * du;
    }

    int j0 = 0;
    if(v0 < 0.0)
    {
        j0 = (int) ceil(-v0 / dv);
        v0 += i0 * dv;
    }

    double v = v0;

    // TODO: ora il bordo e' non inizializzato: usare del codice ad hoc per inizializzarlo

    for(unsigned int j=j0; j<dstHeight; ++j)
    {
        int iv = (int) trunc(v);
        double mv = v - trunc(v);
        double u = u0;
        for(unsigned int i=i0; i<dstWidth; ++i)
        {
            //       u = u0 + du *i;
            int iu = (int) trunc(u);
            double mu = u - trunc(u);

            const T *ptr = &src.data[ iu + iv * src.width];

            double t = norm * (  (double) ptr[0] * (1.0 - mu) * (1.0 - mv) + (double) ptr[1] * (mu) * (1.0 - mv) +
                                 (double) ptr[src.width] * (1.0 - mu) * (mv) + (double)  ptr[src.width + 1] * (mu) * (mv) );

            dst.data[i + dst.width *j] = (T) (t + w0);

            u += du;
        }
        v += dv;
    }

}


/** Resample an Integral image using bilinear interpolation.
 *  An integral image can be downsampled and upsampled with the same method
 *
 * Per maggiori informationi, richiedere a Paolo Medici <medici@ce.unipr.it>
 */
template<class T>
void Resample(const IntegralImageHandle<T> & src, IntegralImageHandle<T> & dst, const rect & srcRect, unsigned int dstWidth, unsigned int dstHeight)
{
    double u0 = srcRect.x0;
    double v0 = srcRect.y0;
    double du = (double) (srcRect.x1 - srcRect.x0) / (double) dstWidth;
    double dv = (double) (srcRect.y1 - srcRect.y0) / (double) dstHeight;

    Resample<T>(dst, dstWidth, dstHeight, src, u0,v0,du,dv);
}


/** IntegralImage release memory at the end of scope. */
template<class T>
struct IntegralImage: public IntegralImageHandle<T> {
public:
    IntegralImage() { }
    ~IntegralImage() {
        IntegralImageHandle<T>::Release();
    }
};


#endif
