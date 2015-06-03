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

#ifndef _CLASSIFIER_UTILS_H
#define _CLASSIFIER_UTILS_H

#include <fstream>
#include <stdexcept>
#include <iostream>

/** @file Utils.h
  * @brief this file implements some usefull API for deal with classifier
  **/


/** Load classifiers from file (it is possible to use also allocate_classifier_from_file)
 * \code
 * BoostClassifier<HaarCascadeClassifier> c;
 * load(network_file, c);
 * \endcode
 **/
template<class Classifier>
void load_classifier(const char *network_file, Classifier & dst,  unsigned int & width, unsigned int & height, char* strconf);


/** Load classifiers from file (it is possible to use also allocate_classifier_from_file)
 * \code
 * BoostClassifier<HaarCascadeClassifier> c;
 * load(network_file, c);
 * \endcode
 **/
template<class Classifier>
void load_classifier(const char *network_file, Classifier & dst,  unsigned int & width, unsigned int & height);

/** Save classifiers to file
 * \code
 * save(output_file, c, 32, 32);
 * \endcode
 **/
template<class Classifier>
void save_classifier(const char *network_file, const Classifier & src, unsigned int width, unsigned int height, const char* config);

////////////////////////////


template<class Classifier>
void load_classifier(const char *network_file, Classifier & dst, unsigned int & width, unsigned int & height)
{
std::ifstream in(network_file);
if(in)
  {
  std::string signature;
  in >> signature >> width >> height;
  if(signature != dst.signature())
    {
    std::cerr << "expected " << dst.signature() << ". Readed " << signature << std::endl;
    throw std::runtime_error("wrong signature");
    }
  in >> dst;
  }
  else
  {
    std::cerr << network_file << " not valid" << std::endl;
    throw std::runtime_error("file not valid");
  }
}

template<class Classifier>
void load_classifier(const char *network_file, Classifier & dst, unsigned int & width, unsigned int & height, char* strconf)
{
std::ifstream in(network_file);
if(in)
  {
  std::string signature;
  in >> signature >> width >> height;
  if(signature != dst.signature())
    {
    std::cerr << "expected " << dst.signature() << ". Readed " << signature << std::endl;
    throw std::runtime_error("wrong signature");
    }
  in >> dst;
  }
  else
  {
    std::cerr << network_file << " not valid" << std::endl;
    throw std::runtime_error("file not valid");
  }
}


template<class Classifier>
void save_classifier(const char *network_file, const Classifier & src, unsigned int width, unsigned int height, const char* config)
{
std::ofstream out(network_file);
if(config)
  out << src.signature() << ' ' << width << ' ' << height << ' ' << config << '\n' << src;
else
  out << src.signature() << ' ' << width << ' ' << height << '\n' << src; // TODO: insert a "fake" config as, for example, '-' ?
}

#endif
