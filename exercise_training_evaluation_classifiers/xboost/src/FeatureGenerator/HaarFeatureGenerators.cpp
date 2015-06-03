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

#include "FeatureGenerator/HaarFeatureGenerators.h"
#include <cstring>
#include <cstdio>

/************************************************

 A-B
 | | = A + D - B - C 
 C-D

 Immagine Integrale:
 I(x,y) = SUM_0..x,0..y i(x,y) (estremi compresi)
 
 Se voglio integrare pertanto da x0,y0 per w,h:
 
 SUM x0..x0+w-1 (estremi inclusi)
 SUM y0..y0+h-1 (estremi inclusi)

 D = I(x0+w-1,y0+h-1)
 B = I(x0+w-1,y0-1)
 C = I(x0-1,y0+h-1)
 A = I(x0-1,y0-1)

 L'unica cosa sono le feature in (0,y) e (x,0) che in questo modo non possono essere calcolate
  
  Esempio di Feature 2h

 A---B---C
 |   |   |
 | + | - |
 |   |   |
 D---E---F

 I = +(E - B + A - D) - ( F - C + B - E)
   = A - 2*B + C - D + 2*E - F

 Result : 6 MAC
 
 A-------B
 |   +   |
 C-------D
 |   -   |
 E-------F
 
 A - B - 2C + 2D + E - F

 A---B---C---D
 |   |   |   |
 | + | - | + |
 |   |   |   |
 E---F---G---H
 
 +(F + A - B - E) - (G + B - C - F) + (H + C - D - G) =
 A - B - E + F - B + C - G + F + C - D - G + H =
 A -2B +2C -D - E +2F - 2G + H

 A---B
 | + |
 C---D
 | - |
 E---F
 | + |
 G---H
  
 (A+D-B-C)-(F+C-D-E)+(H+E-F-G)
 A-B-C+D-C+D+E-F+E-F-G+H
 A-B-2C+2D+2E-2F-G+H


 A---B---C
 | + | - |
 D---E---F
 | - | + |
 G---H---I

 (E+A-B-D)-(F+B-C-E)-(H+D-E-G)+(I+E-F-H)
 A-B-D+E -F-B+C+E -H-D+E+G+I+E-F-H
 A-2B+C-2D+3E-2F+G-2H+I



 *******************************************/

// Gavrila Pedestrian

// Conservative5:  173475 feature / 191913
// Conservative30: 510125 feature 
// Conservative32: 569012 feature
#define CONSERVATIVE

// avoid to generate small feature than vector size
#define PRESERVATIVE

// Nota: l'immagine integrale contiene il pixel stesso
// Per ricavare il valore del pixel (0,0) bisogna fare
// (0,0) - (0,-1) - (-1,0) + (-1,-1)
// allo stesso modo pero'
// nel caso di 2 pixel
// i punti coinvolti sono un area per esempio 3x2 anche se la finestra e' in effetti 2x2

bool feature_2h(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((w<2)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if((w%2)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      
    x--; y--; // vedi sopra
    int sw = w/2;
    f.reserve(6);
    f.push_back(HaarNode(+1,x,y)); 	// A
    f.push_back(HaarNode(-2,x+sw,y));   // B
    f.push_back(HaarNode(+1,x+w,y));    // C
    f.push_back(HaarNode(-1,x,y+h)); 	// D
    f.push_back(HaarNode(+2,x+sw,y+h)); // E
    f.push_back(HaarNode(-1,x+w,y+h));  // F
    f.name = "2h";
   return true;
}

bool feature_2v(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (h<2)
      return false;
#ifdef CONSERVATIVE
    if((h%2)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      
     
    x--; y--; // vedi sopra
    int sh = h/2;
    f.reserve(6);
    f.push_back(HaarNode(+1,x,y));      // A
    f.push_back(HaarNode(-1,x+w,y));    // B
    f.push_back(HaarNode(-2,x,y+sh));   // C
    f.push_back(HaarNode(+2,x+w,y+sh)); // D
    f.push_back(HaarNode(+1,x,y+h));    // E
    f.push_back(HaarNode(-1,x+w,y+h));  // F
    f.name = "2v";	
    return true;
}

// feature 3h, 3xh non iso-energetic
bool feature_3h(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (w<3)
      return false;
#ifdef CONSERVATIVE
    if((w%3)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;	// vedi sopra
   int sw = w/3;
   int sw2 = (w*2)/3;
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));     // A
   out.push_back(HaarNode(-2,x+sw,y));  // B
   out.push_back(HaarNode(+2,x+sw2,y)); // C
   out.push_back(HaarNode(-1,x+w,y));   // D
 
   out.push_back(HaarNode(-1,x,y+h));     // E 
   out.push_back(HaarNode(+2,x+sw,y+h));  // F
   out.push_back(HaarNode(-2,x+sw2,y+h)); // G
   out.push_back(HaarNode(+1,x+w,y+h));   // H
   out.name =  "3h";
   return true;
}

bool feature_3v(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((h<3)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if((h%3)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sh2 = (h*2)/3;
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B

   out.push_back(HaarNode(-2,x,y+sh));    // C
   out.push_back(HaarNode(+2,x+w,y+sh));  // D

   out.push_back(HaarNode(+2,x,y+sh2));   // E
   out.push_back(HaarNode(-2,x+w,y+sh2)); // F

   out.push_back(HaarNode(-1,x,y+h));     // G
   out.push_back(HaarNode(+1,x+w,y+h));   // H
   out.name = "3v";
   return true;
}


// feature 3H, 4xh iso-energetic
bool feature_3H(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (w<4)
      return false;
#ifdef CONSERVATIVE
    if((w%4)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;	// vedi sopra
   int sw = w/4;
   int sw2 = (w*3)/4;
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));     // A
   out.push_back(HaarNode(-2,x+sw,y));  // B
   out.push_back(HaarNode(+2,x+sw2,y)); // C
   out.push_back(HaarNode(-1,x+w,y));   // D
 
   out.push_back(HaarNode(-1,x,y+h));     // E 
   out.push_back(HaarNode(+2,x+sw,y+h));  // F
   out.push_back(HaarNode(-2,x+sw2,y+h)); // G
   out.push_back(HaarNode(+1,x+w,y+h));   // H
   out.name =  "3H";
   return true;
}

bool feature_3V(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((h<4)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if((h%4)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/4;
   int sh2 = (h*3)/4;
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B

   out.push_back(HaarNode(-2,x,y+sh));    // C
   out.push_back(HaarNode(+2,x+w,y+sh));  // D

   out.push_back(HaarNode(+2,x,y+sh2));   // E
   out.push_back(HaarNode(-2,x+w,y+sh2)); // F

   out.push_back(HaarNode(-1,x,y+h));     // G
   out.push_back(HaarNode(+1,x+w,y+h));   // H
   out.name = "3V";
   return true;
}

/* A--B
 * C--D
 * E--F
 * G--H
 * I--J
 **/
/// feature 4v (iso energetic)
bool feature_4v(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((h<4)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if((h%4)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<10)
	return false;
#endif      
      
   x--;y--;
   int sh = h/4;
   out.reserve(10);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B

   out.push_back(HaarNode(-2,x,y+sh));    // C
   out.push_back(HaarNode(+2,x+w,y+sh));  // D

   out.push_back(HaarNode(+2,x,y+2*sh));   // E
   out.push_back(HaarNode(-2,x+w,y+2*sh)); // F

   out.push_back(HaarNode(-2,x,y+3*sh));     // G
   out.push_back(HaarNode(+2,x+w,y+3*sh));   // H

   out.push_back(HaarNode(+1,x,y+h));     // I
   out.push_back(HaarNode(-1,x+w,y+h));   // J

   out.name = "4v";
   return true;
}

/// feature 4h (iso energetic)
// A-B-C-D-E
// | | | | | 
// F-G-H-I-J
bool feature_4h(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (w<4)
      return false;
#ifdef CONSERVATIVE
    if((w%4)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<10)
	return false;
#endif      
      
   x--;y--;	// vedi sopra
   int sw = w/4;
   out.reserve(10);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-2,x+sw,y));    // B
   out.push_back(HaarNode(+2,x+2*sw,y));  // C
   out.push_back(HaarNode(-2,x+3*sw,y));  // D
   out.push_back(HaarNode(+1,x+w,y));     // E
 
   out.push_back(HaarNode(-1,x,y+h));      // F 
   out.push_back(HaarNode(+2,x+sw,y+h));   // G
   out.push_back(HaarNode(-2,x+2*sw,y+h)); // H
   out.push_back(HaarNode(+2,x+3*sw,y+h)); // I
   out.push_back(HaarNode(-1,x+w,y+h));    // J
   out.name =  "4h";
   return true;
}

/*
   A---B---C---D
   | + | 0 | - |
   E---F---G---H
 */
bool feature_bh(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (w<3)
      return false;
#ifdef CONSERVATIVE
    if((w%3)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;	// vedi sopra
   int sw = w/3;
   int sw2 = (w*2)/3;
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));     // A
   out.push_back(HaarNode(-1,x+sw,y));  // B
   out.push_back(HaarNode(-1,x+sw2,y)); // C
   out.push_back(HaarNode(+1,x+w,y));   // D
 
   out.push_back(HaarNode(-1,x,y+h));     // E 
   out.push_back(HaarNode(+1,x+sw,y+h));  // F
   out.push_back(HaarNode(+1,x+sw2,y+h)); // G
   out.push_back(HaarNode(-1,x+w,y+h));   // H
   out.name =  "bh";
   return true;
}

/*
   A---B
   | + |
   C---D
   | 0 |
   E---F
   | - |
   G---H
 */
bool feature_bv(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (h<3)
      return false;
#ifdef CONSERVATIVE
    if((h%3)!=0)
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sh2 = (h*2)/3;
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B

   out.push_back(HaarNode(-1,x,y+sh));    // C
   out.push_back(HaarNode(+1,x+w,y+sh));  // D

   out.push_back(HaarNode(-1,x,y+sh2));   // E
   out.push_back(HaarNode(+1,x+w,y+sh2)); // F

   out.push_back(HaarNode(+1,x,y+h));     // G
   out.push_back(HaarNode(-1,x+w,y+h));   // H
   out.name = "bv";
   return true;
}

/* 
 A---B---C
 | + | - |
 D---E---F
 | - | + |
 G---H---I

 (A+E-B-D)-(F+B-C-E)-(H+D-E-G)+(I+E-F-H)
 A-B-D+E -F-B+C+E -H-D+E+G+I+E-F-H
 A-2B+C-2D+3E-2F+G-2H+I

 (A+I-C-G)-2(F+B-C-E)-2(D+H-E-G)
 A+I-C-G-2F-2B+2C+2E -2D-2H+2E+2G
 A-2B+2C-C-2D+2E+2E-2F-G +2G-2H+I
 A-2B+C-2D+4E-2F+G-2H+I
*/
bool feature_4q(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)&&(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<9)
	return false;
#endif      
      
   x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(9);
   f.push_back(HaarNode(+1,x,y));      // A
   f.push_back(HaarNode(-2,x+sw,y));   // B
   f.push_back(HaarNode(+1,x+w,y));    // C

   f.push_back(HaarNode(-2,x,y+sh));      // D
   f.push_back(HaarNode(+4,x+sw,y+sh));   // E
   f.push_back(HaarNode(-2,x+w,y+sh));    // F

   f.push_back(HaarNode(+1,x,y+h));      // G
   f.push_back(HaarNode(-2,x+sw,y+h));   // H
   f.push_back(HaarNode(+1,x+w,y+h));    // I
   f.name = "4q";
   return true;
}

/* 
 A---B---C---D
 | + |   | - |
 E---F   G---H
 |     0     |
 I---J   K---L
 | - |   | + |
 M---N---O---P

 A+1 B-1 C-1 D+1
 E-1 F+1 G+1 H-1
 I-1 J+1 K+1 L-1
 M+1 N-1 O-1 P+1
*/
bool feature_dq(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)&&(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<16)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = 2*sh;
   int sw2 = 2*sw;

   f.reserve(16);
   f.push_back(HaarNode(+1,x,     y));  
   f.push_back(HaarNode(-1,x+sw,  y));  
   f.push_back(HaarNode(-1,x+sw2, y));  
   f.push_back(HaarNode(+1,x+w,   y));  

   f.push_back(HaarNode(-1,x,     y+sh));  
   f.push_back(HaarNode(+1,x+sw,  y+sh));  
   f.push_back(HaarNode(+1,x+sw2, y+sh));  
   f.push_back(HaarNode(-1,x+w,   y+sh));  

   f.push_back(HaarNode(-1,x,     y+sh2));  
   f.push_back(HaarNode(+1,x+sw,  y+sh2));  
   f.push_back(HaarNode(+1,x+sw2, y+sh2));  
   f.push_back(HaarNode(-1,x+w,   y+sh2));  

   f.push_back(HaarNode(+1,x,     y+h));  
   f.push_back(HaarNode(-1,x+sw,  y+h));  
   f.push_back(HaarNode(-1,x+sw2, y+h));  
   f.push_back(HaarNode(+1,x+w,   y+h));  
   f.name = "dq";
   return true;
}

/* 
 A---B--- 
 | + | 0 |
 C---*---D
 | 0 | - |
  ---E---F

  A+*-B-C -*-F+D+E
  A-B-C+D+E-F

*/
bool feature_ba(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)&&(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      
   x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));      // A
   f.push_back(HaarNode(-1,x+sw,y));   // B

   f.push_back(HaarNode(-1,x,y+sh));     // C
   f.push_back(HaarNode(+1,x+w,y+sh));   // D

   f.push_back(HaarNode(+1,x+sw,y+h));   // E
   f.push_back(HaarNode(-1,x+w,y+h));    // F
   f.name = "ba";
   return true;
}

/* 
  ---A---B 
 | 0 | + |
 C---*---D
 | - | 0 |
 E---F--- 

 A+D-*-B -C-F+*+E
 A-B-C+D+E-F

*/
bool feature_bb(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)&&(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      
      
   x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(6);
   f.push_back(HaarNode(+1,x+sw,y));  // A
   f.push_back(HaarNode(-1,x+w, y));  // B

   f.push_back(HaarNode(-1,x,  y+sh));   // C
   f.push_back(HaarNode(+1,x+w,y+sh));   // D

   f.push_back(HaarNode(+1,x,   y+h));   // E
   f.push_back(HaarNode(-1,x+sw,y+h));   // F
   f.name = "bb";
   return true;
}

/* 
 A---B-------C
 | + | -     |
 D---E---F   |
 |   | + |   |
 |   G---H---I
 |       | + |
 J-------K---L

 - (L+A-C-J) + 
 2*(E+A-B-D) +
 2*(H+E-F-G) +
 2*(L+H-K-I) +    
 
 -L (-A) (+C) (+J) (+2E) (+2A) (-2B) (-2D) (+2H) (+ 2E) (-2F) (-2G) +2L (+2H) -2K (-2I)
 
 A -2B +C  -2D +4E -2F  -2G +4H -2I + J -2K + L

*/
bool feature_5d(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<12)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = sh*2;
   int sw2 = sw*2;

   f.reserve(12);
   f.push_back(HaarNode(+1,x,    y));   // A
   f.push_back(HaarNode(-2,x+sw, y));   // B
   f.push_back(HaarNode(+1,x+w,  y));  // C

   f.push_back(HaarNode(-2,x,     y+sh));   // D
   f.push_back(HaarNode(+4,x+sw,  y+sh));   // E
   f.push_back(HaarNode(-2,x+sw2, y+sh));   // F

   f.push_back(HaarNode(-2,x+sw,  y+sh2));   // G
   f.push_back(HaarNode(+4,x+sw2, y+sh2));   // H
   f.push_back(HaarNode(-2,x+w,   y+sh2));   // I

   f.push_back(HaarNode(+1,x,     y+h));   // J
   f.push_back(HaarNode(-2,x+sw2, y+h));   // K
   f.push_back(HaarNode(+1,x+w,   y+h));   // L
   f.name = "5d";
   return true;
}

/* 
 A-------B---C
 | -     | + |
 |   D---E---F
 |   | + |   |
 G---H---I   |
 | + |     - |
 J---K-------L

 - (L+A-C-J) + 
 2*(B+F-C-E) +
 2*(D+I-E-H) +
 2*(G+K-H-J) +    
 
*/
bool feature_5D(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<12)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = sh*2;
   int sw2 = sw*2;

   f.reserve(12);
   f.push_back(HaarNode(-1,x,     y));  // A
   f.push_back(HaarNode(+2,x+sw2, y));  // B
   f.push_back(HaarNode(-1,x+w,   y));  // C

   f.push_back(HaarNode(+2,x+sw,  y+sh));   // D
   f.push_back(HaarNode(-4,x+sw2, y+sh));   // E
   f.push_back(HaarNode(+2,x+w,   y+sh));   // F

   f.push_back(HaarNode(+2,x,     y+sh2));   // G
   f.push_back(HaarNode(-4,x+sw,  y+sh2));   // H
   f.push_back(HaarNode(+2,x+sw2, y+sh2));   // I

   f.push_back(HaarNode(-1,x,    y+h));   // J
   f.push_back(HaarNode(+2,x+sw, y+h));   // K
   f.push_back(HaarNode(-1,x+w,  y+h));   // L
   f.name = "5D";
   return true;
}

/* 
 A-----------B
 |           |
 C---D    +  |
 |   |       |
 |   E---F   |
 | -     |   |
 G---*---H---I
  
 A+I-B-G
 2*(- C+D+G - H-E+F)

*/
bool feature_2t(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<9)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = sh*2;
   int sw2 = sw*2;

   f.reserve(9);
   f.push_back(HaarNode(+1,x,    y));   // A
   f.push_back(HaarNode(-1,x+w,  y));   // B

   f.push_back(HaarNode(-2,x,     y+sh));   // C
   f.push_back(HaarNode(+2,x+sw,  y+sh));   // D

   f.push_back(HaarNode(-2,x+sw,  y+sh2));   // E
   f.push_back(HaarNode(+2,x+sw2, y+sh2));   // F

   f.push_back(HaarNode(+1,x,     y+h));   // G
   f.push_back(HaarNode(-2,x+sw2, y+h));   // H
   f.push_back(HaarNode(+1,x+w,   y+h));   // I

   f.name = "2t";
   return true;
}

/* 
 A-----------B
 |           |
 |  +    C---D
 |       |   |
 |   E---F   *
 |   |    -  |
 G---H-------I
  
 A+I-B-G
 -2*(C-D-F+E+I-H)

*/
bool feature_2p(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<9)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = sh*2;
   int sw2 = sw*2;

   f.reserve(9);
   f.push_back(HaarNode(+1,x,    y));   // A
   f.push_back(HaarNode(-1,x+w,  y));   // B

   f.push_back(HaarNode(-2,x+sw2, y+sh));   // C
   f.push_back(HaarNode(+2,x+w,   y+sh));   // D

   f.push_back(HaarNode(-2,x+sw,  y+sh2));   // E
   f.push_back(HaarNode(+2,x+sw2, y+sh2));   // F

   f.push_back(HaarNode(-1,x,     y+h));   // G
   f.push_back(HaarNode(+2,x+sw,  y+h));   // H
   f.push_back(HaarNode(-1,x+w,   y+h));   // I

   f.name = "2p";
   return true;
}

/* 
 A---B-------C
 |   |    -  |
 |   D---E   *
 |       |   |
 |       F---G
 |  +        |
 H-----------I
  
 A+I-C-H
 -2*(B-C+G-F+E-D)

*/
bool feature_2s(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<9)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = sh*2;
   int sw2 = sw*2;

   f.reserve(9);
   f.push_back(HaarNode(+1,x,    y));   // A
   f.push_back(HaarNode(-2,x+sw, y));   // B
   f.push_back(HaarNode(+1,x+w,  y));   // C

   f.push_back(HaarNode(+2,x+sw,  y+sh));   // D
   f.push_back(HaarNode(-2,x+sw2, y+sh));   // E

   f.push_back(HaarNode(+2,x+sw2, y+sh2));   // F
   f.push_back(HaarNode(-2,x+w,   y+sh2));   // G

   f.push_back(HaarNode(-1,x,     y+h));   // H
   f.push_back(HaarNode(+1,x+w,   y+h));   // I

   f.name = "2s";
   return true;
}

/* 
 A------ B---C
 | -     |   |
 |   D---E   |
 |   |       |
 F---G   +   |
 |           |
 H-----------I

 A+I-C-H
 -2(A-B+E-D+G-F)  

*/
bool feature_2m(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<9)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = sh*2;
   int sw2 = sw*2;

   f.reserve(9);
   f.push_back(HaarNode(-1,x,     y));   // A
   f.push_back(HaarNode(+2,x+sw2, y));   // B
   f.push_back(HaarNode(-1,x+w,   y));   // C

   f.push_back(HaarNode(+2,x+sw,  y+sh));   // D
   f.push_back(HaarNode(-2,x+sw2, y+sh));   // E

   f.push_back(HaarNode(+2,x,     y+sh2));   // F
   f.push_back(HaarNode(-2,x+sw,  y+sh2));   // G

   f.push_back(HaarNode(-1,x,     y+h));   // H
   f.push_back(HaarNode(+1,x+w,   y+h));   // I

   f.name = "2m";
   return true;
}

/*
 A-----------B
 |           |
 |   C---D   |
 | + | - | + |
 |   E---F   |
 |           |
 G-----------H

 (A + H - B - G) - 2*(C+F-D-E)
 A + H - B - G - 2C-2F+2D+2E
*/
bool feature_2c(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = (2*h)/3;
   int sw2 = (2*w)/3;
   f.reserve(8);
   f.push_back(HaarNode(+1,x,y));      // A
   f.push_back(HaarNode(-1,x+w,y));    // B
   f.push_back(HaarNode(-1,x,y+h));    // G
   f.push_back(HaarNode(+1,x+w,y+h));  // H

   f.push_back(HaarNode(-2,x+sw,y+sh));    // C
   f.push_back(HaarNode(+2,x+sw2,y+sh));  // F
   f.push_back(HaarNode(+2,x+sw,y+sh2));   // D
   f.push_back(HaarNode(-2,x+sw2,y+sh2));  // E

   f.name = "2c";
   return true;
}

/*
 A---B---C---D
 | - |   | - |
 E---F   G---H
 |     +     |
 I---J   K---L
 | - |   | - |
 M---N---O---P

 (P+A-D-M) 
 -2*(F+A-B-E)
 -2*(C+H-D-G)
 -2*(I+N-J-M)
 -2*(K+P-L-O)
 
*/
bool feature_5c(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<16)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = (2*h)/3;
   int sw2 = (2*w)/3;
   f.reserve(16);
   f.push_back(HaarNode(-1,x,     y));  
   f.push_back(HaarNode(+2,x+sw,  y));  
   f.push_back(HaarNode(-2,x+sw2, y));  
   f.push_back(HaarNode(+1,x+w,   y));  

   f.push_back(HaarNode(+2,x,     y+sh));  
   f.push_back(HaarNode(-2,x+sw,  y+sh));  
   f.push_back(HaarNode(+2,x+sw2, y+sh));  
   f.push_back(HaarNode(-2,x+w,   y+sh));  

   f.push_back(HaarNode(-2,x,     y+sh2));  
   f.push_back(HaarNode(+2,x+sw,  y+sh2));  
   f.push_back(HaarNode(-2,x+sw2, y+sh2));  
   f.push_back(HaarNode(+2,x+w,   y+sh2));  

   f.push_back(HaarNode(+1,x,     y+h));  
   f.push_back(HaarNode(-2,x+sw,  y+h));  
   f.push_back(HaarNode(+2,x+sw2, y+h));  
   f.push_back(HaarNode(-1,x+w,   y+h));  

   f.name = "5c";
   return true;
}



/*
 A---B---C---D
 | - | + | - |
 E---F---G---H
 | + | 0 | + |
 I---J---K---L
 | - | + | - |
 M---N---O---P
 
 -(A+F-B-E)+(B+G-C-F)-(C+H-D-G)
 +(E+J-F-I)+(G+L-H-K)-(I+N-J-M)
 +(J+O-K-N)-(K+P-L-O)
 
 -A-F+B+E +B+G-C-F -C-H+D+G +E+J-F-I +G+L-H-K -I-N+J+M +J+O-K-N -K-P+L+O
 
 -A+2*B-2*C+D +2*E-3*F+3*G-2*H -2*I+3*J-3*K+2*L +M-2*N+2*O-P
 
 
*/
bool feature_9c(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<16)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = (2*h)/3;
   int sw2 = (2*w)/3;
   f.reserve(16);
   f.push_back(HaarNode(-1,x,     y));  // A
   f.push_back(HaarNode(+2,x+sw,  y));  
   f.push_back(HaarNode(-2,x+sw2, y));  
   f.push_back(HaarNode(+1,x+w,   y));  

   f.push_back(HaarNode(+2,x,     y+sh)); // E  
   f.push_back(HaarNode(-3,x+sw,  y+sh));  
   f.push_back(HaarNode(+3,x+sw2, y+sh));  
   f.push_back(HaarNode(-2,x+w,   y+sh));  

   f.push_back(HaarNode(-2,x,     y+sh2));  //I
   f.push_back(HaarNode(+3,x+sw,  y+sh2));  
   f.push_back(HaarNode(-3,x+sw2, y+sh2));  
   f.push_back(HaarNode(+2,x+w,   y+sh2));  

   f.push_back(HaarNode(+1,x,     y+h));  //M
   f.push_back(HaarNode(-2,x+sw,  y+h));  
   f.push_back(HaarNode(+2,x+sw2, y+h));  
   f.push_back(HaarNode(-1,x+w,   y+h));  

   f.name = "9c";
   return true;
}


/*
 A---B---C
 | + | - |
 |   D---E
 |       |
 F-------G

 (A+G-C-F)-(2B+2E-2C-2D)
 A-2B+C+2D-2E-F+G
*/

// A-2B+C+2D-2E-F+G
bool feature_2x(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)||(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

    x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-2,x+sw,y));    // B
   f.push_back(HaarNode(+1,x+w,y));    // C
   f.push_back(HaarNode(+2,x+sw,y+sh)); // D
   f.push_back(HaarNode(-2,x+w,y+sh));  // E
   f.push_back(HaarNode(-1,x,y+h));     // F
   f.push_back(HaarNode(+1,x+w,y+h));   // G
   f.name="2x";
   return true;
}

bool feature_2X(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

   x--;y--;

   int sh = 2*(h/3);
   int sw = (w/3);
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-2,x+sw,y));    // B
   f.push_back(HaarNode(+1,x+w,y));    // C
   f.push_back(HaarNode(+2,x+sw,y+sh)); // D
   f.push_back(HaarNode(-2,x+w,y+sh));  // E
   f.push_back(HaarNode(-1,x,y+h));     // F
   f.push_back(HaarNode(+1,x+w,y+h));   // G
   f.name="2X";
   return true;
}

/*
 A---B---C
 | + | - |
 D---E---F
 | 0 | + |
 G---H---I

 (A+E-B-D) - (B+F-C-E) + (E+I-F-H)
 A+E-B-D -B-F+C+E+E+I-F-H
 A-2B+C-D+3E-2F-H+I
 
*/


bool feature_3X(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      

   x--;y--;

   int sh = 2*(h/3);
   int sw = (w/3);
   f.reserve(8);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-2,x+sw,y));    // B
   f.push_back(HaarNode(+1,x+w,y));     // C
   
   f.push_back(HaarNode(-1,x,y+sh));	// D
   f.push_back(HaarNode(+3,x+sw,y+sh)); // E
   f.push_back(HaarNode(-2,x+w,y+sh));  // F
   
//    f.push_back(HaarNode(+1,x,y+h));   // G
   f.push_back(HaarNode(-1,x+sw,y+h));   // H
   f.push_back(HaarNode(+1,x+w,y+h));    // I

   f.name="3X";
   return true;
}

/*
 A---B---C
 | - |   |
 D---E   |
 |     + |
 F-------G

 (A+G-C-F)-2*(A+E-B-D) =
 A+G-C-F-2A-2E+2B+2D
 -A+2B-C+2D-2E-F+G
*/
bool feature_2y(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((w<2)||(h<2)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

   x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(6);
   f.push_back(HaarNode(-1,x,y));       // A
   f.push_back(HaarNode(+2,x+sw,y));    // B
   f.push_back(HaarNode(-1,x+w,y));     // C
   f.push_back(HaarNode(+2,x,y+sh));    // D
   f.push_back(HaarNode(-2,x+sw,y+sh));  // E
   f.push_back(HaarNode(-1,x,y+h));     // F
   f.push_back(HaarNode(+1,x+w,y+h));   // G
   f.name="2y";
   return true;
}

bool feature_2Y(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

   x--;y--;
   int sh = 2*(h/3);
   int sw = 2*(w/3);
   f.reserve(6);
   f.push_back(HaarNode(-1,x,y));       // A
   f.push_back(HaarNode(+2,x+sw,y));    // B
   f.push_back(HaarNode(-1,x+w,y));     // C
   f.push_back(HaarNode(+2,x,y+sh));    // D
   f.push_back(HaarNode(-2,x+sw,y+sh));  // E
   f.push_back(HaarNode(-1,x,y+h));     // F
   f.push_back(HaarNode(+1,x+w,y+h));   // G
   f.name="2y";
   return true;
}

/*
 A---B---C
 | - | + |
 D---E---F
 | + | 0 |
 G---H---I

 -(A+E-B-D) + (B+F-C-E) + (D+H-E-G)

 -A-E+B+D + B+F-C-E + D+H-E-G
 
 -A+2B -C +2D-3E+F -G +H

*/


bool feature_3Y(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      

   x--;y--;

   int sh = 2*(h/3);
   int sw = 2*(w/3);
   f.reserve(8);
   f.push_back(HaarNode(-1,x,y));       // A
   f.push_back(HaarNode(+2,x+sw,y));    // B
   f.push_back(HaarNode(-1,x+w,y));     // C
   
   f.push_back(HaarNode(+2,x,y+sh));	// D
   f.push_back(HaarNode(-3,x+sw,y+sh)); // E
   f.push_back(HaarNode(+1,x+w,y+sh));  // F
   
   f.push_back(HaarNode(-1,x,y+h));   // G
   f.push_back(HaarNode(+1,x+sw,y+h));   // H
//   f.push_back(HaarNode(+1,x+w,y+h));    // I

   f.name="3Y";
   return true;
}

/*
 A-------B
 | +     |
 |   C---D
 |   | - |
 E---F---G

 (A+G-B-E)-2(C+G-D-F)
 A+G-B-E-2C-2G+2D+2F
 A-B-2C+2D-E+2F-G
*/
bool feature_2z(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((w<2)||(h<2)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      
      
   x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-1,x+w,y));     // B
   f.push_back(HaarNode(-2,x+sw,y+sh)); // C
   f.push_back(HaarNode(+2,x+w,y+sh));  // D
   f.push_back(HaarNode(-1,x,y+h));     // E
   f.push_back(HaarNode(+2,x+sw,y+h));  // F
   f.push_back(HaarNode(-1,x+w,y+h));   // G
   f.name="2z";
   return true;
}

/*
 A-------B
 | +     |
 |   C---D
 |   | - |
 E---F---G

 (A+G-B-E)-2(C+G-D-F)
 A+G-B-E-2C-2G+2D+2F
 A-B-2C+2D-E+2F-G
*/
bool feature_2Z(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

   x--;y--;
   int sh = (h/3);
   int sw = (w/3);
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-1,x+w,y));     // B
   f.push_back(HaarNode(-2,x+sw,y+sh)); // C
   f.push_back(HaarNode(+2,x+w,y+sh));  // D
   f.push_back(HaarNode(-1,x,y+h));     // E
   f.push_back(HaarNode(+2,x+sw,y+h));  // F
   f.push_back(HaarNode(-1,x+w,y+h));   // G
   f.name="2Z";
   return true;
}

/*
 A-------B
 |     + |
 C---D   |
 | - |   |
 E---F---G

 (A+G-B-E)-2(C+F-D-E)
 A+G-B-E-2C-2f+2D+2E
 A-B-2C+2D+E-2F+G
*/
bool feature_2w(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)||(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

   x--;y--;
   int sh = h/2;
   int sw = w/2;
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-1,x+w,y));     // B
   f.push_back(HaarNode(-2,x,y+sh));    // C
   f.push_back(HaarNode(+2,x+sw,y+sh));  // D
   f.push_back(HaarNode(+1,x,y+h));     // E
   f.push_back(HaarNode(-2,x+sw,y+h));  // F
   f.push_back(HaarNode(+1,x+w,y+h));   // G
   f.name="2w";
   return true;
}

/*
 A-------B
 |     + |
 C---D   |
 | - |   |
 E---F---G

 (A+G-B-E)-2(C+F-D-E)
 A+G-B-E-2C-2f+2D+2E
 A-B-2C+2D+E-2F+G
*/
bool feature_2W(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<6)
	return false;
#endif      

   x--;y--;
   int sh = h/3;
   int sw = 2*(w/3);
   f.reserve(6);
   f.push_back(HaarNode(+1,x,y));       // A
   f.push_back(HaarNode(-1,x+w,y));     // B
   f.push_back(HaarNode(-2,x,y+sh));    // C
   f.push_back(HaarNode(+2,x+sw,y+sh));  // D
   f.push_back(HaarNode(+1,x,y+h));     // E
   f.push_back(HaarNode(-2,x+sw,y+h));  // F
   f.push_back(HaarNode(+1,x+w,y+h));   // G
   f.name="2W";
   return true;
}

/*
 A---B---C---D
 | + | - | + |
 |   E---F   |
 |           |
 G-----------H

 A+H-D-G-2(B+F-C-E)
 A-2B+2C-D+2E-2F-G+H
*/
bool feature_2u(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/2;
   int sw = w/3;
   int sw2 = (2*w)/3;
   f.reserve(8);
   f.push_back(HaarNode(+1,x,y));      // A
   f.push_back(HaarNode(-2,x+sw,y));    // B
   f.push_back(HaarNode(+2,x+sw2,y));    // G
   f.push_back(HaarNode(-1,x+w,y));  // H

   f.push_back(HaarNode(+2,x+sw,y+sh));    // C
   f.push_back(HaarNode(-2,x+sw2,y+sh));   // F

   f.push_back(HaarNode(-1,x,y+h));   // D
   f.push_back(HaarNode(+1,x+w,y+h));  // E

   f.name = "2u";
   return true;
}

/*
 A-----------B
 |     -     |
 C---D   E---F
 | + | - | + |
 |   G---H   |
 |           |
 I-----------J

 -(A+F-C-B)
 +(C+J-F-I)
 -2*(H+D-E-G)
 
 -A-F+C+B+C+J-F-I
 -2H-2D+2E+2G

*/
bool feature_2U(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sh2 = 2*(h/3);
   int sw = w/3;
   int sw2 = (2*w)/3;
   f.reserve(8);
   f.push_back(HaarNode(-1,x,y));      // A
   f.push_back(HaarNode(+1,x+w,y));    // B

   f.push_back(HaarNode(+2,x,    y+sh));  // C
   f.push_back(HaarNode(-2,x+sw, y+sh));  // D
   f.push_back(HaarNode(+2,x+sw2,y+sh));  // E
   f.push_back(HaarNode(-2,x+w,  y+sh));  // F

   f.push_back(HaarNode(+2,x+sw, y+sh2));  // G
   f.push_back(HaarNode(-2,x+sw2,y+sh2));   // H

   f.push_back(HaarNode(-1,x,  y+h));    // I
   f.push_back(HaarNode(+1,x+w,y+h));  // J

   f.name = "2U";
   return true;
}

/*
 A-----------B
 |     +     |
 |   C---D   |
 |   | - |   |
 E---F---G---H

 A+H-B-E - 2 (C+G-D-F)

*/
bool feature_2n(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<2))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%2)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/2;
   int sw = w/3;
   int sw2 = (2*w)/3;
   f.reserve(8);
   f.push_back(HaarNode(+1,x,y));      // A
   f.push_back(HaarNode(-1,x+w,y));    // B

   f.push_back(HaarNode(-2,x+sw,y+sh));    // C
   f.push_back(HaarNode(+2,x+sw2,y+sh));  // D

   f.push_back(HaarNode(-1,x,y+h));    // E

   f.push_back(HaarNode(+2,x+sw,y+h));   // F
   f.push_back(HaarNode(-2,x+sw2,y+h));  // G

   f.push_back(HaarNode(+1,x+w,y+h));  // H

   f.name = "2n";
   return true;
}

/*
 A-----------B
 |     +     |
 |   C---D   |
 |   |   |   |
 E---F   G---H
 |     -     |
 I-----------J

 A+H-B-E - 2 (C+G-D-F)
 - (E+J-H-I)

*/
#if 0
bool feature_2N(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((w<3)||(h<3)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
   x--;y--;
   int sh = h/3;
   int sh2 = 2*(h/3);
   int sw = w/3;
   int sw2 = (2*w)/3;
   f.reserve(10);
   f.push_back(HaarNode(+1,x,  y));     // A
   f.push_back(HaarNode(-1,x+w,y));     // B

   f.push_back(HaarNode(-2,x+sw, y+sh));  // C
   f.push_back(HaarNode(+2,x+sw2,y+sh));  // D

   f.push_back(HaarNode(-2,x,    y+sh2)); // E
   f.push_back(HaarNode(+2,x+sw, y+sh2)); // F
   f.push_back(HaarNode(-2,x+sw2,y+sh2)); // G
   f.push_back(HaarNode(+2,x+w,  y+sh2)); // H

   f.push_back(HaarNode(+1,x,   y+h));  // I
   f.push_back(HaarNode(-1,x+w, y+h));  // J

   f.name = "2N";
   return true;
}
#endif
/*
 A-------B    
 |       |    
 |   C---D    
 | + | - |  
 |   E---F    
 |       |    
 G-------H    

 A+H-B-G
 -2(C+F-D-E)

*/
bool feature_2l(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sh2 = 2*(h/3);
   int sw = w/2;
   f.reserve(8);
   f.push_back(HaarNode(+1,x,    y));    // A
   f.push_back(HaarNode(-1,x+w,  y));    // B

   f.push_back(HaarNode(-2,x+sw,    y+sh));    // C
   f.push_back(HaarNode(+2,x+w,  y+sh));    // D

   f.push_back(HaarNode(+2,x+sw,    y+sh2));    // E
   f.push_back(HaarNode(-2,x+w,  y+sh2));    // F

   f.push_back(HaarNode(-1,x,    y+h));    // G
   f.push_back(HaarNode(+1,x+w,  y+h));    // H

   f.name = "2l";
   return true;
}

/*
 A-------B---C
 |       |   |
 |   D---E   |
 | + |     - |
 |   F---G   |
 |     - |   |
 H-------I---J

 A+I-B-H
 -2(D+G-E-F)
 -(B+J-C-I)

 A+I-B-H -2D-2G+2E+2F
 -B-J+C+I

*/
bool feature_2L(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<10)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sh2 = 2*(h/3);
   int sw = w/3;
   int sw2 = (2*w)/3;
   f.reserve(10);
   f.push_back(HaarNode(+1,x,    y));    // A
   f.push_back(HaarNode(-2,x+sw2,y));    // B
   f.push_back(HaarNode(+1,x+w,  y));    // C

   f.push_back(HaarNode(-2,x+sw, y+sh));    // D
   f.push_back(HaarNode(+2,x+sw2,y+sh));   // E

   f.push_back(HaarNode(+2,x+sw, y+sh2));    // F
   f.push_back(HaarNode(-2,x+sw2,y+sh2));   // G

   f.push_back(HaarNode(-1,x,    y+h));    // H
   f.push_back(HaarNode(+2,x+sw2,y+h));    // I
   f.push_back(HaarNode(-1,x+w,  y+h));    // J

   f.name = "2L";
   return true;
}

/*
 A-------B
 |       |
 C---D   |
 | - | + |
 E---F   |
 |       |
 G-------H

 A+H-B-G
 -2(C+F-D-E)

*/
bool feature_2r(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if ((w<2)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%2)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sh2 = 2*(h/3);
   int sw = w/2;
   f.reserve(8);

   f.push_back(HaarNode(+1,x,    y));    // A
   f.push_back(HaarNode(-1,x+w,  y));    // B

   f.push_back(HaarNode(-2,x,    y+sh));    // C
   f.push_back(HaarNode(+2,x+sw,  y+sh));    // D

   f.push_back(HaarNode(+2,x,    y+sh2));    // E
   f.push_back(HaarNode(-2,x+sw,  y+sh2));    // F

   f.push_back(HaarNode(-1,x,    y+h));    // G
   f.push_back(HaarNode(+1,x+w,  y+h));    // H

   f.name = "2r";
   return true;
}

/*
 A---B-------C
 |   |       |
 |   D---E   |
 | +     | - |
 |   F---G   |
 |   | -     |
 H---I-------J

 A+I-B-H
 -(B+J-C-I)
 +2(D+G-E-F)

 A+I-B-H-B-J+C+I

*/
#if 0
bool feature_2R(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((w<3)||(h<3)||(x==0)||(y==0))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
   x--;y--;
   int sh = h/3;
   int sh2 = 2*(h/3);
   int sw = w/3;
   int sw2 = (2*w)/3;
   f.reserve(10);
   f.push_back(HaarNode(+1,x,   y));    // A
   f.push_back(HaarNode(-2,x+sw,y));    // B
   f.push_back(HaarNode(+1,x+w, y));    // C

   f.push_back(HaarNode(+2,x+sw,y+sh));    // D
   f.push_back(HaarNode(-2,x+sw2,y+sh));   // E

   f.push_back(HaarNode(-2,x+sw, y+sh2));    // F
   f.push_back(HaarNode(+2,x+sw2,y+sh2));   // G

   f.push_back(HaarNode(-1,x,   y+h));    // H
   f.push_back(HaarNode(+2,x+sw,y+h));    // I
   f.push_back(HaarNode(-1,x+w, y+h));    // J

   f.name = "2R";
   return true;
}
#endif

/*
 A---B---C---D
 | - | + | - |
 E---F---G---H
 | + | - | + |
 I---J---K---L
 | - | + | - |
 M---N---O---P

 (A+P-D-M)
 -2(A+F-B-E)
 -2(C+H-D-G)
 -2(F+K-G-J)
 -2(I+N-J-M)
 -2(K+P-L-O)
 
*/
bool feature_9d(HaarFeature & f, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if((w<3)||(h<3))
      return false;
#ifdef CONSERVATIVE
    if ( ((w%3)!=0) || ((h%3)!=0) )
      return false;
#endif
#ifdef PRESERVATIVE
      if(w*h<16)
	return false;
#endif      
      
   x--;y--;
   int sh = h/3;
   int sw = w/3;
   int sh2 = (2*h)/3;
   int sw2 = (2*w)/3;
   f.reserve(16);
   f.push_back(HaarNode(-1,x,     y));  
   f.push_back(HaarNode(+2,x+sw,  y));  
   f.push_back(HaarNode(-2,x+sw2, y));  
   f.push_back(HaarNode(+1,x+w,   y));  

   f.push_back(HaarNode(+2,x,     y+sh));   // E
   f.push_back(HaarNode(-4,x+sw,  y+sh));   // F
   f.push_back(HaarNode(+4,x+sw2, y+sh));   // G
   f.push_back(HaarNode(-2,x+w,   y+sh));   // H

   f.push_back(HaarNode(-2,x,     y+sh2));  // I
   f.push_back(HaarNode(+4,x+sw,  y+sh2));  // J
   f.push_back(HaarNode(-4,x+sw2, y+sh2));  // K
   f.push_back(HaarNode(+2,x+w,   y+sh2));  // L

   f.push_back(HaarNode(+1,x,     y+h));  
   f.push_back(HaarNode(-2,x+sw,  y+h));  
   f.push_back(HaarNode(+2,x+sw2, y+h));  
   f.push_back(HaarNode(-1,x+w,   y+h));  

   f.name = "9d";
   return true;
}


bool feature_mv(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (h<1)
      return false;
    if((y>=(ph/2))||(h>=(ph/2)))
      return false;
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
	
   x--;y--;
   
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B
   out.push_back(HaarNode(-1,x,y+h));     // C
   out.push_back(HaarNode(+1,x+w,y+h));   // D

   out.push_back(HaarNode(-1,x,  ph-2-y-h));   	 // E
   out.push_back(HaarNode(+1,x+w,ph-2-y-h));	 // F
   out.push_back(HaarNode(+1,x,  ph-2-y));      // G
   out.push_back(HaarNode(-1,x+w,ph-2-y));    // H
   out.name = "mv";
   
   return true;
}


bool feature_mh(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (w<1)
      return false;
    if((x>=pw/2) || (w>=pw/2))
      return false;
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B
   out.push_back(HaarNode(-1,x,y+h));     // C
   out.push_back(HaarNode(+1,x+w,y+h));   // D

   out.push_back(HaarNode(-1,pw-2-x-w,y));       // A
   out.push_back(HaarNode(+1,pw-2-x,y));     // B
   out.push_back(HaarNode(+1,pw-2-x-w,y+h));     // C
   out.push_back(HaarNode(-1,pw-2-x,y+h));   // D
   out.name = "mh";
   
   return true;
}

/* ASPECT RATIO: 1/2
   A---B
   | + |
   C---D
 --- 0---
   E---F
   | - |
   G---H
 */
bool feature_P1(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (h<2)
      return false;
    if(y+h+pw>=ph)
      return false;
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B
   out.push_back(HaarNode(-1,x,y+h));    // C
   out.push_back(HaarNode(+1,x+w,y+h));  // D

   out.push_back(HaarNode(-1,x,y+pw));   // E
   out.push_back(HaarNode(+1,x+w,y+pw)); // F
   out.push_back(HaarNode(+1,x,y+h+pw));     // G
   out.push_back(HaarNode(-1,x+w,y+h+pw));   // H
   out.name = "P1";
   return true;
}

//  ASPECT RATIO: 1/3
//  A-C
bool feature_P2(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (h<2)
      return false;
    if(y+h+2*pw>=ph)
      return false;
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y));       // A
   out.push_back(HaarNode(-1,x+w,y));     // B
   out.push_back(HaarNode(-1,x,y+h));    // C
   out.push_back(HaarNode(+1,x+w,y+h));  // D

   out.push_back(HaarNode(-1,x,y+2*pw));   // E
   out.push_back(HaarNode(+1,x+w,y+2*pw)); // F
   out.push_back(HaarNode(+1,x,y+h+2*pw));     // G
   out.push_back(HaarNode(-1,x+w,y+h+2*pw));   // H
   out.name = "P2";
   return true;
}

//  ASPECT RATIO: 1/3
// B-C
bool feature_P3(HaarFeature & out, int x, int y, int w, int h, unsigned int pw, unsigned int ph)
{
    if (h<2)
      return false;
    if(y+h+2*pw>=ph)
      return false;
#ifdef PRESERVATIVE
      if(w*h<8)
	return false;
#endif      
      
   x--;y--;
   
   out.reserve(8);
   out.push_back(HaarNode(+1,x,y+pw));       // A
   out.push_back(HaarNode(-1,x+w,y+pw));     // B
   out.push_back(HaarNode(-1,x,y+h+pw));    // C
   out.push_back(HaarNode(+1,x+w,y+h+pw));  // D

   out.push_back(HaarNode(-1,x,y+2*pw));   // E
   out.push_back(HaarNode(+1,x+w,y+2*pw)); // F
   out.push_back(HaarNode(+1,x,y+h+2*pw));     // G
   out.push_back(HaarNode(-1,x+w,y+h+2*pw));   // H
   out.name = "P3";
   return true;
}

///////////// list of features name and callback //////////////
const FeatureDescription featureDescription[]={
   {"2h", feature_2h},
   {"2v", feature_2v},
   {"3h", feature_3h},
   {"3v", feature_3v},
   {"4q", feature_4q}, 
   {"2c", feature_2c},
   {"2x", feature_2x},
   {"2X", feature_2X},
   {"2y", feature_2y},
   {"2Y", feature_2Y},
   {"2z", feature_2z},
   {"2Z", feature_2Z},
   {"2w", feature_2w},
   {"2W", feature_2W},
   {"2u", feature_2u},
   {"2U", feature_2U},
   {"2n", feature_2n},
   {"3H", feature_3H}, 
   {"3V", feature_3V},
   {"5d", feature_5d},
   {"5D", feature_5D},
   {"5c", feature_5c},
   {"2l", feature_2l},
   {"2L", feature_2L},
   {"2r", feature_2r},
   {"2m", feature_2m},
   {"2p", feature_2p},
   {"2s", feature_2s},
   {"2t", feature_2t},
   {"9d", feature_9d},
   {"4v", feature_4v},
   {"4h", feature_4h},
   {"9c", feature_9c},
// Hole IsoEnergetic Feature
   {"bv", feature_bv},
   {"bh", feature_bh},
   {"dq", feature_dq},
   {"ba", feature_ba},
   {"bb", feature_bb},
   {"3X", feature_3X},
   {"3Y", feature_3Y},
// Specular IsoEnergetic features
   {"mv", feature_mv},
   {"mh", feature_mh},
// MultiChannel (should be not used normally)
   {"P1", feature_P1},
   {"P2", feature_P2},
   {"P3", feature_P3},
   {NULL}
   };

//// all
const char * BaseHaarFeatureGenerator::default_pattern="2h2v3h3v4q2c2x2X2y2Y2z2Z2w2W2u2U2n3H3V5d5D5c2l2L2r2m2p2s2t9d9cbvbhdqbabb3X3Y4v4hmvmhP1P2P3";

const char iso_pattern[]="2v2h3V3H4qbvbhbabbdq4v4h9c3X3Ymvmh";

const char standard_pattern[]="2v2h3V3H4q";


//// strip string 
const FeatureDescription *findFeature(const char *str)
{
for(const FeatureDescription * i = featureDescription; (i->name); i++)
  {
  if(!memcmp(str, i->name,2))
    return i;
  }
return NULL;
}

bool BaseHaarFeatureGenerator::generate(HaarFeature & f, int x, int y, int w, int h, int idx)
{
  return m_feats[idx](f,x,y,w,h, m_mw, m_mh);
}

void BaseHaarFeatureGenerator::SetGeometry(int mw, int mh)
{
//   std::cout << "SetGeometry(" << mw << 'x' << mh << ")" << std::endl;
  m_mw= mw; m_mh = mh;
}
  
void BruteForceFeatureGenerator::Reset()
{
  m_featCount = 0;

  if(m_features.empty())
  {
  unsigned int idx;
  unsigned int i,j;
  unsigned int w,h;
  idx = i = j = 0; 
  const int min_width = std::max(m_step, m_minWidth);
  w = h = min_width; 
  std::cout << "Precompute features...." << std::endl;
    
  for(;;)
  {
  HaarFeature out;
  bool ret = false;
  do {

  // check constraint on area and width (constraing on minWidth are automatically resolved by construction)
  if((w*h>=m_minFeatArea) /* && (w>=m_minWidth) && (h>=m_minWidth) */ )
  {
//     std::cout << i << ' ' << j << ' ' << w << ' ' << h << ' ' << idx << std::endl;
      ret = generate(out, i, j, w, h, idx);
//       if(1)
// 	{
// 	char buffer[1024];
// 	sprintf(buffer, "%s-%ux%u-%ux%u", out.name, i,j,w,h,idx);
// 	out.name = new char [strlen(buffer)+1];
// 	strcpy(out.name, buffer);
// 	}
  }

  idx++;
  if(idx>=m_feats.size() )
    {
    idx = 0;
    w+=m_step;
    if(i+w>m_mw)
      {
      w = min_width;
      h+=m_step;
      if(j+h>m_mh)
	{
	h = min_width;
	i+=m_step;
	if(i + min_width > m_mw)
	  {
	  i = 0;
	  j+=m_step;
	  if(j + min_width > m_mh)
	    {
            j = 0;
	    std::cout << m_features.size() << " features precomputed" << std::endl;
	    return;
	    }
	  }
	
	}

      }
    }
  } while(!ret);
  
  m_features.push_back(out);
  }
  
  }
}

BaseHaarFeatureGenerator::BaseHaarFeatureGenerator(const char *pattern) : m_mw(1), m_mh(1) , m_minFeatArea(1), m_minWidth(1), m_step(1)
{ 
  while(*pattern)
    {
    const FeatureDescription *f = findFeature(pattern);
    if (f==NULL)
	throw "Feature Unknown";
    m_feats.push_back(f->proc);
    pattern+=2;
    }

  }

bool BruteForceFeatureGenerator::Next(HaarFeature & out)
{
if(m_featCount>=m_features.size())
  return false;

out = m_features[m_featCount];
m_featCount++;
return true;
}

  
BruteForceFeatureGenerator::~BruteForceFeatureGenerator() { }

///////////////////////////  
  
#include <cstdlib>
#include <time.h> // time()

RandomFeatureGenerator::~RandomFeatureGenerator() { }

void RandomFeatureGenerator::Reset()
{
  m_cur=0;
  srand(time(NULL));
}

bool RandomFeatureGenerator::Next(HaarFeature & out)
{
   out.clear();
   
  if(m_cur>=m_featRand)
      return false;
  int i,j,w,h,idx;
  
  int minWidth = (m_minWidth + m_step - 1)/m_step; // quantized m_minWidth
  int i_range = (W() / m_step) - minWidth;
  int j_range = (H() / m_step) - minWidth;
  
  do {
    
  // iterate looking for m_minFeatArea
  do{
    i = (rand() % i_range) * m_step;//-1
    j = (rand() % j_range) * m_step;//-1
    int wLimit = ((W() - i) / m_step);
    int hLimit = ((H() - j) / m_step);
    
    if(wLimit > 0)
      w = (rand() % wLimit ) * m_step;
    else 
      w = 0;
    
    if(hLimit > 0)
      h =  (rand() % hLimit ) * m_step;
    else
      h = 0;
    
  } while ( (w*h < m_minFeatArea) || (w<m_minWidth) || (h<m_minWidth) );
  
  idx = rand() % Feature();
  } while (!generate(out, i, j, w, h, idx));
  m_cur++;
  return true;
}



void JointSparseGranularFeatureGenerator::Reset()
{
  m_cur=0;  
  srand(time(NULL));
}

/** ritorna la risposta del rettangolo (x0,y0)-(x0+w-1,y0+h-1) estremi COMPRESI */
void push_rect(HaarFeature & out, int x0, int y0, int w, int h, int sign)
{
  out.push_back(HaarNode(sign, x0-1,y0-1)); // A
  out.push_back(HaarNode(sign, x0+w-1,y0+h-1)); // B
  out.push_back(HaarNode(-sign, x0-1,y0+h-1)); // C
  out.push_back(HaarNode(-sign, x0+w-1,y0-1)); // D
}

bool JointSparseGranularFeatureGenerator::Next(HaarFeature & out)
{
   out.clear();
   
  if(m_cur>=m_featRand)
      return false;
  
  // TODO: throw exception if m_minArea>width*height
      
   int i,j,w,h,n;
   n = rand() % 3;

    do {
      w = 1+(rand() % (W()/2));
      h = 1+(rand() % (H()/2));
    } while((w*h)<m_minArea);
    
   if(n == 0)
    {
    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, +1);
   
    if((w<H()) && (h<W()) && (rand() & 1))
      {
      std::swap(w,h);
      out.name = "jsgf2x";
      }
    else
      {
      out.name = "jsgf2";
      }

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, -1);
    }
    else
   if(n == 1)      
    {
    w=w/2;
    if(w<1) w=1;

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, +1);

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, +1);
   
    w*=2;
    
    if((w<H()) && (h<W()) && (rand() & 1))
      {
      std::swap(w,h);
      out.name = "jsgf3hx";
      }
    else
      {
      out.name = "jsgf3h";
      }

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, -1);
      
    }
    else
   if(n == 2)      
    {
    h=h/2;
    if(h<1) h=1;

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, +1);

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, +1);
   
    h*=2;
    
     if((w<H()) && (h<W()) && (rand() & 1))	
     {
      std::swap(w,h);
      out.name = "jsgf3vx";
      }
    else
      {
      out.name = "jsgf3v";
      }

    i = rand() % (W()-w);
    j = rand() % (H()-h);
    push_rect(out, i,j, w, h, -1);
      
    }
  m_cur++;
  return true;  
}

JointSparseGranularFeatureGenerator::~JointSparseGranularFeatureGenerator() { }
