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

#ifndef MTBROBOTDIALOG_H
#define MTBROBOTDIALOG_H

#include <QDialog>

namespace Ui {
    class CMtbRobotDialog;
}

class CMtbRobotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CMtbRobotDialog(QWidget *parent = 0);
    ~CMtbRobotDialog();

	void refresh();

	void makeVisible(bool visible);
	bool getVisible();
	void updateObjectsInList();
	void selectObjectInList(int objectHandle);
	int getSelectedObjectInList();

	int dialogMenuItemHandle;
	bool inRefreshRoutine;

	void reject();

private slots:

	void on_qqAutomatic_clicked();

	void on_qqRobotList_itemSelectionChanged();

	void on_qqRobotProgram_textChanged();

private:
    Ui::CMtbRobotDialog *ui;
};

#endif // MTBROBOTDIALOG_H
