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
	
	CRect rectWindow;
	GetWindowRect( &rectWindow );
	
	// set the control pane position and size
	mDialogX.GetControlPane().SetPanePos( rectWindow );

	// call method to create the controls
	UINT nID = 1000;
	mDialogX.GetControlPane().CreateControls(mDialogX.GetSourceForm(), nID);
	
	//  setup for assigning the form it's properties
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();

	// get the left and top values to center the form on the screen	
	CPoint pt( (::GetSystemMetrics(SM_CYSCREEN) - rectWindow.Width()) / 2,
						 (::GetSystemMetrics(SM_CXSCREEN) - rectWindow.Height()) / 2 );
	
	// call method to set the start width and position of the form
	SetWindowPos(NULL, pt.x, pt.y, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE);
	
	// call methods to invoke the event
	InvokeMethod(pProps->GetStrProperty(nFormEventInitialize), false);	

	// call methods to invoke the event
	InvokeMethodIntInt(pProps->GetStrProperty(nFormEventSize), rectWindow.Width(), rectWindow.Height(), false);	

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

	mDialogX.GetControlPane().RecalcLayout();	
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod( pProps->GetStrProperty(nFormEventShow), false);	
}

void CfgTabPane::OnDestroy() 
{
	mDialogX.GetControlPane().CleanUpControls();
	CAcUiTabExtension::OnDestroy();
}
