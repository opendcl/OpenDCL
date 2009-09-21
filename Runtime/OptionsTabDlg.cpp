// COptionsTabDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsTabDlg.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "ArxProject.h"


/////////////////////////////////////////////////////////////////////////////
// COptionsTabDlg dialog

COptionsTabDlg::COptionsTabDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAcUiTabChildDialog( pParent, pParams? (HINSTANCE)pParams->lpData : NULL ) 
, CArxDialogObject( pSourceForm, this )
{
}

COptionsTabDlg::~COptionsTabDlg()
{
}

void COptionsTabDlg::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
	SetClosing();
	GetMainDialog()->EndDialog( nStatus );
}


BEGIN_MESSAGE_MAP(COptionsTabDlg, CAcUiTabChildDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsTabDlg message handlers

void COptionsTabDlg::OnTabActivation (BOOL bActivate) 
{
	//----- TODO: Add your code here
}

BOOL COptionsTabDlg::OnTabChanging () {
	//----- TODO: Add your code here

	return (TRUE) ;
}

BOOL COptionsTabDlg::OnInitDialog() 
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
	
	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );	
	GetClientRect( &rectWindow );
	GetArxServices()->HandleEvent( Prop::FormEventSize,
																 args_NN( rectWindow.Width(), rectWindow.Height() ) );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void COptionsTabDlg::OnMainDialogOK()
// This function is called when the main dialog OK button is pressed.
{
	GetArxServices()->HandleEvent( Prop::EventOptionsOK, false );	
}

void COptionsTabDlg::OnMainDialogAPPLY()
// This function is called when the main dialog Apply button is pressed.
{
	GetArxServices()->HandleEvent( Prop::EventOptionsApply, false );	
}

void COptionsTabDlg::OnMainDialogCancel()
// This function is called when the main dialog CANCEL button is pressed.
{
	GetArxServices()->HandleEvent( Prop::EventOptionsCancel, false );	
}

BOOL COptionsTabDlg::OnMainDialogHelp()
// This function is called when the main dialog HELP button is pressed
// and this is the active tab.
{
	GetArxServices()->HandleEvent( Prop::EventOptionsHelp, false );	
  return TRUE;
}

void COptionsTabDlg::PostNcDestroy() 
{
	CAcUiTabExtension::PostNcDestroy();
	delete this;
}

void COptionsTabDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CAcUiTabExtension::OnShowWindow(bShow, nStatus);
	if( GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_B( (bShow != FALSE) ) ) )
		return;
}

void COptionsTabDlg::OnDestroy() 
{
	GetControlPane()->CleanUpControls();
	CAcUiTabExtension::OnDestroy();
}

void COptionsTabDlg::OnSize(UINT nType, int cx, int cy)
{
	//__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
	GetArxServices()->HandleEvent( Prop::FormEventSize, false, args_NN( cx, cy ) );
}
