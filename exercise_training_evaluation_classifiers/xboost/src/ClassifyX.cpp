/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This file is part of AdaBoost Tools
 *
 *  Classify is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Classify is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Classify.  If not, see <http://www.gnu.org/licenses/>.
 */

/// Experimental App: do not distribute
/// Test Classifier using SSE/NEON instruction

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath> // abs

#include "xbversion.h"
#include "Utils.h"
#include "timer.h"

#include "Feature/CollapsedHaarFeature.h"

#include "Converter/HaarFeature.h"
// Feature Level Classifier

#include "Classifier/SimpleDecisionStump.h"

// Strong Learner
#include "Classifier/BoostClassifier.h"
#include "Classifier/SoftCascade.h"

// A collection of precomputed classifier
#include "HaarClassifiers.h"
#include "Classifier/BinaryClassifier.h"

#include "_stdint.h"
#include "Thread/thread.h"
#include "Thread/thread_group.h"
#include "Thread/bind.h"
#include "Common.h"

void help()
{
    std::cout << "Usage:\n"
              "\tclassify <trained file> <image> [OPTIONS]\n"
              " -v\n\tVerbose output\n"
              " -vv\n\tMore Verbose output\n"
              " -n\n\tVerbose output with response as first\n"
              " -q\n\tNot print statistics\n"
              " --list\n\tPrint list of available classifier\n"
              " -t threshold\n\tSet separation threshold (default 0)\n";
    std::cout.flush();
}

// global parameters

const char *network_file = NULL, *image_file = NULL;
const char *output_file = NULL;
double threshold = 0.0;
bool normalize = false;

bool response_first = false;
bool statistics = true;
int n_classifiers = -1;
bool resort = false;
int verbose = 0;
int add_noise = 0;
int ncore = sprint::thread::hardware_concurrency();

void parse_cmd_line(int argc, char *argv[])
{
    // Parse Command Line
    if(argc < 2)
    {
        help();
        exit(0);
    }

    int i = 1;
    while(i<argc)
    {
        const char *str = argv[i];
        if ((!strcmp(str, "-h") ) || (!strcmp(str, "--help") ) )
        {
            help();
            exit(0);
        }
        else if(!strcmp(str, "--list") )
        {
            print_classifier_list(std::cout);
            exit(0);
        }

        else if(!strcmp(str, "-v") )
        {
            verbose = 1;
        }
        else if(!strcmp(str, "-o") )
        {
            i++;
            output_file = argv[i];
        }
        else if(!strcmp(str, "-t") )
        {
            i++;
            threshold = atof(argv[i]);
        }
        else if(!strcmp(str, "-n") )
        {
            i++;
            ncore = atoi(argv[i]);
        }
        else if(network_file == NULL)
            network_file = str;
        else if(image_file == NULL)
            image_file = str;
        else
        {
            help();
            exit(0);
        }
        i++;
    }
}

// void process(const IntegralImageData & intImage, Classifier *c, float *r, int j0, int j1, int step, int skip)
// {
//     static const int left_margin = 1;
// 
//     // numero di colonne da processare (da 1 a width - winW)
//     int n = ( (intImage.width - winW) - left_margin) / step;
// 
//     // processo tutte le righe dell'immagine assegnate a questo thread, da j0 a j1 (a passo skip).
//     for(int j=j0; j<j1; j+=step*skip)
//     {
//         // accepted range: wmin-wmax (funzione della coordinata bottom della finestra)
//         // TODO: ci vorrebbe una policy (non mi ricordo pero' per che motivo...)
//         int wmin = m_search_ranges[j + winH].first; // dimensione minima della window
//         int wmax = m_search_ranges[j + winH].second; // dimensione massima della window
// 
//         // se la winW attuale e' dentro il range [wmin,wmax]
//         if(winW >= wmin && winW <= wmax)
//         {
//             // processo completamente la riga in questione, a passo step
//             // top coord of window + left_margin (TODO: ottimizzabile)
//             c->classify(r, intImage.data + j * intImage.width + left_margin, n, step);
//         }
// 
//         r+= kwidth*skip;
//     }
// }
//

struct light_adaboost_classifier {
  float alpha;
  int th;
  CollapsedHaarNode *nodes;
  int n_node;
};

struct light_softcascade_classifier {
  float alpha;
  int th;
  CollapsedHaarNode *nodes;
  int n_node;
  float rejection; // soft cascade threshold
};


struct process_adaboost_params {
  IntegralImageData img;
  IntegralImageParams params;

  light_adaboost_classifier *c;
  int n_class;
    
  template_geometry sz;

  int j0, j1;
};

struct process_softcascade_params {
  IntegralImageData img;
  IntegralImageParams params;

  light_softcascade_classifier *c;
  int n_class;
    
  template_geometry sz;

  int j0, j1;
};


/// adaboost apply c/c++ no optimized
void process_line_source(float *out, int w, const uint8_t *in, const light_adaboost_classifier *_c, int n_class)
{
   for(int i=0;i<w; i++)
   {
     float sum = 0.0f;
     
     for(int k=0;k<n_class;++k)
     {
        const light_adaboost_classifier & c = _c[k];
        int feat = 0;

       for(int l=0;l<c.n_node;++l)
       {
         feat += ( *(uint32_t*)( (long) (in + c.nodes[l].offset) ) ) * c.nodes[l].sign;
       }

       sum += (feat > c.th) ? c.alpha : -c.alpha;
     }

     *out++ = sum;
     in+=4;
   }
}

void process_line_source(float *out, int w, const uint8_t *in, const light_softcascade_classifier *_c, int n_class)
{
   for(int i=0;i<w; i++)
   {
     float sum = 0.0f;
     
     for(int k=0;k<n_class;++k)
     {
        const light_softcascade_classifier & c = _c[k];
        int feat = 0;

       for(int l=0;l<c.n_node;++l)
       {
         feat += ( *(uint32_t*)( (long) (in + c.nodes[l].offset) ) ) * c.nodes[l].sign;
       }

       sum += (feat < c.th) ? c.alpha : -c.alpha;
#if 0
       if(sum < c.rejection)
       {
         break;
       }
#endif       
     }

     *out++ = sum;
     in+=4;
   }
}

// #undef __ARM_NEON__

#ifdef __ARM_NEON__
# include <arm_neon.h>

void print(const float32x4_t & x)
{
  std::cout << vgetq_lane_f32(x, 0) << '\t' <<vgetq_lane_f32(x, 1) << '\t' << vgetq_lane_f32(x, 2) << '\t' << vgetq_lane_f32(x, 3) << std::endl;
}

void print(const uint32x4_t & x)
{
  std::cout << vgetq_lane_u32(x, 0) << '\t' <<vgetq_lane_u32(x, 1) << '\t' << vgetq_lane_u32(x, 2) << '\t' << vgetq_lane_u32(x, 3) << std::endl;
}

void print(const int32x4_t & x)
{
  std::cout << vgetq_lane_s32(x, 0) << '\t' <<vgetq_lane_s32(x, 1) << '\t' << vgetq_lane_s32(x, 2) << '\t' << vgetq_lane_s32(x, 3) << std::endl;
}

/// adaboost apply arm neon
void process_line_neon(float *out, int w, const uint8_t *in, const light_adaboost_classifier *_c, int n_class)
{
  
#if 0
   for(int i=0;i<w; i+=4)
   {
//      float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
     float32x4_t sum = vdupq_n_f32(0.0f);

     for(int k=0;k<n_class;++k)
     {
        const light_adaboost_classifier & c = _c[k];
        
     __asm__ volatile (
       
    : "=r"(in), "=r"(c.n_node), "=r"(out) : "memory" );
     
        int32x4_t th = vdupq_n_s32(c.th);
        
        uint32x4_t mask = vcgtq_s32(feat, th);
        
        float32x4_t alpha = vdupq_n_f32(c.alpha);
        
        sum = vaddq_f32(sum, reinterpret_cast<float32x4_t>( vandq_u32(mask, reinterpret_cast<uint32x4_t>(alpha) ) ));
        sum = vsubq_f32(sum, reinterpret_cast<float32x4_t>( vbicq_u32(mask, reinterpret_cast<uint32x4_t>(alpha) ) ));
     }
     
     vst1q_f32(out, sum);
     in+=4*4;
   }

#else
   for(int i=0;i<w; i+=4)
   {
//      float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
     float32x4_t sum = vdupq_n_f32(0.0f);
     
     for(int k=0;k<n_class;++k)
     {
        const light_adaboost_classifier & c = _c[k];
        int32x4_t feat = vdupq_n_s32(0);

       for(int l=0;l<c.n_node;++l)
       {
         int32x4_t data = vld1q_s32( (const  __attribute__ ((aligned (16))) int32_t*)(in + c.nodes[l].offset) );
         int32x4_t f =  vdupq_n_s32(c.nodes[l].sign);

         feat = vmlaq_s32(feat, data, f);
        }
        
        int32x4_t th = vdupq_n_s32(c.th);
        
        uint32x4_t mask = vcgtq_s32(feat, th);
        
        float32x4_t alpha = vdupq_n_f32(c.alpha);

        
        sum = vaddq_f32(sum, vreinterpretq_f32_u32( vandq_u32(mask, vreinterpretq_u32_f32(alpha) ) ));
        sum = vsubq_f32(sum, vreinterpretq_f32_u32( vbicq_u32(mask, vreinterpretq_u32_f32(alpha) ) ));
     }
     
     vst1q_f32(out, sum);
     in+=4*4;
   }
#endif   
}

/// adaboost apply arm neon
void process_line_neon(float *out, int w, const uint8_t *in, const light_softcascade_classifier *_c, int n_class)
{

   for(int i=0;i<w; i+=4)
   {
//      float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
     float32x4_t sum = vdupq_n_f32(0.0f);
     
     for(int k=0;k<n_class;++k)
     {
        const light_softcascade_classifier & c = _c[k];
        int32x4_t feat = vdupq_n_s32(0);

       for(int l=0;l<c.n_node;++l)
       {
         int32x4_t data = vld1q_s32( (const  __attribute__ ((aligned (16))) int32_t*)(in + c.nodes[l].offset) );
         int32x4_t f =  vdupq_n_s32(c.nodes[l].sign);

         feat = vmlaq_s32(feat, data, f);
        }
        
        int32x4_t th = vdupq_n_s32(c.th);
        
        uint32x4_t mask = vcltq_s32(feat, th);
        
        float32x4_t alpha = vdupq_n_f32(c.alpha);
        
        
//          std::cout << "alpha:"; print(alpha);
//          std::cout << "mask:";  print(mask);
//          std::cout << "vbic:";  print(vreinterpretq_f32_u32( vbicq_u32(vreinterpretq_u32_f32(alpha), mask ) ));
//          std::cout << "vand:";  print(vreinterpretq_f32_u32( vandq_u32(vreinterpretq_u32_f32(alpha), mask ) ));
        
        sum = vaddq_f32(sum, vreinterpretq_f32_u32( vandq_u32(vreinterpretq_u32_f32(alpha), mask ) )); // + alpha
        sum = vsubq_f32(sum, vreinterpretq_f32_u32( vbicq_u32(vreinterpretq_u32_f32(alpha), mask ) )); // - alpha
        
//         std::cout << "sum:";  print(sum);

         float32x2_t a_lo, a_hi, max;
           a_lo = vget_low_f32(sum);
             a_hi = vget_high_f32(sum);
             max = vpmax_f32(a_lo, a_hi);
             max = vpmax_f32(max, max);
             if(vget_lane_f32(max, 0) < c.rejection)
               break;
             
 
//             if((vgetq_lane_f32(sum, 0) < c.rejection) && (vgetq_lane_f32(sum, 1) < c.rejection) && (vgetq_lane_f32(sum, 2) < c.rejection) && (vgetq_lane_f32(sum, 3) < c.rejection))
//             if(vgetq_lane_f32(sum, 0) < c.rejection)
//                break;

     }
     
//      std::cout << "sum:";  print(sum);     
     vst1q_f32(out, sum);
     in+=4*4;
   }

}


#endif

#ifdef __SSE4_1__
# include <smmintrin.h>
# include <emmintrin.h>
# include <mmintrin.h>

void print_epi32(__m128i var)
{
  const int32_t *p = (const int32_t *)(&var);
  std::cout << p[0] << ' ' <<p[1] << ' ' << p[2] << ' ' << p[3] << std::endl;
}

void print_ps(__m128 var)
{
  const float *p = (const float *)(&var);
  std::cout << p[0] << ' ' <<p[1] << ' ' << p[2] << ' ' << p[3] << std::endl;
}

void process_line_sse4(float *out, int w, const uint8_t *in, const light_adaboost_classifier *_c, int n_class)
{
  for(int i=0;i<w; i+=4)
   {
     __m128 sum = _mm_setzero_ps();
     
     for(int k=0;k<n_class;++k)
     {
        const light_adaboost_classifier & c = _c[k];
        __m128i feat =  _mm_setzero_si128();
        
//         print_epi32(feat);

       for(int l=0;l<c.n_node;++l)
        {
         __m128i data = _mm_load_si128((__m128i *)( (long) (in + c.nodes[l].offset) ) );
         __m128i f =  _mm_set1_epi32(c.nodes[l].sign);
         
//          print_epi32(data);
//          print_epi32(f);
         
         feat = _mm_add_epi32(feat, _mm_mullo_epi32(data, f) );
         
//          print_epi32(feat);
         
        }
        
//         print_epi32(feat);
        
        __m128i th =  _mm_set1_epi32(c.th);
        __m128 mask = _mm_castsi128_ps( _mm_cmpgt_epi32(feat, th) );
        __m128  acc = _mm_set1_ps(c.alpha);
        
//         print_epi32(_mm_cmpgt_epi32(feat, th));
        
//         print_ps(_mm_and_ps(mask, acc));
//         print_ps( _mm_andnot_ps(mask, acc));
        
        sum = _mm_add_ps(sum, _mm_and_ps(mask, acc));
        sum = _mm_sub_ps(sum, _mm_andnot_ps(mask, acc) );
        
//         print_ps(sum);
     }
     
     _mm_store_ps(out, sum);
     out+=4;
     in+=4*4;
   }
}

void process_line_sse4(float *out, int w, const uint8_t *in, const light_softcascade_classifier *_c, int n_class)
{
  for(int i=0;i<w; i+=4)
   {
     __m128 sum = _mm_setzero_ps();
     
     for(int k=0;k<n_class;++k)
     {
        const light_softcascade_classifier & c = _c[k];
        __m128i feat =  _mm_setzero_si128();
        
//         print_epi32(feat);

       for(int l=0;l<c.n_node;++l)
        {
         __m128i data = _mm_load_si128((__m128i *)( (long) (in + c.nodes[l].offset) ) );
         __m128i f =  _mm_set1_epi32(c.nodes[l].sign);
         
//          print_epi32(data);
//          print_epi32(f);
         
         feat = _mm_add_epi32(feat, _mm_mullo_epi32(data, f) );
         
//          print_epi32(feat);
         
        }
        
//         print_epi32(feat);
        
        __m128i th =  _mm_set1_epi32(c.th);
        __m128 mask = _mm_castsi128_ps( _mm_cmplt_epi32(feat, th) );
        __m128  acc = _mm_set1_ps(c.alpha);
        
//         print_epi32(_mm_cmpgt_epi32(feat, th));
        
//         print_ps(_mm_and_ps(mask, acc));
//         print_ps( _mm_andnot_ps(mask, acc));
        
        sum = _mm_add_ps(sum, _mm_and_ps(mask, acc));
        sum = _mm_sub_ps(sum, _mm_andnot_ps(mask, acc) );
        
        const float *p = (const float *)(&sum);
        
        if(/* ((k&0x03)==0) && */(p[0] < c.rejection && p[1] < c.rejection && p[2] < c.rejection && p[3] < c.rejection))
          break;
        
        
//         print_ps(sum);
     }
     
     _mm_store_ps(out, sum);
     out+=4;
     in+=4*4;
   }
}

#endif

template<class Model>
void process_core(float *out, Model p );

// #define TEST

template<>
void process_core<process_softcascade_params>(float *out, process_softcascade_params p )
{
  const uint8_t *in = (const uint8_t *)( p.img.data + p.j0 * p.params.width );
  out += p.j0 * p.params.width;
  
#ifdef TEST    
  float *test = new float[ p.params.width ];
#endif
  
  for(int j=p.j0;j<p.j1; ++j)
  {
#ifdef __ARM_NEON__
   process_line_neon(out, p.params.width - p.sz.width, in, p.c, p.n_class);
#elif __SSE4_1__
   process_line_sse4(out, p.params.width - p.sz.width, in, p.c, p.n_class);
#else
   process_line_source(out, p.params.width - p.sz.width, in, p.c, p.n_class);
#endif
   
#ifdef TEST   
   process_line_source(test, p.params.width - p.sz.width, in, p.c, p.n_class);
   
   for(int i = 0;i< p.params.width - p.sz.width; ++i)
   {
     if(test[i]!=out[i])
     {
       std::cout << "ERROR: " << j << ' ' << i << " " << test[i] << ' ' << out[i] << std::endl;
     }
   }
#endif   
   
   out += p.params.width;
   in += p.params.width * sizeof(uint32_t);
  }
  
#ifdef TEST  
  delete [] test;
#endif
  
}

template<>
void process_core<process_adaboost_params>(float *out, process_adaboost_params p )
{
  const uint8_t *in = (const uint8_t *)( p.img.data + p.j0 * p.params.width );
  out += p.j0 * p.params.width;
  
#ifdef TEST    
  float *test = new float[ p.params.width ];
#endif
  
  for(int j=p.j0;j<p.j1; ++j)
  {
#ifdef __ARM_NEON__
   process_line_neon(out, p.params.width - p.sz.width, in, p.c, p.n_class);
#elif __SSE4_1__
   process_line_sse4(out, p.params.width - p.sz.width, in, p.c, p.n_class);
#else
   process_line_source(out, p.params.width - p.sz.width, in, p.c, p.n_class);
#endif
   
#ifdef TEST   
   process_line_source(test, p.params.width - p.sz.width, in, p.c, p.n_class);
   
   for(int i = 0;i< p.params.width - p.sz.width; ++i)
   {
     if(test[i]!=out[i])
     {
       std::cout << "ERROR: " << j << ' ' << i << " " << test[i] << ' ' << out[i] << std::endl;
     }
   }
#endif
   
   out += p.params.width;
   in += p.params.width * sizeof(uint32_t);
  }
  
#ifdef TEST  
  delete [] test;
#endif
  
}


template<class ParamType>
void process(const ParamType & params)
{
  float *resp; // = new float[params.params.width * params.params.height];
  
  posix_memalign((void**)&resp, 16, params.params.width * params.params.height * sizeof(float) );

  if(ncore <=1)
  {
    process_core(resp, params);
  }
  else
  {
    sprint::thread_group pool;
    ParamType bak = params;
    for(int i =0;i<ncore;++i)
    {
      bak.j0 = (i * params.j1) / ncore;
      bak.j1 = ((i+1) * params.j1) / ncore;

      pool.create_thread(sprint::thread_bind(&process_core<ParamType>, resp, bak));
    }

    pool.join_all();
  }

  free(resp);
}


int main(int argc, char *argv[])
{
    std::cout << "xBoost+Haar XClassifier " XBOOST_VERSION << std::endl;
    process_softcascade_params params;

    parse_cmd_line(argc, argv);


    if(load_image(params.img, params.params, image_file))
    {
       std::cout << "N cores: " << ncore << std::endl;
       #ifdef __ARM_NEON__
       std::cout << "ARM-NEON" << std::endl;
       #elif __SSE4_1__
       std::cout << "SSE4.1" << std::endl;
       #else
       std::cout << "NoAssembly" << std::endl;
       #endif
       std::cout << "Image " << image_file << ' ' << params.params.width << 'x' << params.params.height << std::endl;
       
#if 0
       BoostClassifier< BinaryClassifier<HaarFeature, DecisionStump<int> > > c;
       load_classifier(network_file, c, params.sz.width, params.sz.height);

       std::cout << "Classifier " << network_file << ' ' <<  params.sz.width << 'x' << params.sz.height << std::endl;

//        BoostClassifier< BinaryClassifier<HaarFeature, SimpleDecisionStump<int> > > c1;
       // convertire c in c1

       std::vector<light_adaboost_classifier> light;
       CollapsedHaarNode *cache = new CollapsedHaarNode[1000*10]; // TODO
       int idx = 0;

       for(std::vector< BoostableClassifier<  BinaryClassifier<HaarFeature, DecisionStump<int> > > >::const_iterator i = c.list().begin(); i!=c.list().end();++i)
       {
         light_adaboost_classifier l;
         l.alpha = i->alpha;
         l.th = i->th * i->parity;
         l.nodes = cache + idx;
         l.n_node = 0;
//          l.nodes = i->m_nodes;
         const std::vector<HaarNode> & feat = *i;
         for(int j=0;j<feat.size();++j)
         {
           cache[idx].offset = ( (feat[j].x+1 + (feat[j].y+1) * (long) params.params.width)*sizeof(uint32_t) ) & ~0x0F;
           cache[idx].sign = feat[j].sign * i->parity;
           idx++;
           l.n_node++;
         }
         light.push_back(l);
       }
       
       
       

       // BoostClassifier< BinaryClassifier<CollapsedHaarFeature, DecisionStump<int> > > c2(c, 1, params.params.width, params.params.height);

//        params.c = &c;
       params.c = &light[0];
       params.n_class = light.size();
       params.j0 = 0;
       params.j1 = params.params.height - params.sz.height;

       Timer t;
       t.Start();
       process(params);
       std::cout << "Process complete in " << t.GetTime() << "s" << std::endl;
#else
       SoftCascadeClassifier< BinaryClassifier<HaarFeature, DecisionStump<int> > > c;
       load_classifier(network_file, c, params.sz.width, params.sz.height);

       std::cout << "Classifier " << network_file << ' ' <<  params.sz.width << 'x' << params.sz.height << std::endl;

//        BoostClassifier< BinaryClassifier<HaarFeature, SimpleDecisionStump<int> > > c1;
       // convertire c in c1

       std::vector<light_softcascade_classifier> light;
       CollapsedHaarNode *cache = new CollapsedHaarNode[1000*10]; // TODO
       int idx = 0;

       for(std::vector< SoftCascade<  BinaryClassifier<HaarFeature, DecisionStump<int> > > >::const_iterator i = c.list().begin(); i!=c.list().end();++i)
       {
         light_softcascade_classifier l;
         l.alpha = i->alpha;
         l.rejection = i->rejection;
         l.th = i->th * i->parity;
         l.nodes = cache + idx;
         l.n_node = 0;
//          l.nodes = i->m_nodes;
         const std::vector<HaarNode> & feat = *i;
         for(int j=0;j<feat.size();++j)
         {
           cache[idx].offset = ( (feat[j].x+1 + (feat[j].y+1) * (long) params.params.width)*sizeof(uint32_t) ) & ~0x0F;
           cache[idx].sign = feat[j].sign * i->parity;
           idx++;
           l.n_node++;
         }
         light.push_back(l);
       }
       
       
       

       // BoostClassifier< BinaryClassifier<CollapsedHaarFeature, DecisionStump<int> > > c2(c, 1, params.params.width, params.params.height);

//        params.c = &c;
       params.c = &light[0];
       params.n_class = light.size();
       params.j0 = 0;
       params.j1 = params.params.height - params.sz.height;

       Timer t;
       t.Start();
       process(params);
       std::cout << "Process complete in " << t.GetTime() << "s" << std::endl;
       
#endif
    }
    else
    {
      std::cerr << image_file << " is not a valid image file" << std::endl;
    }

    return 0;
}
