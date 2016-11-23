// AcadPaletteHost.cpp : implementation file
//

#include "stdafx.h"
#include "AcadPaletteHost.h"
#include "PaletteDlg.h"


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
// CAcadPaletteHost dialog

CAcadPaletteHost::CAcadPaletteHost( CPaletteDlg* pDlgObject, CWnd *pParent /*= NULL*/ )
: CAdUiPaletteSet( ADUI_DOCK_CS_STDMOUSECLICKS | ADUI_DOCK_CS_DESTROY_ON_CLOSE )
, mpDlgObject( pDlgObject )
, mpParent( pParent? pParent : acedGetAcadFrame() )
, mbTrackingMouse( false )
, mbMouseLeft( true )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
#ifdef _BRXTARGET
, mszMRUSize( -1, -1)
#endif
{
#ifndef _GRXTARGET
	m_bAutoDelete = FALSE;
#endif
}


CAcadPaletteHost::~CAcadPaletteHost()
{
}

void CAcadPaletteHost::GetClientArea( CRect& rect )
{
	GetClientRect( &rect );
	if( !mpDlgObject->IsFloating() )
	{
		CRect rcAdj;
		GetAdjustedClientRect( rcAdj );
	#if !defined(_BRXTARGET) || (_BRXTARGET <= 15) //BRX16+ requires no adjustment
		rect.top += 5;
	#endif
		IntersectRect( &rect, &rect, &rcAdj );
	#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
		CRect rcContainer;
		GetParent()->GetClientRect( &rcContainer );
		IntersectRect( &rcAdj, &rcAdj, &rcContainer );
		rect.bottom = rcAdj.bottom;
		rect.MoveToXY(0, 0);
	#endif
	}
	else
	{
		CRect rcWnd;
		GetFullRect( rcWnd );
		ScreenToClient( &rcWnd.TopLeft() );
		rect.MoveToY( -rcWnd.top );
	}
}

bool CAcadPaletteHost::Create( LPCTSTR lpszTitle, CRect rect )
{
	bool bDisabled = (mpParent && !mpParent->IsWindowEnabled());
	if( !__super::Create( lpszTitle,
												WS_OVERLAPPED/* | WS_DLGFRAME*/ | WS_VISIBLE | (bDisabled? WS_DISABLED : 0) | WS_CLIPCHILDREN,
												rect, mpParent,
												PSS_AUTO_ROLLUP | PSS_PROPERTIES_MENU | PSS_CLOSE_BUTTON | PSS_SNAP | PSS_USE_SINGLE_PALETTE_TAB_NAME ) )
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


BEGIN_MESSAGE_MAP(CAcadPaletteHost, CAdUiPaletteSet)
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
// CAcadPaletteHost message handlers


BOOL CAcadPaletteHost::OnCommand (WPARAM wParam, LPARAM lParam)
{
	if( wParam == ID_ADUI_HIDEBAR )
	{ //ignore Hide command and close instead
		__super::OnClosing();
		mpDlgObject->CloseDialog( IDCANCEL );
		return TRUE;
	}
	return __super::OnCommand( wParam, lParam );
}

bool CAcadPaletteHost::CanFrameworkTakeFocus ()
{
	if( !__super::CanFrameworkTakeFocus() )
		return false;
	return (!mpDlgObject->IsKeepFocus() && mbMouseLeft );
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

void CAcadPaletteHost::SizeChanged( CRect *lpRect, BOOL bFloating, int flags )
{
	if( flags & ADUI_DOCK_NF_FRAMECHANGED )
		PostMessage( refWM_FRAMECHANGED() );
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	else if( flags & ADUI_DOCK_NF_SIZECHANGED ) //Bricscad doesn't set the correct flag when docking/undocking
		PostMessage( refWM_FRAMECHANGED() );
#endif
	__super::SizeChanged( lpRect, bFloating, flags );
}

LRESULT CAcadPaletteHost::OnFrameChanged(WPARAM wParam, LPARAM lParam)
{
	if( mhwndKeyboardFocus )
		ReleaseCapture();
	TDclControlPtr pProps = mpDlgObject->GetSourceForm()->GetControlProperties();
	TPropertyPtr pResizableProp = pProps->GetPropertyObject( Prop::AllowResizing );
	mpDlgObject->OnApplyResizable( pResizableProp );
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	if( !IsFloating() && mpDlgObject->IsResizable() )
	{ //Bricscad palette doesn't automatically expand to fill all available space
		CRect rcClient;
		GetClientArea( rcClient );
		SetWindowPos( NULL, 0, 0,
									rcClient.Width() + mpDlgObject->GetNCWidth(),
									rcClient.Height() + mpDlgObject->GetNCHeight(),
									SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
		UINT nFlags = (SWP_NOZORDER | SWP_NOACTIVATE/* | SWP_NOCOPYBITS*/ | SWP_NOOWNERZORDER);
		if( mpDlgObject->IsIgnoreSizing() || !mpDlgObject->IsResizable() )
			nFlags |= SWP_NOSIZE;
		mpDlgObject->SetWindowPos( NULL, rcClient.left, rcClient.top,
															 rcClient.Width(), rcClient.Height(),
															 nFlags );
	}
#endif
	return 0;
}

bool CAcadPaletteHost::OnClosing()
{
	__super::OnClosing();
	mpDlgObject->OnClosing();
	return true;
}

void CAcadPaletteHost::OnUserSizing(UINT fwSide, LPRECT pRect)
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

CSize CAcadPaletteHost::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	if( !mpDlgObject->IsResizable() )
		return CSize( mpDlgObject->GetTemplate()->GetLongProperty( Prop::Width ) + mpDlgObject->GetNCWidth(),
									mpDlgObject->GetTemplate()->GetLongProperty( Prop::Height ) + mpDlgObject->GetNCHeight() );
	CSize sizeDefault = __super::CalcFixedLayout( bStretch, bHorz );
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

#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
CSize CAcadPaletteHost::CalcDynamicLayout(int nLength, DWORD nMode)
{
//TCHAR szMsg[1024];
//_stprintf(szMsg, _T("## CalcDynamicLayout(nLength=%d, dMode = %x)\r\n"), nLength, nMode);
//OutputDebugString(szMsg);
	static long lLastLength = -1;
	static bool bLastHoriz = false;
	long lTitleBarWidth = (IsFloating()? 20 : 0);

	if( mszMRUSize.cx < 0 || mszMRUSize.cy < 0 )
		mszMRUSize.SetSize( mpDlgObject->GetTemplate()->GetLongProperty( Prop::Width ),
												mpDlgObject->GetTemplate()->GetLongProperty( Prop::Height ) );

	if( !mpDlgObject->IsResizable() )
		return CSize( mpDlgObject->GetTemplate()->GetLongProperty( Prop::Width ) + mpDlgObject->GetNCWidth() + lTitleBarWidth,
									mpDlgObject->GetTemplate()->GetLongProperty( Prop::Height ) + mpDlgObject->GetNCHeight() );

	CSize sizeDefault(mszMRUSize.cx, mszMRUSize.cy);
	if (nLength > 0)
	{
		if (nMode & LM_LENGTHY)
			sizeDefault.cy = nLength;
		else
			sizeDefault.cx = nLength - lTitleBarWidth;
	}

	if( nMode & LM_MRUWIDTH )
		sizeDefault.SetSize( mszMRUSize.cx, mszMRUSize.cy );
	else if( nMode & LM_COMMIT )
	{ //Bricscad sends LM_COMMIT always with the width, so we have to use the last sent length when it was vertical sizing
		if( bLastHoriz )
			mszMRUSize.cx = nLength - lTitleBarWidth;
		else if( lLastLength > 0 )
			mszMRUSize.cy = lLastLength;
	}
	//else if( nMode & (LM_HORZDOCK | LM_VERTDOCK) )
	//{
	//	if( !IsFloating() )
	//	{
	//	}
	//}
	lLastLength = nLength;
	bLastHoriz = !(nMode & LM_LENGTHY);

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
	sizeDefault.cx += (mpDlgObject->GetNCWidth() + lTitleBarWidth);
	sizeDefault.cy += mpDlgObject->GetNCHeight();
	return sizeDefault;
}
#endif

void CAcadPaletteHost::GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight)
{
}

void CAcadPaletteHost::GetMinimumSize(CSize& size)
{
	CSize szMax;
	mpDlgObject->GetMinMaxSize( size, szMax );
}

void CAcadPaletteHost::GetMaximumSize(CSize& size)
{
	CSize szMin;
	mpDlgObject->GetMinMaxSize( szMin, size );
}

void CAcadPaletteHost::TitleBarLocationUpdated( AdUiTitleBarLocation newLoc )
{
	if( mhwndKeyboardFocus )
		ReleaseCapture();
	__super::TitleBarLocationUpdated( newLoc );
	mpDlgObject->OnFrameChanged();
}

BOOL CAcadPaletteHost::Load(IUnknown* pNode)
{
	return __super::Load( pNode );
}

BOOL CAcadPaletteHost::Save(IUnknown* pNode)
{
	//The paletteset state is not saved unless at least one palette is added, so
	//here a temporary palette is added for that purpose. [ORW]
	CAdUiPalette pal;
	AddPalette( &pal );
	BOOL bResult = __super::Save( pNode );
	RemovePalette( &pal );
	return bResult;
}

void CAcadPaletteHost::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if( !mpDlgObject->GetControlWnd()->m_hWnd )
		return;
#ifdef _BRXTARGET
	if( !mpDlgObject->IsResizable() )
		mpDlgObject->OnFrameChanged();
	if( !IsFloating() )
		return;
#endif
	CRect rcClient;
	GetClientArea( rcClient );
	UINT nFlags = (SWP_NOZORDER | SWP_NOACTIVATE/* | SWP_NOCOPYBITS*/ | SWP_NOOWNERZORDER);
	if( mpDlgObject->IsIgnoreSizing() || !mpDlgObject->IsResizable() )
		nFlags |= SWP_NOSIZE;
	mpDlgObject->SetWindowPos( NULL, rcClient.left, rcClient.top,
														 rcClient.Width(), rcClient.Height(),
														 nFlags );
}

BOOL CAcadPaletteHost::PreTranslateMessage(MSG* pMsg)
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
					UINT nHitTest = (UINT)pTarget->SendMessage( WM_NCHITTEST, 0, MAKELPARAM(pMsg->pt.x, pMsg->pt.y) );
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

void CAcadPaletteHost::OnDestroy()
{
	__super::OnDestroy();
}

LRESULT CAcadPaletteHost::OnMouseEnter(WPARAM wParam, LPARAM lParam)
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

LRESULT CAcadPaletteHost::OnMouseLeave(WPARAM wParam, LPARAM lParam)
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

void CAcadPaletteHost::OnEnterMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = true;
	__super::OnEnterMenuLoop(bPopupMenu);
}

void CAcadPaletteHost::OnExitMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = false;
	__super::OnExitMenuLoop(bPopupMenu);
}

void CAcadPaletteHost::OnTimer(UINT_PTR nIDEvent)
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

void CAcadPaletteHost::PostNcDestroy()
{
	__super::PostNcDestroy();
	//delete this;
}

void CAcadPaletteHost::OnCaptureChanged(CWnd *pWnd)
{
	OnTimer( WM_MOUSELEAVE );
	__super::OnCaptureChanged(pWnd);
}

BOOL CAcadPaletteHost::OnEraseBkgnd(CDC* pDC)
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

void CAcadPaletteHost::OnClose()
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
