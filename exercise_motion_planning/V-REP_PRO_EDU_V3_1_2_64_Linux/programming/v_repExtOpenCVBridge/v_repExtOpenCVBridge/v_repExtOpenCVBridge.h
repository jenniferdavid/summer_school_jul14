/*
 * v-rep OpenCVBridge v1.1
 * by Marco "marcomurk" Bellaccini - marco.bellaccini[at!]gmail.com
 *
 * This software is provided "as is", without any warranty,
 * under the terms of the GNU LESSER GENERAL PUBLIC LICENSE, Version 3
 *
 */

#ifndef V_REPOPENCVBRIDGE_H
#define V_REPOPENCVBRIDGE_H

#define DEVELOPER_DATA_HEADER 92820


#ifdef _WIN32
	#define VREP_DLLEXPORT extern "C" __declspec(dllexport)
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#define VREP_DLLEXPORT extern "C"
#endif /* __linux || __APPLE__ */

// The 3 required entry points of the plugin:
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt);
VREP_DLLEXPORT void v_repEnd();
VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData);

#endif // V_REPOPENCVBRIDGE_H
