/*
 * v-rep OpenCVBridge v1.1
 * by Marco "marcomurk" Bellaccini - marco.bellaccini[at!]gmail.com
 *
 * This software is provided "as is", without any warranty,
 * under the terms of the GNU LESSER GENERAL PUBLIC LICENSE, Version 3
 *
 */

#include "v_repExtOpenCVBridge.h"
#include "OCVbridgeType.h"
#include "../include/v_repLib.h"
#include <iostream>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#ifdef _WIN32
	#include <direct.h>
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
	#include <string.h>
	#define _stricmp(x,y) strcasecmp(x,y)
#endif

using namespace cv;

//user image elaboration function prototype
OCVBridge openCVElab(Mat);


const int filterCount=1; // this plugin exports 1 filter
int filterID[filterCount]={-1}; // Filters with negative IDs won't have a dialog or special triggering conditions (negative IDs for simple filters!)
// Header ID (DEVELOPER_DATA_HEADER), filterIDs and parameter values of a filter are serialized!! (don't change at will!)
int nextFilterEnum=0; // used during enumeration
const char* filterName[filterCount]={"OpenCV elaboration"}; // filter names

LIBRARY vrepLib;

VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP.
	// Dynamically load and bind V-REP functions:
	 // ******************************************
	 // 1. Figure out this plugin's directory:
	 char curDirAndFile[1024];
 #ifdef _WIN32
	 _getcwd(curDirAndFile, sizeof(curDirAndFile));
 #elif defined (__linux) || defined (__APPLE__)
	 getcwd(curDirAndFile, sizeof(curDirAndFile));
 #endif
	 std::string currentDirAndPath(curDirAndFile);
	 // 2. Append the V-REP library's name:
	 std::string temp(currentDirAndPath);
 #ifdef _WIN32
	 temp+="/v_rep.dll";
 #elif defined (__linux)
	 temp+="/libv_rep.so";
 #elif defined (__APPLE__)
	 temp+="/libv_rep.dylib";
 #endif /* __linux || __APPLE__ */
	 // 3. Load the V-REP library:
	 vrepLib=loadVrepLibrary(temp.c_str());
	 if (vrepLib==NULL)
	 {
         std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'OpenCVBridge' plugin.\n";
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 if (getVrepProcAddresses(vrepLib)==0)
	 {
         std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'OpenCVBridge' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	 // Check the version of V-REP:
	 // ******************************************
	 int vrepVer;
	 simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	 if (vrepVer<20604) // if V-REP version is smaller than 2.06.04
	 {
         std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'OpenCVBridge' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	return(4);	// initialization went fine, return the version number of this plugin!
				// version 1 was for V-REP 2.5.11 or earlier
				// version 2 was for V-REP 2.5.12 or earlier
				// version 3 was for V-REP versions before V-REP 2.6.7
				// version 4 is the Qt version
}

VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	unloadVrepLibrary(vrepLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle

	// This function should not generate any error messages:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

	void* retVal=NULL;

	if (message==sim_message_eventcallback_imagefilter_enumreset)
		nextFilterEnum=0; // Reset the enumeration procedure for these filters

	if (message==sim_message_eventcallback_imagefilter_enumerate)
	{ // Filter enumeration procedure. Here we inform V-REP of all filters that this plugin has
		if (nextFilterEnum<filterCount)
        {
            replyData[0]=DEVELOPER_DATA_HEADER;
            replyData[1]=filterID[nextFilterEnum];
			char* retValTmp=simCreateBuffer(int(strlen(filterName[nextFilterEnum])+1));
			strcpy(retValTmp,filterName[nextFilterEnum]);
			retVal=(void*)retValTmp;
			nextFilterEnum++;
		}
	}


	if (message==sim_message_eventcallback_imagefilter_process)
	{ // V-REP calls this when a plugin filter should be applied
		if (auxiliaryData[0]==DEVELOPER_DATA_HEADER)
        {
            unsigned int res[2]={auxiliaryData[2],auxiliaryData[3]}; // size of the images/buffers/depth maps
			void** ptrs=(void**)customData;
			float* workImage=(float*)ptrs[2]; // work image

            //let's import the workImage in an OpenCV Mat
            Mat ocvimage;

            ocvimage=cvCreateImage(cvSize(res[0], res[1]), 8, 3);

            //from v-rep to OpenCV...
            //remeber that colors in opencv Mat are ordered as BGR
            for (unsigned int i=0;i<res[1];i++){
                for (unsigned int j=0;j<res[0];j++){
                    int r,g,b;
                    r=cvRound(255*workImage[3*((res[1]-i)*res[0]+j)+0]);
                    g=cvRound(255*workImage[3*((res[1]-i)*res[0]+j)+1]);
                    b=cvRound(255*workImage[3*((res[1]-i)*res[0]+j)+2]);

                    ocvimage.at<Vec3b>(i,j)[0] = (uchar)b;
                    ocvimage.at<Vec3b>(i,j)[1] = (uchar)g;
                    ocvimage.at<Vec3b>(i,j)[2] = (uchar)r;
                }
            }


            //connect to user image elaboration function
            OCVBridge retStruct=openCVElab(ocvimage);

            //get the elaborated image
            ocvimage=retStruct.img;

            //get the return values
            std::vector<float> retVct=retStruct.returnValues;


            // That auxiliary information (as well as the trigger state) are retrieved with simHandleVisionSensor
            replyData[1]=retVct.size(); // the number of floats we return
            retVal=simCreateBuffer(sizeof(float)*retVct.size()); // is automatically released by V-REP upon callback return

            for(unsigned int i=0; i<retVct.size();i++){
                ((float*)retVal)[i]=retVct.at(i);
            }



            //... and back from OpenCV to v-rep
            for (unsigned int i=0;i<res[1];i++){
                for (unsigned int j=0;j<res[0];j++){
                    float r,g,b;
                    b= (float) ocvimage.at<Vec3b>(i,j)[0];
                    g= (float) ocvimage.at<Vec3b>(i,j)[1];
                    r= (float) ocvimage.at<Vec3b>(i,j)[2];

                    workImage[3*((res[1]-i-1)*res[0]+j)+0]=r/255;
                    workImage[3*((res[1]-i-1)*res[0]+j)+1]=g/255;
                    workImage[3*((res[1]-i-1)*res[0]+j)+2]=b/255;

                }
            }




		}
	}


	// You can add more messages to handle here

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}
