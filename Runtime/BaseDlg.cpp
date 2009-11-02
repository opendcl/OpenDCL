// BaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseDlg.h"
#include "Workspace.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PictureObject.h"
#include "InvokeMethod.h"
#include "MultiMon.h"


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

CBaseDlg::CBaseDlg(TDclFormPtr pSourceForm, UINT idd, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/)
: CDialog(idd, pParent)
, CArxDialogObject( pSourceForm, this )
, mnInitialX( pParams? pParams->position.x : -1 )
, mnInitialY( pParams? pParams->position.y : -1 )
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
	CWinApp* pApp = AfxGetApp();
	CRect rcThis;
	GetWindowRect( &rcThis );
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath(); 
	pApp->WriteProfileInt( sProfileName, _T("Width"), rcThis.Width() );
	pApp->WriteProfileInt( sProfileName, _T("Height"), rcThis.Height() );
	pApp->WriteProfileInt( sProfileName, _T("TopLeftX"), rcThis.left );
	pApp->WriteProfileInt( sProfileName, _T("TopLeftY"), rcThis.top );
}

CRect CBaseDlg::ReadPosition() const
{	
	CRect rcRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath();
	rcRet.left = pApp->GetProfileInt( sProfileName, _T("TopLeftX"), -5000 );
	rcRet.top = pApp->GetProfileInt( sProfileName, _T("TopLeftY"), -5000 );
	rcRet.right = rcRet.left + pApp->GetProfileInt( sProfileName, _T("Width"), -5000 );
	rcRet.bottom = rcRet.top + pApp->GetProfileInt( sProfileName, _T("Height"), -5000 );
	return rcRet;
}

void CBaseDlg::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	InvalidateRect( &mrectGrip );
	GetClientRect( &mrectGrip );
	mrectGrip.left = mrectGrip.right - msizeGrip.cx;
	mrectGrip.top = mrectGrip.bottom - msizeGrip.cy;
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
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg message handlers

BOOL CBaseDlg::OnInitDialog()
{
	CRect rectSaved = ReadPosition(); //get the saved position before it gets overwritten during SetWindowPos()
	OnApplyProperty( mpTemplate->GetPropertyObject( Prop::AllowResizing ) );
	OnApplyProperty( mpTemplate->GetPropertyObject( Prop::TitleBar ) );

	__super::OnInitDialog();

	CRect rectWindow;
	GetWindowRect( &rectWindow );
	rectWindow.right = rectWindow.left + mpTemplate->GetLongProperty( Prop::Width ) + GetNCWidth();
	rectWindow.bottom = rectWindow.top + mpTemplate->GetLongProperty( Prop::Height ) + GetNCHeight();

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
	if( mnInitialX >= 0 )
		rectWindow.MoveToX( mnInitialX );
	else if( rectSaved.left >= rcDesktop.left - 10 && rectSaved.left < rcDesktop.right - 10 )
		rectWindow.MoveToX( rectSaved.left );
	else
		rectWindow.MoveToX( rectParent.left + (rectParent.Width() - rectWindow.Width()) / 2 );
	if( mnInitialY >= 0 )
		rectWindow.MoveToY( mnInitialY );
	else if( rectSaved.top >= rcDesktop.top - 10 && rectSaved.top < rcDesktop.right - 10 )
		rectWindow.MoveToY( rectSaved.top );
	else
		rectWindow.MoveToY( rectParent.top + (rectParent.Height() - rectWindow.Height()) / 2 );
	if( IsResizable() && rectSaved.right > rectSaved.left && rectSaved.bottom > rectSaved.top )
	{
		rectWindow.right = rectWindow.left + rectSaved.Width();
		rectWindow.bottom = rectWindow.top + rectSaved.Height();
	}
	if( GetStyle() & WS_CHILD )
		GetParent()->ScreenToClient( &rectWindow );
	MoveWindow( &rectWindow, FALSE );
	mpTemplate->SetLongProperty( Prop::Width, rectWindow.Width() - GetNCWidth() );
	mpTemplate->SetLongProperty( Prop::Height, rectWindow.Height() - GetNCHeight() );
	ApplyPropertiesEnum();
	IgnoreSizing( false );

	UINT nID = 1000;
	GetControlPane()->CreateControls( nID );
	GetControlPane()->RecalcLayout();

	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );
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
	GetArxServices()->HandleEvent( Prop::FormEventClose, args_NN( rcThis.left, rcThis.top ) );
	DestroyIcon( SetIcon(NULL, FALSE) );
	GetControlPane()->CleanUpControls();	
	__super::OnDestroy();
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
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
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
	if( (lpwndpos->flags & (SWP_NOSIZE | SWP_NOMOVE) != (SWP_NOSIZE | SWP_NOMOVE)) )
		SavePosition();
}

BOOL CBaseDlg::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = HandleEraseBkgnd( pDC );
	if( !bResult )
		bResult = __super::OnEraseBkgnd(pDC);
	if( mbResizable && !IsZoomed() )
	{
		int nBlock = mrectGrip.Width() / 8;
		for( int nRow = 2; nRow >= 0; --nRow )
		{
			for( int nCol = (2 - nRow); nCol >= 0; --nCol )
			{
				CPoint ptLR( mrectGrip.BottomRight() - CSize( (nCol * 2 + 1) * nBlock, (nRow * 2 + 1) * nBlock ) );
				CRect rcBlock( ptLR - CSize( nBlock, nBlock ), ptLR );
				pDC->FillSolidRect( &rcBlock, GetSysColor( COLOR_BTNSHADOW ) );
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
	if( message == WM_PAINT )
		OnValidateBkgnd( NULL );

	return __super::WindowProc(message, wParam, lParam);
}
