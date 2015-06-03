/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#pragma once

#include <vector>
#include "shareMemory.h"

class CShareMemoryContainer
{

public:
	 CShareMemoryContainer();
	 virtual ~CShareMemoryContainer();

	 void removeAll();
	 void closeAll();
	 void removeFromID(int theID);
	 int insert(shareMemory* theShareMemory);
	 shareMemory* getFromID(int theID);
	 shareMemory* getFromName(char *theName);
	 shareMemory* getFromIndex(int ind);
	 int getCount();
	 char *getList();

protected:
	 std::vector<shareMemory*> _allShareMemorys; 
};
