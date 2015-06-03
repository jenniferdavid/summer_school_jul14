/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#include "shareMemory.h"
#include <iostream>

shareMemory::shareMemory(char *Name){
	_id=-1;
	pMemory=nullptr;
	cpName=nullptr;		
	_data=nullptr;		
	setName(Name);
	setSize(DEFAULT_SIZE);
}
shareMemory::shareMemory(char *Name, size_t size){ //initializes and opens the shared memory
	_id=-1;
    pMemory=nullptr;
	cpName=nullptr;		
	_data=nullptr;	
	//Load name and size of share memory
	setName(Name);
	setSize(size);
}

shareMemory::~shareMemory(){
	//empty so far
}

bool shareMemory::openShareMemory(){

	if( pMemory != nullptr){
		MessageBox(NULL,"Error openShareMemory(): THE SHARE MEMORY WAS ALREADY OPEN","Error",MB_OK);
		return false;
	}
	if (cpName==nullptr){
		MessageBox(NULL,"Error openShareMemory(): THE SHARE MEMORY WASN'T INITIALIZED","Error",MB_OK);
		return false;
	}

	#ifdef _WIN32
	hShare = CreateFileMapping((HANDLE)NULL,NULL, PAGE_READWRITE, 0, _size, cpName);
	if(hShare == nullptr){
		MessageBox(NULL,"Error openShareMemory(): CreateFileMapping","Error",MB_OK);
		return false;
	}else{
		pMemory = (char *) MapViewOfFile(hShare,FILE_MAP_ALL_ACCESS,0, 0, _size);
	}
	if(pMemory == nullptr){
		MessageBox(NULL,"Error openShareMemory() :MapViewOfFile","Error",MB_OK);
		return false;
	}
	#endif /* _WIN32 */
	#if defined (__linux) || defined (__APPLE__)
	//complete with linux functions 
	#endif /* __linux || __APPLE__ */

	return true;
}

bool shareMemory::closeShareMemory(){
	#ifdef _WIN32
	if((pMemory != nullptr)) {
		if( FlushViewOfFile(pMemory, _size) == 0){
			MessageBox(NULL,"Error closeShareMemory() :FlushViewOfFile","Error",MB_OK);
			return false;
		}
		if( UnmapViewOfFile(pMemory) == 0){
			MessageBox(NULL,"Error closeShareMemory() :UnmapViewOfFile","Error",MB_OK);
			return false;
		}
		if( CloseHandle(hShare) == 0){
			MessageBox(NULL,"Error closeShareMemory() :CloseHandle","Error",MB_OK);
			return false;
		}
		
		pMemory = NULL;
		return true;
	}else{
		MessageBox(NULL,"Error: THE SHARE MEMORY WAS ALREADY CLOSE","Error",MB_OK);
		return false;
	}
	#endif
	#if defined (__linux) || defined (__APPLE__)
	//complete with linux functions 
	#endif /* __linux || __APPLE__ */
}

bool shareMemory::readShareMemory(char *pfData){

	if(pMemory == nullptr){
		MessageBox(NULL,"Error:Read Virtual Device CANT FIND THE SHARE MEMORY","Error",MB_OK);
		return false;
	}
	if(pfData==nullptr){
		return false;
	}
	memcpy(pfData,pMemory, _size);
	return true;
}

bool shareMemory::readShareMemory(char *pfData, size_t size){

	if(pMemory == nullptr){
		MessageBox(NULL,"Error:Read Virtual Device CANT FIND THE SHARE MEMORY","Error",MB_OK);
		return false;
	}
	if(pfData==nullptr){
		return false;
	}
	if (size>=_size){
		MessageBox(NULL,"Error:Read Virtual Device INVALID READ SIZE","Error",MB_OK);
		return false;
	}
	memcpy(pfData,pMemory, size);
	return true;
}

bool shareMemory::writeShareMemory(char *pfData){
	if(pMemory == NULL){
		MessageBox(NULL,"Error:Read Virtual Device CANT FIND THE SHARE MEMORY","Error",MB_OK);
		return false;
	}
	memcpy( (char *)pMemory, (char *)pfData,_size);

	#ifdef _WIN32
	if( FlushViewOfFile(pMemory, _size) == 0){
		MessageBox(NULL,"Error: Close Virtual Device::FlushViewOfFile","Error",MB_OK);
		return false;
	}
	#endif /* _WIN32 */
	#if defined (__linux) || defined (__APPLE__)
		//complete with linux functions 
	#endif /* __linux || __APPLE__ */
	return true;
}

bool shareMemory::writeShareMemory(char *pfData, size_t size){
	#ifdef _WIN32
	if (size>_size){
		MessageBox(NULL,"Error in writeShareMemory(char *pfData, size_t size): size biger than memory size","Error",MB_OK);
		return false;
	}
	if(pMemory == NULL){
		MessageBox(NULL,"Error in writeShareMemory(char *pfData, size_t size): CANT FIND THE SHARE MEMORY","Error",MB_OK);
		return false;
	}
	memcpy( (char *)pMemory, (char *)pfData,size);
	if( FlushViewOfFile(pMemory, size) == 0){
		MessageBox(NULL,"Error in writeShareMemory(char *pfData, size_t size): FlushViewOfFile","Error",MB_OK);
		return false;
	}
	#endif /* _WIN32 */

	#if defined (__linux) || defined (__APPLE__)
	//complete with linux functions 
	#endif /* __linux || __APPLE__ */
	return true;
}

void shareMemory::setID(int newId){
	_id=newId;
}

int  shareMemory::getID(){
		return(_id);
}

char* shareMemory::getName(){
	return cpName;
}

void shareMemory::setName(char *name){
	if(name!=nullptr){
		cpName= new char[strlen(name)];
		strcpy(cpName,name);
	}
}

int shareMemory::getSize(){
	return _size;
}

void shareMemory::setSize(size_t size){
	if(size>0)
		_size=size;
}
