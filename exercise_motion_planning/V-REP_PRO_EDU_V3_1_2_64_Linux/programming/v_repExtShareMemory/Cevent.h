/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#pragma once

#ifdef _WIN32
	#include <windows.h>
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#include <sys/shm.h>
    #include <sys/types.h>
    #include <sys/ipc.h>
#endif /* __linux || __APPLE__ */


class Cevent
{
public:
	Cevent(char *cpName);
	virtual ~Cevent();
	//Events functions
	bool create();
	bool close();
	bool set();
	bool reset();
	bool waitForEvent(DWORD time);
	void setID(int newId);
	int getID();
	char* getName(); //Get the share memory name
	void setName(char *name);

protected:
	HANDLE hEvent;	//Handle
	bool _state;	//State
	char *cpName;	//Name
	int _id;		// Id 
};