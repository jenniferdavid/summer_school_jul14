/* Author : Ing. Diego Daniel Santiago. INAUT - CONICET */
#include "ShareMemoryContainer.h"
#include "v_repLib.h"


CShareMemoryContainer::CShareMemoryContainer()
{
}

CShareMemoryContainer::~CShareMemoryContainer()
{
	removeAll();	
}

void CShareMemoryContainer::closeAll()
{
	for (int i=0;i<getCount();i++){
		if(_allShareMemorys[i]->closeShareMemory()==false){
			MessageBox(NULL,"Error: THE SHARE MEMORY WASN'T CLOSE","Error",MB_OK);
		}
	}
}
void CShareMemoryContainer::removeAll()
{
	closeAll(); 
	for (int i=0;i<getCount();i++)
		delete _allShareMemorys[i];
	_allShareMemorys.clear();

}

void CShareMemoryContainer::removeFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allShareMemorys[i]->getID()==theID)
		{
			if(_allShareMemorys[i]->closeShareMemory()==false){
				MessageBox(NULL,"Error: THE SHARE MEMORY WASN'T CLOSE","Error",MB_OK);
			}
			delete _allShareMemorys[i];
			_allShareMemorys.erase(_allShareMemorys.begin()+i);
			break;
		}
	}
}

int CShareMemoryContainer::insert(shareMemory* theShareMemory)
{
	int newID=0;
	while (getFromID(newID)!=NULL)
		newID++;
	_allShareMemorys.push_back(theShareMemory);
	theShareMemory->setID(newID);
	return(newID);	
}

shareMemory* CShareMemoryContainer::getFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allShareMemorys[i]->getID()==theID)
			return(_allShareMemorys[i]);
	}
	return(NULL);
}

shareMemory* CShareMemoryContainer::getFromIndex(int ind)
{
	if ( (ind<0)||(ind>=getCount()) )
		return(NULL);
	return(_allShareMemorys[ind]);
}

shareMemory* CShareMemoryContainer::getFromName(char *theName)
{
	for (int i=0;i<getCount();i++)
	{
		if(strcmp(_allShareMemorys[i]->getName(),theName)==0)
			return(_allShareMemorys[i]);
	}
	return(NULL);
}

int CShareMemoryContainer::getCount()
{
	return(int(_allShareMemorys.size()));
}

char* CShareMemoryContainer::getList()
{
	int count=getCount();
	char *stringOut=new char[40*count];
	for (int i=0;i<count;i++){
		shareMemory* theShareMemory=getFromIndex(i);
		sprintf(stringOut,"%s%i %s %i\n",stringOut,theShareMemory->getID(),theShareMemory->getName(),theShareMemory->getSize());
	}
	return stringOut;
}



