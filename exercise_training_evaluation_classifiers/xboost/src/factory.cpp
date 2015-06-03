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

#include "factory/allocate.h"
#include "factory/register.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>

typedef std::map<std::string, std::pair<ClassifierFactory, DetectorFactory> > MapType;

static MapType & instance()
{
  static MapType factory;
  
  return factory;
}

void print_classifier_list(std::ostream & out)
{
 MapType & factory = instance();
    std::cerr << "available classifiers are: ";
    for(MapType::const_iterator i = factory.begin(); i!=factory.end();++i)
      std::cerr << i->first << ' ';
    std::cout << std::endl;
}

ImageClassifier *allocate_classifier(const char *name, std::istream &in)
{
  MapType & factory = instance();
  MapType::iterator i = factory.find(name);
  
  //std::cout << factory.size()<<endl;
  if(i!=factory.end())
  {
   return (*i->second.first)(in);
  }
  else
  {
    std::cerr << "classifier " << name << " not found" << std::endl;
    print_classifier_list(std::cerr);
    return 0;
  }
}

ObjectDetector *allocate_detector(const char *name, const char * filename)
{
  MapType & factory = instance();
  MapType::iterator i = factory.find(name);
  
  if(i!=factory.end())
  {
   return (*i->second.second)(filename);
  }
  else
  {
    std::cerr << "classifier " << name << " not found" << std::endl;
    print_classifier_list(std::cerr);
    return 0;
  }
}


bool register_classifier_factory(const std::string & name, ClassifierFactory proc, DetectorFactory proc2)
{
  MapType & factory = instance();
  factory.insert(std::pair<std::string, std::pair<ClassifierFactory, DetectorFactory> >(name, std::make_pair(proc, proc2) ) );
  return true;
}


ImageClassifier *allocate_classifier_from_file(const char *filename)
{
  std::ifstream in(filename);
  if(in)
  {
	  std::string code;
	  in >> code; // load code
	  return allocate_classifier(code.c_str(), in);
  }
  else
  {
	  std::cerr << "Error: File " << filename << " is not readable. Cannot allocate ImageClassifier." << std::endl;
	  return 0;
  }
}

ObjectDetector *allocate_detector_from_file(const char *filename)
{
  std::ifstream in(filename);
  if(in)
  {
          std::string code;
          in >> code; // load code
          return allocate_detector(code.c_str(), filename);
  }
  else
  {
          std::cerr << "Error: File " << filename << " is not readable. Cannot allocate ObjectDetector." << std::endl;
          return 0;
  }
}

