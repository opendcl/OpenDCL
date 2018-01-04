// AcadDockBarHost.cpp : implementation file
//

#include "stdafx.h"
#include "AcadDockBarHost.h"
#include "ControlBarDlg.h"


static const UINT& refWM_MOUSEENTER()
{
	static const UINT WM_MOUSEENTER = RegisterWindowMessage( _T("OpenDCL.MouseEnter") );
	return WM_MOUSEENTER;
}

static const UINT& refWM_FRAMECHANGED()
{
	static const UINT WM_FRAMECHANGED = RegisterWindowMessage( _T("OpenDCL.FrameChanged") );
	return WM_FRAMECHANGED;
}

static bool IsDescendant( CWnd* pParent, CWnd* pDescendant )
{
	if( !pParent )
		return false;
	if( pParent == pDescendant )
		return true;
	CWnd* pNextParent = pDescendant->GetParent();
	while( pNextParent )
	{
		if( pNextParent == pParent )
			return true;
		pNextParent = pNextParent->GetParent();
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CAcadDockBarHost dialog

CAcadDockBarHost::CAcadDockBarHost( CControlBarDlg* pDlgObject, CWnd *pParent /*= NULL*/ )
: CAdUiDockControlBar( ADUI_DOCK_CS_STDMOUSECLICKS | ADUI_DOCK_CS_DESTROY_ON_CLOSE )
, mpDlgObject( pDlgObject )
, mpParent( pParent )
, mbTrackingMouse( false )
, mbMouseLeft( true )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
{
#ifndef _GRXTARGET
	m_bAutoDelete = FALSE;
#endif
}


CAcadDockBarHost::~CAcadDockBarHost()
{
}

void CAcadDockBarHost::GetClientArea( CRect& rect )
{
	GetUsedRect( rect );
#if (defined(_BRXTARGET) && _BRXTARGET <= 15)
	//GetUsedRect() returns invalid values in Bricscad!
	GetClientRect( &rect );
#endif
	if( !mpDlgObject->IsFloating() )
	{
		rect.top += 5;
	}
}

bool CAcadDockBarHost::Create( LPCTSTR lpszTitle, CRect rect, UINT nID )
{
	CString strWndClass = AfxRegisterWndClass( CS_DBLCLKS, LoadCursor( NULL, IDC_ARROW ) );
	bool bDisabled = (mpParent && !mpParent->IsWindowEnabled());
	if( !CAdUiDockControlBar::Create( strWndClass, lpszTitle,
																		WS_VISIBLE | WS_CHILD | (bDisabled? WS_DISABLED : 0) | WS_CLIPCHILDREN,
																		rect, mpParent, nID ) )
		return false;
	ModifyStyleEx( 0, WS_EX_CONTROLPARENT );
	__if_exists(idPinBtn)
	{
	CWnd* pPinBtn = GetDlgItem( idPinBtn );
	if( pPinBtn )
		pPinBtn->DestroyWindow();
	}
	return true;
}


BEGIN_MESSAGE_MAP(CAcadDockBarHost, CAdUiDockControlBar)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(refWM_FRAMECHANGED(),OnFrameChanged)
	ON_REGISTERED_MESSAGE(refWM_MOUSEENTER(),OnMouseEnter)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAcadDockBarHost message handlers


bool CAcadDockBarHost::CanFrameworkTakeFocus ()
{
	return (!mpDlgObject->IsKeepFocus() && mbMouseLeft );
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

void CAcadDockBarHost::SizeChanged( CRect *lpRect, BOOL bFloating, int flags )
{
	if( flags & ADUI_DOCK_NF_FRAMECHANGED )
		PostMessage( refWM_FRAMECHANGED() );
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	else if( flags & ADUI_DOCK_NF_SIZECHANGED ) //Bricscad doesn't set the correct flag when docking/undocking
		PostMessage( refWM_FRAMECHANGED() );
#endif
	__super::SizeChanged( lpRect, bFloating, flags );
}

LRESULT CAcadDockBarHost::OnFrameChanged(WPARAM wParam, LPARAM lParam)
{
	if( mhwndKeyboardFocus )
		ReleaseCapture();
	TDclControlPtr pProps = mpDlgObject->GetSourceForm()->GetControlProperties();
	TPropertyPtr pResizableProp = pProps->GetPropertyObject( Prop::AllowResizing );
	mpDlgObject->OnApplyResizable( pResizableProp );
	return 0;
}

bool CAcadDockBarHost::OnClosing()
{
	__super::OnClosing();
	mpDlgObject->OnClosing();
	return true;
	//mpDlgObject->OnClosing();
	//return __super::OnClosing();
}

BOOL CAcadDockBarHost::AddCustomMenuItems(LPARAM hMenu)
{
	if (IsWindowVisible())
	{
		::RemoveMenu( (HMENU)hMenu, ID_ADUI_ALLOWDOCK, MF_BYCOMMAND );
		::RemoveMenu( (HMENU)hMenu, ID_ADUI_HIDEBAR, MF_BYCOMMAND );
	}

	return TRUE;
}

void CAcadDockBarHost::OnUserSizing(UINT fwSide, LPRECT pRect)
{
	if( !mpDlgObject->IsResizable() )
	{
		GetParent()->GetParent()->SendMessage( WM_CANCELMODE, 0, 0 );
		return;
	}
	int nNewWidth = pRect->right - pRect->left;
	int nNewHeight = pRect->bottom - pRect->top;
	CSize szMin( 0, 0 );
	CSize szMax( 0, 0 );
	mpDlgObject->GetMinMaxSize( szMin, szMax );

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
	__super::OnUserSizing( fwSide, pRect );
}

CSize CAcadDockBarHost::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	if( !mpDlgObject->IsResizable() )
	{
		CSize sizeDefault( mpDlgObject->GetTemplate()->GetLongProperty( Prop::Width ),
											 mpDlgObject->GetTemplate()->GetLongProperty( Prop::Height ) );
		mpDlgObject->FromDIP( sizeDefault );
		return sizeDefault;
	}
#if (defined(_BRXTARGET) && _BRXTARGET <= 10)
	CSize sizeDefault;
	sizeDefault.cx = mpDlgObject->GetTemplate()->GetLongProperty( Prop::Width );
	sizeDefault.cy = mpDlgObject->GetTemplate()->GetLongProperty( Prop::Height );
	mpDlgObject->FromDIP( sizeDefault );
#else
	CSize sizeDefault = __super::CalcFixedLayout( bStretch, bHorz );
#endif
	CSize szMin( 0, 0 );
	CSize szMax( 0, 0 );
	mpDlgObject->GetMinMaxClientSize( szMin, szMax );

	if( szMin.cx > 0 && sizeDefault.cx < szMin.cx )
		sizeDefault.cx = szMin.cx;
	else if( szMax.cx > 0 && sizeDefault.cx > szMax.cx )
		sizeDefault.cx = szMax.cx;
	if( szMin.cy > 0 && sizeDefault.cy < szMin.cy )
		sizeDefault.cy = szMin.cy;
	else if( szMax.cy > 0 && sizeDefault.cy > szMax.cy )
		sizeDefault.cy = szMax.cy;
	return sizeDefault;
}

void CAcadDockBarHost::GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight)
{
	// by not adjusting the pnMinWidth or pnMinHeight, the min pnMinWidth will become small enough
	// that the OnResizeEvent will take care of the width size itself.
}

void CAcadDockBarHost::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if( !mpDlgObject->GetControlWnd()->m_hWnd )
		return;
	CRect rcClient;
	GetClientArea( rcClient );
	UINT nFlags = (SWP_NOZORDER | SWP_NOACTIVATE/* | SWP_NOCOPYBITS*/ | SWP_NOOWNERZORDER);
	if( mpDlgObject->IsIgnoreSizing() || !mpDlgObject->IsResizable() )
		nFlags |= SWP_NOSIZE;
	mpDlgObject->SetWindowPos( NULL, rcClient.left, rcClient.top,
														 rcClient.Width(), rcClient.Height(),
														 nFlags );
}

BOOL CAcadDockBarHost::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
	{
		if( pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL) )
		{
			HWND hItem = ::GetDlgItem( m_hWnd, IDCANCEL );
			if( hItem == NULL || ::IsWindowEnabled( hItem ) )
			{
				SendMessage( WM_COMMAND, IDCANCEL, 0 );
				return TRUE;
			}
		}
		if( mhwndKeyboardFocus )
		{
			pMsg->hwnd = mhwndKeyboardFocus;
			if( !IsDialogMessage( pMsg ) && !TranslateMessage( pMsg ) )
				DispatchMessage( pMsg );
			mhwndKeyboardFocus = ::GetFocus(); //in case the key caused focus to change (e.g. [Tab])
			return TRUE;
		}
		TDialogControlPtr pControl = mpDlgObject->GetControlPane()->FindControl( pMsg->hwnd );
		if( pControl && pControl->GetControlType() == CtlActiveX )
			return CWnd::PreTranslateMessage( pMsg ); //if it's for an ActiveX control, bypass the immediate base class
	}
	else if( pMsg->message == WM_NCMOUSEMOVE && !mbTrackingMouse )
	{
		SendMessage( refWM_MOUSEENTER(), 0, 0 );
	}
	else if( pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST )
	{
		if( mhwndKeyboardFocus && GetCapture() == this )
		{
			CWnd* pTarget = WindowFromPoint( pMsg->pt );
			if( pTarget )
			{
				CWnd* pTop = mpDlgObject->GetTopLevelWnd();
				if( IsDescendant( pTop, pTarget ) )
					SendMessage( refWM_MOUSEENTER(), 0, 0 );
				if( pMsg->message != WM_MOUSEMOVE )
				{
					mhwndKeyboardFocus = NULL;
					ReleaseCapture();
					pTarget->SetFocus();
					UINT nHitTest = pTarget->SendMessage( WM_NCHITTEST, 0, MAKELPARAM(pMsg->pt.x, pMsg->pt.y) );
					pTarget->SendMessage( WM_SETCURSOR, (WPARAM)pTarget->m_hWnd, MAKELPARAM(nHitTest, WM_MOUSEMOVE) );
				}
				if( pTarget != this )
				{
					if( !pTarget->PreTranslateMessage( pMsg ) )
					{
						CPoint ptMouse( pMsg->pt );
						pTarget->ScreenToClient( &ptMouse );
						LPARAM lParam = MAKELPARAM(ptMouse.x, ptMouse.y);
						pTarget->SendMessage( pMsg->message, pMsg->wParam, lParam );
					}
					return TRUE;
				}
			}
		}
		else if( !mbTrackingMouse )
		{
			CWnd* pTarget = WindowFromPoint( pMsg->pt );
			if( pTarget )
			{
				CWnd* pTop = mpDlgObject->GetTopLevelWnd();
				if( IsDescendant( pTop, pTarget ) )
					SendMessage( refWM_MOUSEENTER(), 0, 0 );
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

void CAcadDockBarHost::OnDestroy()
{
	__super::OnDestroy();
}

LRESULT CAcadDockBarHost::OnMouseEnter(WPARAM wParam, LPARAM lParam)
{
	if( !mbTrackingMouse )
	{
		mbTrackingMouse = true;
		SetTimer( WM_MOUSELEAVE, 200, NULL );
	}
	if( mhwndKeyboardFocus )
	{
		::SetFocus( mhwndKeyboardFocus );
		mhwndKeyboardFocus = NULL;
		ReleaseCapture();
	}
	if( mbMouseLeft )
	{
		mbMouseLeft = false;
		mpDlgObject->OnMouseEnter();
	}
	return 0;
}

LRESULT CAcadDockBarHost::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	mbTrackingMouse = false;
	if( !mbInMenuLoop && mpDlgObject->IsKeepFocus() && !::GetCapture() )
	{
		CWnd* pFocusWnd = GetFocus();
		if( !pFocusWnd || ((pFocusWnd->SendMessage( WM_GETDLGCODE, 0, 0 ) & (DLGC_WANTCHARS | DLGC_WANTARROWS)) == 0) )
		{
			CWnd* pCmdLine = acedGetAcadDockCmdLine();
			if( pCmdLine && pCmdLine->IsTopParentActive() && pCmdLine->IsWindowEnabled() && pCmdLine->IsWindowVisible() )
				pCmdLine->SetFocus();
		}
		else if( IsDescendant( mpDlgObject->GetTopLevelWnd(), pFocusWnd ) )
		{
			mhwndKeyboardFocus = pFocusWnd->m_hWnd;
			if( IsWindowEnabled() )
			{
				SetCapture();
				SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			}
		}
	}
	if( !mbMouseLeft )
	{
		mbMouseLeft = true;
		mpDlgObject->OnMouseLeave();
	}
#if defined(_BRXTARGET) || defined(_ZRXTARGET)
	CWnd* pFocusWnd = GetFocus();
	if( pFocusWnd && IsDescendant( this, pFocusWnd ) )
	{
		if( !SendMessage( WM_ACAD_KEEPFOCUS, 0, 0 ) )
			::SetFocus( adsw_acadMainWnd() );
	}
#endif
	return 0;
}

void CAcadDockBarHost::OnEnterMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = true;
	__super::OnEnterMenuLoop(bPopupMenu);
}

void CAcadDockBarHost::OnExitMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = false;
	__super::OnExitMenuLoop(bPopupMenu);
}

void CAcadDockBarHost::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == WM_MOUSELEAVE )
	{
		if( GetCapture() )
			return;
		CPoint ptCursor;
		if( GetCursorPos( &ptCursor ) )
		{
			CRect rcDlg;
			mpDlgObject->GetTopLevelWnd()->GetWindowRect( &rcDlg );
			if( !rcDlg.PtInRect( ptCursor ) )
			{
				KillTimer( WM_MOUSELEAVE );
				SendMessage( WM_MOUSELEAVE, 0, 0 );
			}
		}
		return;
	}
	__super::OnTimer(nIDEvent);
}

void CAcadDockBarHost::PostNcDestroy()
{
	__super::PostNcDestroy();
	//delete this;
}

void CAcadDockBarHost::OnCaptureChanged(CWnd *pWnd)
{
	OnTimer( WM_MOUSELEAVE );
	__super::OnCaptureChanged(pWnd);
}

BOOL CAcadDockBarHost::OnEraseBkgnd(CDC* pDC)
{
	if( !mpDlgObject->GetColorService()->IsBackgroundTransparent() )
	{
		CRect rcPaint;
		pDC->GetClipBox( &rcPaint );
		pDC->FillSolidRect( &rcPaint, mpDlgObject->GetColorService()->GetBackgroundColor() );
		return TRUE;
	}
	return __super::OnEraseBkgnd(pDC);
}

void CAcadDockBarHost::OnClose()
{
#if(_BRXTARGET >= 17)
	bool bMustDestroy = !IsFloating(); //BRX17+ only destroys the window in OnClose() if floating
#endif
	__super::OnClose();
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
#if (_BRXTARGET <= 14)
	SendMessage( WM_COMMAND, ID_ADUI_HIDEBAR, 0 );
#endif
	DestroyWindow();
#elif(_BRXTARGET >= 17)
	if (bMustDestroy)
		DestroyWindow();
#endif
}
