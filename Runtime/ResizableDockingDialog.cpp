// DockingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ResizableDockingDialog.h"
#include "InvokeMethod.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"


const int nDeflateRect = 3;
const int nBottomAdjustment = 5;
const int nHeightOffset = 2;


// CResizableDockingDialogX interface implementation
CResizableDockingDialogX::CResizableDockingDialogX( CResizableDockingDialog& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CResizableDockingDialogX::~CResizableDockingDialogX()
	{
}

DclFormType CResizableDockingDialogX::GetType() const
{
	return VdclDockable;
}

HWND CResizableDockingDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

bool CResizableDockingDialogX::IsFloating() const
{
	return (mpOwner->IsFloating() != FALSE);
}

bool CResizableDockingDialogX::CreateModeless( UINT nID ) const
{
	CDclControlObject* pProps = mpSourceForm->GetControlProperties();
	int nWindowHeight = pProps->GetLongProperty(nHeight);
	DWORD dwDockableSides = 0;
	DWORD dwDefaultDockableSide = 0;

	switch (pProps->GetLongProperty(nDockableSides))
	{
	case 1:
		// set the form to only dock on the top side
		dwDockableSides = CBRS_ALIGN_TOP;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		if( !mpSourceForm->UsesClientRect() )
			nWindowHeight += 8;
		break;
	case 2:
		// set the form to only dock on the bottom side
		dwDockableSides = CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_BOTTOM;
		if( !mpSourceForm->UsesClientRect() )
			nWindowHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 3:
		// set the form to only dock on the top or bottom sides
		dwDockableSides = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;				
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		if( !mpSourceForm->UsesClientRect() )
			nWindowHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 4:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP;				
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		if( !mpSourceForm->UsesClientRect() )
			nWindowHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 5:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		if( !mpSourceForm->UsesClientRect() )
			nWindowHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	default:
		// set the form to only dock on the left or right sides
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		if( !mpSourceForm->UsesClientRect() )
			nWindowHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	}

	CRect rect = CFrameWnd::rectDefault;
	if (rect.left < 0)
		rect.left = 0;
	if (rect.top < 0)
		rect.top = 0;				
	rect.bottom = rect.top + nWindowHeight;
	rect.right = rect.left + pProps->GetLongProperty(nWidth);

	if( !mpOwner->Create( mpSourceForm->GetKeyPath(), rect, nID ) )
		return false;
	if (mpSourceForm->GetUUIDAsString().IsEmpty())
	{
		UUID uuid;
		UuidCreate(&uuid);
		mpOwner->SetToolID (&uuid);
	}
	else
	{
    UUID uuid = mpSourceForm->GetUUID();
		mpOwner->SetToolID (&uuid);
	}			
	mpOwner->EnableDocking(dwDockableSides);
	mpOwner->RestoreControlBar(dwDefaultDockableSide); // loads the dockable form but does not display it
	return true;
}

void CResizableDockingDialogX::CloseDialog(int nStatus) const
{
	CWnd* pTopLevel = IsFloating()? (mpOwner->m_hWnd? mpOwner->GetParent()->GetParent() : NULL) : mpOwner;
	HWND hwndTopLevel = pTopLevel? pTopLevel->m_hWnd : NULL;
	HWND hwndOwner = mpOwner->m_hWnd;
	mpOwner->EndModalLoop(nStatus); //set the status
	if( hwndTopLevel && ::IsWindow(hwndTopLevel) )
		::SendMessage(hwndTopLevel, WM_CLOSE, 0, 0);
	if( hwndOwner && ::IsWindow(hwndOwner) )
		::DestroyWindow(hwndOwner);
}

bool CResizableDockingDialogX::GetWindowRect( CRect& rcDlg ) const
{
	if (mpOwner->IsFloating())
		mpOwner->GetFloatingRect(&rcDlg);
	else
		mpOwner->GetClientArea(rcDlg);
	return true;
}

bool CResizableDockingDialogX::GetClientRect( CRect& rcDlg ) const
{
	mpOwner->GetClientArea(rcDlg);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog dialog

CResizableDockingDialog::CResizableDockingDialog( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAdUiDockControlBar( ADUI_DOCK_CS_STDMOUSECLICKS | ADUI_DOCK_CS_DESTROY_ON_CLOSE )
, mpParent( pParent )
, mDialogX( *this, pSourceForm )
, mbClosing( false )
, mbHiding( false )
, mbTrackingMouse( false )
, mbInMenuLoop( false )
, mhwndKeyboardFocus( NULL )
{
}

CResizableDockingDialog::~CResizableDockingDialog()
{
	if( mbTrackingMouse )
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, NULL, 0 };
		_TrackMouseEvent( &tm );
	}
}


BEGIN_MESSAGE_MAP(CResizableDockingDialog, CAdUiDockControlBar)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_NCMOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog message handlers

bool CResizableDockingDialog::Create( LPCTSTR lpszTitle, CRect rect, UINT nID ) 
{
	CString strWndClass = AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW));	
  if (!CAdUiDockControlBar::Create( strWndClass, lpszTitle,
																		WS_VISIBLE | WS_CHILD /*| WS_CLIPCHILDREN*/,
																		rect, mpParent, nID))
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

int CResizableDockingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAdUiDockControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//  setup for assigning the form it's properties
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();

	// set the window text
	SetWindowText(pProps->GetStrProperty(nTitleBarText));
	
	CRect rectPane;
	if (mDialogX.GetSourceForm()->UsesClientRect() && IsFloating())
		GetClientArea( rectPane );
	else
	{
		GetWindowRect( &rectPane );
		ScreenToClient( &rectPane );
	}

	rectPane.top += (nDeflateRect + 20);
	rectPane.left += nDeflateRect;
	rectPane.bottom += (nBottomAdjustment - nHeightOffset);

	mDialogX.GetControlPane().SetPanePos( rectPane );

	// call method to create the controls
	UINT nID = 1000;
	mDialogX.GetControlPane().CreateControls(nID);
	mDialogX.GetControlPane().RecalcLayout();

	//if (pProps->GetLongProperty(nMaxDialogWidth) > -1)
	//{
	//	long lMinHeight;
	//	long lMinWidth;
	//	GetFloatingMinSize(&lMinHeight, &lMinWidth);
	//	lMinWidth = pProps->GetLongProperty(nMinDialogWidth);
	//}

	// call methods to invoke the event
	InvokeMethod(pProps->GetStrProperty(nFormEventInitialize), true);	
	
	CRect rcThis;
	if( mDialogX.GetSourceForm()->UsesClientRect() )
		GetClientRect( &rcThis );
	else
		GetWindowRect( &rcThis );
	// call methods to invoke the event
	InvokeMethodIntInt(
		pProps->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	

	return 1;
}

void CResizableDockingDialog::GetClientArea(CRect &rect)
{
	GetUsedRect(rect);
}


void CResizableDockingDialog::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	CAdUiDockControlBar::SizeChanged(lpRect, bFloating, flags);
	if (!mbClosing)
	{
		lpRect->top += (nDeflateRect + 2);
		lpRect->left += nDeflateRect;
		lpRect->bottom += (nBottomAdjustment - nHeightOffset);

		// resize the control pane so all offsets are set correctly
		mDialogX.GetControlPane().SetPanePos( *lpRect, true );
		
		// call methods to invoke the event
		CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		InvokeMethodIntInt(
			pProps->GetStrProperty(nFormEventSize), 
			rcThis.Width(), 
			rcThis.Height(),
			true);	
	}
}

BOOL CResizableDockingDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nEventOnHelp), true);
	return TRUE;
}

void CResizableDockingDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nFormEventShow), true);	

	mbHiding = !mbClosing && !bShow;
	CAdUiDockControlBar::OnShowWindow(bShow, nStatus);
}

bool CResizableDockingDialog::CanFrameworkTakeFocus ()
{
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

void CResizableDockingDialog::PostNcDestroy() 
{
	CAdUiDockControlBar::PostNcDestroy();
	//the floating docbar container will 'delete this' when floating because 
	//the ADUI_DOCK_CS_DESTROY_ON_CLOSE style is set in the constructor
	if( !IsFloating() )
		delete this;
}

bool CResizableDockingDialog::OnClosing()
{
	mbClosing = true;
	CAdUiDockControlBar::OnClosing();
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nFormEventClose), true);
	if( !mbHiding && !IsFloating() )
		PostMessage(WM_CLOSE); //to make sure the window gets destroyed no matter how we got here
	return true;
}

//*****************************************************************************
// 
// Method: CResizableDockingDialog::AddCustomMenuItems()
// 
// Purpose: [Override for removing right click pop-up menu]
// 
// Parameters:  
//		[hMenu]:  [pointer to the pop-up menu]
// 
// Returns:	BOOL
// 
//*****************************************************************************
BOOL CResizableDockingDialog::AddCustomMenuItems(LPARAM hMenu)
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
// Method: CResizableDockingDialog::OnUserSizing()
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
void CResizableDockingDialog::OnUserSizing(UINT nSide, LPRECT pRect)
{	
	// pass on OnUserSizing method
	CAdUiDockControlBar::OnUserSizing(nSide, pRect);
		
	CRect FloatingRect;
	CAdUiDockControlBar::GetFloatingRect(&FloatingRect);

	int nThisWidth = pRect->right - pRect->left;
}


//*****************************************************************************
// 
// Method: CResizableDockingDialog::GetFloatingMinSize()
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
void CResizableDockingDialog::GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight)
{
	// by not adjusting the pnMinWidth or pnMinHeight, the min pnMinWidth will become small enough
	// that the OnResizeEvent will take care of the width size itself.
}


BOOL CResizableDockingDialog::PreTranslateMessage(MSG* pMsg) 
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

	return CAdUiDockControlBar::PreTranslateMessage(pMsg);
}

void CResizableDockingDialog::OnDestroy() 
{
	if( !mbClosing )
		OnClosing();
	mDialogX.GetControlPane().CleanUpControls();
	CAdUiDockControlBar::OnDestroy();
}

void CResizableDockingDialog::OnCaptureChanged(CWnd* pWnd)
{
	mhwndKeyboardFocus = NULL;
	__super::OnCaptureChanged(pWnd);
}

void CResizableDockingDialog::OnMouseMove(UINT nFlags, CPoint point)
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

void CResizableDockingDialog::OnNcMouseMove(UINT nHitTest, CPoint point)
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

LRESULT CResizableDockingDialog::OnMouseLeave(WPARAM wParam, LPARAM lParam)
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

void CResizableDockingDialog::OnEnterMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = true;
	__super::OnEnterMenuLoop(bPopupMenu);
}

void CResizableDockingDialog::OnExitMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = false;
	__super::OnExitMenuLoop(bPopupMenu);
}

void CResizableDockingDialog::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == WM_MOUSELEAVE )
	{
		CPoint ptCursor;
		if( GetCursorPos( &ptCursor ) )
		{
			CWnd* pTarget = WindowFromPoint( ptCursor );
			if( !pTarget || (pTarget != this && !IsChild( pTarget )) )
			{
				PostMessage( WM_MOUSELEAVE, 0, 0 );
				KillTimer( WM_MOUSELEAVE );
			}
		}
	}
	__super::OnTimer(nIDEvent);
}
