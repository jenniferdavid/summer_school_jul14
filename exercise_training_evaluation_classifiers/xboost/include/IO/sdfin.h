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

#ifndef _SDF_IN_H
#define _SDF_IN_H

/** @file sdfin.h
 * @brief Set Description File importer class sdfin
 * */

#include "datasetin.h"
#include <fstream>
#include <cstdlib>
#include <ctime>

/// a SDF file reader
class sdfin: public datasetin {
  std::vector< datasetitem > m_list;
  int read_point;
public:
  sdfin(const char *filename);
  virtual ~sdfin();

   virtual bool next(datasetitem & item);
};


#endif
