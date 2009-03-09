// ControlManager.cpp : implementation file
//

#include "stdafx.h"
#include "ControlManager.h"
#include "StudioDialogControl.h"
#include "AxContainerCtrl.h"
#include "PictureBoxCtrl.h"
#include "StudioWorkspace.h"
#include "StudioDialogObject.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CControlManager

BEGIN_MESSAGE_MAP(CControlManager, CStatic)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_ACTIVATEAPP()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_NCLBUTTONUP()
END_MESSAGE_MAP()


CControlManager::CControlManager( CDialogControl* pDlgControl, bool bCreate /*= true*/ )
: CStatic()
, mpDlgControl( pDlgControl )
, mpTemplate( pDlgControl->GetTemplate() )
, mpControlPane( pDlgControl->GetControlPane() )
, mpControlWnd( pDlgControl->GetControlWnd() )
, mGripper( this )
, mbSelected( false )
, mbIgnoreSizing( false )
, mptDragInitialPos( -1, -1 )
, mrcDragInitialPos( 0, 0, 0, 0 )
, mnSizingType( 0 )
, mbUndoGroupActive( false )
{
	CWnd* pTopLevelWnd = pDlgControl->GetTopLevelWnd();
	TraceFmt( _T("> CControlManager::CControlManager(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)mpTemplate->GetKeyPath(), (const CDclControlObject*)mpTemplate, pDlgControl->GetControlPane(),
						(LPCTSTR)CString(mpControlWnd->GetRuntimeClass()->m_lpszClassName),
						pTopLevelWnd->m_hWnd, this );
	pDlgControl->SetControlManager( this );
	if( bCreate )
	{
		if( pTopLevelWnd->m_hWnd )
			pTopLevelWnd->SetRedraw( FALSE );
		CRect rcControl = pDlgControl->GetEffectiveWindowRect();
		Create( _T(""), WS_CHILD | WS_DISABLED, rcControl, pTopLevelWnd->GetParent() );
		ModifyStyle( WS_CLIPCHILDREN, WS_CLIPSIBLINGS, 0 );
		ModifyStyleEx( 0, WS_EX_TRANSPARENT, 0 );
		rcControl.MoveToXY( 0, 0 );
		mGripper.Create( _T(""), WS_CHILD, rcControl, this );
		mGripper.ModifyStyleEx( 0, WS_EX_TRANSPARENT, 0 );
		//SetWindowPos( mpControlWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING );
		//mpControlWnd->SetWindowPos( this, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING );
		pTopLevelWnd->SetParent( this );
		pTopLevelWnd->SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING );
		if( pTopLevelWnd->m_hWnd )
			pTopLevelWnd->SetRedraw( TRUE );
		ShowWindow( SW_SHOW );
	}
}

CControlManager::~CControlManager()
{
	if( mpDlgControl )
		mpDlgControl->SetControlManager( NULL );
	TraceFmt( _T("< CControlManager::~CControlManager() [this: %p]\r\n"), this );
}

CStudioDialogObject* CControlManager::GetDialogObject() const
{
	TDclFormPtr pOwnerForm = mpTemplate->GetOwnerForm();
	if( !pOwnerForm )
		return NULL;
	return (CStudioDialogObject*)pOwnerForm->GetFormInstance();
}

void CControlManager::SetSelected( bool bSelected /*= true*/ )
{
	OnSelected( bSelected );
	mbSelected = bSelected;
}

void CControlManager::OnSelected( bool bSelected )
{
	mGripper.SetThickFrame( false );
	if( mGripper.m_hWnd )
	{
		mGripper.ShowWindow( bSelected? SW_SHOW : SW_HIDE );
		if( !bSelected && mbSelected )
		{
			ShowWindow( SW_HIDE );
			ShowWindow( SW_SHOW );
		}
	}
}

void CControlManager::StartSizing( UINT nHitTest, const CPoint& ptStart )
{
	mptDragInitialPos = ptStart;
	mnSizingType = nHitTest;
	GetWindowRect( &mrcDragInitialPos );
	GetParent()->ScreenToClient( &mrcDragInitialPos );
	CUndoManager* pUndoManager = mpTemplate->GetUndoManager();
	if( pUndoManager && !mbUndoGroupActive )
	{
		mbUndoGroupActive = true;
		pUndoManager->BeginGroup( theWorkspace.LoadResourceString( IDS_UNDO_SIZECONTROL ) );
	}
	SetCapture();
}

void CControlManager::CheckNewWindowPosition( CRect& rcManager ) const
{
	CStudioDialogObject* pDlgObject = GetDialogObject();
	if( !pDlgObject || mpDlgControl == pDlgObject )
		return;

	//pRect is the window rect in host client coordinates
	CRect rcClient( rcManager );

	//check to make sure the control is within the control area
	CRect rcHost = pDlgObject->GetWndRect();
	rcHost.MoveToXY( 0, 0 );
	if( rcClient.right > rcHost.right )
		rcClient.MoveToX( rcHost.right - rcClient.Width() );
	if( rcClient.bottom > rcHost.bottom )
		rcClient.MoveToY( rcHost.bottom - rcClient.Height() );
	if( rcClient.left < 0 )
		rcClient.MoveToX( 0 );
	if( rcClient.top < 0 )
		rcClient.MoveToY( 0 );

	//calculate the snapped point, then convert back to screen coordinates
	CPoint ptUL = rcClient.TopLeft();
	pDlgObject->SnapToGrid( ptUL );
	rcClient.MoveToXY( ptUL );

	//save the results
	rcManager = rcClient;
}

void CControlManager::ClientToControl( CRect& rcTarget ) const
{
	CWnd* pHostDlg = GetDialogObject()->GetControlWnd();
	if( !pHostDlg )
		return;
	ClientToScreen( &rcTarget );
	pHostDlg->ScreenToClient( &rcTarget );
}

void CControlManager::ClientToControl( CPoint& ptTarget ) const
{
	CWnd* pHostDlg = GetDialogObject()->GetControlWnd();
	if( !pHostDlg )
		return;
	ClientToScreen( &ptTarget );
	pHostDlg->ScreenToClient( &ptTarget );
}

void CControlManager::ControlToClient( CRect& rcTarget ) const
{
	CWnd* pHostDlg = GetDialogObject()->GetControlWnd();
	if( !pHostDlg )
		return;
	pHostDlg->ClientToScreen( &rcTarget );
	ScreenToClient( &rcTarget );
}

void CControlManager::ControlToClient( CPoint& ptTarget ) const
{
	CWnd* pHostDlg = GetDialogObject()->GetControlWnd();
	if( !pHostDlg )
		return;
	pHostDlg->ClientToScreen( &ptTarget );
	ScreenToClient( &ptTarget );
}

UINT CControlManager::HitTest( const CPoint& point, bool bNoResize /*= false*/ ) const
{
	CRect rcControl = mpDlgControl->GetEffectiveWindowRect();
	CPoint ptClient = point;
	GetDialogObject()->GetControlWnd()->ScreenToClient( &ptClient );
	rcControl.InflateRect( 3, 3 ); //allow 4 pixels beyond the edge of the control for easier resizing
	if( !rcControl.PtInRect( ptClient ) )
		return HTNOWHERE;
	LRESULT htControl = mpControlWnd->SendMessage( WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y) );
	if( htControl == HTOBJECT )
	{
		rcControl.DeflateRect( 3, 3 );
		if( rcControl.PtInRect( ptClient ) )
			return HTOBJECT; //if the control returns HTOBJECT, it's not moveable or resizable
		return HTNOWHERE;
	}
	rcControl.DeflateRect( 6, 6 );
	if( rcControl.PtInRect( ptClient ) )
	{
		if( htControl == HTCLIENT )
			return HTCLIENT;
		if( htControl == HTTRANSPARENT )
			return HTCLIENT; //allow transparent controls to be selected
		return HTNOWHERE;
	}
	//if( !mbSelected )
	//	return HTNOWHERE;
	if( bNoResize )
		return HTCLIENT;
	bool bLeft = (ptClient.x < rcControl.left);
	bool bTop = (ptClient.y < rcControl.top);
	if( bTop )
	{
		if( bLeft )
			return HTTOPLEFT;
		if( ptClient.x > rcControl.right )
			return HTTOPRIGHT;
		return HTTOP;
	}
	if( ptClient.y > rcControl.bottom )
	{
		if( bLeft )
			return HTBOTTOMLEFT;
		if( ptClient.x > rcControl.right )
			return HTBOTTOMRIGHT;
		return HTBOTTOM;
	}
	if( bLeft )
		return HTLEFT;
	return HTRIGHT;
}

void CControlManager::OnControlPositionChanged()
{
	if( mbIgnoreSizing )
		return;
	mbIgnoreSizing = true;
	CRect rcCtrl = mpTemplate->GetWndRect();
	mpControlWnd->SetWindowPos( NULL, 0, 0, rcCtrl.Width(), rcCtrl.Height(),
															SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER );
	CRect rcCtrlNew;
	mpControlWnd->GetWindowRect( &rcCtrlNew );
	GetDialogObject()->GetControlWnd()->ScreenToClient( &rcCtrlNew );
	//in case it didn't like its new size (i.e. combo boxes)
	if( rcCtrlNew.Width() != mpTemplate->GetLongProperty( Prop::Width ) )
	{
		rcCtrl.right = rcCtrl.left + rcCtrlNew.Width();
		mpTemplate->SetLongProperty( Prop::Width, rcCtrlNew.Width() );
		CStudioDialogControl::UpdateProperty( mpTemplate, Prop::UseRightFromRight );
	}
	if( rcCtrlNew.Height() != mpTemplate->GetLongProperty( Prop::Height ) )
	{
		rcCtrl.bottom = rcCtrl.top + rcCtrlNew.Height();
		mpTemplate->SetLongProperty( Prop::Height, rcCtrlNew.Height() );
		CStudioDialogControl::UpdateProperty( mpTemplate, Prop::UseBottomFromBottom );
	}
	MoveWindow( &rcCtrl, TRUE);
	mbIgnoreSizing = false;
}

void CControlManager::AutoSize()
{
	if( !mpControlWnd )
		return;
	
	switch( mpTemplate->GetType() )
	{
	case CtlPictureBox:
		{
			if( !mpTemplate->GetBooleanProperty( Prop::AutoSize ) )
				break;
			int nOldWidth = mpTemplate->GetLongProperty(Prop::Width);
			int nOldHeight = mpTemplate->GetLongProperty(Prop::Height);
			((CPictureBoxCtrl*)mpControlWnd)->AutoSize();
			CRect rcNew;
			mpControlWnd->GetWindowRect( &rcNew );
			int nWidthDelta = rcNew.Width() - nOldWidth;
			int nHeightDelta = rcNew.Height() - nOldHeight;
			if( nWidthDelta != 0 )
				mpTemplate->SetLongProperty(Prop::Width, rcNew.Width());
			if( nHeightDelta != 0 )
				mpTemplate->SetLongProperty(Prop::Height, rcNew.Height());
			if( nWidthDelta != 0 || nHeightDelta != 0 )
				((CPictureBoxCtrl*)mpControlWnd)->ApplyPosition();
		}
		break;
	case CtlActiveX:
		break;
	default:
		{
			CRect rcControl;
			if( mpTemplate->GetType() == CtlCalendar )
				((CMonthCalCtrl*)mpControlWnd)->GetMinReqRect( &rcControl );
			else
				mpControlWnd->GetWindowRect( &rcControl );
			CSize szControl( rcControl.Width(), rcControl.Height() );
			int nOldWidth = mpTemplate->GetLongProperty(Prop::Width);
			int nOldHeight = mpTemplate->GetLongProperty(Prop::Height);
			int nWidthDelta = szControl.cx - nOldWidth;
			int nHeightDelta = szControl.cy - nOldHeight;
			if( nWidthDelta != 0 )
				mpTemplate->SetLongProperty( Prop::Width, szControl.cx );
			if( nHeightDelta != 0 )
				mpTemplate->SetLongProperty( Prop::Height, szControl.cy );
			if( nWidthDelta != 0 || nHeightDelta != 0 )
			{
				CRect rectThis;
				mpControlWnd->GetWindowRect( &rectThis );
				mpControlWnd->GetParent()->ScreenToClient( &rectThis );
				rectThis.right = rectThis.left + szControl.cx;
				rectThis.bottom = rectThis.top + szControl.cy;
				CRect rectControl(0, 0, rectThis.Width(), rectThis.Height());
				mpControlWnd->MoveWindow( &rectThis, TRUE );
			}
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CControlManager message handlers


void CControlManager::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//CRect rcThis;
	//GetClientRect( &rcThis );
	//dc.FillSolidRect( &rcThis, GetSysColor( COLOR_GRAYTEXT ) );
	return;
}

void CControlManager::OnDestroy() 
{
	mpDlgControl = NULL;
	__super::OnDestroy();
}

BOOL CControlManager::OnEraseBkgnd(CDC* pDC)
{
	return __super::OnEraseBkgnd( pDC );
}

void CControlManager::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

__UINT_LRESULT CControlManager::OnNcHitTest(CPoint point)
{
	return HitTest( point );
	return __super::OnNcHitTest(point);
}

void CControlManager::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	CRect rcControl;
	GetClientRect( &rcControl );
	if( mGripper.m_hWnd )
		mGripper.MoveWindow( &rcControl, TRUE );

	if( mbIgnoreSizing || !IsSelected() )
		return;

	CStudioDialogObject* pDlgObject = GetDialogObject();
	if( pDlgObject == mpDlgControl )
		return;

	ClientToControl( rcControl );
	mpTemplate->SetLongProperty( Prop::Left, rcControl.left );
	mpTemplate->SetLongProperty( Prop::Top, rcControl.top );
	mpTemplate->SetLongProperty( Prop::Width, rcControl.Width() );
	mpTemplate->SetLongProperty( Prop::Height, rcControl.Height() );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Left );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Top );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Width );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Height );
	theStudioWorkspace.ActivateDclControl( mpTemplate ); //update property pane
	RedrawWindow( NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW );
}

void CControlManager::OnMove(int x, int y)
{
	__super::OnMove(x, y);

	if( mbIgnoreSizing || !IsSelected() )
		return;

	CStudioDialogObject* pDlgObject = GetDialogObject();
	if( pDlgObject == mpDlgControl )
		return;

	CRect rcControl;
	GetClientRect( &rcControl );
	ClientToControl( rcControl );
	mpTemplate->SetLongProperty( Prop::Left, rcControl.left );
	mpTemplate->SetLongProperty( Prop::Top, rcControl.top );
	mpTemplate->SetLongProperty( Prop::Width, rcControl.Width() );
	mpTemplate->SetLongProperty( Prop::Height, rcControl.Height() );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Left );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Top );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Width );
	CStudioDialogControl::UpdateProperty( mpTemplate, Prop::Height );
	mpControlWnd->SendMessage( WM_MOVE, 0, 0 ); //in case the control wants to react to position changes
	theStudioWorkspace.ActivateDclControl( mpTemplate ); //update property pane
	RedrawWindow( NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW );
}

void CControlManager::OnMouseMove(UINT nFlags, CPoint point)
{
	CStudioDialogObject* pDlgObject = GetDialogObject();
	CRect rcManager = mrcDragInitialPos;
	ClientToScreen( &point );
	switch( mnSizingType )
	{
	case HTLEFT:
		{
			CPoint ptSnap = rcManager.TopLeft();
			ptSnap.x += (point.x - mptDragInitialPos.x);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( rcManager.right - ptSnap.x < 1 )
				rcManager.left = rcManager.right - 1;
			else
				rcManager.left = ptSnap.x;
		}
		break;
	case HTRIGHT:
		{
			CPoint ptSnap = rcManager.BottomRight();
			ptSnap.x += (point.x - mptDragInitialPos.x);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( ptSnap.x - rcManager.left < 1 )
				rcManager.right = rcManager.left + 1;
			else
				rcManager.right = ptSnap.x;
		}
		break;
	case HTTOP:
		{
			CPoint ptSnap = rcManager.TopLeft();
			ptSnap.y += (point.y - mptDragInitialPos.y);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( rcManager.bottom - ptSnap.y < 1 )
				rcManager.top = rcManager.bottom - 1;
			else
				rcManager.top = ptSnap.y;
		}
		break;
	case HTBOTTOM:
		{
			CPoint ptSnap = rcManager.BottomRight();
			ptSnap.y += (point.y - mptDragInitialPos.y);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( ptSnap.y - rcManager.top < 1 )
				rcManager.bottom = rcManager.top + 1;
			else
				rcManager.bottom = ptSnap.y;
		}
		break;
	case HTTOPLEFT:
		{
			CPoint ptSnap = rcManager.TopLeft();
			ptSnap += (point - mptDragInitialPos);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( rcManager.bottom - ptSnap.y < 1 )
				rcManager.top = rcManager.bottom - 1;
			else
				rcManager.top = ptSnap.y;
			if( rcManager.right - ptSnap.x < 1 )
				rcManager.left = rcManager.right - 1;
			else
				rcManager.left = ptSnap.x;
		}
		break;
	case HTBOTTOMLEFT:
		{
			CPoint ptSnap( rcManager.left, rcManager.bottom );
			ptSnap += (point - mptDragInitialPos);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( ptSnap.y - rcManager.top < 1 )
				rcManager.bottom = rcManager.top + 1;
			else
				rcManager.bottom = ptSnap.y;
			if( rcManager.right - ptSnap.x < 1 )
				rcManager.left = rcManager.right - 1;
			else
				rcManager.left = ptSnap.x;
		}
		break;
	case HTBOTTOMRIGHT:
		{
			CPoint ptSnap = rcManager.BottomRight();
			ptSnap += (point - mptDragInitialPos);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( ptSnap.y - rcManager.top < 1 )
				rcManager.bottom = rcManager.top + 1;
			else
				rcManager.bottom = ptSnap.y;
			if( ptSnap.x - rcManager.left < 1 )
				rcManager.right = rcManager.left + 1;
			else
				rcManager.right = ptSnap.x;
		}
		break;
	case HTTOPRIGHT:
		{
			CPoint ptSnap( rcManager.right, rcManager.top );
			ptSnap += (point - mptDragInitialPos);
			pDlgObject->SnapToGrid( ptSnap, true );
			if( rcManager.bottom - ptSnap.y < 1 )
				rcManager.top = rcManager.bottom - 1;
			else
				rcManager.top = ptSnap.y;
			if( ptSnap.x - rcManager.left < 1 )
				rcManager.right = rcManager.left + 1;
			else
				rcManager.right = ptSnap.x;
		}
		break;
	default:
		assert( false ); //shouldn't be called with anything other than standard sizing values!
	}
	MoveWindow( &rcManager );
}

void CControlManager::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive, dwThreadID);
	ReleaseCapture();
}

void CControlManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	__super::OnLButtonUp(nFlags, point);
}

void CControlManager::OnCaptureChanged(CWnd *pWnd)
{
	if( mbUndoGroupActive )
	{
		CUndoManager* pUndoManager = mpTemplate->GetUndoManager();
		if( pUndoManager )
		{
			pUndoManager->EndGroup();
			mbUndoGroupActive = false;
		}
	}

	__super::OnCaptureChanged(pWnd);
}

void CControlManager::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	ReleaseCapture();
	__super::OnNcLButtonUp(nHitTest, point);
}
