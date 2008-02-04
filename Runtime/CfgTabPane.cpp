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


/////////////////////////////////////////////////////////////////////////////
// CfgTabPane dialog

CfgTabPane::CfgTabPane( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAcUiTabChildDialog( pParent, pParams? (HINSTANCE)pParams->lpData : NULL ) 
, CArxDialogObject( pSourceForm, this )
{
}

CfgTabPane::~CfgTabPane()
{
}

void CfgTabPane::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
	SetClosing();
	GetMainDialog()->EndDialog( nStatus );
}


BEGIN_MESSAGE_MAP(CfgTabPane, CAcUiTabChildDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

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

BOOL CfgTabPane::OnInitDialog() 
{
	CAcUiTabExtension::OnInitDialog();
	ApplyPropertiesEnum();

	// call method to create the controls
	UINT nID = 1000;
	GetControlPane()->CreateControls(nID);
	GetControlPane()->RecalcLayout();

	// get the left and top values to center the form on the screen	
	CRect rectWindow;
	GetWindowRect( &rectWindow );
	CPoint pt( (::GetSystemMetrics(SM_CXSCREEN) - rectWindow.Width()) / 2,
						 (::GetSystemMetrics(SM_CYSCREEN) - rectWindow.Height()) / 2 );
	
	// call method to set the start width and position of the form
	SetWindowPos( NULL, pt.x, pt.y, rectWindow.Width(), rectWindow.Height(),
								SWP_NOZORDER | SWP_NOACTIVATE );
	
	// call methods to invoke the event
	InvokeMethod( mpTemplate->GetStringProperty(Prop::FormEventInitialize), false );	
	GetClientRect( &rectWindow );
	InvokeMethodIntInt( mpTemplate->GetStringProperty(Prop::FormEventSize),
											rectWindow.Width(), rectWindow.Height(), false );	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CfgTabPane::OnMainDialogOK()
// This function is called when the main dialog OK button is pressed.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::CfgEventOK), false);	
}

void CfgTabPane::OnMainDialogAPPLY()
// This function is called when the main dialog Apply button is pressed.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::CfgEventApply), false);	
}

void CfgTabPane::OnMainDialogCancel()
// This function is called when the main dialog CANCEL button is pressed.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::CfgEventCancel), false);	
}

BOOL CfgTabPane::OnMainDialogHelp()
// This function is called when the main dialog HELP button is pressed
// and this is the active tab.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::CfgEventHelp), false);	
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
	InvokeMethod( mpTemplate->GetStringProperty(Prop::FormEventShow), false);	
}

void CfgTabPane::OnDestroy() 
{
	GetControlPane()->CleanUpControls();
	CAcUiTabExtension::OnDestroy();
}

void CfgTabPane::OnSize(UINT nType, int cx, int cy)
{
	//__super::OnSize(nType, cx, cy);
	if( mbIgnoreSizing )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
}
