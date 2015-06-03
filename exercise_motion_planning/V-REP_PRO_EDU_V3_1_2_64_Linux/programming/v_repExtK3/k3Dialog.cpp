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

#include "stdafx.h"
#include "v_repExtK3.h"
#include "k3Dialog.h"
#include "v_repLib.h"
#include "access.h"


IMPLEMENT_DYNAMIC(CK3Dialog, CDialog)

CK3Dialog::CK3Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(CK3Dialog::IDD, pParent)
{
	_isVisible=false;
	dialogMenuItemHandle=-1;
}

CK3Dialog::~CK3Dialog()
{
}

void CK3Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_K3_LIST, m_k3List);
}


BEGIN_MESSAGE_MAP(CK3Dialog, CDialog)
	ON_LBN_SELCHANGE(IDC_K3_LIST, &CK3Dialog::OnLbnSelchangeK3List)
END_MESSAGE_MAP()


void CK3Dialog::refresh()
{ // This refreshed the dialog's items:
	int lockHandle=simLockResources(sim_lock_ui_wants_to_read,-1);
	if (lockHandle!=-1)
	{
		int lastSel=simGetObjectLastSelection();

		m_k3List.ResetContent();
		for (int i=0;i<CAccess::k3Container->getCount();i++)
		{
			CK3* k3=CAccess::k3Container->getFromIndex(i);
			int sceneObjectHandle=k3->getAssociatedObject();
			std::string txt="Object: ";
			char* name=NULL;
			if (sceneObjectHandle!=-1)
				name=simGetObjectName(sceneObjectHandle);
			if (name==NULL)
				txt+="Unassociated";
			else
			{
				txt+="Associated with ";
				txt+=name;
				simReleaseBuffer(name);
			}
			int index=m_k3List.AddString(txt.c_str());
			m_k3List.SetItemData(index,(DWORD)k3->getID());
		}
		CK3* taggedObj=NULL;
		if (lastSel!=-1)
		{
			taggedObj=CAccess::k3Container->getFromAssociatedObject(lastSel);
			if (taggedObj==NULL)
				selectObjectInList(-1);
			else
				selectObjectInList(taggedObj->getID());
		}
		else
			selectObjectInList(-1);
		taggedObj=CAccess::k3Container->getFromID(getSelectedObjectInList());
		simUnlockResources(lockHandle);
	}
}

void CK3Dialog::selectObjectInList(int objectHandle)
{
	m_k3List.SetCurSel(-1);
	for (int i=0;i<m_k3List.GetCount();i++)
	{
		if (m_k3List.GetItemData(i)==objectHandle)
		{
			m_k3List.SetCurSel(i);
			break;
		}
	}
}

int CK3Dialog::getSelectedObjectInList()
{
	int cs=m_k3List.GetCurSel();
	if (cs==LB_ERR)
		return(-1);
	int data=int(m_k3List.GetItemData(cs));
	CK3* k3=CAccess::k3Container->getFromID(data);
	if (k3!=NULL)
		return(k3->getID());
	return(-1);
}

bool CK3Dialog::getVisible()
{
	return(_isVisible);
}

void CK3Dialog::setVisible(bool isVisible)
{
	_isVisible=isVisible;
	if (isVisible)
		ShowWindow(SW_SHOW);
	else
		ShowWindow(SW_HIDE);
	// Reflext the visibility state in the menu bar item:
	simSetModuleMenuItemState(dialogMenuItemHandle,(_isVisible?3:1),"Khepera III Properties");
}

void CK3Dialog::OnCancel()
{ // We closed the dialog
	setVisible(false);
}

void CK3Dialog::OnOK()
{
}

void CK3Dialog::OnLbnSelchangeK3List()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_read,-1);
	if (lockHandle!=-1)
	{
		int objectHandle=getSelectedObjectInList();
		simRemoveObjectFromSelection(sim_handle_all,-1);
		CK3* k3=CAccess::k3Container->getFromID(objectHandle);
		if (k3!=NULL)
		{
			int sceneObjHandle=k3->getAssociatedObject();
			if (sceneObjHandle!=-1)
				simAddObjectToSelection(sim_handle_single,sceneObjHandle);
		}
		simUnlockResources(lockHandle);
	}
}
