// AcadDockBarHost.cpp : implementation file
//

#include "stdafx.h"
#include "AcadDockBarHost.h"
#include "DockingDialog.h"


static const UINT& refWM_MOUSEENTER()
{
	static const UINT WM_MOUSEENTER = RegisterWindowMessage( _T("OpenDCL.MouseEnter") );
	return WM_MOUSEENTER;
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

CAcadDockBarHost::CAcadDockBarHost( CDockingDialog* pDlgObject, CWnd *pParent /*= NULL*/ )
: CAdUiDockControlBar( ADUI_DOCK_CS_STDMOUSECLICKS | ADUI_DOCK_CS_DESTROY_ON_CLOSE )
, mpDlgObject( pDlgObject )
, mpParent( pParent )
, mbTrackingMouse( false )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
{
	m_bAutoDelete = FALSE;
}


CAcadDockBarHost::~CAcadDockBarHost()
{
	if( mbTrackingMouse )
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, NULL, 0 };
		_TrackMouseEvent( &tm );
	}
}

void CAcadDockBarHost::GetClientArea( CRect& rect )
{
	GetUsedRect( rect );
}

bool CAcadDockBarHost::Create( LPCTSTR lpszTitle, CRect rect, UINT nID ) 
{
	CString strWndClass = AfxRegisterWndClass( CS_DBLCLKS, LoadCursor( NULL, IDC_ARROW ) );	
	bool bDisabled = (mpParent && !mpParent->IsWindowEnabled());
	if( !CAdUiDockControlBar::Create( strWndClass, lpszTitle,
																		WS_VISIBLE | WS_CHILD | (bDisabled? WS_DISABLED : 0) /*| WS_CLIPCHILDREN*/,
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
	ON_REGISTERED_MESSAGE(refWM_MOUSEENTER(),OnMouseEnter)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAcadDockBarHost message handlers


void CAcadDockBarHost::SizeChanged( CRect *lpRect, BOOL bFloating, int flags ) 
{
	__super::SizeChanged( lpRect, bFloating, flags );
}

bool CAcadDockBarHost::CanFrameworkTakeFocus ()
{
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

bool CAcadDockBarHost::OnClosing()
{
	mpDlgObject->OnClosing();
	return __super::OnClosing();
}

//*****************************************************************************
// 
// Method: CAcadDockBarHost::AddCustomMenuItems()
// 
// Purpose: [Override for removing right click pop-up menu]
// 
// Parameters:  
//		[hMenu]:  [pointer to the pop-up menu]
// 
// Returns:	BOOL
// 
//*****************************************************************************
BOOL CAcadDockBarHost::AddCustomMenuItems(LPARAM hMenu)
{
	if (IsWindowVisible())
	{
		// Get the menu object.
		CMenu menu;
		menu.Attach((HMENU) hMenu);

		// remove the menu its
		// I am doing this so the user can right click on the ISO view picture and have
		// is scroll thru the different ISO Plane Options
		menu.RemoveMenu(ID_ADUI_ALLOWDOCK, MF_BYCOMMAND);
		menu.RemoveMenu(ID_ADUI_HIDEBAR, MF_BYCOMMAND);

		// Release the menu object.
		menu.Detach();
	}
	
	return TRUE;
}

//*****************************************************************************
// 
// Method: CAcadDockBarHost::OnUserSizing()
// 
// Purpose: [Overrides the OnSizing event and allows us to force the Recomended width 
//			while the user is resizing]
// 
// Parameters:  
//		[nSide]:  [which side is being stretched]
//		[pRect]:  [a pointer to the CRect of the Floating form]
// 
// Returns:	nothing
// 
//*****************************************************************************
void CAcadDockBarHost::OnUserSizing(UINT nSide, LPRECT pRect)
{	
	if( !mpDlgObject->IsResizable() )
	{
		GetParent()->GetParent()->SendMessage( WM_CANCELMODE, 0, 0 );
		return;
	}
	__super::OnUserSizing( nSide, pRect );
}


//*****************************************************************************
// 
// Method: CAcadDockBarHost::CalcDynamicLayout()
// 
// Purpose: [Call what floating to query what the form size should be]
// 
// Returns:	CSize
// 
//*****************************************************************************
CSize CAcadDockBarHost::CalcDynamicLayout( int nLength, DWORD dwMode )
{
	return __super::CalcDynamicLayout( nLength, dwMode );
}

//*****************************************************************************
// 
// Method: CAcadDockBarHost::CalcFixedLayout()
// 
// Purpose: [Called when docked to query what the docked form size should be]
// 
// 
// Returns:	CSize
// 
//*****************************************************************************

CSize CAcadDockBarHost::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	mpDlgObject->OnFrameChanged();
	if( !mpDlgObject->IsResizable() )
		return CSize( mpDlgObject->GetTemplate()->GetLongProperty( Prop::Width ),
									mpDlgObject->GetTemplate()->GetLongProperty( Prop::Height ) );
	return __super::CalcFixedLayout( bStretch, bHorz );
}

//*****************************************************************************
// 
// Method: CAcadDockBarHost::GetFloatingMinSize()
// 
// Purpose: [This method is pressent to allow a smaller than normal minum width and heigth of
//			 the floating form.  Removing this method would make the min width to big]
// 
// Parameters:  
//		[pnMinWidth]:  [Mininum width]
//		[pnMinHeight]:  [Mininum height]
// 
// Returns:	nothing
// 
//*****************************************************************************
void CAcadDockBarHost::GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight)
{
	// by not adjusting the pnMinWidth or pnMinHeight, the min pnMinWidth will become small enough
	// that the OnResizeEvent will take care of the width size itself.
}

void CAcadDockBarHost::OnSize(UINT nType, int cx, int cy)
{
	CAdUiDockControlBar::OnSize(nType, cx, cy);
	CRect rcClient;
	mpDlgObject->GetClientArea( rcClient );
	mpDlgObject->SetWindowPos( NULL, rcClient.left, rcClient.top,
														 rcClient.Width(), rcClient.Height(),
														 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
}

BOOL CAcadDockBarHost::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL))
		{
			HWND hItem = ::GetDlgItem(m_hWnd, IDCANCEL);
			if (hItem == NULL || ::IsWindowEnabled(hItem))
			{
				SendMessage(WM_COMMAND, IDCANCEL, 0);
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
			return CWnd::PreTranslateMessage(pMsg); //if it's for an ActiveX control, bypass the immediate base class
	}
	if( GetCapture() == this )
	{
		if( mhwndKeyboardFocus && 
				pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST &&
				pMsg->message != WM_MOUSEMOVE && pMsg->message != WM_NCMOUSEMOVE )
		{
			CWnd* pTarget = WindowFromPoint( pMsg->pt );
			if( pTarget )
			{
				if( IsDescendant( this, pTarget ) )
				{
					mhwndKeyboardFocus = pTarget->m_hWnd;
					pMsg->hwnd = mhwndKeyboardFocus;
					SendMessage( refWM_MOUSEENTER(), 0, 0 );
					if( !pTarget->PreTranslateMessage( pMsg ) )
					{
						CPoint ptMouse( pMsg->pt );
						pTarget->ScreenToClient( &ptMouse );
						LPARAM lParam = MAKELPARAM(ptMouse.x, ptMouse.y);
						pTarget->SendMessage( pMsg->message, pMsg->wParam, lParam );
					}
					return TRUE;
				}
				::SetCapture( NULL );
				pTarget->SetFocus();
			}
			return TRUE;
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
		SetTimer( WM_MOUSELEAVE, 200, NULL );
		mbTrackingMouse = true;
	}
	if( mhwndKeyboardFocus )
	{
		::SetFocus( mhwndKeyboardFocus );
		ReleaseCapture();
		mhwndKeyboardFocus = NULL;
	}
	return 0;
}

LRESULT CAcadDockBarHost::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	mbTrackingMouse = false;
	if( !mbInMenuLoop && !::GetCapture() )
	{
		CWnd* pFocusWnd = GetFocus();
		if( !pFocusWnd || ((pFocusWnd->SendMessage( WM_GETDLGCODE, 0, 0 ) & (DLGC_WANTCHARS | DLGC_WANTARROWS)) == 0) )
		{
			CWnd* pCmdLine = acedGetAcadDockCmdLine();
			if( pCmdLine && pCmdLine->IsWindowEnabled() && pCmdLine->IsWindowVisible() )
				pCmdLine->SetFocus();
		}
		else if( IsDescendant( this, pFocusWnd ) )
		{
			mhwndKeyboardFocus = pFocusWnd->m_hWnd;
			SetCapture();
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		}
	}
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
		CPoint ptCursor;
		if( GetCursorPos( &ptCursor ) )
		{
			CWnd* pTarget = WindowFromPoint( ptCursor );
			if( !IsDescendant( this, pTarget ) )
			{
				PostMessage( WM_MOUSELEAVE, 0, 0 );
				KillTimer( WM_MOUSELEAVE );
			}
		}
	}
	__super::OnTimer(nIDEvent);
}

__UINT_LRESULT CAcadDockBarHost::OnNcHitTest(CPoint point)
{
	SendMessage( refWM_MOUSEENTER(), 0, 0 );
	return __super::OnNcHitTest(point);
}

BOOL CAcadDockBarHost::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint ptCursor;
	if( ::GetCursorPos( &ptCursor ) )
	{
		CWnd* pTarget = WindowFromPoint( ptCursor );
		if( IsDescendant( this, pTarget ) )
			SendMessage( refWM_MOUSEENTER(), 0, 0 );
	}
	return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CAcadDockBarHost::PostNcDestroy() 
{
	__super::PostNcDestroy();
	//delete this;
}
