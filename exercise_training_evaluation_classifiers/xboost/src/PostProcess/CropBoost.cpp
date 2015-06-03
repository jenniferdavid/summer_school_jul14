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

#include <fstream>

#include <cmath> // abs
#include <cstring>

// #include "factory.h"

#ifndef _MSC_VER
# include <stdint.h>
#else
typedef unsigned int uint32_t;
#endif

#include "IO/pnmio.h"
#include "IntegralImage.h"
#include "HaarFeatureTools.h"
#include "Utility/Utils.h"

#include "HaarClassifiers.h"
#include "Classifier/BoostClassifier.h"

// global parameters

static const char *network_file = NULL;
static const char *output_file = NULL;

static int n_classifiers = -1;
static bool resort = false;
static bool verbose = false;

static void help()
{
std::cout << "Usage: process crop <input boost file> [OPTIONS]\n\n";
std::cout << "-h\tthis help\n";
std::cout << "-v\tverbose output\n";
std::cout << "-o <file>\tOutput file name\n";
std::cout << "-n <n>\tKeep only first n classifier\n";
std::cout << "-s\tResort before crop\n";
std::cout.flush();
}

static void parse_cmd_line(int argc, const char *argv[])
{
  // Parse Command Line
  if(argc < 3)
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
    else
    if(!strcmp(str, "-v") )
      {
      verbose = true;
      }
    else
    if(!strcmp(str, "-o") )
      {
      i++;
      output_file = argv[i];
      }
    else
    if(!strcmp(str, "-s") )
      {
	resort = true;
      }
    else
    if(!strcmp(str, "-n") )
      {
	 i++;
      n_classifiers = atoi(argv[i]);
      }
    else
    if(network_file == NULL)
	network_file = str;
    else
	{
	help();
	exit(0);
	}
     i++;
     }
}

void crop(int argc, const char *argv[])
{
  parse_cmd_line(argc, argv);
  
  if(network_file == 0)
  {
    help();
    return;
  }
  

  BoostClassifier<HaarClassifier> c;
  unsigned int width, height;
  load_classifier(network_file, c, width, height); // TODO
  
  if(resort) 
     c.sort();
  if(n_classifiers!=-1) 
     c.resize(n_classifiers);
  if(output_file)
    save_classifier(output_file, c, width, height, NULL); // TODO

}
