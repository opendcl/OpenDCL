// MainFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainFileDlg.h"
#include "Project.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"


const TCHAR sSizeWidth[] = _T("sizeWidth");
const TCHAR sSizeHeight[] = _T("sizeHeight");
const TCHAR sTopLeftX[] = _T("nTopLeftX");
const TCHAR sTopLeftY[] = _T("nTopLeftY");


/////////////////////////////////////////////////////////////////////////////
// CMainFileDlg dialog

CMainFileDlg::CMainFileDlg(CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/)
: CCommonDialog( pParent )
, mpSourceForm( pSourceForm )
, mnInitialX( pParams? pParams->position.x : -1 )
, mnInitialY( pParams? pParams->position.y : -1 )
, mnMinWidth( pSourceForm->GetControlProperties()->GetLongProperty( nMinDialogWidth ) )
, mnMinHeight( pSourceForm->GetControlProperties()->GetLongProperty( nMinDialogHeight ) )
, mnMaxWidth( pSourceForm->GetControlProperties()->GetLongProperty( nMaxDialogWidth ) )
, mnMaxHeight( pSourceForm->GetControlProperties()->GetLongProperty( nMaxDialogHeight ) )
, mnNCWidth( 0 )
, mnNCHeight( 0 )
, mbInitialized( false )
{
}

CMainFileDlg::~CMainFileDlg()
{
}

void CMainFileDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

void CMainFileDlg::SetDialogMinExtents(int nWidth, int nHeight)
{
	mnMinWidth = nWidth;
	mnMinHeight = nHeight;
}
	
void CMainFileDlg::SetDialogMaxExtents(int nWidth, int nHeight)
{
	mnMaxWidth = nWidth;
	mnMaxHeight = nHeight;
}

void CMainFileDlg::SavePosition()
{
	if( !IsWindow( m_hWnd ) )
		return;
	CWinApp* pApp = AfxGetApp();
	CRect rcThis;
	GetWindowRect( &rcThis );
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath(); 
	pApp->WriteProfileInt( sProfileName, sSizeWidth, rcThis.Width() );
	pApp->WriteProfileInt( sProfileName, sSizeHeight, rcThis.Height() );
	pApp->WriteProfileInt( sProfileName, sTopLeftX, rcThis.left );
	pApp->WriteProfileInt( sProfileName, sTopLeftY, rcThis.top );
}

CRect CMainFileDlg::ReadPosition() const
{	
	CRect rcRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath();
	rcRet.left = pApp->GetProfileInt( sProfileName, sTopLeftX, -100 );
	rcRet.top = pApp->GetProfileInt( sProfileName, sTopLeftY, -100 );
	rcRet.right = rcRet.left + pApp->GetProfileInt( sProfileName, sSizeWidth, -100 );
	rcRet.bottom = rcRet.top + pApp->GetProfileInt( sProfileName, sSizeHeight, -100 );
	return rcRet;
}

BEGIN_MESSAGE_MAP(CMainFileDlg, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFileDlg message handlers

void CMainFileDlg::Initialize()
{
	CRect rectSaved = ReadPosition(); //get the saved position before it gets overwritten during SetWindowPos()

	CDclControlObject* pFormProps = mpSourceForm->GetControlProperties();
	CRect rectWindow;
	GetWindowRect( &rectWindow );
	CRect rectClient;
	GetClientRect( &rectClient );
	mnNCWidth = rectWindow.Width() - rectClient.Width();
	mnNCHeight = rectWindow.Height() - rectClient.Height();
	rectWindow.right = rectWindow.left + pFormProps->GetLongProperty( nWidth );
	rectWindow.bottom = rectWindow.top + pFormProps->GetLongProperty( nHeight );
	bool bUsesClientRect = mpSourceForm->UsesClientRect();
	if( bUsesClientRect )
	{
		rectWindow.right += mnNCWidth;
		rectWindow.bottom += mnNCHeight;
	}

	SetWindowText( pFormProps->GetStrProperty( nTitleBarText ) );

	//create the control pane and the design time controls
	GetClientRect( &rectClient );
	CDialogObject* mpDlg = mpSourceForm->GetFormInstance();
	mpDlg->GetControlPane().SetPanePos( rectClient, false );
	UINT nID = 1000;
	mpDlg->GetControlPane().CreateControls( nID );
	mpDlg->GetControlPane().RecalcLayout();

	CRect rectParent;
	::GetWindowRect( ::GetParent(m_hWnd), &rectParent );
	if( mnInitialX >= 0 )
		rectWindow.MoveToX( mnInitialX );
	else if( rectSaved.left >= -10 && rectSaved.left < (::GetSystemMetrics( SM_CXSCREEN ) - 10) )
		rectWindow.MoveToX( rectSaved.left );
	else
		rectWindow.MoveToX( rectParent.left + (rectParent.Width() - rectWindow.Width()) / 2 );
	if( mnInitialY >= 0 )
		rectWindow.MoveToY( mnInitialY );
	else if( rectSaved.top >= -10 && rectSaved.top < (::GetSystemMetrics( SM_CYSCREEN ) - 10) )
		rectWindow.MoveToY( rectSaved.top );
	else
		rectWindow.MoveToY( rectParent.top + (rectParent.Height() - rectWindow.Height()) / 2 );
	if( mpDlg->IsResizable() && rectSaved.right > rectSaved.left && rectSaved.bottom > rectSaved.top )
	{
		rectWindow.right = rectWindow.left + rectSaved.Width();
		rectWindow.bottom = rectWindow.top + rectSaved.Height();
	}

	if( mnMinWidth > 0 )
	{
		if( bUsesClientRect )
			mnMinWidth += mnNCWidth;
		if( rectWindow.Width() < mnNCWidth )
			rectWindow.right = rectWindow.left + mnMinWidth;
	}
	if( mnMaxWidth > 0 )
	{
		if( mnMaxWidth > 0 )
			mnMaxWidth += mnNCWidth;
		if( rectWindow.Width() > mnMaxWidth )
			rectWindow.right = rectWindow.left + mnMaxWidth;
	}
	if( mnMinHeight > 0 )
	{
		if( bUsesClientRect )
			mnMinHeight += mnNCHeight;
		if( rectWindow.Height() < mnNCHeight )
			rectWindow.bottom = rectWindow.top + mnMinHeight;
	}
	if( mnMaxHeight > 0 )
	{
		if( mnMaxHeight > 0 )
			mnMaxHeight += mnNCHeight;
		if( rectWindow.Height() > mnMaxHeight )
			rectWindow.bottom = rectWindow.top + mnMaxHeight;
	}
	MoveWindow( &rectWindow, FALSE );
	
	InvokeMethod( pFormProps->GetStrProperty( nFormEventInitialize ), false );
	if( bUsesClientRect )
		GetClientRect( &rectWindow );
	else
		GetWindowRect( &rectWindow );
	InvokeMethodIntInt( pFormProps->GetStrProperty( nFormEventSize ), rectWindow.Width(), rectWindow.Height(), false );	
}

BOOL CMainFileDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	InvokeMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nEventOnHelp), false);
	return TRUE;
}

void CMainFileDlg::OnOK() 
{
	CString sEvent = mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose);
	if (sEvent.IsEmpty() || !(InvokeCancelMethod(sEvent, false)))
		__super::OnOK();
}

void CMainFileDlg::OnCancel() 
{
	CString sEvent = mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose);
	if (sEvent.IsEmpty() || !(InvokeCancelMethod(sEvent, false)))
		__super::OnCancel();
}

void CMainFileDlg::OnClose() 
{
	CString sEvent = mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose);
	if (sEvent.IsEmpty() || !(InvokeCancelMethod(sEvent, false)))
		__super::OnClose();				
}

void CMainFileDlg::OnDestroy() 
{
	SavePosition();
	CRect rcThis;
	GetWindowRect( &rcThis );
	InvokeMethodIntInt(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventClose), rcThis.left, rcThis.top, false);	
	CDialogObject* mpDlg = mpSourceForm->GetFormInstance();
	mpDlg->GetControlPane().CleanUpControls();	
	__super::OnDestroy();
}

void CMainFileDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	int nNewWidth = pRect->right - pRect->left;
	int nNewHeight = pRect->bottom - pRect->top;
	
	// ensure the user does not size below the min allowable size
	if (mnMinWidth > 0 && nNewWidth < mnMinWidth)
		pRect->right = pRect->left + mnMinWidth;

	if (mnMinHeight > 0 && nNewHeight < mnMinHeight)
		pRect->bottom = pRect->top + mnMinHeight;

	// ensure the user does not size above the max allowable size
	if (mnMaxWidth > 0 && nNewWidth > mnMaxWidth)
		pRect->right = pRect->left + mnMaxWidth;

	if (mnMaxHeight > 0 && nNewHeight > mnMaxHeight)
		pRect->bottom = pRect->top + mnMaxHeight;

	__super::OnSizing(fwSide, pRect);
}

void CMainFileDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
	if( mbInitialized )
	{
		CRect rectWindow;
		if( mpSourceForm->UsesClientRect() )
			GetClientRect( &rectWindow );
		else
			GetWindowRect( &rectWindow );
		InvokeMethodIntInt( mpSourceForm->GetControlProperties()->GetStrProperty( nFormEventSize ), rectWindow.Width(), rectWindow.Height(), false );	
		SavePosition();
	}
}

void CMainFileDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if( bShow && !mbInitialized )
	{
		Initialize();
		Invalidate();
		mbInitialized = true;
	}
	__super::OnShowWindow(bShow, nStatus);
}
