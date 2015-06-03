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

#ifndef _XBOOST_HAAR_FEATURE_JIT_H
#define _XBOOST_HAAR_FEATURE_JIT_H

/** @file JITHaarFeature.h
 * @brief A runtime compiler for haar feature (only X86/Linux for now)
 **/

#include <vector>
// #include <cmath>
#include <sys/mman.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <stdexcept>

/** a uint32 memory accumulator JIT */
class jit {
public:
  typedef int (* callback)(const unsigned int *) __attribute__((fastcall));

private:
  
  std::vector<unsigned char *> m_pages;
  
  unsigned char *m_proc;
  int index;
  int pagesize;

  void push_back(unsigned char c)
        {
        if(index>=pagesize)
        {
          std::cerr << "Exceed " << pagesize << " bytes" << std::endl;
          throw std::runtime_error("too large application");
        }
        m_proc[index]=c;
        index++;
        }
        
  int is_8bit(int offset)
  {
    return (offset<128) && (offset>-129);
  }
  
  void reserve_page()
  {
      if(posix_memalign((void**) &m_proc, pagesize,pagesize)!=0)
        throw std::runtime_error("posix_memalign failed");
      memset(m_proc, 0x90, pagesize);
      if(mprotect(m_proc, pagesize,  PROT_EXEC|PROT_READ|PROT_WRITE)!=0)
        throw std::runtime_error("mprotect failed");
      m_pages.push_back(m_proc);
  }
  
public:
  
  jit() {
        index = 0;
        pagesize = sysconf(_SC_PAGE_SIZE)*4;
        reserve_page();
  }
  
  ~jit()
  {
    for(std::vector<unsigned char *>::const_iterator i = m_pages.begin(); i!= m_pages.end(); ++i)
      ::free(*i);
  }
  
  void clear()
  {
    for(std::vector<unsigned char *>::const_iterator i = m_pages.begin(); i!= m_pages.end(); ++i)
      ::free(*i);
    m_pages.clear();
    reserve_page();
    index = 0;
  }


/***********************************************************/

/// Return the current procedure addres
   callback GetProcAddr() const
   {
    return reinterpret_cast<callback>(&m_proc[index]);
   }
     
/// move in accumulator mem+offset
/// NOTE: offset in bytes and not in uint32_t!!!
  void move(int offset)
  {
    if(is_8bit(offset))
    {
      // 8b 47 xx
      push_back(0x8b);
      push_back(0x47);
      push_back(offset & 0xff);
    }
    else
    {
      // 8b 87 xx xx xx xx
      push_back(0x8b);
      push_back(0x87);
      push_back(offset & 0xff);
      push_back((offset & 0xff00)>>8);
      push_back((offset & 0xff0000)>>16);
      push_back((offset & 0xff000000)>>24);
    }
  }

/// add to accumulator mem+offset
/// NOTE: offset in bytes and not in uint32_t!!!
  void add(int offset)
  {
    if(is_8bit(offset))
    {
      // 03 47 xx                add    -0x4(%rdi),%eax
      push_back(0x03);
      push_back(0x47);
      push_back(offset & 0xff);
    }
    else
    {
      // 03 87 xx xx xx xx       add    0x1f40(%rdi),%eax
      push_back(0x03);
      push_back(0x87);
      push_back(offset & 0xff);
      push_back((offset & 0xff00)>>8);
      push_back((offset & 0xff0000)>>16);
      push_back((offset & 0xff000000)>>24);
    }
    
  }

/// suubtract from accumulator mem+offset
/// NOTE: offset in bytes and not in uint32_t!!!
  void sub(int offset)
  {
   if(is_8bit(offset))
        {
        // 2b 47 xx                sub    0x8(%rdi),%eax
        push_back(0x2b);
        push_back(0x47);
        push_back(offset&0xff);
        }
        else
        {
        // 2b 87 xx xx xx xx       sub    0x1f40(%rdi),%eax
        push_back(0x2b);
        push_back(0x87);
        push_back(offset & 0xff);
        push_back((offset & 0xff00)>>8);
        push_back((offset & 0xff0000)>>16);
        push_back((offset & 0xff000000)>>24);
        }
  }
  
/// NOTE: offset in bytes and not in uint32_t!!!  
  void mul(int f)
  {
    if(f==4)
    {
      // c1 e0 02                shl    $0x2,%eax
      push_back(0xc1);
      push_back(0xe0);
      push_back(0x02);
    }
    else
    if(f==3)
    {
      // 8d 04 40                lea    (%rax,%rax,2),%eax
      push_back(0x8d);
      push_back(0x04);
      push_back(0x40);
    }
    else
    if(f==2)
    {
      // 01 c0                   add    %eax,%eax
      push_back(0x01);
      push_back(0xc0);
    }
  }
  
/// complete and return the procedure
  void retq()
  {
   push_back(0xc3);
//    std::cout << "index:" <<index << std::endl;

   // align 16byte boundaries
   index = (index+15)&(~0xF);
  }
  
};

extern jit op;

struct JITHaarFeature {
  jit::callback proc;
  
  CollapsedHaarFeature test;
public:
JITHaarFeature(const HaarFeature &src, long offset, long stride) : test(src, offset, stride)
{
 int mul[4];
 mul[0]=0; // 1
 mul[1]=0; // 2
 mul[2]=0; // 3
 mul[3]=0; // 4
 
 // MULTIPLYER
 for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
   mul[ std::abs(i->sign) - 1] = 1;
 
 int factor = 1;
 
 if(mul[1]!=0)
   factor = 2;
 if(mul[2]!=0)
   factor = 3;
 if(mul[3]!=0)
   factor = 4; 

 proc = op.GetProcAddr();
 
 if((mul[1]+mul[2]+mul[3])>1)
 {
   bool first = true;
//    std::cout << "Slow Feature" << std::endl;
   // POSITIVI
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
   if( i->sign > 0)
   {
     int __offset =  (offset +  i->x + i->y * stride)*4;
     if(first)
     {
       op.move( __offset );
       op.mul( i->sign );
       first = false;
     }
     else
     {
      for(int j=0;j<i->sign;++j)
        op.add ( __offset);
     }
   }
   // NEGATIVI
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
    if( i->sign < 0)
      {
        int __offset =  (offset +  i->x + i->y * stride)*4;
        for(int j=0;j<std::abs(i->sign);++j)
          op.sub ( __offset);
      }
 }
 else
 if(factor!=1)
 {
   bool first = true;
   // POSITIVI
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
   if( i->sign == factor)
   {
     int __offset =  (offset +  i->x + i->y * stride)*4;
     if(first)
     {
       op.move( __offset );
       first = false;
     }
     else
       op.add ( __offset);
   }
   // NEGATIVI
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
    if( i->sign == -factor)
      {
        int __offset =  (offset +  i->x + i->y * stride)*4;
        op.sub(__offset);
      }
   
   // MULTIPLICA
   op.mul(factor);
   
   // +1-1
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
   {
     int __offset = (offset +  i->x + i->y * stride)*4;
     if( i->sign == 1)
       op.add( __offset);
     else
      if( i->sign == -1)
      op.sub( __offset);
   }
 }
 else
 {
   bool first = true;
   // POSITIVI
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
   if( i->sign == 1)
   {
     int __offset = (offset +  i->x + i->y * stride)*4;
     if(first)
     {
       op.move( __offset );
       first = false;
     }
     else
       op.add ( __offset );
   }
   // NEGATIVI
   for(std::vector<HaarNode>::const_iterator i=src.begin();i!=src.end();++i)
    if( i->sign == -1)
      {
        int __offset = (offset +  i->x + i->y * stride)*4;
        op.sub(__offset);
      }
 }
  
  op.retq();
}


/// compute the response
  inline int response(const unsigned int *pIntImage) const
  {
//     int a =proc(pIntImage);
//     int b = test.response(pIntImage);
//     if(a!=b)
//       std::cout << "error: " << a << ' ' << b << std::endl;
//     return b;
    return proc(pIntImage);
  }

  inline int operator() (const unsigned int *pIntImage) const
  {
    return response(pIntImage);
  }

};

#endif
