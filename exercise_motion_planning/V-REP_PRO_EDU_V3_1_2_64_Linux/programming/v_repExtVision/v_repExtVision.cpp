// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#include "v_repExtVision.h"
#include "v_repLib.h"
#include <vector>
#include <iostream>
//#include <sstream>
#include "visionTransfCont.h"


#ifdef _WIN32
	#ifdef QT_COMPIL
		#include <direct.h>
	#else
		#include <shlwapi.h> // required for PathRemoveFileSpec function
		#pragma comment(lib, "Shlwapi.lib")
	#endif
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#define WIN_AFX_MANAGE_STATE
	#include <unistd.h>
#endif /* __linux || __APPLE__ */

LIBRARY vrepLib;

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)	CONCAT(x,y,z)
#define LUA_HANDLE_SPHERICAL_COMMAND "simExtVision_handleSpherical"
#define LUA_HANDLE_ANAGLYPHSTEREO_COMMAND "simExtVision_handleAnaglyphStereo"

CVisionTransfCont* visionTransfContainer;

void LUA_HANDLE_SPHERICAL_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>3)
	{ // Ok, we have at least 4 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) && // passive vision sensor handle
			(p->inputArgTypeAndSize[1*2+0]==(sim_lua_arg_int|sim_lua_arg_table))&&(p->inputArgTypeAndSize[1*2+1]>=6) && // 6 active vision sensor handles
			(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_float) && // horizontal angle
			(p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_float) ) // vertical angle
		{ // Ok, we have all required arguments
			int passiveVisionSensorHande=p->inputInt[0];
			int activeVisionSensorHandes[6];
			for (int i=0;i<6;i++)
				activeVisionSensorHandes[i]=p->inputInt[1+i];
			float horizontalAngle=p->inputFloat[0];
			float verticalAngle=p->inputFloat[1];
			CVisionTransf* obj=visionTransfContainer->getVisionTransfFromPassiveVisionSensor(passiveVisionSensorHande);
			if (obj!=NULL)
			{
				if (!obj->isSame(activeVisionSensorHandes,horizontalAngle,verticalAngle))
				{
					visionTransfContainer->removeObject(passiveVisionSensorHande);
					obj=NULL;
				}
			}
			if (obj==NULL)
			{
				obj=new CVisionTransf(passiveVisionSensorHande,activeVisionSensorHandes,horizontalAngle,verticalAngle);
				visionTransfContainer->addObject(obj);
			}

			if (obj->doAllObjectsExistAndAreVisionSensors())
			{
				if (obj->isActiveVisionSensorResolutionCorrect())
				{
					if (obj->areActiveVisionSensorsExplicitelyHandled())
					{
						obj->handleObject();
						result=1; // success
					}
					else
						simSetLastError(LUA_HANDLE_SPHERICAL_COMMAND,"Active vision sensors should be explicitely handled."); // output an error
				}
				else
					simSetLastError(LUA_HANDLE_SPHERICAL_COMMAND,"Invalid vision sensor resolutions."); // output an error
			}
			else
				simSetLastError(LUA_HANDLE_SPHERICAL_COMMAND,"Invalid handles, or handles are not vision sensor handles."); // output an error

			if (result==-1)
				visionTransfContainer->removeObject(passiveVisionSensorHande);

		}
		else
			simSetLastError(LUA_HANDLE_SPHERICAL_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_HANDLE_SPHERICAL_COMMAND,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}

void LUA_HANDLE_ANAGLYPHSTEREO_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>1)
	{ // Ok, we have at least 2 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) && // passive vision sensor handle
			(p->inputArgTypeAndSize[1*2+0]==(sim_lua_arg_int|sim_lua_arg_table))&&(p->inputArgTypeAndSize[1*2+1]>=2) ) // 2 active vision sensor handles
		{ // Ok, we have all required arguments
			int passiveVisionSensorHande=p->inputInt[0];
			int leftSensorHandle=p->inputInt[1];
			int rightSensorHandle=p->inputInt[2];
			// Check the object types:
			bool existAndAreVisionSensors=true;
			if (simGetObjectType(passiveVisionSensorHande)!=sim_object_visionsensor_type)
				existAndAreVisionSensors=false;
			if (simGetObjectType(leftSensorHandle)!=sim_object_visionsensor_type)
				existAndAreVisionSensors=false;
			if (simGetObjectType(rightSensorHandle)!=sim_object_visionsensor_type)
				existAndAreVisionSensors=false;
			if (existAndAreVisionSensors)
			{ // check the sensor resolutions:
				int r[2];
				simGetVisionSensorResolution(passiveVisionSensorHande,r);
				int rl[2];
				simGetVisionSensorResolution(leftSensorHandle,rl);
				int rr[2];
				simGetVisionSensorResolution(rightSensorHandle,rr);
				if ((r[0]==rl[0])&&(r[0]==rr[0])&&(r[1]==rl[1])&&(r[1]==rr[1]))
				{ // check if the sensors are explicitely handled:
					int e=simGetExplicitHandling(passiveVisionSensorHande);
					int el=simGetExplicitHandling(leftSensorHandle);
					int er=simGetExplicitHandling(rightSensorHandle);
					if ((e&el&er&1)==1)
					{
						if ( (p->inputArgCount==2)||(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_nil)||((p->inputArgTypeAndSize[2*2+0]==(sim_lua_arg_float|sim_lua_arg_table))&&(p->inputArgTypeAndSize[2*2+1]>=6)) )
						{
							// Check for the auxiliary argument:
							float leftAndRightColors[6]={1.0f,0.0f,0.0f,0.0f,1.0f,1.0f}; // default
							if ((p->inputArgCount>2)&&(p->inputArgTypeAndSize[2*2+0]!=sim_lua_arg_nil))
							{ // we have the auxiliary argument
								for (int i=0;i<6;i++)
									leftAndRightColors[i]=p->inputFloat[i];
							}

							simHandleVisionSensor(leftSensorHandle,NULL,NULL);
							float* leftImage=simGetVisionSensorImage(leftSensorHandle);
							simHandleVisionSensor(rightSensorHandle,NULL,NULL);
							float* rightImage=simGetVisionSensorImage(rightSensorHandle);
							for (int i=0;i<r[0]*r[1];i++)
							{
								float il=(leftImage[3*i+0]+leftImage[3*i+1]+leftImage[3*i+2])/3.0f;
								float ir=(rightImage[3*i+0]+rightImage[3*i+1]+rightImage[3*i+2])/3.0f;
								leftImage[3*i+0]=il*leftAndRightColors[0]+ir*leftAndRightColors[3];
								leftImage[3*i+1]=il*leftAndRightColors[1]+ir*leftAndRightColors[4];
								leftImage[3*i+2]=il*leftAndRightColors[2]+ir*leftAndRightColors[5];
							}
							simSetVisionSensorImage(passiveVisionSensorHande,leftImage);
							simReleaseBuffer((char*)leftImage);
							simReleaseBuffer((char*)rightImage);
							result=1;
						}
						else
							simSetLastError(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"Wrong argument type/size."); // output an error
					}
					else
						simSetLastError(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"Vision sensors should be explicitely handled."); // output an error
				}
				else
					simSetLastError(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"Invalid vision sensor resolutions."); // output an error
			}
			else
				simSetLastError(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"Invalid handles, or handles are not vision sensor handles."); // output an error
		}
		else
			simSetLastError(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}

// This is the plugin start routine:
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP
	// Dynamically load and bind V-REP functions:
	char curDirAndFile[1024];
#ifdef _WIN32
	#ifdef QT_COMPIL
		_getcwd(curDirAndFile, sizeof(curDirAndFile));
	#else
		GetModuleFileName(NULL,curDirAndFile,1023);
		PathRemoveFileSpec(curDirAndFile);
	#endif
#elif defined (__linux) || defined (__APPLE__)
	getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif

	std::string currentDirAndPath(curDirAndFile);
	std::string temp(currentDirAndPath);

#ifdef _WIN32
	temp+="\\v_rep.dll";
#elif defined (__linux)
	temp+="/libv_rep.so";
#elif defined (__APPLE__)
	temp+="/libv_rep.dylib";
#endif /* __linux || __APPLE__ */

	vrepLib=loadVrepLibrary(temp.c_str());
	if (vrepLib==NULL)
	{
		std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'Vision' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib)==0)
	{
		std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'Vision' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	if (vrepVer<30100) // if V-REP version is smaller than 3.01.00
	{
		std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'Vision' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	// Register the new Lua commands:
	int inArgs_handle_spherical[]={4,sim_lua_arg_int,sim_lua_arg_int|sim_lua_arg_table,sim_lua_arg_float,sim_lua_arg_float};
	simRegisterCustomLuaFunction(LUA_HANDLE_SPHERICAL_COMMAND,strConCat("number result=",LUA_HANDLE_SPHERICAL_COMMAND,"(number passiveVisionSensorHandle,table_6 activeVisionSensorHandles,number horizontalAngle,number verticalAngle)"),inArgs_handle_spherical,LUA_HANDLE_SPHERICAL_CALLBACK);

	int inArgs_handle_anaglyphstereo[]={3,sim_lua_arg_int,sim_lua_arg_int|sim_lua_arg_table,sim_lua_arg_float|sim_lua_arg_table};
	simRegisterCustomLuaFunction(LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,strConCat("number result=",LUA_HANDLE_ANAGLYPHSTEREO_COMMAND,"(number passiveVisionSensorHandle,table_2 activeVisionSensorHandles,table_6 leftAndRightColors=nil)"),inArgs_handle_anaglyphstereo,LUA_HANDLE_ANAGLYPHSTEREO_CALLBACK);

	visionTransfContainer = new CVisionTransfCont();

	return(1); // initialization went fine, we return the version number of this extension module (can be queried with simGetModuleName)
}

// This is the plugin end routine:
VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP

	delete visionTransfContainer;

	unloadVrepLibrary(vrepLib); // release the library
}

// This is the plugin messaging routine (i.e. V-REP calls this function very often, with various messages):
VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle

	// This function should not generate any error messages:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

	void* retVal=NULL;

	if (message==sim_message_eventcallback_instancepass)
	{
		if (auxiliaryData[0]&1)
			visionTransfContainer->removeInvalidObjects();
	}

	if (message==sim_message_eventcallback_simulationended)
	{ // Simulation just ended
		visionTransfContainer->removeAll();
	}

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}

