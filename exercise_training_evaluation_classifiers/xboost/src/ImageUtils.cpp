#include "Utility/ImageUtils.h"
#include "IntegralImage.h"
#include <stdio.h>
#include "IO/pnmio.h"

ImageResampler::ImageResampler()
{
}

ImageResampler::~ImageResampler()
{
}

void ImageResampler::ImportImage(const ImageHandle & in)
{
  img.Build(in.data, in.width, in.height, in.stride, true);
}

void ImageResampler::ExportImage(ImageHandle & out, const rect & area)
{
  img.ResampleAndExport(out.data,  area.x0, area.y0, area.x1, area.y1, out.width, out.height);
  
//   static int count = 0;
//   char buffer[256];
//   sprintf(buffer, "/tmp/%u.pgm", count);
//   count++;
//   
//   pgm_write(buffer, out.data, out.width, out.height, 255);

  
}

void ImageResampler::ExportImage(ImageHandle & out)
{
//  img.ResampleAndExport(out.data, 0, 0, img.width-1, img.height-1, out.width, out.height);
  img.ResampleAndExport(out.data, 1, 1, img.width, img.height, out.width, out.height);
  /*
   static int count = 0;
   char buffer[256];
   sprintf(buffer, "/tmp/R%u.pgm", count);
   count++;
   pgm_write(buffer, out.data, out.width, out.height, 255);
   */
}

void BlendBox(ImageHandle & out, rect r, unsigned int color, float factor)
{
  if(r.x0<0) r.x0 = 0;
  if(r.y0<0) r.y0 = 0;
  if(r.x1>=out.width) r.x1 = out.width - 1;
  if(r.y1>=out.height) r.y1 = out.height - 1;
  
  unsigned int ifactor = factor * 255;
 
  if(out.bpp == 1)
  {
  for(int j=r.y0; j<r.y1; ++j)
  {
    for(int i=r.x0; i<r.x1; ++i)
    {
      out.data[i + j *out.stride] = (((unsigned int) out.data[i + j *out.stride])*ifactor + color*(255 - ifactor))>>8;
    }
  }
  }
  else
  if(out.bpp == 3)
  {
  unsigned char rgb[3];
  rgb[2] = color & 0xFF;
  rgb[1] = (color>>8) & 0xFF;
  rgb[0] = (color>>16) & 0xFF;
  for(int j=r.y0; j<r.y1; ++j)
  {
    for(int i=r.x0; i<r.x1; ++i)
    {
      out.data[3*i + j *out.stride+0] = (((unsigned int) out.data[3*i + j *out.stride+0])*ifactor + rgb[0]*(255 - ifactor))>>8;
      out.data[3*i + j *out.stride+1] = (((unsigned int) out.data[3*i + j *out.stride+1])*ifactor + rgb[1]*(255 - ifactor))>>8;
      out.data[3*i + j *out.stride+2] = (((unsigned int) out.data[3*i + j *out.stride+2])*ifactor + rgb[2]*(255 - ifactor))>>8;
    }
  }
  }
  
}

/// convert color
void Convert(ImageHandle & out, const ImageHandle & in)
{
  unsigned char *pout = out.data;
  const unsigned char *pin = in.data;
  // TODO: che
  // TODO: check ranges and clipping
  for(int j = 0; j<out.height ;++j)
  {
   for(int i = 0; i<out.width ;++i)
   {
     pout[3*i+0] = pout[3*i+1] = pout[3*i+2] = pin[i];
   }
   
   pout += out.stride;
   pin += in.stride;
  }
}

void DrawRect(ImageHandle & out, rect r, unsigned int color)
{
  // TODO: clipping!
  if(r.x0<0) r.x0 = 0;
  if(r.y0<0) r.y0 = 0;
  if(r.x1>=out.width) r.x1 = out.width - 1;
  if(r.y1>=out.height) r.y1 = out.height - 1;
  
  if(out.bpp == 1)
  {
  for(int x=r.x0;x<=r.x1;++x)
    out.data[r.y0 * out.stride + x] = color;

  for(int y=r.y0+1;y<r.y1;++y)
    out.data[y * out.stride + r.x0] = color;
  
  for(int y=r.y0+1;y<r.y1;++y)
    out.data[y * out.stride + r.x1] = color;
  
  for(int x=r.x0;x<=r.x1;++x)
    out.data[r.y1 * out.stride + x] = color;
  }
  else
  if(out.bpp == 3)
  {
  unsigned char rgb[3];
  rgb[2] = color & 0xFF;
  rgb[1] = (color>>8) & 0xFF;
  rgb[0] = (color>>16) & 0xFF;
  
  for(int k=0;k<3;++k)
  {
  for(int x=r.x0;x<=r.x1;++x)
    out.data[r.y0 * out.stride + 3*x+k] = rgb[k];

  for(int y=r.y0+1;y<r.y1;++y)
    out.data[y * out.stride + 3*r.x0+k] = rgb[k];
  
  for(int y=r.y0+1;y<r.y1;++y)
    out.data[y * out.stride + 3*r.x1+k] = rgb[k];
  
  for(int x=r.x0;x<=r.x1;++x)
    out.data[r.y1 * out.stride + 3*x+k] = rgb[k];
  }
  }
}

// #include "IO/pnmio.h"
// #include <stdio.h>
void Downsample2X(ImageHandle & out, const ImageHandle & in)
{
  unsigned char *pout = out.data;
  const unsigned char *pin = in.data;
  for(int j =0;j<out.height;++j)
  {
    
   for(int i =0;i<out.width;++i)
   {
     pout[i] = ((unsigned int)pin[2*i+0] + (unsigned int)pin[2*i+1] + (unsigned int)pin[2*i+in.stride] + (unsigned int)pin[2*i+in.stride+1])>>2;
   }
   
   pout += out.stride;
   pin += in.stride * 2;
  }
}

void Downsample1_5X(ImageHandle & out, const ImageHandle & in)
{
  unsigned char *pout = out.data;
  const unsigned char *pin = in.data;
  for(int j =0;j<out.height;j+=3)
  {
    
   for(int i =0;i<out.width;i+=2)
   {
     pout[i]   = ((unsigned int)pin[3*i+0]*4 + (unsigned int)pin[3*i+1]*2 + (unsigned int)pin[3*i+in.stride]*2 + (unsigned int)pin[3*i+in.stride+1])>>3;
     pout[i+1] = ((unsigned int)pin[3*i+2]*4 + (unsigned int)pin[3*i+1]*2 + (unsigned int)pin[3*i+2+in.stride]*2 + (unsigned int)pin[3*i+in.stride+1])>>3;

     pout[i+out.stride]   = ((unsigned int)pin[3*i+0+in.stride*2]*4 + (unsigned int)pin[3*i+1+in.stride*2]*2 + (unsigned int)pin[3*i+in.stride]*2 + (unsigned int)pin[3*i+in.stride+1])>>3;
     pout[i+out.stride+1] = ((unsigned int)pin[3*i+2+in.stride*2]*4 + (unsigned int)pin[3*i+1+in.stride*2]*2 + (unsigned int)pin[3*i+2+in.stride]*2 + (unsigned int)pin[3*i+in.stride+1])>>3;
  }
   
   pout += out.stride*2;
   pin += in.stride * 3;
  }
}

void Downsample2X(ImageHandle & out, const ImageHandle & in, const rect & area)
{
  Downsample2X(out, in.crop(area));
}


void Resample(ImageHandle & out, const ImageHandle & in, const rect & area)
{
  ImageResampler res;
  res.ImportImage(in);
  res.ExportImage(out, area);
}

void BilinearResample(ImageHandle & out, const ImageHandle & in, const rect & area)
{
  float du = (float) area.width() / (float) out.width;
  float dv = (float) area.height() / (float) out.height;
  unsigned char *pout = out.data;
  const unsigned char *pin = in.data + area.x0 + area.y0 * in.stride;
  float v = 0.0f;
  for(int j=0;j<out.height;++j)
  {
   float u = 0.0f;
   const unsigned char *ppin = pin;
   for(int i=0;i<out.width;++i)
   {
     pout[i] = ppin[0]*(1.0f-u)*(1.0f-v) + ppin[1]*u*(1.0f-v) + ppin[in.stride]*(1.0f-u)*v + ppin[in.stride+1]*u*v;
     u += du;
     int iu = u;
     u -= iu;
     ppin += iu;
   }
   pout += out.stride;
   
   v+= dv;
   int iv = v;
   v -= iv;
   pin += iv * in.stride;
   
  }
}


