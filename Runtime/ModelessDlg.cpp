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


// CModelessDialogX interface implementation
CModelessDialogX::CModelessDialogX( CModelessDlg& Owner, CDclFormObject* pDclForm )
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

void CModelessDialogX::CloseDialog(int nStatus) const
{
	if( mpOwner->IsClosing() )
		return; //already in the process of closing
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

CModelessDlg::CModelessDlg( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CBaseDlg( pSourceForm, CModelessDlg::IDD, pParent, pParams )
, mpParent( pParent )
, mDialogX( *this, pSourceForm )
, mbResizable( pSourceForm->GetControlProperties()->GetBoolProperty( nResizable ) )
, mbTrackingMouse( false )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
{
	m_bAsModal = false;
	m_bClosing = false;
	m_bAboutToClose = false;
}

CModelessDlg::~CModelessDlg()
{
	if( mbTrackingMouse )
		KillTimer( WM_MOUSELEAVE );
}

bool CModelessDlg::Create( UINT nTemplateID )
{
	return (__super::Create( nTemplateID, mpParent ) != FALSE);
}

BEGIN_MESSAGE_MAP(CModelessDlg, CBaseDlg)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus)	
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCMOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

BOOL CModelessDlg::OnInitDialog() 
{
	ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
	CBaseDlg::OnInitDialog();

	if( m_bAsModal )
	{
		GetParent()->EnableWindow( FALSE );
		EnableWindow( TRUE );
	}

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
	if (IsWindowVisible() && m_bClosing == false  && m_bAboutToClose == false)
	{
		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		// call methods to invoke the event
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
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
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventShow), false);	
}

void CModelessDlg::OnDestroy() 
{
	m_bClosing = true;
	__super::OnDestroy();
}

void CModelessDlg::OnOK()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventOnOk), true);
		__super::OnOK();
		mDialogX.CloseDialog(IDOK);
	}
}

void CModelessDlg::OnCancel()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), true))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventOnCancel), true);
		__super::OnCancel();
		mDialogX.CloseDialog(IDCANCEL);
	}
}

void CModelessDlg::OnMove(int x, int y) 
{
	CBaseDlg::OnMove(x, y);

  //Removed 10-19-06	
  /*BOOL bVisible = CDialog::IsWindowVisible();
  if (!m_bAboutToClose && bVisible)
  {
    CRect rc;
    CWnd::GetWindowRect(&rc);
    m_nX = rc.left;
    m_nY = rc.top;
  }*/
}


void CModelessDlg::SizeDialog ()
{
	if (CWnd::IsWindowVisible() && m_bClosing == false)
	{
		mDialogX.GetControlPane().RecalcLayout();
		
		if (!m_bAboutToClose)
		{
			CRect rcThis;
			if( mDialogX.GetSourceForm()->UsesClientRect() )
				GetClientRect( &rcThis );
			else
				GetWindowRect( &rcThis );
			// call methods to invoke the event
			InvokeMethodIntInt(
				mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
				rcThis.Width(),
				rcThis.Height(),
				true );
		}

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
		CWnd* pTarget = WindowFromPoint( pMsg->pt );
		if( pTarget )
		{
			if( pTarget == this || IsChild( pTarget ) )
			{
				if( !mbTrackingMouse )
				{
					SetTimer( WM_MOUSELEAVE, 200, NULL );
					mbTrackingMouse = true;
				}
				if( mhwndKeyboardFocus )
				{
					::SetFocus( mhwndKeyboardFocus );
					::SetCapture( NULL );
				}
			}
			if( pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST )
			{
				CPoint ptMouse( pMsg->pt );
				pTarget->ScreenToClient( &ptMouse );
				pMsg->lParam = MAKELPARAM(ptMouse.x, ptMouse.y);
				if( mhwndKeyboardFocus && pMsg->message != WM_MOUSEMOVE && pMsg->message != WM_NCMOUSEMOVE )
				{
					::SetCapture( NULL );
					pTarget->SetFocus();
				}
			}
			//pTarget->SendMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
			return TRUE;
		}
	}

	return CBaseDlg::PreTranslateMessage(pMsg);
}

void CModelessDlg::OnCaptureChanged(CWnd* pWnd)
{
	mhwndKeyboardFocus = NULL;
	__super::OnCaptureChanged(pWnd);
}

void CModelessDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint ptMsg( point );
	ClientToScreen( &ptMsg );
	CWnd* pTarget = WindowFromPoint( ptMsg );
	if( pTarget && (pTarget == this || IsChild( pTarget )) )
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
		}
	}
	__super::OnMouseMove(nFlags, point);
}

void CModelessDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	CWnd* pTarget = WindowFromPoint( point );
	if( pTarget && (pTarget == this || IsChild( pTarget )) )
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
		}
	}
	__super::OnNcMouseMove(nHitTest, point);
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
