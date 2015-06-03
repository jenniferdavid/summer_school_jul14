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

#pragma once
#include "resource.h"
#include "afxwin.h"

class CK3Dialog : public CDialog
{
	DECLARE_DYNAMIC(CK3Dialog)

public:
	CK3Dialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CK3Dialog();
	enum { IDD = IDD_K3_DIALOG };

	void refresh();
	bool getVisible();
	void setVisible(bool isVisible);
	void OnCancel();
	void OnOK();

	void selectObjectInList(int sceneObjectHandle);
	int getSelectedObjectInList();

	int dialogMenuItemHandle;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool _isVisible;

	DECLARE_MESSAGE_MAP()
	CListBox m_k3List;
	afx_msg void OnLbnSelchangeK3List();
};
