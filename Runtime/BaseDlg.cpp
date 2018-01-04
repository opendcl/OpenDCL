// BaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseDlg.h"
#include "Workspace.h"
#include "PropertyIds.h"
#include "DclControlTemplate.h"
#include "DclPicture.h"
#include "InvokeMethod.h"
#include "MultiMon.h"


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

CBaseDlg::CBaseDlg(TDclFormPtr pSourceForm, UINT idd, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/)
: CDialog(idd, pParent)
, CArxDialogObject( pSourceForm, this )
, mptInitial( pParams? pParams->position : CPoint(INT_MIN, INT_MIN) )
, msizeInitial( pParams? pParams->size : CSize(0, 0) )
, mbHasTitleBar( pSourceForm->GetControlProperties()->GetBooleanProperty( Prop::TitleBar ) ||
								 !pSourceForm->GetControlProperties()->GetPropertyObject( Prop::TitleBar ) )
, mbResizable( pSourceForm->GetControlProperties()->GetBooleanProperty( Prop::AllowResizing ) )
, msizeGrip( GetSystemMetrics( SM_CXVSCROLL ), GetSystemMetrics( SM_CYHSCROLL ) )
, mrectGrip( 0, 0, 0, 0 )
{
	IgnoreSizing();
}

CBaseDlg::~CBaseDlg()
{
}

void CBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

void CBaseDlg::SavePosition()
{
	if( !IsWindow( m_hWnd ) )
		return;
	CRect rcWnd;
	GetWindowRect( &rcWnd );
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath(); 
	pApp->WriteProfileInt( sProfileName, _T("Width"), mpTemplate->GetLongProperty( Prop::Width ) );
	pApp->WriteProfileInt( sProfileName, _T("Height"), mpTemplate->GetLongProperty( Prop::Height ) );
	pApp->WriteProfileInt( sProfileName, _T("TopLeftX"), rcWnd.left );
	pApp->WriteProfileInt( sProfileName, _T("TopLeftY"), rcWnd.top );
}

void CBaseDlg::ReadPosition(POINT& ptTopLeft, SIZE& size) const
{	
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath();
	ptTopLeft.x = pApp->GetProfileInt( sProfileName, _T("TopLeftX"), INT_MIN );
	ptTopLeft.y = pApp->GetProfileInt( sProfileName, _T("TopLeftY"), INT_MIN );
	size.cx = pApp->GetProfileInt( sProfileName, _T("Width"), -1 );
	size.cy = pApp->GetProfileInt( sProfileName, _T("Height"), -1 );
}

void CBaseDlg::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	CRect rcGrip = mrectGrip;
	GetClientRect( &mrectGrip );
	mrectGrip.left = mrectGrip.right - msizeGrip.cx;
	mrectGrip.top = mrectGrip.bottom - msizeGrip.cy;
	CRgn rgnInvalid;
	rgnInvalid.CreateRectRgnIndirect( &rcGrip );
	CRgn rgnNew;
	rgnNew.CreateRectRgnIndirect( &mrectGrip );
	rgnInvalid.CombineRgn( &rgnInvalid, &rgnNew, RGN_OR );
	RedrawWindow( NULL, &rgnInvalid, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN );
}

bool CBaseDlg::OnApplyResizable( TPropertyPtr pProp )
{
	mbResizable = pProp->GetBooleanValue();
	return __super::OnApplyResizable( pProp );
}

BEGIN_MESSAGE_MAP(CBaseDlg, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZING()
	ON_WM_CAPTURECHANGED()
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CBaseDlg::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg message handlers

LRESULT CBaseDlg::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CBaseDlg::OnInitDialog()
{
	CPoint ptSaved( INT_MIN, INT_MIN );
	CSize sizeSaved( -1, -1 );
	ReadPosition(ptSaved, sizeSaved); //get the saved position before it gets overwritten during SetWindowPos()
	ApplyProperty( mpTemplate->GetPropertyObject( Prop::AllowResizing ) );
	ApplyProperty( mpTemplate->GetPropertyObject( Prop::TitleBar ) );

	__super::OnInitDialog();

	CRect rectWindow;
	GetWindowRect( &rectWindow );
	rectWindow.right = rectWindow.left + FromDIP( mpTemplate->GetLongProperty( Prop::Width ) ) + GetNCWidth();
	rectWindow.bottom = rectWindow.top + FromDIP( mpTemplate->GetLongProperty( Prop::Height ) ) + GetNCHeight();

	CRect rcDesktop;
	GetDesktopWindow()->GetClientRect( &rcDesktop );
#if defined(SM_CXVIRTUALSCREEN)
	int nVirtualScreenWidth = ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
	if( nVirtualScreenWidth > 0 ) //We have virtual screen support
	{
		rcDesktop.left = ::GetSystemMetrics( SM_XVIRTUALSCREEN );
		rcDesktop.top = ::GetSystemMetrics( SM_YVIRTUALSCREEN );
		rcDesktop.right = rcDesktop.left + nVirtualScreenWidth;
		rcDesktop.bottom = rcDesktop.top + ::GetSystemMetrics( SM_CYVIRTUALSCREEN );
	}
#endif //SM_CXVIRTUALSCREEN
	CRect rectParent;
	::GetWindowRect( ::GetParent(m_hWnd), &rectParent );
	if( mptInitial.x > INT_MIN )
		rectWindow.MoveToX( FromDIP( mptInitial.x ) );
	else if( ptSaved.x >= rcDesktop.left - 10 && ptSaved.x < rcDesktop.right - 10 )
		rectWindow.MoveToX( ptSaved.x );
	else
		rectWindow.MoveToX( rectParent.left + (rectParent.Width() - rectWindow.Width()) / 2 );
	if( mptInitial.y > INT_MIN )
		rectWindow.MoveToY( FromDIP( mptInitial.y ) );
	else if( ptSaved.y >= rcDesktop.top - 10 && ptSaved.y < rcDesktop.right - 10 )
		rectWindow.MoveToY( ptSaved.y );
	else
		rectWindow.MoveToY( rectParent.top + (rectParent.Height() - rectWindow.Height()) / 2 );
	if( IsResizable() )
	{
		if( msizeInitial.cx > 0 )
			rectWindow.right = rectWindow.left + FromDIP( msizeInitial.cy ) + GetNCWidth();
		else if( sizeSaved.cx > 0 )
			rectWindow.right = rectWindow.left + FromDIP( sizeSaved.cx ) + GetNCWidth();
		if( msizeInitial.cy > 0 )
			rectWindow.bottom = rectWindow.top + FromDIP( msizeInitial.cy ) + GetNCHeight();
		else if( sizeSaved.cy > 0 )
			rectWindow.bottom = rectWindow.top + FromDIP( sizeSaved.cy ) + GetNCHeight();
	}
	mpTemplate->SetLongProperty( Prop::Width, ToDIP( rectWindow.Width() - GetNCWidth() ) );
	mpTemplate->SetLongProperty( Prop::Height, ToDIP( rectWindow.Height() - GetNCHeight() ) );
	if( GetStyle() & WS_CHILD )
		GetParent()->ScreenToClient( &rectWindow );
	MoveWindow( &rectWindow, FALSE );
	mpControlPane->CheckDpiChanged();
	ApplyPropertiesEnum();
	IgnoreSizing( false );

	UINT nID = 1000;
	GetControlPane()->CreateControls( nID );
	GetControlPane()->RecalcLayout();

	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );
	GetArxServices()->HandleEvent( Prop::FormEventMove, false,
																 args_NN( ToDIP( rectWindow.left ), ToDIP( rectWindow.top ) ) );
	GetArxServices()->HandleEvent( Prop::FormEventSize, false,
																 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																					mpTemplate->GetLongProperty( Prop::Height ) ) );

	return FALSE;  // return TRUE  unless you set the focus to a control
}

BOOL CBaseDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	GetArxServices()->HandleEvent( Prop::EventHelp, false );
	return TRUE;
}

void CBaseDlg::OnClose() 
{
	bool bCancelling = (this->m_nModalResult == IDCANCEL || this->m_nModalResult == -1);
	if( IsClosing() || IsCloseAllowed( bCancelling ) )
	{
		SetClosing();
		__super::OnClose();
		if( IsModeless() && ::IsWindow( m_hWnd ) )
			DestroyWindow(); //if it's a modeless dialog, we have to destroy it yet
	}
	else
		SetClosing( false );
}

void CBaseDlg::OnDestroy() 
{
	SavePosition();
	CRect rcThis;
	GetWindowRect( &rcThis );
	GetArxServices()->HandleEvent( Prop::FormEventClose, args_NN( ToDIP( rcThis.left ), ToDIP( rcThis.top ) ) );
	DestroyIcon( SetIcon(NULL, FALSE) );
	GetControlPane()->CleanUpControls();	
	__super::OnDestroy();
}

void CBaseDlg::OnTimer( UINT_PTR nID )
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

void CBaseDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	int nNewWidth = pRect->right - pRect->left;
	int nNewHeight = pRect->bottom - pRect->top;
	CSize szMin( 0, 0 );
	CSize szMax( 0, 0 );
	GetMinMaxSize( szMin, szMax );

	if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT )
	{
		if (szMin.cx > 0 && nNewWidth < szMin.cx)
			pRect->left = pRect->right - szMin.cx;
		if (szMax.cx > 0 && nNewWidth > szMax.cx)
			pRect->left = pRect->right - szMax.cx;
	}

	if( fwSide == WMSZ_BOTTOMRIGHT || fwSide == WMSZ_RIGHT || fwSide == WMSZ_TOPRIGHT )
	{
		if (szMin.cx > 0 && nNewWidth < szMin.cx)
			pRect->right = pRect->left + szMin.cx;
		if (szMax.cx > 0 && nNewWidth > szMax.cx)
			pRect->right = pRect->left + szMax.cx;
	}

	if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMRIGHT )
	{
		if (szMin.cy > 0 && nNewHeight < szMin.cy)
			pRect->bottom = pRect->top + szMin.cy;
		if (szMax.cy > 0 && nNewHeight > szMax.cy)
			pRect->bottom = pRect->top + szMax.cy;
	}

	if( fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_TOP || fwSide == WMSZ_TOPRIGHT )
	{
		if (szMin.cy > 0 && nNewHeight < szMin.cy)
			pRect->top = pRect->bottom - szMin.cy;
		if (szMax.cy > 0 && nNewHeight > szMax.cy)
			pRect->top = pRect->bottom - szMax.cy;
	}

	__super::OnSizing(fwSide, pRect);
}

void CBaseDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	UpdateGripPos();
	if( IsIgnoreSizing() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, ToDIP( cx ) );
	mpTemplate->SetLongProperty( Prop::Height, ToDIP( cy ) );
	mpControlPane->RecalcLayout();
	//RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

__UINT_LRESULT CBaseDlg::OnNcHitTest(CPoint point) 
{
	CPoint pt = point;
	ScreenToClient( &pt );
	if( mbResizable &&
			pt.x >= 0 && pt.y >= 0 &&
			pt.x <= mrectGrip.right && pt.y <= mrectGrip.bottom )
	{ // if in client area
		if( (mrectGrip.right - pt.x) + (mrectGrip.bottom - pt.y) <= mrectGrip.Width() )
			return HTBOTTOMRIGHT; // and in size grip
	}

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
	if( IsIgnoreSizing() )
		return;
	if( ((lpwndpos->flags & (SWP_NOSIZE | SWP_NOMOVE)) != (SWP_NOSIZE | SWP_NOMOVE)) )
	{
		if( (lpwndpos->flags & SWP_NOMOVE) == 0 )
			HandleDpiChanged();
		SavePosition();
	}
}

BOOL CBaseDlg::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = HandleEraseBkgnd( pDC );
	if( !bResult )
		bResult = __super::OnEraseBkgnd(pDC);
	if( mbResizable && !IsZoomed() )
	{
		CRect rcClip;
		pDC->GetClipBox( &rcClip );
		CRect rcGrip;
		if( rcGrip.IntersectRect( &mrectGrip, rcClip ) )
		{
			int nBlock = mrectGrip.Width() / 8;
			for( int nRow = 2; nRow >= 0; --nRow )
			{
				for( int nCol = (2 - nRow); nCol >= 0; --nCol )
				{
					CPoint ptLR( mrectGrip.BottomRight() - CSize( (nCol * 2 + 1) * nBlock, (nRow * 2 + 1) * nBlock ) );
					CRect rcBlock( ptLR - CSize( nBlock, nBlock ), ptLR );
					if( rcBlock.IntersectRect( &rcBlock, &rcClip ) )
						pDC->FillSolidRect( &rcBlock, GetSysColor( COLOR_BTNSHADOW ) );
				}
			}
		}
	}
	return bResult;
}

void CBaseDlg::PostNcDestroy() 
{
	__super::PostNcDestroy();
	if( IsModeless() )
		delete this;
}

LRESULT CBaseDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}
