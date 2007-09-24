// ModelessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelessDlg.h"
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


// CModelessDialogX interface implementation
CModelessDialogX::CModelessDialogX( CModelessDlg& Owner, TDclFormPtr pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CModelessDialogX::~CModelessDialogX()
{
}

DclFormType CModelessDialogX::GetType() const
{
	return VdclModeless;
}

bool CModelessDialogX::IsResizable() const
{
	return mpOwner->IsResizable();
}

HWND CModelessDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

bool CModelessDialogX::CreateModeless( UINT nID ) const
{
	return mpOwner->Create( IsResizable()? IDD_RESIZEABLE : IDD_MODALDIALOG );
}

void CModelessDialogX::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return; //already in the process of closing
	SetClosing();
	mpOwner->EndDialog(nStatus);
	mpOwner->DestroyWindow();
}

bool CModelessDialogX::SetMinMaxSize( const CSize& min, const CSize& max )
{
	CDialogObject::SetMinMaxSize( min, max );
	mpOwner->SetMinMaxSize (min, max);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

CModelessDlg::CModelessDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CBaseDlg( pSourceForm, CModelessDlg::IDD, pParent, pParams )
, mpParent( pParent )
, mDialogX( *this, pSourceForm )
, mbResizable( pSourceForm->GetControlProperties()->GetBooleanProperty( Prop::Resizable ) )
, mbTrackingMouse( false )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
{
}

CModelessDlg::~CModelessDlg()
{
	if( mbTrackingMouse )
		KillTimer( WM_MOUSELEAVE );
}

bool CModelessDlg::Create( UINT nTemplateID )
{
	if( __super::Create( nTemplateID, mpParent ) == FALSE )
		return false;
	if( mpParent && !mpParent->IsWindowEnabled() )
		EnableWindow( FALSE );
	return true;
}

BEGIN_MESSAGE_MAP(CModelessDlg, CBaseDlg)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus)	
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_REGISTERED_MESSAGE(refWM_MOUSEENTER(),OnMouseEnter)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

BOOL CModelessDlg::OnInitDialog() 
{
	ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
	CBaseDlg::OnInitDialog();

	// Modify the style
	//GetLayeredDialog()->AddLayeredStyle(m_hWnd);

	// Set the trasparency effect to 70%
	//GetLayeredDialog()->SetTransparentPercentage(m_hWnd, 80);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

LRESULT CModelessDlg::onAcadKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

void CModelessDlg::OnSize(UINT nType, int cx, int cy) 
{
	CBaseDlg::OnSize(nType, cx, cy);
	mDialogX.GetControlPane().RecalcLayout();
	if( IsWindowVisible() && !mDialogX.IsClosing() )
	{
		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void CModelessDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBaseDlg::OnShowWindow(bShow, nStatus);
	
	// call methods to invoke the event
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventShow), false);	
}

void CModelessDlg::OnDestroy() 
{
	mDialogX.SetClosing();
	__super::OnDestroy();
}

void CModelessDlg::OnOK()
{
	if (mDialogX.IsClosing() ||
			!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventCancelClose), false))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventOnOk), true);
		__super::OnOK();
		mDialogX.CloseDialog(IDOK);
	}
}

void CModelessDlg::OnCancel()
{
	if (mDialogX.IsClosing() ||
			!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventCancelClose), true))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventOnCancel), true);
		__super::OnCancel();
		mDialogX.CloseDialog(IDCANCEL);
	}
}

void CModelessDlg::OnMove(int x, int y) 
{
	CBaseDlg::OnMove(x, y);
}


void CModelessDlg::SizeDialog ()
{
	if (CWnd::IsWindowVisible() && !mDialogX.IsClosing())
	{
		mDialogX.GetControlPane().RecalcLayout();
		
		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStringProperty(Prop::FormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			true );
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
		TDialogControlPtr pControl = GetDialogObject().GetControlPane().FindControl( pMsg->hwnd );
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
				if( pTarget == this || IsChild( pTarget ) )
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

	return CBaseDlg::PreTranslateMessage(pMsg);
}

LRESULT CModelessDlg::OnMouseEnter(WPARAM wParam, LPARAM lParam)
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

LRESULT CModelessDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
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
		else if( pFocusWnd && IsChild( pFocusWnd ) )
		{
			mhwndKeyboardFocus = pFocusWnd->m_hWnd;
			SetCapture();
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		}
	}
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
		CPoint ptCursor;
		if( GetCursorPos( &ptCursor ) )
		{
			CWnd* pTarget = WindowFromPoint( ptCursor );
			if( pTarget != this && !IsChild( pTarget ) )
			{
				PostMessage( WM_MOUSELEAVE, 0, 0 );
				KillTimer( WM_MOUSELEAVE );
			}
		}
	}
	__super::OnTimer(nIDEvent);
}

__LRESULT CModelessDlg::OnNcHitTest(CPoint point)
{
	SendMessage( refWM_MOUSEENTER(), 0, 0 );
	return __super::OnNcHitTest(point);
}

BOOL CModelessDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint ptCursor;
	if( ::GetCursorPos( &ptCursor ) )
	{
		CWnd* pTarget = WindowFromPoint( ptCursor );
		if( pTarget && (pTarget == this || IsChild( pTarget )) )
			SendMessage( refWM_MOUSEENTER(), 0, 0 );
	}
	return __super::OnSetCursor(pWnd, nHitTest, message);
}
