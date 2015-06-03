/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#include "Cevent.h"
#include <iostream>

Cevent::Cevent(char *Name){
	_id=-1;
	cpName=nullptr;		
	setName(Name);
}

Cevent::~Cevent(){
	
}
bool Cevent::create(){
	hEvent=CreateEvent( 
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        TRUE,              // initial state is signaled
        cpName				// object name
    ); 
	if (hEvent == NULL){ 
        printf("CreateEvent failed (%d)\n", GetLastError());
		return false;
    }
	return true;
}

bool Cevent::close(){
	set(); //unlock any WaitForSingleObject avoid freeze
    if(CloseHandle(hEvent)!=0)
		return true;
	return false;
}

bool Cevent::set(){
	if (!SetEvent(hEvent)){
        printf("SetEvent failed (%d)\n", GetLastError());
        return false;
    }
	return true;
}

bool Cevent::reset(){
	if (!ResetEvent(hEvent)){
        printf("ResetEvent failed (%d)\n", GetLastError());
        return false;
    }
	return true;
}

bool Cevent::waitForEvent(DWORD time_ms){
	printf("\n Event name: %s",cpName);
	DWORD state= WaitForSingleObject(hEvent,time_ms);
	switch (state) 
    {
        // Event object was signaled
        case WAIT_OBJECT_0: 
            return true;
        // An error occurred
        default: 
            return false; 
    }
}

void Cevent::setID(int newId){
	_id=newId;
}

int  Cevent::getID(){
		return(_id);
}

char* Cevent::getName(){
	return cpName;
}

void Cevent::setName(char *name){
	if(name!=nullptr && (strlen(name)<MAX_PATH)){
		cpName= new char[strlen(name)];
		strcpy(cpName,name);
	}
}
