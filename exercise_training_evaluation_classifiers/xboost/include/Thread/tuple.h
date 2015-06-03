/******************************************************************************
*   sprint::tuple
*
*   Copyright (C) 2005-2014  Paolo Medici <www.pmx.it>
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

#ifndef _SPRINT_tuple_H
#define _SPRINT_tuple_H

// $generated on May 14 2014 13:16:57$

/** @file tuple.h
  * @brief an extension of std::pair to multiple dimensions
  **/

namespace sprint {

/** @brief a tuple, representing 1 ordered elements */
template<class P1>
struct tuple1 {
        P1 p1;

tuple1(P1 __p1=P1()
        ) : p1(__p1)   { }
};

/** @brief a tuple, representing 2 ordered elements */
template<class P1, class P2>
struct tuple2 {
        P1 p1;
        P2 p2;

tuple2(P1 __p1=P1(),
        P2 __p2=P2()
        ) : p1(__p1),
        p2(__p2)   { }
};

/** @brief a tuple, representing 3 ordered elements */
template<class P1, class P2, class P3>
struct tuple3 {
        P1 p1;
        P2 p2;
        P3 p3;

tuple3(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3)   { }
};

/** @brief a tuple, representing 4 ordered elements */
template<class P1, class P2, class P3, class P4>
struct tuple4 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;

tuple4(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4)   { }
};

/** @brief a tuple, representing 5 ordered elements */
template<class P1, class P2, class P3, class P4, class P5>
struct tuple5 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;

tuple5(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5)   { }
};

/** @brief a tuple, representing 6 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6>
struct tuple6 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;

tuple6(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6)   { }
};

/** @brief a tuple, representing 7 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
struct tuple7 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;

tuple7(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7)   { }
};

/** @brief a tuple, representing 8 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
struct tuple8 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;

tuple8(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8)   { }
};

/** @brief a tuple, representing 9 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
struct tuple9 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;
        P9 p9;

tuple9(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8(),
        P9 __p9=P9()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8),
        p9(__p9)   { }
};

/** @brief a tuple, representing 10 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
struct tuple10 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;
        P9 p9;
        P10 p10;

tuple10(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8(),
        P9 __p9=P9(),
        P10 __p10=P10()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8),
        p9(__p9),
        p10(__p10)   { }
};

/** @brief a tuple, representing 11 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
struct tuple11 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;
        P9 p9;
        P10 p10;
        P11 p11;

tuple11(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8(),
        P9 __p9=P9(),
        P10 __p10=P10(),
        P11 __p11=P11()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8),
        p9(__p9),
        p10(__p10),
        p11(__p11)   { }
};

/** @brief a tuple, representing 12 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
struct tuple12 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;
        P9 p9;
        P10 p10;
        P11 p11;
        P12 p12;

tuple12(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8(),
        P9 __p9=P9(),
        P10 __p10=P10(),
        P11 __p11=P11(),
        P12 __p12=P12()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8),
        p9(__p9),
        p10(__p10),
        p11(__p11),
        p12(__p12)   { }
};

/** @brief a tuple, representing 13 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
struct tuple13 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;
        P9 p9;
        P10 p10;
        P11 p11;
        P12 p12;
        P13 p13;

tuple13(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8(),
        P9 __p9=P9(),
        P10 __p10=P10(),
        P11 __p11=P11(),
        P12 __p12=P12(),
        P13 __p13=P13()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8),
        p9(__p9),
        p10(__p10),
        p11(__p11),
        p12(__p12),
        p13(__p13)   { }
};

/** @brief a tuple, representing 14 ordered elements */
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13, class P14>
struct tuple14 {
        P1 p1;
        P2 p2;
        P3 p3;
        P4 p4;
        P5 p5;
        P6 p6;
        P7 p7;
        P8 p8;
        P9 p9;
        P10 p10;
        P11 p11;
        P12 p12;
        P13 p13;
        P14 p14;

tuple14(P1 __p1=P1(),
        P2 __p2=P2(),
        P3 __p3=P3(),
        P4 __p4=P4(),
        P5 __p5=P5(),
        P6 __p6=P6(),
        P7 __p7=P7(),
        P8 __p8=P8(),
        P9 __p9=P9(),
        P10 __p10=P10(),
        P11 __p11=P11(),
        P12 __p12=P12(),
        P13 __p13=P13(),
        P14 __p14=P14()
        ) : p1(__p1),
        p2(__p2),
        p3(__p3),
        p4(__p4),
        p5(__p5),
        p6(__p6),
        p7(__p7),
        p8(__p8),
        p9(__p9),
        p10(__p10),
        p11(__p11),
        p12(__p12),
        p13(__p13),
        p14(__p14)   { }
};


/************************* make_tuple **************************/

/** helping function to create a tuple1 object*/
template<class P1>
inline tuple1<P1> make_tuple(const P1 & p1)
{
        return tuple1<P1> (p1);
}

/** helping function to create a tuple2 object*/
template<class P1, class P2>
inline tuple2<P1, P2> make_tuple(const P1 & p1,
        const P2 & p2)
{
        return tuple2<P1, P2> (p1, p2);
}

/** helping function to create a tuple3 object*/
template<class P1, class P2, class P3>
inline tuple3<P1, P2, P3> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3)
{
        return tuple3<P1, P2, P3> (p1, p2, p3);
}

/** helping function to create a tuple4 object*/
template<class P1, class P2, class P3, class P4>
inline tuple4<P1, P2, P3, P4> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4)
{
        return tuple4<P1, P2, P3, P4> (p1, p2, p3, p4);
}

/** helping function to create a tuple5 object*/
template<class P1, class P2, class P3, class P4, class P5>
inline tuple5<P1, P2, P3, P4, P5> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5)
{
        return tuple5<P1, P2, P3, P4, P5> (p1, p2, p3, p4, p5);
}

/** helping function to create a tuple6 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6>
inline tuple6<P1, P2, P3, P4, P5, P6> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6)
{
        return tuple6<P1, P2, P3, P4, P5, P6> (p1, p2, p3, p4, p5, p6);
}

/** helping function to create a tuple7 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline tuple7<P1, P2, P3, P4, P5, P6, P7> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7)
{
        return tuple7<P1, P2, P3, P4, P5, P6, P7> (p1, p2, p3, p4, p5, p6, p7);
}

/** helping function to create a tuple8 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline tuple8<P1, P2, P3, P4, P5, P6, P7, P8> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8)
{
        return tuple8<P1, P2, P3, P4, P5, P6, P7, P8> (p1, p2, p3, p4, p5, p6, p7, p8);
}

/** helping function to create a tuple9 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline tuple9<P1, P2, P3, P4, P5, P6, P7, P8, P9> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9)
{
        return tuple9<P1, P2, P3, P4, P5, P6, P7, P8, P9> (p1, p2, p3, p4, p5, p6, p7, p8, p9);
}

/** helping function to create a tuple10 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
inline tuple10<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10)
{
        return tuple10<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
}

/** helping function to create a tuple11 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
inline tuple11<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11)
{
        return tuple11<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
}

/** helping function to create a tuple12 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
inline tuple12<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11,
        const P12 & p12)
{
        return tuple12<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
}

/** helping function to create a tuple13 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
inline tuple13<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11,
        const P12 & p12,
        const P13 & p13)
{
        return tuple13<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
}

/** helping function to create a tuple14 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13, class P14>
inline tuple14<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14> make_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11,
        const P12 & p12,
        const P13 & p13,
        const P14 & p14)
{
        return tuple14<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
}


/************************* new_tuple **************************/

/** helping function to allocate a tuple1 object*/
template<class P1>
inline tuple1<P1> *new_tuple(const P1 & p1)
{
        return new tuple1<P1> (p1);
}

/** helping function to allocate a tuple2 object*/
template<class P1, class P2>
inline tuple2<P1, P2> *new_tuple(const P1 & p1,
        const P2 & p2)
{
        return new tuple2<P1, P2> (p1, p2);
}

/** helping function to allocate a tuple3 object*/
template<class P1, class P2, class P3>
inline tuple3<P1, P2, P3> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3)
{
        return new tuple3<P1, P2, P3> (p1, p2, p3);
}

/** helping function to allocate a tuple4 object*/
template<class P1, class P2, class P3, class P4>
inline tuple4<P1, P2, P3, P4> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4)
{
        return new tuple4<P1, P2, P3, P4> (p1, p2, p3, p4);
}

/** helping function to allocate a tuple5 object*/
template<class P1, class P2, class P3, class P4, class P5>
inline tuple5<P1, P2, P3, P4, P5> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5)
{
        return new tuple5<P1, P2, P3, P4, P5> (p1, p2, p3, p4, p5);
}

/** helping function to allocate a tuple6 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6>
inline tuple6<P1, P2, P3, P4, P5, P6> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6)
{
        return new tuple6<P1, P2, P3, P4, P5, P6> (p1, p2, p3, p4, p5, p6);
}

/** helping function to allocate a tuple7 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline tuple7<P1, P2, P3, P4, P5, P6, P7> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7)
{
        return new tuple7<P1, P2, P3, P4, P5, P6, P7> (p1, p2, p3, p4, p5, p6, p7);
}

/** helping function to allocate a tuple8 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline tuple8<P1, P2, P3, P4, P5, P6, P7, P8> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8)
{
        return new tuple8<P1, P2, P3, P4, P5, P6, P7, P8> (p1, p2, p3, p4, p5, p6, p7, p8);
}

/** helping function to allocate a tuple9 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline tuple9<P1, P2, P3, P4, P5, P6, P7, P8, P9> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9)
{
        return new tuple9<P1, P2, P3, P4, P5, P6, P7, P8, P9> (p1, p2, p3, p4, p5, p6, p7, p8, p9);
}

/** helping function to allocate a tuple10 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
inline tuple10<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10)
{
        return new tuple10<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
}

/** helping function to allocate a tuple11 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
inline tuple11<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11)
{
        return new tuple11<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
}

/** helping function to allocate a tuple12 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
inline tuple12<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11,
        const P12 & p12)
{
        return new tuple12<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
}

/** helping function to allocate a tuple13 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13>
inline tuple13<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11,
        const P12 & p12,
        const P13 & p13)
{
        return new tuple13<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
}

/** helping function to allocate a tuple14 object*/
template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12, class P13, class P14>
inline tuple14<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14> *new_tuple(const P1 & p1,
        const P2 & p2,
        const P3 & p3,
        const P4 & p4,
        const P5 & p5,
        const P6 & p6,
        const P7 & p7,
        const P8 & p8,
        const P9 & p9,
        const P10 & p10,
        const P11 & p11,
        const P12 & p12,
        const P13 & p13,
        const P14 & p14)
{
        return new tuple14<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14> (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
}


} //sprint

#endif

