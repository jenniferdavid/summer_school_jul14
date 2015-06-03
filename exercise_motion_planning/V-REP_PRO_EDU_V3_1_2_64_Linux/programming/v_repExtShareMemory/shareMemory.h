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

#define DEFAULT_SIZE 200000 //bytes


class shareMemory
{
public:
	shareMemory(char *cpName);
	shareMemory(char *cpName, size_t size);
	virtual ~shareMemory();
	//Share memory functions
	bool openShareMemory(); 
	bool closeShareMemory();
	bool readShareMemory(char *pfData);
	bool readShareMemory(char *pfData, size_t size);//read custom_size<_size
	bool writeShareMemory(char *pfData);
	bool writeShareMemory(char *pfData, size_t size);//write custom_size<_size
	void setID(int newId);
	int getID();
	char* getName(); //Get the share memory name
	void setName(char *name);
	int getSize();
	void setSize(size_t size);

protected:
	char *pMemory;		//ponter to Memory site
	char *cpName;		//Name of the Sare Memory 
	char *_data;		//copy of content of Share memory
	size_t _size;		//Size of Share Memory 
	int _id;		// Id of Share Memory
	HANDLE hShare;		// system Handle of share memory	
};