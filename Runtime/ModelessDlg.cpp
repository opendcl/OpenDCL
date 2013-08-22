// ModelessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelessDlg.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "ArxProject.h"
#include "PictureObject.h"
#include "DialogControl.h"
#include "ControlTypes.h"


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
// CModelessDlg dialog

CModelessDlg::CModelessDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CBaseDlg( pSourceForm, IDD_BASERESIZABLEDLG, pParent, pParams )
, mpParent( pParent )
, mbKeepFocus( true )
, mbTrackingMouse( false )
, mbMouseLeft( true )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
{
}

CModelessDlg::~CModelessDlg()
{
}

bool CModelessDlg::CreateModeless( UINT nID )
{
	if( CDialog::Create( (IsResizable()? IDD_BASERESIZABLEDLG : IDD_BASEDLG), mpParent ) == FALSE )
		return false;
	if( mpParent && !mpParent->IsWindowEnabled() )
		EnableWindow( FALSE );
	return true;
}

void CModelessDlg::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return; //already in the process of closing
	SetClosing();
	EndDialog(nStatus);
	DestroyWindow();
}

bool CModelessDlg::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::KeepFocus:
		mbKeepFocus = pProp->GetBooleanValue();
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CModelessDlg, CBaseDlg)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus)	
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(refWM_MOUSEENTER(),OnMouseEnter)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

BOOL CModelessDlg::OnInitDialog() 
{
	ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
	__super::OnInitDialog();

	// Modify the style
	//GetLayeredDialog()->AddLayeredStyle(m_hWnd);

	// Set the trasparency effect to 70%
	//GetLayeredDialog()->SetTransparentPercentage(m_hWnd, 80);

	return TRUE; //return TRUE so the dialog manager does its own control focus work
}

LRESULT CModelessDlg::onAcadKeepFocus(WPARAM, LPARAM)
{
	return LRESULT(mbKeepFocus || !mbMouseLeft || GetCapture());
}

void CModelessDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if( IsIgnoreSizing() || !IsWindowVisible() )
		return;
	GetArxServices()->HandleEvent( Prop::FormEventMove, args_NN( x, y ) );
}

void CModelessDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if (IsWindowVisible())
	{	
		GetArxServices()->HandleEvent( Prop::FormEventSize,
																	 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																						mpTemplate->GetLongProperty( Prop::Height ) ) );
	}	
}

void CModelessDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	__super::OnShowWindow(bShow, nStatus);
	GetArxServices()->HandleEvent( Prop::FormEventShow );	
}

void CModelessDlg::OnDestroy() 
{
	if( mbTrackingMouse )
	{
		KillTimer( WM_MOUSELEAVE );
		mbTrackingMouse = false;
	}
	SetClosing();
	__super::OnDestroy();
}

void CModelessDlg::OnOK()
{
	if( IsClosing() || IsCloseAllowed( false ) )
	{
		if( GetArxServices()->HandleEvent( Prop::FormEventOnOk ) )
			return;
		__super::OnOK();
		CloseDialog(IDOK);
	}
}

void CModelessDlg::OnCancel()
{
	if( IsClosing() || IsCloseAllowed( true ) )
	{
		if( GetArxServices()->HandleEvent( Prop::FormEventOnCancel ) )
			return;
		__super::OnCancel();
		CloseDialog(IDCANCEL);
	}
}

BOOL CModelessDlg::PreTranslateMessage(MSG* pMsg) 
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
		TDialogControlPtr pControl = GetControlPane()->FindControl( pMsg->hwnd );
		if( pControl && pControl->GetControlType() == CtlActiveX )
			return CWnd::PreTranslateMessage(pMsg); //if it's for an ActiveX control, bypass the immediate base class
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
				if( IsDescendant( this, pTarget ) )
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
				if( IsDescendant( this, pTarget ) )
					SendMessage( refWM_MOUSEENTER(), 0, 0 );
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

LRESULT CModelessDlg::OnMouseEnter(WPARAM wParam, LPARAM lParam)
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
		GetArxServices()->HandleEvent( Prop::EventMouseEntered );
	}
	return 0;
}

LRESULT CModelessDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	mbTrackingMouse = false;
	if( !mbInMenuLoop && mbKeepFocus && !::GetCapture() )
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
		GetArxServices()->HandleEvent( Prop::EventMouseMovedOff );
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

void CModelessDlg::OnEnterMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = true;
	__super::OnEnterMenuLoop(bPopupMenu);
}

void CModelessDlg::OnExitMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = false;
	__super::OnExitMenuLoop(bPopupMenu);
}

void CModelessDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == WM_MOUSELEAVE )
	{
		if( GetCapture() )
			return;
		CPoint ptCursor;
		if( GetCursorPos( &ptCursor ) )
		{
			CRect rcDlg;
			GetWindowRect( &rcDlg );
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

void CModelessDlg::OnCaptureChanged(CWnd *pWnd)
{
	OnTimer( WM_MOUSELEAVE );
	__super::OnCaptureChanged(pWnd);
}
