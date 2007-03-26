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
, mnMinWidth( pSourceForm->GetControlProperties()->GetLngProperty( nMinDialogWidth ) )
, mnMinHeight( pSourceForm->GetControlProperties()->GetLngProperty( nMinDialogHeight ) )
, mnMaxWidth( pSourceForm->GetControlProperties()->GetLngProperty( nMaxDialogWidth ) )
, mnMaxHeight( pSourceForm->GetControlProperties()->GetLngProperty( nMaxDialogHeight ) )
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
	rcRet.left = pApp->GetProfileInt( sProfileName, sTopLeftX, 0 );
	rcRet.top = pApp->GetProfileInt( sProfileName, sTopLeftY, 0 );
	rcRet.right = rcRet.left + pApp->GetProfileInt( sProfileName, sSizeWidth, -1 );
	rcRet.bottom = rcRet.top + pApp->GetProfileInt( sProfileName, sSizeHeight, -1 );
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
	ON_WM_CLOSE()
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
	rectWindow.right = rectWindow.left + pFormProps->GetLngProperty( nWidth );
	rectWindow.bottom = rectWindow.top + pFormProps->GetLngProperty( nHeight );
	if( mpSourceForm->UsesClientRect() )
	{
		rectWindow.right += mnNCWidth;
		rectWindow.bottom += mnNCHeight;
	}
	if( mnInitialX >= 0 )
		rectWindow.MoveToX( mnInitialX );
	else
		rectWindow.MoveToX( (::GetSystemMetrics( SM_CXSCREEN ) - rectWindow.Width()) / 2 );
	if( mnInitialY > 0 )
		rectWindow.MoveToY( mnInitialY );
	else
		rectWindow.MoveToY( (::GetSystemMetrics( SM_CYSCREEN ) - rectWindow.Height()) / 2 );

	SetWindowPos( NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(),
								SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING );

	CDialog::OnInitDialog();

	SetWindowText( pFormProps->GetStrProperty( nTitleBarText ) );
	SetTitleBarIcon( pFormProps->GetLngProperty( nIcon ) );

	//create the control pane and the design time controls
	GetClientRect( &rectClient );
	GetControlPane().SetPanePos( rectClient, false );
	UINT nID = 1000;
	GetControlPane().CreateControls( nID );

	if( rectSaved.right > rectSaved.left && rectSaved.bottom > rectSaved.top )
	{
		// ensure the dialog box is on screen
		if( rectSaved.left < (::GetSystemMetrics( SM_CXSCREEN ) - 10) &&
				rectSaved.top < (::GetSystemMetrics( SM_CYSCREEN ) - 10) )
			rectWindow.MoveToXY( rectSaved.left, rectSaved.top );
		if( pFormProps->GetBoolProperty( nResizable ) )
		{
			rectWindow.right = rectWindow.left + rectSaved.Width();
			rectWindow.bottom = rectWindow.top + rectSaved.Height();
		}
		MoveWindow( &rectWindow, FALSE );
	}

	UpdateGripPos();

	if( mnMinWidth > 0 && mpSourceForm->UsesClientRect() )
	{
		mnMinWidth += mnNCWidth;
		mnMaxWidth += mnNCWidth;
		mnMinHeight += mnNCHeight;
		mnMaxHeight += mnNCHeight;
	}
	
	InvokeMethod( pFormProps->GetStrProperty( nFormEventInitialize ), false );
	bool bClientRect = mpSourceForm->UsesClientRect();
	if( mpSourceForm->UsesClientRect() )
		GetClientRect( &rectClient );
	InvokeMethodIntInt( pFormProps->GetStrProperty( nFormEventSize ), rectWindow.Width(), rectWindow.Height(), false );	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBaseDlg::OnClose() 
{
	SavePosition();
	__super::OnClose();				
}

void CBaseDlg::OnDestroy() 
{
	DestroyIcon( SetIcon(NULL, FALSE) );
	SavePosition();
	CRect rectWindow;
	GetWindowRect( &rectWindow );
	InvokeMethodIntInt( mpSourceForm->GetControlProperties()->GetStrProperty( nFormEventClose ),
											rectWindow.left, rectWindow.top, false );
	GetControlPane().CleanUpControls();	
	__super::OnDestroy();
}

void CBaseDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	int nNewWidth = pRect->right - pRect->left;
	int nNewHeight = pRect->bottom - pRect->top;
	
	// ensure the user does not size below the min allowable size
	if (nNewWidth < mnMinWidth)
		pRect->right = pRect->left + mnMinWidth;

	if (nNewHeight < mnMinHeight)
		pRect->bottom = pRect->top + mnMinHeight;

	// ensure the user does not size above the max allowable size
	if (nNewWidth > mnMaxWidth)
		pRect->right = pRect->left + mnMaxWidth;

	if (nNewHeight > mnMaxHeight)
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
