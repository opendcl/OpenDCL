// COptionsTabDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsTabDlg.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "DclFormTemplate.h"
#include "DialogControl.h"
#include "DclControlTemplate.h"
#include "ArxProject.h"

static const UINT& refWM_RECALCLAYOUT()
{
	static const UINT WM_RECALCLAYOUT = RegisterWindowMessage( _T("OpenDCL.RecalcLayout") );
	return WM_RECALCLAYOUT;
}


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
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(refWM_RECALCLAYOUT(),OnRecalcLayout)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsTabDlg message handlers

void COptionsTabDlg::OnTabActivation (BOOL bActivate) 
{
	if( bActivate )
		GetControlPane()->RecalcLayout();
}

BOOL COptionsTabDlg::OnTabChanging () {
	//----- TODO: Add your code here

	return (TRUE) ;
}

BOOL COptionsTabDlg::OnInitDialog() 
{
	CAcUiTabExtension::OnInitDialog();
	ApplyPropertiesEnum();
	ModifyStyleEx( 0, WS_EX_TRANSPARENT | WS_EX_CONTROLPARENT );
	IgnoreSizing( false );

	// call method to create the controls
	UINT nID = 1000;
	GetControlPane()->CreateControls(nID);
	GetControlPane()->RecalcLayout();
	
	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );	
	GetArxServices()->HandleEvent( Prop::FormEventSize, false,
																 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																					mpTemplate->GetLongProperty( Prop::Height ) ) );
	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void COptionsTabDlg::OnMainDialogOK()
// This function is called when the main dialog OK button is pressed.
{
	GetArxServices()->HandleEvent( Prop::EventOptionsOK, false );	
}

void COptionsTabDlg::OnMainDialogApply()
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

LRESULT COptionsTabDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}

void COptionsTabDlg::PostNcDestroy() 
{
	CAcUiTabExtension::PostNcDestroy();
	delete this;
}

void COptionsTabDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CAcUiTabExtension::OnShowWindow(bShow, nStatus);
	if( GetArxServices()->HandleEvent( Prop::FormEventShow, args_B( (bShow != FALSE) ) ) )
		return;
}

void COptionsTabDlg::OnDestroy() 
{
	GetControlPane()->CleanUpControls();
	CAcUiTabExtension::OnDestroy();
}

void COptionsTabDlg::OnTimer( UINT_PTR nID )
{
	switch( nID )
	{
	case CDialogObject::idUserTimer:
		StartTimer( (UINT)-1 );
		GetArxServices()->HandleEvent( Prop::FormEventTimer );
		break;
	default:
		__super::OnTimer( nID );
		break;
	};
}

void COptionsTabDlg::OnSize(UINT nType, int cx, int cy)
{
	//__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	GetArxServices()->HandleEvent( Prop::FormEventSize, false, args_NN( cx, cy ) );
	// needs to be posted so it happens after the deferred window positioning is completed
	PostMessage( refWM_RECALCLAYOUT() );
}

LRESULT COptionsTabDlg::OnRecalcLayout(WPARAM wParam, LPARAM lParam)
{
	mpControlPane->RecalcLayout();
	return 0;
}

BOOL COptionsTabDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

HBRUSH COptionsTabDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if( GetTheme().GetWindowTheme() )
	{
		mColorService.SetBackgroundColor( GetSysColor( COLOR_WINDOW ) );
		return mColorService.GetBackgroundBrush();
	}
	return __super::OnCtlColor(pDC, pWnd, nCtlColor);
}
