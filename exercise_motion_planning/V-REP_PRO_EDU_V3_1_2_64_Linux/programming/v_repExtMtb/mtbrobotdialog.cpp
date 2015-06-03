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

#include "mtbrobotdialog.h"
#include "ui_mtbrobotdialog.h"
#include "v_repLib.h"
#include "Access.h"

CMtbRobotDialog::CMtbRobotDialog(QWidget *parent) :
	QDialog(parent,Qt::Tool),
    ui(new Ui::CMtbRobotDialog)
{
    ui->setupUi(this);
	inRefreshRoutine=false;
}

CMtbRobotDialog::~CMtbRobotDialog()
{
    delete ui;
}

void CMtbRobotDialog::refresh()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_read,-1);
	if (lockHandle!=-1)
	{
		inRefreshRoutine=true;
		// Get handle of last selected object:
		int lastSel=simGetObjectLastSelection();

		updateObjectsInList();

		// Now initialized other dialog elements:
		CMtbRobot* taggedObj=NULL;
		ui->qqRobotProgram->setPlainText("");

		ui->qqAutomatic->setChecked(false);
		if (lastSel!=-1)
		{
			taggedObj=CAccess::mtbRobotContainer->getFromAssociatedObject(lastSel);
			if (taggedObj==NULL)
				selectObjectInList(-1);
			else
			{ // The last selected object is the base object of a MTB model!
				selectObjectInList(taggedObj->getID());
				std::string prg(taggedObj->getProgram());
				ui->qqRobotProgram->setPlainText(prg.c_str());
				ui->qqAutomatic->setChecked((taggedObj->getOptions()&1)!=0);
			}
		}
		else
			selectObjectInList(-1);
		ui->qqRobotProgram->setEnabled((taggedObj!=NULL)&&(simGetSimulationState()==sim_simulation_stopped));
		ui->qqAutomatic->setEnabled((taggedObj!=NULL)&&(simGetSimulationState()==sim_simulation_stopped));
		inRefreshRoutine=false;
		simUnlockResources(lockHandle);
	}
}

void CMtbRobotDialog::makeVisible(bool visible)
{
	setVisible(visible);
	// Reflect the visibility state in the menu bar item:
	simSetModuleMenuItemState(dialogMenuItemHandle,(visible?3:1),"MTB Robot Properties");
}

bool CMtbRobotDialog::getVisible()
{
	return(isVisible());
}

void CMtbRobotDialog::selectObjectInList(int objectHandle)
{ // select an item in the list control:
	for (int i=0;i<ui->qqRobotList->count();i++)
	{
		QListWidgetItem* it=ui->qqRobotList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectHandle)
			{
				it->setSelected(true);
				break;
			}
		}
	}
}

int CMtbRobotDialog::getSelectedObjectInList()
{ // get the item selected in the list control:
	QList<QListWidgetItem*> sel=ui->qqRobotList->selectedItems();
	if (sel.size()>0)
	{
		int data=sel.at(0)->data(Qt::UserRole).toInt();
		CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(data);
		if (mtbRobot!=NULL)
			return(mtbRobot->getID());
	}
	return(-1);
}

void CMtbRobotDialog::updateObjectsInList()
{
	ui->qqRobotList->clear();
	for (int i=0;i<CAccess::mtbRobotContainer->getCount();i++)
	{
		CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromIndex(i);
		int sceneObjectHandle=mtbRobot->getAssociatedObject();
		std::string txt;
		char* name=NULL;
		if (sceneObjectHandle!=-1)
			name=simGetObjectName(sceneObjectHandle);
		if (name==NULL)
			txt="Error";
		else
		{
			txt=name;
			simReleaseBuffer(name);
		}
		QListWidgetItem* itm=new QListWidgetItem(txt.c_str());
		itm->setData(Qt::UserRole,QVariant(mtbRobot->getID()));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui->qqRobotList->addItem(itm);
	}
}

void CMtbRobotDialog::on_qqAutomatic_clicked()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_write,-1);
	if (lockHandle!=-1)
	{
		int objectHandle=getSelectedObjectInList();
		if (objectHandle!=-1)
		{
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(objectHandle);
			if (mtbRobot!=NULL)
			{
				mtbRobot->setOptions(mtbRobot->getOptions()^1);
				simAnnounceSceneContentChange(); // so that we have an undo point here
			}
		}
		simUnlockResources(lockHandle);
	}
}

void CMtbRobotDialog::on_qqRobotList_itemSelectionChanged()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_read,-1);
	if (lockHandle!=-1)
	{
		if (!inRefreshRoutine)
		{
			int objectHandle=getSelectedObjectInList();
			simRemoveObjectFromSelection(sim_handle_all,-1);
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(objectHandle);
			if (mtbRobot!=NULL)
			{
				int sceneObjHandle=mtbRobot->getAssociatedObject();
				if (sceneObjHandle!=-1)
					simAddObjectToSelection(sim_handle_single,sceneObjHandle); // select the associated scene object!
			}
		}
		simUnlockResources(lockHandle);
	}
}

void CMtbRobotDialog::reject()
{
	// Reflect the visibility state in the menu bar item:
	simSetModuleMenuItemState(dialogMenuItemHandle,1,"MTB Robot Properties");
	done(0);
}


void CMtbRobotDialog::on_qqRobotProgram_textChanged()
{
	int lockHandle=simLockResources(sim_lock_ui_wants_to_write,-1);
	if (lockHandle!=-1)
	{
		int objectHandle=getSelectedObjectInList();
		if (objectHandle!=-1)
		{
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(objectHandle);
			if (mtbRobot!=NULL)
			{
				std::string prg=ui->qqRobotProgram->toPlainText().toStdString();
				mtbRobot->setProgram(prg); // Update the program
	//			simAnnounceSceneContentChange(); // we don't allow for undo points here (otherwise every single char could be undone)
			}
		}
		simUnlockResources(lockHandle);
	}
}
