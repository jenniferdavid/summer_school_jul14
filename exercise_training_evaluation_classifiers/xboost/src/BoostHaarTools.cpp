/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "xbversion.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

/** @file BoostHaarTools.cpp
 * @brief Main file for post-process adaboost training
 * */

/////
void softcascade(int argc, const char *argv[]);

/////
void crop(int argc, const char *argv[]);


struct list_type {
  const char *name;
  void (* callback)(int, const char *[]);
};


static const list_type  list[] = {
  {"crop", crop},
  {"softcascade", softcascade},
  {0,0}
};


//////////////////////////////////////////////////////////////////

static void help()
{
std::cout << "Usage: process <operation> [OPTIONS]\n\n";
std::cout << "for operation help type process help <operation>\n";
std::cout << "operation can be: crop, softcascade\n";
std::cout.flush();
}

int main(int argc, const char *argv[])
{
std::cout << "xBoost Post Processor " XBOOST_VERSION << std::endl;

try {

if(argc<=1)
{
  help();
  return 0;
}

if(!strcmp(argv[1], "help"))
{
  if(argc<=2)
  {
    help();
    return 0;
  }
 for(const list_type * i = list; (i->name); ++ i)
    if(!strcmp(argv[2], i->name))
    {
      i->callback(0, 0);
      return 0;
    }
   
}
else
{
  for(const list_type * i = list; (i->name); ++ i)
    if(!strcmp(argv[1], i->name))
    {
      i->callback(argc-1, argv+1);
      return 0;
    }
  help();
}

}
catch(std::exception & e)
{
  std::cout << e.what() << std::endl;
}

return 0;
}
