/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#pragma once

#include "porting.h"
#include <vector>
#include "ShareMemoryContainer.h"
#include "eventContainer.h"
class CAccess  
{
public:
	CAccess();
	virtual ~CAccess();

	static void createNonGui();
	static void destroyNonGui();

	static CShareMemoryContainer* shareMemoryContainer; 
	static CEventContainer* eventContainer; 
};
