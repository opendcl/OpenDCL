//
// (C) Copyright 1998-1999 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
// Tab1.cpp : implementation file
//

#include "stdafx.h"
#include "CfgTabPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "ArxProject.h"


/////////////////////////////////////////////////////////////////////////////
// CfgTabPane dialog

CfgTabPane::CfgTabPane(CDclFormObject* pSourceForm,
											 CWnd* pParent /*=NULL*/,
											 HINSTANCE hInstance /*=NULL*/)
: CAcUiTabChildDialog (pParent, hInstance) 
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm )
, mpControl( NULL )
{
}

CfgTabPane::~CfgTabPane()
{
	OnDestroy();
}


/*
BEGIN_MESSAGE_MAP(CfgTabPane, CAcUiTabExtension)
	//{{AFX_MSG_MAP(CfgTabPane)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// CfgTabPane message handlers

void CfgTabPane::OnTabActivation (BOOL bActivate) 
{
	//----- TODO: Add your code here
}

BOOL CfgTabPane::OnTabChanging () {
	//----- TODO: Add your code here

	return (TRUE) ;
}

BOOL CfgTabPane::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CfgTabPane::OnInitDialog() 
{
	CAcUiTabExtension::OnInitDialog();
	
	//  setup for assigning the form it's properties
	CPoint pt;
	CRect rectThis;
	
	// get the form's properties
	CDclControlObject* pControlObject = mpSourceForm->GetControlProperties();
	m_ArxControl = pControlObject;
	
	GetWindowRect(&rectThis);
	// setup the rect default rect 
	rectThis.top = 0;
	rectThis.left = 0;
	
	// get the width
	//int nCtlWidth = pControlObject->GetLngProperty(nWidth);
	int nCtlWidth = rectThis.Width();
	rectThis.right = nCtlWidth;

	// get the height
	//int nCtlHeight = pControlObject->GetLngProperty(nHeight);
	int nCtlHeight = rectThis.Height();
	rectThis.bottom = nCtlHeight;

    // get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - rectThis.Height()) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - rectThis.Width()) / 2;
	
	// call method to set the start width and position of the form
	SetWindowPos(NULL, pt.x, pt.y, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	
	// set the rect for the control pane to be created
	CRect rcThis(0,0, nCtlWidth, nCtlHeight);
	
	// create the control pane that will display the controls
	mControlPane.m_pControlCol = &m_ControlCol;
	mControlPane.m_pFontCollection = m_pFontCollection;
	mControlPane.m_PanePos = rcThis;
	mControlPane.m_pParentDlg = this;

	// call method to create the controls
	mControlPane.CreateControls(mpSourceForm, 1000);

	CRect rcClient;
	GetClientRect(&rcClient);
	// resize the control pane so all offsets are set correctly
	mControlPane.SizeChanged(rcClient.Width(),rcClient.Height());	
	
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nFormEventInitialize), 
		false);	

	GetWindowRect(&rcThis);
	// call methods to invoke the event
	InvokeMethodIntInt(
		m_ArxControl->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CfgTabPane::OnMainDialogOK()
// This function is called when the main dialog OK button is pressed.
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nCfgEventOK), 
		false);	

}

void CfgTabPane::OnMainDialogAPPLY()
// This function is called when the main dialog Apply button is pressed.
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nCfgEventApply), 
		false);	

}

/*BOOL PreTranslateMessage(MSG* pMsg)
{
	MSG * nMsg = pMsg;
	return TRUE;
}
*/
void CfgTabPane::OnMainDialogCancel()
// This function is called when the main dialog CANCEL button is pressed.
{
    // call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nCfgEventCancel), 
		false);	

}

BOOL CfgTabPane::OnMainDialogHelp()
// This function is called when the main dialog HELP button is pressed
// and this is the active tab.
{
    // call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nCfgEventHelp), 
		false);	


    // Return TRUE if handled.
    return TRUE;
}

void CfgTabPane::PostNcDestroy() 
{
	CAcUiTabExtension::PostNcDestroy();
	theArxWorkspace.RemoveDialog(mpSourceForm);
}


void CfgTabPane::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CAcUiTabExtension::OnShowWindow(bShow, nStatus);

	CRect rectThis;
	
	GetWindowRect(&rectThis);

	mControlPane.SizeChanged(rectThis.Width(),rectThis.Height());	
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nFormEventShow), 
		false);	

	
}

void CfgTabPane::OnDestroy() 
{
	for (int i=0; i<m_ControlCol.GetCount(); i++)
	{
		POSITION pos = m_ControlCol.FindIndex(i);
		if (pos != NULL)
		{
			CArxDialogControl *pCtrl = m_ControlCol.GetAt(pos);
			m_ControlCol.RemoveAt(pos);
			delete pCtrl;
		}
	}
	mControlPane.CleanUpControls();
	mControlPane.m_pParentDlg = NULL;
	
	CAcUiTabExtension::OnDestroy();
	
		
}
