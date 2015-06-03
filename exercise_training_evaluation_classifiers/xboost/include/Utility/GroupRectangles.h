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

#ifndef _GROUP_RECTANGLES_H
#define _GROUP_RECTANGLES_H

// TODO: questa libreria deve essere generalizzata

/** @file GroupRectangles.h
 * @brief method to merge overlapping boxes
 * */

#include "Types.h"
#include <algorithm>
#include <iterator>
#include <vector>

//////

#define gr_verbose  false


/// metodo per segnare come non-candidati i rettangoli sovrapposti piu' di una determinata soglia
void groupRectanglesOverlapped(std::vector<Candidates>::iterator first, std::vector<Candidates>::iterator last, float m_groupThreshold )
{
	for(std::vector<Candidates>::iterator i1=first; i1!=last; ++i1)
 		i1->grouped = false;

	std::vector<float> overlapped;
	overlapped.reserve((std::distance(first, last)*(std::distance(first, last)-1))/2);

	// calcolo tutte le sovrapposizioni (tutti contro tutti: non faccio la matrice quadrata dato che e simmetrica, mi basta il triangolo inferiore)
	for(std::vector<Candidates>::iterator i1=first; i1!=last; ++i1)
		for(std::vector<Candidates>::iterator i2=i1; ++i2!=last; ) {
			//calcolo overlap
			double rect1_area = i1->box.width()*i1->box.height();
            double rect2_area = i2->box.width()*i2->box.height();

            if( std::min(rect1_area, rect2_area) > 0.3 * std::max(rect1_area, rect2_area) )
                overlapped.push_back( boxoverlap(i1->box, i2->box, Overlap_On_Min ) );
            else
                overlapped.push_back( boxoverlap(i1->box, i2->box, Overlap_On_Max ) );
		}

	// quando la percentuale di sovrapposizione supera la soglia..
	//	..e almeno uno dei due non e stato ancora raggruppato
	// li raggruppo
	std::vector<float>::iterator area = overlapped.begin();
	for(std::vector<Candidates>::iterator i1=first; i1!=last; ++i1)
		for(std::vector<Candidates>::iterator i2=i1; ++i2!=last; ++area)
			if(*area > m_groupThreshold && (!i1->grouped || !i2->grouped))
			{
				if(i1->response < i2->response)
					i1->grouped = true;
				else
					i2->grouped = true;
			}
}

/// metodo per segnare come non-candidati i rettangoli sovrapposti piu' di una determinata soglia
void groupRectanglesOverlapped(std::vector<Candidates> & out, float m_groupThreshold )
{
	groupRectanglesOverlapped(out.begin(), out.end(), m_groupThreshold);
}

#endif
