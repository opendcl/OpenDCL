// FormControlManager.cpp : implementation file
//

#include "stdafx.h"
#include "FormControlManager.h"
#include "StudioDialogObject.h"
#include "StudioWorkspace.h"
#include "ControlManager.h"
#include "PropertyPane.h"
#include "ToolboxPane.h"


/////////////////////////////////////////////////////////////////////////////
// CFormControlManager

BEGIN_MESSAGE_MAP(CFormControlManager, CControlManager)
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONUP()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()


CFormControlManager::CFormControlManager( CStudioDialogObject* pDlgObject )
: CControlManager( pDlgObject, false )
, CRectTracker( &CRect(), (dottedLine) )
, mpDlgObject( pDlgObject )
, mDropSource( this )
, mDropTarget( this )
, mptDragStart( -1, -1 )
, mbDragInitiated( false )
, mbPickSelected( false )
, mbSnapTracker( false )
, mpResizeTarget( NULL )
{
	CRect rcControlArea = pDlgObject->GetWndRect();
	Create( WS_CHILD | WS_VISIBLE, rcControlArea, pDlgObject->GetControlWnd() );
	SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOACTIVATE );
	ModifyStyle( WS_CLIPCHILDREN, 0, 0 );
	ModifyStyleEx( 0, WS_EX_TRANSPARENT, 0 );
	//mGripper.Create( _T(""), SS_OWNERDRAW | WS_CHILD | WS_VISIBLE, rcControlArea, this );
	//ShowWindow( SW_HIDE );
}

CFormControlManager::~CFormControlManager()
{
}

void CFormControlManager::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd )
{
	CRect rcTrack( *lpRect );
	if( mbSnapTracker )
	{
		mpDlgObject->SnapToGrid( rcTrack.TopLeft() );
		mpDlgObject->SnapToGrid( rcTrack.BottomRight() );
	}
	__super::DrawTrackerRect( &rcTrack, pWndClipTo, pDC, pWnd );
}

bool CFormControlManager::ActivateControlsInRect( const CRect& rc )
{
	CRect rcSelection( rc );
	rcSelection.NormalizeRect();
	bool bFound = false;
	const TDclControlList& Controls = mpDlgObject->GetSourceForm()->GetControlList();
	for( TDclControlList::const_reverse_iterator iter = Controls.rbegin();
			 iter != Controls.rend();
			 ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		if( pDlgControl == mpDlgObject )
			continue;
		CRect rcTest = pDlgControl->GetEffectiveWindowRect();
		if( rcTest.IntersectRect( &rcTest, &rcSelection ) )
		{
			theStudioWorkspace.ActivateDclControl( pDlgControl->GetTemplate() );
			bFound = true;
		}
	}
	return bFound;
}


/////////////////////////////////////////////////////////////////////////////
// CFormControlManager message handlers

__UINT_LRESULT CFormControlManager::OnNcHitTest(CPoint point)
{
	if( mptDragStart.x != -1 && mptDragStart.y != -1 )
		return HTCLIENT; //starting a drag operation, so need to route through OnMouseMove

	bool bShiftPressed = ((GetAsyncKeyState( VK_SHIFT ) & KF_UP) == KF_UP);
	if( bShiftPressed || theStudioWorkspace.GetToolboxPane()->IsPointer() )
	{
		const TDclControlList& Controls = mpDlgObject->GetSourceForm()->GetControlList();
		for( TDclControlList::const_reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
		{
			TDclControlPtr pDclControl = *iter;
			CDialogControl* pDlgControl = pDclControl->GetControlInstance();
			if( !pDlgControl )
				continue;
			CControlManager* pManager = pDlgControl->GetControlManager();
			if( pManager == this )
				continue;
			//holding [shift] disallows resizing and ignores transparent areas for easier selection
			LRESULT lhtResult = pManager->HitTest( point, bShiftPressed );
			if( lhtResult == HTNOWHERE )
				continue;
			if( lhtResult == HTCLIENT )
				return HTCLIENT;
			if( lhtResult == HTOBJECT )
				return HTOBJECT;
			if( lhtResult == HTBOTTOMRIGHT )
				return HTTOPLEFT;
			if( lhtResult >= HTSIZEFIRST && lhtResult <= HTSIZELAST )
				return lhtResult;
			return HTBORDER;
		}
	}
	CRect rcDlg;
	mpDlgObject->GetWindowRect( &rcDlg );
	mpDlgObject->ScreenToClient( &rcDlg );
	point += rcDlg.TopLeft();
	LRESULT lResult = mpDlgObject->HitTest( point );
	if( lResult >= HTSIZEFIRST && lResult <= HTSIZELAST )
		return HTTRANSPARENT; //to allow resizing
	return HTBORDER;
	return HTTRANSPARENT;

	return __super::OnNcHitTest(point);
}

void CFormControlManager::OnLButtonDown(UINT nFlags, CPoint point)
{
	mbPickSelected = false;
	mptDragStart = point;
	mbSnapTracker = (!theStudioWorkspace.GetToolboxPane()->IsPointer());
	if( !GetCapture() )
	{
		CPoint ptScreen( point );
		ClientToScreen( &ptScreen );
		CDialogControl* pDlgControl = mpDlgObject->GetControlAtPoint( ptScreen );
		if( pDlgControl )
		{
			if( !pDlgControl->GetControlManager()->IsSelected() )
			{
				mbPickSelected = true;
				bool bDeactivateCurrent = (((nFlags & MK_CONTROL) == 0) || mpDlgObject->IsOnlyFormActive());
				theStudioWorkspace.ActivateDclControl( pDlgControl->GetTemplate(), bDeactivateCurrent );
			}
		}
		else if( !TrackRubberBand( this, mpDlgObject->SnapToGrid( point ) ) )
			theStudioWorkspace.ActivateDclControl( GetTemplate(), ((nFlags & MK_CONTROL) == 0) && !mpDlgObject->IsOnlyFormActive() );
		else
		{
			bool bDeactivateCurrent = (((nFlags & MK_CONTROL) == 0) || mpDlgObject->IsOnlyFormActive());
			if( bDeactivateCurrent )
				theStudioWorkspace.ActivateDclControl( GetTemplate(), true );
			if( mbSnapTracker )
			{
				mpDlgObject->SnapToGrid( m_rect.BottomRight() );
				mpDlgObject->InsertControl( theStudioWorkspace.GetToolboxPane()->GetSelectedTool(), m_rect );
			}
			else if( !ActivateControlsInRect( m_rect ) )
				theStudioWorkspace.ActivateDclControl( GetTemplate(), true );
		}
	}

	__super::OnLButtonDown(nFlags, point);
}

void CFormControlManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( mptDragStart.x != -1 && mptDragStart.y != -1 )
	{
		mptDragStart.SetPoint( -1, -1 );
		if( !mbDragInitiated )
		{
			CPoint ptScreen( point );
			ClientToScreen( &ptScreen );
			CDialogControl* pDlgControl = mpDlgObject->GetControlAtPoint( ptScreen );
			if( pDlgControl )
			{
				TDclControlPtr pSelControl = pDlgControl->GetTemplate();
				bool bSelected = pDlgControl->GetControlManager()->IsSelected();
				bool bLoneSelection = (bSelected && (mpDlgObject->CountSelected( false ) == 1));
				if( (nFlags & MK_CONTROL) != 0 )
				{
					if( bLoneSelection )
					{
						if( !mbPickSelected )
							theStudioWorkspace.ActivateDclControl( GetTemplate(), true );
					}
					else if( !bSelected )
						theStudioWorkspace.ActivateDclControl( pSelControl, false );
					else if( !mbPickSelected )
					{
						TDclControlList SelectedControls;
						mpDlgObject->GetSelectedControls( SelectedControls );
						bool bDeactivateSel = true;
						for( TDclControlList::const_iterator iter = SelectedControls.begin();
								 iter != SelectedControls.end();
								 ++iter )
						{
							if( (*iter) == pSelControl )
								continue; //skip the picked control in order to unselect it
							theStudioWorkspace.ActivateDclControl( *iter, bDeactivateSel );
							bDeactivateSel = false;
						}
					}
				}
				else
				{
					if( !bSelected )
						theStudioWorkspace.ActivateDclControl( pSelControl, true );
					else if( !bLoneSelection )
						theStudioWorkspace.ActivateDclControl( pSelControl, true );
				}
			}
		}
	}
	if( mbDragInitiated )
		mbDragInitiated = false;
	mbPickSelected = false;
	__super::OnLButtonUp(nFlags, point);
}

void CFormControlManager::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);
	AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_CONTROLBROWSER, (LPARAM)0 );
}

void CFormControlManager::OnMouseMove(UINT nFlags, CPoint point)
{
	static bool bInMouseMove = false;
	assert( !bInMouseMove );
	bInMouseMove = true;
	bool bCapture = (GetCapture() == this);
	if( mptDragStart.x != -1 && mptDragStart.y != -1 )
	{
		bool bStartDragging = !bCapture;
		bool bDragging = !bStartDragging;
		if( !bDragging )
		{ //should we begin dragging?
			CRect rcDragRegion( mptDragStart, CSize( 0, 0 ) );
			rcDragRegion.InflateRect( GetSystemMetrics( SM_CXDOUBLECLK ) / 2,
																GetSystemMetrics( SM_CYDOUBLECLK ) / 2 );
			bDragging = !rcDragRegion.PtInRect( point );
			if( bDragging )
			{
				CUndoManager* pUndoManager = mpDlgObject->GetSourceForm()->GetUndoManager();
				if( pUndoManager && !mbUndoGroupActive )
				{
					pUndoManager->BeginGroup( theWorkspace.LoadResourceString( IDS_UNDO_MOVECONTROL ) );
					mbUndoGroupActive = true;
				}
				SetCapture();
				mbDragInitiated = true;
			}
		}
		if( bDragging )
		{
			const TDclControlList& Controls = mpDlgObject->GetSourceForm()->GetControlList();
			for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
			{
				TDclControlPtr pDclControl = *iter;
				CDialogControl* pDlgControl = pDclControl->GetControlInstance();
				if( !pDlgControl )
					continue;
				CControlManager* pManager = pDlgControl->GetControlManager();
				if( pManager == this )
					continue;
				if( !pManager->IsSelected() )
					continue;
				CRect rcManager;
				pManager->GetWindowRect( &rcManager );
				if( bStartDragging )
					pManager->SetInitialDragPosition( rcManager.TopLeft() );
				CPoint ptUL = pManager->GetInitialDragPosition();
				ptUL += (point - mptDragStart);
				rcManager.MoveToXY( ptUL );
				mpDlgObject->ScreenToClient( &rcManager );
				pManager->CheckNewWindowPosition( rcManager );
				assert( pManager->GetParent() == mpDlgObject );
				rcManager = pDlgControl->ValidatePosition( rcManager );
				pManager->MoveWindow( &rcManager );
				mpDlgObject->InvalidateRect( &rcManager );
			}
		}
	}
	bInMouseMove = false;
}

void CFormControlManager::OnCancelMode()
{
	ReleaseCapture();
	__super::OnCancelMode();
}

void CFormControlManager::OnCaptureChanged(CWnd *pWnd)
{
	mptDragStart.SetPoint( -1, -1 );
	mpDlgObject->OnUpdateZOrder();
	__super::OnCaptureChanged(pWnd);
}

void CFormControlManager::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call __super::OnNcPaint() for painting messages
}

void CFormControlManager::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	return;
}

BOOL CFormControlManager::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	static HCURSOR hcCross = LoadCursor( NULL, IDC_CROSS );
	static HCURSOR hcArrow = LoadCursor( NULL, IDC_ARROW );
	static HCURSOR hcSizeAll = LoadCursor( NULL, IDC_SIZEALL );
	static HCURSOR hcSizeEW = LoadCursor( NULL, IDC_SIZEWE );
	static HCURSOR hcSizeNS = LoadCursor( NULL, IDC_SIZENS );
	static HCURSOR hcSizeNWSE = LoadCursor( NULL, IDC_SIZENWSE );
	static HCURSOR hcSizeNESW = LoadCursor( NULL, IDC_SIZENESW );
	switch( nHitTest )
	{
	case HTTRANSPARENT:
		__super::OnSetCursor( pWnd, nHitTest, message );
		break;
	case HTCLIENT:
		if( pWnd == this )
			::SetCursor( hcSizeAll );
		else
			pWnd->SendMessage( WM_SETCURSOR, (WPARAM)pWnd, MAKELPARAM(nHitTest, message) );
		break;
	case HTBORDER:
		if( theStudioWorkspace.GetToolboxPane()->IsPointer() )
			::SetCursor( hcArrow );
		else
			::SetCursor( hcCross );
		break;
	case HTLEFT:
	case HTRIGHT:
		::SetCursor( hcSizeEW );
		break;
	case HTTOP:
	case HTBOTTOM:
		::SetCursor( hcSizeNS );
		break;
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		::SetCursor( hcSizeNWSE );
		break;
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		::SetCursor( hcSizeNESW );
		break;
	case HTOBJECT:
		::SetCursor( hcArrow ); //selectable, but not moveable or resizable
		break;
	default:
		::SetCursor( hcCross ); //inserting a control
		break;
	}
	return TRUE;
}

void CFormControlManager::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	__super::OnNcLButtonDown(nHitTest, point);

	if( nHitTest == HTBORDER )
	{
		mptDragStart = point;
		mbSnapTracker = (!theStudioWorkspace.GetToolboxPane()->IsPointer());
		GetParent()->ScreenToClient( &point );
		if( mbSnapTracker )
			mpDlgObject->SnapToGrid( point );
		if( !TrackRubberBand( this, point ) )
		{
			if( !mpDlgObject->IsOnlyFormActive() )
				theStudioWorkspace.ActivateDclControl( GetTemplate() );
		}
		else
		{
			m_rect.NormalizeRect();
			bool bDeactivateCurrent = (((GetAsyncKeyState( VK_CONTROL ) & KF_UP) != KF_UP) || mpDlgObject->IsOnlyFormActive());
			if( bDeactivateCurrent )
				theStudioWorkspace.ActivateDclControl( NULL );
			if( mbSnapTracker )
			{
				mpDlgObject->SnapToGrid( m_rect.TopLeft() );
				mpDlgObject->SnapToGrid( m_rect.BottomRight() );
				mpDlgObject->InsertControl( theStudioWorkspace.GetToolboxPane()->GetSelectedTool(), m_rect );
			}
			else if( !ActivateControlsInRect( m_rect ) )
				theStudioWorkspace.ActivateDclControl( GetTemplate() );
		}
	}
	else if( nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST )
	{
		CDialogControl* pDlgControl = mpDlgObject->GetControlAtPoint( point );
		if( pDlgControl )
		{
			theStudioWorkspace.ActivateDclControl( pDlgControl->GetTemplate(), true );
			CControlManager* pManager = pDlgControl->GetControlManager();
			if( pManager )
			{
				mpResizeTarget = pManager;
				if( nHitTest == HTTOPLEFT )
				{
					//kludge for problem with HTBOTTOMRIGHT causing mouse messages to go to CStudioDialogObject
					nHitTest = pManager->HitTest( point );
				}
				pManager->StartSizing( nHitTest, point );
			}
		}
	}
	else if( nHitTest == HTOBJECT )
	{ //control is selectable, but not moveable or resizable
		CDialogControl* pDlgControl = mpDlgObject->GetControlAtPoint( point );
		if( pDlgControl )
		{
			bool bDeactivateCurrent = (((GetAsyncKeyState( VK_CONTROL ) & KF_UP) != KF_UP) || mpDlgObject->IsOnlyFormActive());
			theStudioWorkspace.ActivateDclControl( pDlgControl->GetTemplate(), bDeactivateCurrent );
		}
	}
}

void CFormControlManager::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	theStudioWorkspace.ActivateDclControl( GetTemplate() );
	PostMessage( WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(point.x, point.y) );
}

void CFormControlManager::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	__super::OnNcLButtonDblClk(nHitTest, point);
	AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_CONTROLBROWSER, (LPARAM)0 );
}
