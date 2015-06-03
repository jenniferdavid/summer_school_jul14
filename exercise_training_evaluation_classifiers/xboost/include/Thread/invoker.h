/******************************************************************************
*   sprint::invoker
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

#ifndef _SPRINT_invoker_H
#define _SPRINT_invoker_H

// $generated on Oct 30 2011 10:49:02$

/** @file
  * @brief methods used to wrap function calls 
  **/

#include "tuple.h"

namespace sprint {
        namespace detail {

/* ptr function invoker */

/// ptr fun with 0 parameters invoker
template<class R>
struct ptr_fun_0 {
        typedef R (*CallbackType)();
        typedef tuple1<CallbackType> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)();
                }
};
template<>
struct ptr_fun_0<void> {
        typedef void (*CallbackType)();
        typedef tuple1<CallbackType> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)();
                }
};
/// ptr fun with 1 parameters invoker
template<class R, class P1>
struct ptr_fun_1 {
        typedef R (*CallbackType)(P1);
        typedef tuple2<CallbackType, P1> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2);
                }
};
template<class P1>
struct ptr_fun_1<void, P1> {
        typedef void (*CallbackType)(P1);
        typedef tuple2<CallbackType, P1> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2);
                }
};
/// ptr fun with 2 parameters invoker
template<class R, class P1, class P2>
struct ptr_fun_2 {
        typedef R (*CallbackType)(P1, P2);
        typedef tuple3<CallbackType, P1, P2> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3);
                }
};
template<class P1, class P2>
struct ptr_fun_2<void, P1, P2> {
        typedef void (*CallbackType)(P1, P2);
        typedef tuple3<CallbackType, P1, P2> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3);
                }
};
/// ptr fun with 3 parameters invoker
template<class R, class P1, class P2, class P3>
struct ptr_fun_3 {
        typedef R (*CallbackType)(P1, P2, P3);
        typedef tuple4<CallbackType, P1, P2, P3> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4);
                }
};
template<class P1, class P2, class P3>
struct ptr_fun_3<void, P1, P2, P3> {
        typedef void (*CallbackType)(P1, P2, P3);
        typedef tuple4<CallbackType, P1, P2, P3> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4);
                }
};
/// ptr fun with 4 parameters invoker
template<class R, class P1, class P2, class P3, class P4>
struct ptr_fun_4 {
        typedef R (*CallbackType)(P1, P2, P3, P4);
        typedef tuple5<CallbackType, P1, P2, P3, P4> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5);
                }
};
template<class P1, class P2, class P3, class P4>
struct ptr_fun_4<void, P1, P2, P3, P4> {
        typedef void (*CallbackType)(P1, P2, P3, P4);
        typedef tuple5<CallbackType, P1, P2, P3, P4> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5);
                }
};
/// ptr fun with 5 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5>
struct ptr_fun_5 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5);
        typedef tuple6<CallbackType, P1, P2, P3, P4, P5> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6);
                }
};
template<class P1, class P2, class P3, class P4, class P5>
struct ptr_fun_5<void, P1, P2, P3, P4, P5> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5);
        typedef tuple6<CallbackType, P1, P2, P3, P4, P5> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6);
                }
};
/// ptr fun with 6 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6>
struct ptr_fun_6 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6);
        typedef tuple7<CallbackType, P1, P2, P3, P4, P5, P6> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6>
struct ptr_fun_6<void, P1, P2, P3, P4, P5, P6> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6);
        typedef tuple7<CallbackType, P1, P2, P3, P4, P5, P6> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7);
                }
};
/// ptr fun with 7 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
struct ptr_fun_7 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7);
        typedef tuple8<CallbackType, P1, P2, P3, P4, P5, P6, P7> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
struct ptr_fun_7<void, P1, P2, P3, P4, P5, P6, P7> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7);
        typedef tuple8<CallbackType, P1, P2, P3, P4, P5, P6, P7> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8);
                }
};
/// ptr fun with 8 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
struct ptr_fun_8 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8);
        typedef tuple9<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
struct ptr_fun_8<void, P1, P2, P3, P4, P5, P6, P7, P8> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8);
        typedef tuple9<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9);
                }
};
/// ptr fun with 9 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
struct ptr_fun_9 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
        typedef tuple10<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
struct ptr_fun_9<void, P1, P2, P3, P4, P5, P6, P7, P8, P9> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
        typedef tuple10<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10);
                }
};
/// ptr fun with 10 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
struct ptr_fun_10 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
        typedef tuple11<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
struct ptr_fun_10<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
        typedef tuple11<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11);
                }
};
/// ptr fun with 11 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
struct ptr_fun_11 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11);
        typedef tuple12<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
struct ptr_fun_11<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11);
        typedef tuple12<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12);
                }
};
/// ptr fun with 12 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
struct ptr_fun_12 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12);
        typedef tuple13<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
struct ptr_fun_12<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12);
        typedef tuple13<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13);
                }
};
/// ptr fun with 13 parameters invoker
template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
struct ptr_fun_13 {
        typedef R (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13);
        typedef tuple14<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> ParamType;
        static R run(const ParamType & p)
                {
                return (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13, p.p14);
                }
};
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
struct ptr_fun_13<void, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> {
        typedef void (*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13);
        typedef tuple14<CallbackType, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> ParamType;
        static void run(const ParamType & p)
                {
                (*p.p1)(p.p2, p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13, p.p14);
                }
};
/* member function invoker */

/// member function with 0 parameters invoker
template<class R, class T>
struct mem_fun_0 {
        typedef R (T::*CallbackType)();
        typedef tuple2<CallbackType, T*> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)();
                }
};
template<class T>
struct mem_fun_0<void, T> {
        typedef void (T::*CallbackType)();
        typedef tuple2<CallbackType, T*> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)();
                }
};
/// member function with 1 parameters invoker
template<class R, class T, class P1>
struct mem_fun_1 {
        typedef R (T::*CallbackType)(P1);
        typedef tuple3<CallbackType, T*, P1> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3);
                }
};
template<class T, class P1>
struct mem_fun_1<void, T, P1> {
        typedef void (T::*CallbackType)(P1);
        typedef tuple3<CallbackType, T*, P1> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3);
                }
};
/// member function with 2 parameters invoker
template<class R, class T, class P1, class P2>
struct mem_fun_2 {
        typedef R (T::*CallbackType)(P1, P2);
        typedef tuple4<CallbackType, T*, P1, P2> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4);
                }
};
template<class T, class P1, class P2>
struct mem_fun_2<void, T, P1, P2> {
        typedef void (T::*CallbackType)(P1, P2);
        typedef tuple4<CallbackType, T*, P1, P2> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4);
                }
};
/// member function with 3 parameters invoker
template<class R, class T, class P1, class P2, class P3>
struct mem_fun_3 {
        typedef R (T::*CallbackType)(P1, P2, P3);
        typedef tuple5<CallbackType, T*, P1, P2, P3> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5);
                }
};
template<class T, class P1, class P2, class P3>
struct mem_fun_3<void, T, P1, P2, P3> {
        typedef void (T::*CallbackType)(P1, P2, P3);
        typedef tuple5<CallbackType, T*, P1, P2, P3> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5);
                }
};
/// member function with 4 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4>
struct mem_fun_4 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4);
        typedef tuple6<CallbackType, T*, P1, P2, P3, P4> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6);
                }
};
template<class T, class P1, class P2, class P3, class P4>
struct mem_fun_4<void, T, P1, P2, P3, P4> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4);
        typedef tuple6<CallbackType, T*, P1, P2, P3, P4> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6);
                }
};
/// member function with 5 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5>
struct mem_fun_5 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5);
        typedef tuple7<CallbackType, T*, P1, P2, P3, P4, P5> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5>
struct mem_fun_5<void, T, P1, P2, P3, P4, P5> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5);
        typedef tuple7<CallbackType, T*, P1, P2, P3, P4, P5> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7);
                }
};
/// member function with 6 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6>
struct mem_fun_6 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6);
        typedef tuple8<CallbackType, T*, P1, P2, P3, P4, P5, P6> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6>
struct mem_fun_6<void, T, P1, P2, P3, P4, P5, P6> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6);
        typedef tuple8<CallbackType, T*, P1, P2, P3, P4, P5, P6> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8);
                }
};
/// member function with 7 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
struct mem_fun_7 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7);
        typedef tuple9<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
struct mem_fun_7<void, T, P1, P2, P3, P4, P5, P6, P7> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7);
        typedef tuple9<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9);
                }
};
/// member function with 8 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
struct mem_fun_8 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8);
        typedef tuple10<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
struct mem_fun_8<void, T, P1, P2, P3, P4, P5, P6, P7, P8> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8);
        typedef tuple10<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10);
                }
};
/// member function with 9 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
struct mem_fun_9 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
        typedef tuple11<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
struct mem_fun_9<void, T, P1, P2, P3, P4, P5, P6, P7, P8, P9> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
        typedef tuple11<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11);
                }
};
/// member function with 10 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
struct mem_fun_10 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
        typedef tuple12<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
struct mem_fun_10<void, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
        typedef tuple12<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12);
                }
};
/// member function with 11 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
struct mem_fun_11 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11);
        typedef tuple13<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
struct mem_fun_11<void, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11);
        typedef tuple13<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13);
                }
};
/// member function with 12 parameters invoker
template<class R, class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
struct mem_fun_12 {
        typedef R (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12);
        typedef tuple14<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> ParamType;
        static R run(const ParamType & p)
                {
                return (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13, p.p14);
                }
};
template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
struct mem_fun_12<void, T, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> {
        typedef void (T::*CallbackType)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12);
        typedef tuple14<CallbackType, T*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> ParamType;
        static void run(const ParamType & p)
                {
                (p.p2->*p.p1)(p.p3, p.p4, p.p5, p.p6, p.p7, p.p8, p.p9, p.p10, p.p11, p.p12, p.p13, p.p14);
                }
};

        } // detail
} // sprint

#endif

