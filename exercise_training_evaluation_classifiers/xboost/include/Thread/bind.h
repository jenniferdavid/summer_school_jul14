/******************************************************************************
*   sprint::bind
*
*   Copyright (C) 2005-2011  Paolo Medici <www.pmx.it>
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
*******************************************************************************/

#ifndef _SPRINT_THREAD_bind_H
#define _SPRINT_THREAD_bind_H

// $generated on Oct 31 2011 15:01:05$

/** @file
  * @brief method to create function pointer for thread call 
  * @note Attenzione che questi bind vanno bene solo i thread e non per le funzioni in genere
  * run a thread using a Ptr Fun or a Mem Fun */

#include "invoker.h"
#include "function.h"

namespace sprint {

  /** instantiate a thread callback from an hardcoded mem fun
  * @a obj an instance of object
  * @a T is a Class Name
  * @a R is the Function Return Type (important to match)
  * @a F is a member function 
  * \code
  * th.create_thread(sprint::thread_bind<ServerCom,DWORD,&ServerCom::Thread>(this));
  * \endcode
  * @note NO HEAP fragmentation: viene creata una funzione trampolino per ogni funzione thread*/
  template<typename T, typename R, R (T::*F)(void)>
  inline thread_function thread_bind(T * obj)
  {
                return thread_function( sprint::detail::embedded_mem_fun<R,T,F>, reinterpret_cast<void *>(obj));
  }

/* ptr function invoker */

/// instantiate a 0 parameters thread callback
template<class R>
inline thread_function thread_bind(R (*Proc)())
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_0<R> >, reinterpret_cast<void *>( new_tuple(Proc) ));
        }

template<>
inline thread_function thread_bind(void (*Proc)())
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_0<void> >, reinterpret_cast<void *>( new_tuple(Proc) ));
        }

/// instantiate a 1 parameters thread callback
template<class R, class A1, class P1>
inline thread_function thread_bind(R (*Proc)(A1), P1 p1)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_1<R, P1> >, reinterpret_cast<void *>( new_tuple(Proc, p1) ));
        }

template<class A1, class P1>
inline thread_function thread_bind(void (*Proc)(A1), P1 p1)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_1<void, P1> >, reinterpret_cast<void *>( new_tuple(Proc, p1) ));
        }

/// instantiate a 2 parameters thread callback
template<class R, class A1, class A2, class P1, class P2>
inline thread_function thread_bind(R (*Proc)(A1, A2), P1 p1, P2 p2)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_2<R, P1, P2> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2) ));
        }

template<class A1, class A2, class P1, class P2>
inline thread_function thread_bind(void (*Proc)(A1, A2), P1 p1, P2 p2)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_2<void, P1, P2> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2) ));
        }

/// instantiate a 3 parameters thread callback
template<class R, class A1, class A2, class A3, class P1, class P2, class P3>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3), P1 p1, P2 p2, P3 p3)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_3<R, P1, P2, P3> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3) ));
        }

template<class A1, class A2, class A3, class P1, class P2, class P3>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3), P1 p1, P2 p2, P3 p3)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_3<void, P1, P2, P3> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3) ));
        }

/// instantiate a 4 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class P1, class P2, class P3, class P4>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4), P1 p1, P2 p2, P3 p3, P4 p4)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_4<R, P1, P2, P3, P4> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4) ));
        }

template<class A1, class A2, class A3, class A4, class P1, class P2, class P3, class P4>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4), P1 p1, P2 p2, P3 p3, P4 p4)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_4<void, P1, P2, P3, P4> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4) ));
        }

/// instantiate a 5 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5,
        class P1, class P2, class P3, class P4, class P5>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_5<R, P1, P2, P3, P4, P5> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class P1, class P2, class P3, class P4, class P5>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_5<void, P1, P2, P3, P4, P5> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5) ));
        }

/// instantiate a 6 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6,
        class P1, class P2, class P3, class P4, class P5, class P6>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_6<R, P1, P2, P3, P4, P5, P6> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class P1, class P2, class P3, class P4, class P5, class P6>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_6<void, P1, P2, P3, P4, P5, P6> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6) ));
        }

/// instantiate a 7 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_7<R, P1, P2, P3, P4, P5, P6, P7> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_7<void, P1, P2, P3, P4, P5, P6, P7> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7) ));
        }

/// instantiate a 8 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_8<R, P1, P2, P3, P4, P5, P6, P7, P8> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_8<void, P1, P2, P3, P4, P5, P6, P7, P8> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8) ));
        }

/// instantiate a 9 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_9<R, P1, P2, P3, P4, P5, P6, P7, P8, P9> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_9<void, P1, P2, P3, P4, P5, P6, P7, P8, P9> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9) ));
        }

/// instantiate a 10 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_10<R, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_10<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) ));
        }

/// instantiate a 11 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_11<R, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_11<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) ));
        }

/// instantiate a 12 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_12<R, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_12<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) ));
        }

/// instantiate a 13 parameters thread callback
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
inline thread_function thread_bind(R (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_13<R, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) ));
        }

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
inline thread_function thread_bind(void (*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::ptr_fun_13<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> >, reinterpret_cast<void *>( new_tuple(Proc, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) ));
        }


/* mem function invoker */

/// instantiate a 0 parameters thread callback
template<class T, class R>
inline thread_function thread_bind(R (T::*Proc)(), T *_this)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_0<R, T> > , reinterpret_cast<void *>(new_tuple(Proc, _this) ));
        }
template<class T, class R>
inline thread_function thread_bind(R (T::*Proc)() const, T *_this)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_0<R, T> > , reinterpret_cast<void *>(new_tuple(Proc, _this) ));
        }

/// instantiate a 1 parameters thread callback
template<class T, class R, class A1, class P1>
inline thread_function thread_bind(R (T::*Proc)(A1), T *_this, P1 p1)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_1<R, T, P1> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1) ));
        }
template<class T, class R, class A1, class P1>
inline thread_function thread_bind(R (T::*Proc)(A1) const, T *_this, P1 p1)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_1<R, T, P1> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1) ));
        }

/// instantiate a 2 parameters thread callback
template<class T, class R, class A1, class A2, class P1, class P2>
inline thread_function thread_bind(R (T::*Proc)(A1, A2), T *_this, P1 p1, P2 p2)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_2<R, T, P1, P2> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2) ));
        }
template<class T, class R, class A1, class A2, class P1, class P2>
inline thread_function thread_bind(R (T::*Proc)(A1, A2) const, T *_this, P1 p1, P2 p2)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_2<R, T, P1, P2> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2) ));
        }

/// instantiate a 3 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class P1, class P2, class P3>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3), T *_this, P1 p1, P2 p2, P3 p3)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_3<R, T, P1, P2, P3> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3) ));
        }
template<class T, class R, class A1, class A2, class A3, class P1, class P2, class P3>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3) const, T *_this, P1 p1, P2 p2, P3 p3)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_3<R, T, P1, P2, P3> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3) ));
        }

/// instantiate a 4 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class P1, class P2, class P3, class P4>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4), T *_this, P1 p1, P2 p2, P3 p3, P4 p4)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_4<R, T, P1, P2, P3, P4> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class P1, class P2, class P3, class P4>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_4<R, T, P1, P2, P3, P4> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4) ));
        }

/// instantiate a 5 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5,
        class P1, class P2, class P3, class P4, class P5>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_5<R, T, P1, P2, P3, P4, P5> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5,
        class P1, class P2, class P3, class P4, class P5>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_5<R, T, P1, P2, P3, P4, P5> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5) ));
        }

/// instantiate a 6 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6,
        class P1, class P2, class P3, class P4, class P5, class P6>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_6<R, T, P1, P2, P3, P4, P5, P6> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6,
        class P1, class P2, class P3, class P4, class P5, class P6>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_6<R, T, P1, P2, P3, P4, P5, P6> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6) ));
        }

/// instantiate a 7 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_7<R, T, P1, P2, P3, P4, P5, P6, P7> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_7<R, T, P1, P2, P3, P4, P5, P6, P7> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7) ));
        }

/// instantiate a 8 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_8<R, T, P1, P2, P3, P4, P5, P6, P7, P8> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_8<R, T, P1, P2, P3, P4, P5, P6, P7, P8> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8) ));
        }

/// instantiate a 9 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_9<R, T, P1, P2, P3, P4, P5, P6, P7, P8, P9> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8, p9) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_9<R, T, P1, P2, P3, P4, P5, P6, P7, P8, P9> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8, p9) ));
        }

/// instantiate a 10 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_10<R, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_10<R, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) ));
        }

/// instantiate a 11 parameters thread callback
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_11<R, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) ));
        }
template<class T, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11,
        class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
inline thread_function thread_bind(R (T::*Proc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const, T *_this, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
        {
        return thread_function( sprint::detail::thread_fun< sprint::detail::mem_fun_11<R, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> > , reinterpret_cast<void *>(new_tuple(Proc, _this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) ));
        }


} //sprint

#endif

