/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#pragma once

#include <vector>
#include "Cevent.h"

class CEventContainer
{

public:
	 CEventContainer();
	 virtual ~CEventContainer();

	 void closeAll();
	 void removeFromID(int theID);
	 int insert(Cevent* theShareMemory);
	 Cevent* getFromID(int theID);
	 Cevent* getFromName(char *theName);
	 Cevent* getFromIndex(int ind);
	 int getCount();
	 char *getList();
	 void unlockAll();
protected:
	 std::vector<Cevent*> _allEvents; 
};
