// OptionsTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsTabPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "ArxProject.h"


/////////////////////////////////////////////////////////////////////////////
// COptionsTabPane dialog

COptionsTabPane::COptionsTabPane( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAcUiTabChildDialog( pParent, pParams? (HINSTANCE)pParams->lpData : NULL ) 
, CArxDialogObject( pSourceForm, this )
{
}

COptionsTabPane::~COptionsTabPane()
{
}

void COptionsTabPane::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
	SetClosing();
	GetMainDialog()->EndDialog( nStatus );
}


BEGIN_MESSAGE_MAP(COptionsTabPane, CAcUiTabChildDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsTabPane message handlers

void COptionsTabPane::OnTabActivation (BOOL bActivate) 
{
	//----- TODO: Add your code here
}

BOOL COptionsTabPane::OnTabChanging () {
	//----- TODO: Add your code here

	return (TRUE) ;
}

BOOL COptionsTabPane::OnInitDialog() 
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

void COptionsTabPane::OnMainDialogOK()
// This function is called when the main dialog OK button is pressed.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventOptionsOK), false);	
}

void COptionsTabPane::OnMainDialogAPPLY()
// This function is called when the main dialog Apply button is pressed.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventOptionsApply), false);	
}

void COptionsTabPane::OnMainDialogCancel()
// This function is called when the main dialog CANCEL button is pressed.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventOptionsCancel), false);	
}

BOOL COptionsTabPane::OnMainDialogHelp()
// This function is called when the main dialog HELP button is pressed
// and this is the active tab.
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventOptionsHelp), false);	
  return TRUE;
}

void COptionsTabPane::PostNcDestroy() 
{
	CAcUiTabExtension::PostNcDestroy();
	delete this;
}

void COptionsTabPane::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CAcUiTabExtension::OnShowWindow(bShow, nStatus);
	CString sShowEvent = mpTemplate->GetStringProperty( Prop::FormEventShow );
	if( !sShowEvent.IsEmpty() )
	{
		resbuf rbShow = { NULL, bShow? RTT : RTNIL };
		resbuf rbEventName = { &rbShow, RTSTR };
		rbEventName.resval.rstring = sShowEvent.LockBuffer();
		resbuf* prbResult = NULL;
		int nResult = acedInvokeNoDocStateSafe( &rbEventName, &prbResult );
		if( nResult == RTNORM && prbResult )
			acutRelRb( prbResult );
	}
}

void COptionsTabPane::OnDestroy() 
{
	GetControlPane()->CleanUpControls();
	CAcUiTabExtension::OnDestroy();
}

void COptionsTabPane::OnSize(UINT nType, int cx, int cy)
{
	//__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
	InvokeMethodIntInt( mpTemplate->GetStringProperty(Prop::FormEventSize), cx, cy, false );	
}
