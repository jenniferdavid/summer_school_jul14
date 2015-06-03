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

// TODO: poter usare questo file con tutti i tipi di feature


#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "FeatureGenerator/HaarFeatureGenerators.h"

#include "HaarClassifiers.h"
#include "Classifier/BoostClassifier.h"

#include "Utility/Utils.h"
#include "IO/pnmio.h"
#include "HaarFeatureTools.h"
#include "DataSetUtils.h"

#include "Oracle/BinaryClassifierOracle.h"
#include "Oracle/RealDecisionStumpOracle.h"
#include "Oracle/BayesianStumpOracle.h"
#include "Oracle/NaiveDecisionStumpOracle.h"

#include "factory/allocate.h"

#include "Classifier/BoostClassifier.h"
#include "Classifier/BoostableClassifier.h"

#include "IchnClassifiers.h"



template<class T>
void dump_feature(const T & classifier, unsigned int width, unsigned int height)
{
  int count = 1;
  for(typename T::ClassifierListType::const_iterator i = classifier.list().begin(); i != classifier.list().end(); ++i)
  {
    char name[64];
    sprintf(name, "%03u.pgm", count);
    DumpFeatureImg(name, *i, width, height);
    count++;
  }  
}




//TODO
template<class T>
void dump_classifier_icf(T & classifier, unsigned int width, unsigned int height, int nbin){

	nbin +=2;//lm
	double sum_alpha;
	double* channels = new double[width*height*nbin];
	memset(channels,0,width*height*nbin);

	for(typename T::ClassifierListType::const_iterator cl = classifier.list().begin(); cl!= classifier.list().end();cl++ ){
		int offset_channel = cl->channel*width*height;
		for(int i=cl->y0;i<cl->y1;i++ ){
			for(int j=cl->x0;j<cl->x1;j++){
				channels[offset_channel+i*width +j]+=cl->alpha;
			}
		}

	}
	//norm max element?
	double max_el = 0;
	for(int i=0;i<width*height*nbin;i++)
		if(channels[i]>max_el)
			max_el=channels[i];

	sum_alpha = classifier.max_response();
	//std::cout<< "sum alpha: " <<sum_alpha<< " | max aplha"<<classifier.max_response()<< " | max el "<<max_el <<std::endl;
	unsigned char* channels_img = new unsigned char [width*height*nbin*3];
	for(int i=0;i<width*height*nbin;i++){
		channels_img[i*3+0]=(channels[i]/max_el)*255;
		channels_img[i*3+1]=0;
		channels_img[i*3+2]=255-(channels[i]/max_el)*255;
	}
	for(int i=0;i<nbin;i++){
		std::string file_out = "channel-"+ static_cast<std::ostringstream*>( &(std::ostringstream() << i) )->str() + ".ppm";
		//pgm_write(file_out.c_str(), channels_img + i*(width*height), width, height, 255);
		ppm_write(file_out.c_str(), channels_img + i*(width*height)*3, width, height);
	}

	delete [] channels;
	delete [] channels_img;


}


int main(int argc, char *argv[])
{

	std::ifstream net_classif;
	net_classif.open(argv[1]);
	if(net_classif){

		BoostClassifier<IchnClassifier> boostCl;

		std::string str_sig,str_conf;
		int width,height;
		net_classif >> str_sig;
		net_classif >> width;
		net_classif >> height;
		net_classif >> str_conf;

		boostCl.load(net_classif);
		int nbin=str_conf.c_str()[0]-0x30;
		std::cout << "load "<< argv[1] << " "<< nbin<<" "<<width<<"x"<<height<<std::endl;
		dump_classifier_icf(boostCl,width,height,nbin);

	}

}

/*
int main(int argc, char *argv[])
{
if(argc!=2 && argc<6)
{
 puts("usage 1:");
 puts("dumpfeature <width> <height> <x0> <y0> <w> <h> [<pattern1> <dest1.pgm> ... <patternN> <destN.pgm>]");
 puts("usage 2:");
 puts("dumpfeature <classifier>");
 
 return -1;
}

if(argc==2)
{
  
  std::ifstream in(argv[1]);
  if(in)
  {
  unsigned int width, height;
  std::string signature;
  in >> signature >> width >> height;
  if(signature == BoostClassifier<HaarClassifier>::signature() )
    {
      BoostClassifier<HaarClassifier> source(in);
      dump_feature(source, width, height);
    }
  else
  if(signature == BoostClassifier< BayesianHaarClassifier >::signature() )
    {
      BoostClassifier< BayesianHaarClassifier > source(in);
      dump_feature(source, width, height);
    }
  else
    {
    std::cerr << argv[1] << " is a classifier not supported yet by dumpfeature" << std::endl;
    }
  
  }
  else
  {
    std::cerr << argv[1] << " not valid" << std::endl;
  }  
  
}
else
{

int width  = atoi(argv[1]);
int height = atoi(argv[2]);
int x0     = atoi(argv[3]);
int y0     = atoi(argv[4]);
int w      = atoi(argv[5]);
int h      = atoi(argv[6]);
if(argc<8)
  {
  for(const FeatureDescription *fd = featureDescription; (fd->name); fd++)
    {	
    char name[8];
    HaarFeature hf;
    if(fd->proc(hf, x0,y0,w,h, width, height))
      {
      puts(fd->name);
      sprintf(name, "%s.pgm", fd->name);
      DumpFeatureImg(name, hf, width, height);
      }
      else { printf("%s is not exported due to geometry\n", fd->name); }
    }
  }
  else
  {
  const char *p = argv[7];
  const char *t = argv[8];

  const FeatureDescription *fd = findFeature(p);
  if(fd == NULL)
    {
    printf("Unknown feature %s\n", p);
    return 1;
    }
  puts(fd->name);
  HaarFeature hf;
  if(fd->proc(hf, x0,y0,w,h, width, height))
  {
  DumpFeatureImg(t, hf, width, height);
  }
  else
    puts("invalid geometry");
  }
}
return 0;
}
*/
