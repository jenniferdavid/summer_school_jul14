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

#include "v_repExtBubbleRob.h"
#include "bubbleRobDialog.h"
#include "access.h"
#include "v_repLib.h"


IMPLEMENT_DYNAMIC(CBubbleRobDialog, CDialog)

CBubbleRobDialog::CBubbleRobDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBubbleRobDialog::IDD, pParent)
{
	_isVisible=false;
	dialogMenuItemHandle=-1;
}

CBubbleRobDialog::~CBubbleRobDialog()
{
}

void CBubbleRobDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAX_VELOCITY, m_maxVelocity);
	DDX_Control(pDX, IDC_BUBBLEROB_LIST, m_bubbleRobList);
}


BEGIN_MESSAGE_MAP(CBubbleRobDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_BUBBLEROB_LIST, &CBubbleRobDialog::OnLbnSelchangeBubblerobList)
	ON_EN_KILLFOCUS(IDC_MAX_VELOCITY, &CBubbleRobDialog::OnEnKillfocusMaxVelocity)
END_MESSAGE_MAP()


void CBubbleRobDialog::refresh()
{ // This refreshed the dialog's items:
	int lockHandle=simLockResources(sim_lock_ui_wants_to_read,-1);
	if (lockHandle!=-1)
	{
		int lastSel=simGetObjectLastSelection();

		m_bubbleRobList.ResetContent();
		for (int i=0;i<CAccess::bubbleRobContainer->getCount();i++)
		{
			CBubbleRob* bubbleRob=CAccess::bubbleRobContainer->getFromIndex(i);
			int sceneObjectHandle=bubbleRob->getAssociatedObject();
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
			int index=m_bubbleRobList.AddString(txt.c_str());
			m_bubbleRobList.SetItemData(index,(DWORD)bubbleRob->getID());
		}
		CBubbleRob* taggedObj=NULL;
		if (lastSel!=-1)
		{
			taggedObj=CAccess::bubbleRobContainer->getFromAssociatedObject(lastSel);
			if (taggedObj==NULL)
				selectObjectInList(-1);
			else
				selectObjectInList(taggedObj->getID());
		}
		else
			selectObjectInList(-1);
		taggedObj=CAccess::bubbleRobContainer->getFromID(getSelectedObjectInList());
		m_maxVelocity.EnableWindow(taggedObj!=NULL);
		if (taggedObj!=NULL)
		{
			CString tmp;
			tmp.Format("%0.2f",120.0f*taggedObj->getMaxVelocity()/3.1415f); // display it in RPM
			m_maxVelocity.SetWindowText(tmp);
		}
		else
			m_maxVelocity.SetWindowText("");
		simUnlockResources(lockHandle);
	}
}

void CBubbleRobDialog::selectObjectInList(int objectHandle)
{
	m_bubbleRobList.SetCurSel(-1);
	for (int i=0;i<m_bubbleRobList.GetCount();i++)
	{
		if (m_bubbleRobList.GetItemData(i)==objectHandle)
		{
			m_bubbleRobList.SetCurSel(i);
			break;
		}
	}
}

int CBubbleRobDialog::getSelectedObjectInList()
{
	int cs=m_bubbleRobList.GetCurSel();
	if (cs==LB_ERR)
		return(-1);
	int data=int(m_bubbleRobList.GetItemData(cs));
	CBubbleRob* bubbleRob=CAccess::bubbleRobContainer->getFromID(data);
	if (bubbleRob!=NULL)
		return(bubbleRob->getID());
	return(-1);
}

bool CBubbleRobDialog::getVisible()
{
	return(_isVisible);
}

void CBubbleRobDialog::setVisible(bool isVisible)
{
	_isVisible=isVisible;
	if (isVisible)
		ShowWindow(SW_SHOW);
	else
		ShowWindow(SW_HIDE);
	// Reflect the visibility state in the menu bar item:
	simSetModuleMenuItemState(dialogMenuItemHandle,(_isVisible?3:1),"BubbleRob Properties");
}

void CBubbleRobDialog::OnCancel()
{ // We closed the dialog
	setVisible(false);
}

void CBubbleRobDialog::OnOK()
{
}

void CBubbleRobDialog::OnLbnSelchangeBubblerobList()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_read,-1);
	if (lockHandle!=-1)
	{
		int objectHandle=getSelectedObjectInList();
		simRemoveObjectFromSelection(sim_handle_all,-1);
		CBubbleRob* bubbleRob=CAccess::bubbleRobContainer->getFromID(objectHandle);
		if (bubbleRob!=NULL)
		{
			int sceneObjHandle=bubbleRob->getAssociatedObject();
			if (sceneObjHandle!=-1)
				simAddObjectToSelection(sim_handle_single,sceneObjHandle);
		}
		simUnlockResources(lockHandle);
	}
}

void CBubbleRobDialog::OnEnKillfocusMaxVelocity()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_write,-1);
	if (lockHandle!=-1)
	{
		CBubbleRob* bubbleRob=CAccess::bubbleRobContainer->getFromAssociatedObject(simGetObjectLastSelection());
		if (bubbleRob!=NULL)
		{
			CString tmp;
			GetDlgItemText(IDC_MAX_VELOCITY,tmp);
			char* last;
			float v=float(strtod(tmp,&last));
			if (last[0]==0)
			{
				bubbleRob->setMaxVelocity(3.1415f*v/120.0f); // we had it displayed in RPM 
				simAnnounceSceneContentChange(); // this is so that V-REP registers a new undo-point here
			}
		}
		refresh();
		simUnlockResources(lockHandle);
	}
}

