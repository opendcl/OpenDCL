// BaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseDlg.h"
#include "Project.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PictureObject.h"
#include "InvokeMethod.h"


const TCHAR sSizeWidth[] = _T("sizeWidth");
const TCHAR sSizeHeight[] = _T("sizeHeight");
const TCHAR sTopLeftX[] = _T("nTopLeftX");
const TCHAR sTopLeftY[] = _T("nTopLeftY");


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

CBaseDlg::CBaseDlg(CDclFormObject* pSourceForm, UINT idd, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/)
: CDialog(idd, pParent)
, mpSourceForm( pSourceForm )
, mnInitialX( pParams? pParams->position.x : -1 )
, mnInitialY( pParams? pParams->position.y : -1 )
, mbHasTitleBar( pSourceForm->GetControlProperties()->GetBoolProperty( nTitleBar ) )
, mnMinWidth( pSourceForm->GetControlProperties()->GetLongProperty( nMinDialogWidth ) )
, mnMinHeight( pSourceForm->GetControlProperties()->GetLongProperty( nMinDialogHeight ) )
, mnMaxWidth( pSourceForm->GetControlProperties()->GetLongProperty( nMaxDialogWidth ) )
, mnMaxHeight( pSourceForm->GetControlProperties()->GetLongProperty( nMaxDialogHeight ) )
, mnNCWidth( 0 )
, mnNCHeight( 0 )
, mbShowGrip( pSourceForm->GetControlProperties()->GetBoolProperty( nResizable ) )
{
	m_sizing = FALSE;
	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);
}

CBaseDlg::~CBaseDlg()
{
}

void CBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CBaseDlg::SetDialogMinExtents(int nWidth, int nHeight)
{
	mnMinWidth = nWidth;
	mnMinHeight = nHeight;
}
	
void CBaseDlg::SetDialogMaxExtents(int nWidth, int nHeight)
{
	mnMaxWidth = nWidth;
	mnMaxHeight = nHeight;
}

void CBaseDlg::SavePosition()
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

CRect CBaseDlg::ReadPosition() const
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

void CBaseDlg::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	InvalidateRect( &m_rcGripRect );
	GetClientRect( &m_rcGripRect );
	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;
}

void CBaseDlg::SetTitleBarIcon(int nPictureID)
{
	DestroyIcon( SetIcon( NULL, FALSE ) );
	CPictureObject* pPicture = mpSourceForm->GetProject()->FindPicture( nPictureID );
	if( pPicture )
		SetIcon( pPicture->CloneIcon(), FALSE );
	else
		SetIcon( CopyIcon( AfxGetApp()->GetMainWnd()->GetIcon( FALSE ) ), FALSE );
}

BEGIN_MESSAGE_MAP(CBaseDlg, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZING()
	ON_WM_CAPTURECHANGED()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg message handlers

BOOL CBaseDlg::OnInitDialog()
{
	CRect rectSaved = ReadPosition(); //get the saved position before it gets overwritten during SetWindowPos()
	if( mbHasTitleBar )
		ModifyStyle( 0, WS_CAPTION, 0 );
	else
		ModifyStyle( WS_CAPTION, 0, 0 );
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING );

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

	CDialog::OnInitDialog();

	SetWindowText( pFormProps->GetStrProperty( nTitleBarText ) );
	SetTitleBarIcon( pFormProps->GetLongProperty( nIcon ) );

	//create the control pane and the design time controls
	GetClientRect( &rectClient );
	GetControlPane().SetPanePos( rectClient, false );
	UINT nID = 1000;
	GetControlPane().CreateControls( nID );
	GetControlPane().RecalcLayout();

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
	if( GetDialogObject().IsResizable() && rectSaved.right > rectSaved.left && rectSaved.bottom > rectSaved.top )
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
	UpdateGripPos();
	
	InvokeMethod( pFormProps->GetStrProperty( nFormEventInitialize ), false );
	if( bUsesClientRect )
		GetClientRect( &rectWindow );
	InvokeMethodIntInt( pFormProps->GetStrProperty( nFormEventSize ), rectWindow.Width(), rectWindow.Height(), false );	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CBaseDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	InvokeMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nEventOnHelp), false);
	return TRUE;
}

void CBaseDlg::OnClose() 
{
	CString sEvent = mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose);
	if (sEvent.IsEmpty() || !(InvokeCancelMethod(sEvent, false)))
	{
		__super::OnClose();
		if( GetDialogObject().IsModeless() && ::IsWindow( m_hWnd ) )
			DestroyWindow(); //if it's a modeless dialog, we have to destroy it yet
	}
}

void CBaseDlg::OnDestroy() 
{
	SavePosition();
	CRect rcThis;
	GetWindowRect( &rcThis );
	InvokeMethodIntInt(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventClose), rcThis.left, rcThis.top, false);	
	DestroyIcon( SetIcon(NULL, FALSE) );
	GetControlPane().CleanUpControls();	
	__super::OnDestroy();
}

void CBaseDlg::OnSizing(UINT fwSide, LPRECT pRect) 
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
	
	m_sizing = TRUE;
}

void CBaseDlg::OnCaptureChanged(CWnd *pWnd) 
{
	m_sizing = FALSE;
	__super::OnCaptureChanged(pWnd);
}

void CBaseDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	UpdateGripPos();
}

void CBaseDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (mbShowGrip)
		dc.DrawFrameControl(&m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}

#if (_ACADTARGET == 16)
UINT CBaseDlg::OnNcHitTest(CPoint point) 
#else
LRESULT CBaseDlg::OnNcHitTest(CPoint point) 
#endif
{
	CPoint pt = point;
	ScreenToClient(&pt);
	if (mbShowGrip && m_rcGripRect.PtInRect(pt) && pt.x >= 0 && pt.y >= 0) // if in size grip and in client area
		return HTBOTTOMRIGHT;

	if( !mbHasTitleBar ) //if no title bar, return HTCAPTION for any uninhabited area of the dialog
	{
		CWnd* pWnd = WindowFromPoint( point );
		if( pWnd == this )
			return HTCAPTION;
	}
	return __super::OnNcHitTest(point);
}

void CBaseDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
	SavePosition();
}

void CBaseDlg::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
