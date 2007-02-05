// CfgTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "CfgTabPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "ArxProject.h"


// CConfigTabPaneX interface implementation
CConfigTabPaneX::CConfigTabPaneX( CfgTabPane& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CConfigTabPaneX::~CConfigTabPaneX()
{
}

DclFormType CConfigTabPaneX::GetType() const
{
	return VdclConfigTab;
}

HWND CConfigTabPaneX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

bool CConfigTabPaneX::IsDirty() const
{
	return mpOwner->IsDirty();
}

bool CConfigTabPaneX::SetDirty( bool bDirty )
{
	mpOwner->SetDirty( bDirty );
	return true;
}

void CConfigTabPaneX::CloseDialog(int nStatus) const
{
	mpOwner->GetMainDialog()->EndDialog( nStatus );
}


/////////////////////////////////////////////////////////////////////////////
// CfgTabPane dialog

CfgTabPane::CfgTabPane( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAcUiTabChildDialog( pParent, pParams? (HINSTANCE)pParams->lpData : NULL ) 
, mDialogX( *this, pSourceForm )
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
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	
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
	mDialogX.GetControlPane().GetPaneWindowRect() = rcThis;

	// call method to create the controls
	UINT nID = 1000;
	mDialogX.GetControlPane().CreateControls(mDialogX.GetSourceForm(), nID);

	CRect rcClient;
	GetClientRect(&rcClient);
	// resize the control pane so all offsets are set correctly
	mDialogX.GetControlPane().SizeChanged(rcClient.Width(),rcClient.Height());	
	
	// call methods to invoke the event
	InvokeMethod(pProps->GetStrProperty(nFormEventInitialize), false);	

	GetWindowRect(&rcThis);
	// call methods to invoke the event
	InvokeMethodIntInt(pProps->GetStrProperty(nFormEventSize), rcThis.Width(), rcThis.Height(), false);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CfgTabPane::OnMainDialogOK()
// This function is called when the main dialog OK button is pressed.
{
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nCfgEventOK), false);	
}

void CfgTabPane::OnMainDialogAPPLY()
// This function is called when the main dialog Apply button is pressed.
{
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nCfgEventApply), false);	
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
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nCfgEventCancel), false);	
}

BOOL CfgTabPane::OnMainDialogHelp()
// This function is called when the main dialog HELP button is pressed
// and this is the active tab.
{
  // call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nCfgEventHelp), false);	
  // Return TRUE if handled.
  return TRUE;
}

void CfgTabPane::PostNcDestroy() 
{
	CAcUiTabExtension::PostNcDestroy();
	delete this;
}


void CfgTabPane::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CAcUiTabExtension::OnShowWindow(bShow, nStatus);

	CRect rectThis;
	
	GetWindowRect(&rectThis);

	mDialogX.GetControlPane().SizeChanged(rectThis.Width(),rectThis.Height());	
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod( pProps->GetStrProperty(nFormEventShow), false);	
}

void CfgTabPane::OnDestroy() 
{
	mDialogX.GetControlPane().CleanUpControls();
	CAcUiTabExtension::OnDestroy();
}
