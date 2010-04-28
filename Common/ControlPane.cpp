// ControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ControlPane.h"
#include "DialogObject.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "UndoManager.h"
#include "Project.h"


/////////////////////////////////////////////////////////////////////////////
// CControlPane

CControlPane::CControlPane()
: mpSourceForm( NULL )
, mpProject( NULL )
, mpDlgObject( NULL )
, mpHostDlg( NULL )
, mbRecalcInProgress( false )
, mbDeferWindowPos( false )
{
}

CControlPane::CControlPane(TDclFormPtr pSourceForm, CWnd* pHostDlg)
: mpSourceForm( pSourceForm )
, mpProject( pSourceForm? pSourceForm->GetProject() : NULL )
, mpDlgObject( pSourceForm? pSourceForm->GetFormInstance() : NULL )
, mpHostDlg( pHostDlg )
, mbRecalcInProgress( false )
, mbDeferWindowPos( false )
{
}

CControlPane::~CControlPane()
{
}

CRect CControlPane::GetControlArea() const
{
	assert( mpDlgObject != NULL );
	return mpDlgObject->GetWndRect();
}

void CControlPane::ZOrderFront( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
{
	CWnd* pControlWnd = pDlgControl->GetControlWnd();
	if( pControlWnd )
	{
		if( hDeferred )
			DeferWindowPos( hDeferred, pControlWnd->m_hWnd, HWND_TOP, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
		else
			pControlWnd->SetWindowPos( &CWnd::wndTop, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
	}
}

void CControlPane::ZOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
{
	CWnd* pControlWnd = pDlgControl->GetTopLevelWnd();
	if( pControlWnd )
	{
		if( hDeferred )
			DeferWindowPos( hDeferred, pControlWnd->m_hWnd, HWND_BOTTOM, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
		else
			pControlWnd->SetWindowPos( &CWnd::wndBottom, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
	}
}

void CControlPane::AddControl( TDialogControlPtr pDlgControl )
{
	mControls.push_back( pDlgControl );
}

void CControlPane::RemoveControl( CDialogControl* pDlgControl )
{
	for( TDialogControls::iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (*iter) == pDlgControl )
		{
			mControls.erase( iter );
			return;
		}
	}
}

bool CControlPane::CreateControls(UINT& nId)
{
	DisableUndoManager DisableUndo( mpSourceForm->GetUndoManager() );
	//mpHostDlg->ModifyStyleEx( 0, 0x02000000L/*WS_EX_COMPOSITED*/ );
	TDclControlList& Controls = mpSourceForm->mDclControls;
	if( Controls.empty() )
		return true;
	bool bFailed = false;
	bool bVisible = (mpHostDlg->IsWindowVisible() != FALSE);
	if( bVisible )
		mpHostDlg->SetRedraw( FALSE );
	for( TDclControlList::reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		TDclControlPtr pDclControl = (*iter);
		if( pDclControl->GetType() <= _CtlForm )
			continue;
		if( pDclControl->GetType() == CtlFileExplorer )
			continue;
		UINT idDlg = pDclControl->GetID();
		if( idDlg <= 2 || pDclControl->GetType() != CtlSplitter )
			idDlg = nId++;
		TDialogControlPtr pDlgControl = CreateNewDialogControl( pDclControl, idDlg );
		assert( pDlgControl != NULL );
		if( pDlgControl )
			mControls.push_back( pDlgControl );
		else
			bFailed = true;
	}
	//ApplyZOrder();
	//InvalidateControls();
	if( bVisible )
	{
		mpHostDlg->SetRedraw( TRUE );
		//mpHostDlg->RedrawWindow( NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN );
	}
	SetFirstControlFocus();
	return !bFailed;
}

void CControlPane::RecalcLayout()
{
	if( mbRecalcInProgress )
		return;

	if( !mpSourceForm )
		return;

	if( mpSourceForm->GetControlCount() == 0 )
		return;

	DisableUndoManager DisableUndo( mpSourceForm->GetUndoManager() ); //no need to record undo during recalc

	mbRecalcInProgress = true;
	TraceFmt( _T("CControlPane(%s)::RecalcLayout() [%d x %d]\r\n"),
						asString( mpSourceForm ),
						mpSourceForm->GetFormSize().cx,
						mpSourceForm->GetFormSize().cy );
	TDclControlList& Controls = mpSourceForm->mDclControls;

	// first lets calc all the control positions for any splitter controls.
	for( TDclControlList::reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		if( (*iter)->GetType() == CtlSplitter )
			RecalcControlPos( (*iter) );
	}

	//next lets calc all the control positions for all NON-splitter controls.
	for( TDclControlList::reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		if( (*iter)->GetType() == _CtlForm )
			continue;
		if( (*iter)->GetType() != CtlSplitter )
			RecalcControlPos( (*iter) );
	}
	mbRecalcInProgress = false;
}

void CControlPane::InvalidateControls()
{
	if( !mpSourceForm )
		return;

	TDclControlList& Controls = mpSourceForm->mDclControls;
	for( TDclControlList::reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		ControlType type = (*iter)->GetType();
		if( type == _CtlForm )
			continue;
		if( type != CtlBlockView && type != CtlHatch )
		{
			const CDialogControl* pDlgControl = (*iter)->GetControlInstance();
			if( pDlgControl )
				pDlgControl->OnNeedRepaint( false, false );
		}
	}
}

CPoint CControlPane::GetSplitterPos( int nSplitterId ) const
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = (*iter);
		if( pDclControl->GetType() == CtlSplitter && pDclControl->GetID() == nSplitterId )
			return pDclControl->GetWndRect().TopLeft();
	}
	return CPoint( 0, 0 );	
}

bool CControlPane::HasSplitter( int nSplitterId ) const
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = (*iter);
		if( pDclControl->GetType() == CtlSplitter && pDclControl->GetID() == nSplitterId )
			return true;
	}
	return false;	
}

void CControlPane::RecalcControlPos( TDclControlPtr pDclControl )
{
	if( !pDclControl || !pDclControl->IsZOrderAllowed() )
		return;
	CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return;

	assert( mbDeferWindowPos == false );
	mbDeferWindowPos = true;
	CRect rcBefore = pDlgControl->GetWndRect();
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseLeftFromRight ) );
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseRightFromRight ) );
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseTopFromBottom ) );
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseBottomFromBottom ) );
	mbDeferWindowPos = false;
	CRect rcAfter = pDlgControl->GetWndRect();
	if( rcAfter != rcBefore )
		ApplyPosition( TDialogControlLockedPtr( pDlgControl ) );
}

void CControlPane::CleanUpControls() 
{	
	for( TDialogControls::iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		CWnd* pControl = (*iter)->GetControlWnd();
		if( pControl )
			pControl->DestroyWindow();
	}
	mControls.clear();
}

void CControlPane::ApplyZOrder()
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	HDWP hdwp = (Controls.empty()? NULL : BeginDeferWindowPos( Controls.size() ));
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDialogControlLockedPtr pDlgControl = (*iter)->GetControlInstance();
		if( !pDlgControl )
			continue;
		if( !pDlgControl->GetTemplate()->IsZOrderAllowed() )
			continue;
		ZOrderFront( pDlgControl, hdwp );
	}
	if( hdwp )
		EndDeferWindowPos( hdwp );
	//InvalidateControls();
}

void CControlPane::ApplyPosition( TDialogControlPtr pDlgControl )
{
	if( mbDeferWindowPos )
		return;
	CWnd* pWndToMove = pDlgControl->GetControlWnd();
	if( !pWndToMove )
		return;
	bool bTransparent = (0 != (pWndToMove->GetExStyle() & WS_EX_TRANSPARENT));
	//if the control is hosted inside another window, find the ancestor that is a child
	//of the host dialog and move it instead
	bool bWindowLess = (pWndToMove->m_hWnd == NULL);
	assert( bWindowLess || mpHostDlg->IsChild( pWndToMove ) || mpHostDlg == pWndToMove );
	if( !bWindowLess )
	{
		while( (pWndToMove->GetControlUnknown() || (pWndToMove->GetStyle() & WS_CHILD)) &&
					 (pWndToMove != mpHostDlg && pWndToMove->GetParent() != mpHostDlg) )
			pWndToMove = pWndToMove->GetParent();
	}
	CRect rcCtrl;
	pWndToMove->GetWindowRect( &rcCtrl );
	if( pWndToMove->GetStyle() & WS_CHILD )
		pWndToMove->GetParent()->ScreenToClient( &rcCtrl );
	CRect rcNew = pDlgControl->GetWndRect();
	if( rcNew == rcCtrl )
		return; //no-op
	TraceFmt( _T("CControlPane(%s)::ApplyPosition(%s)\r\n"),
						asString( this ),
						asString( pDlgControl ) );
	UINT nFlags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	if( rcNew.Size() == rcCtrl.Size() )
		nFlags |= SWP_NOSIZE;
	if( rcNew.TopLeft() == rcCtrl.TopLeft() )
		nFlags |= SWP_NOMOVE;
	//if( bTransparent )
	//	nFlags |= SWP_NOCOPYBITS;
	nFlags |= SWP_NOCOPYBITS;
	pWndToMove->SetWindowPos( NULL, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), nFlags );
}

void CControlPane::ApplyVisibility( TDialogControlPtr pDlgControl )
{
	if( !IsInvisibleControlAllowed( pDlgControl ) )
		return;
	CWnd* pWndToMove = pDlgControl->GetControlWnd();
	//if the control is hosted inside another window, find the ancestor that is a child
	//of the host dialog and show or hide it instead
	assert( mpHostDlg->IsChild( pWndToMove ) );
	while( (pWndToMove->GetStyle() & WS_CHILD) && pWndToMove->GetParent() != mpHostDlg )
		pWndToMove = pWndToMove->GetParent();
	TPropertyPtr pVisibility = pDlgControl->GetTemplate()->GetPropertyObject( Prop::Visible );
	bool bHide = (pVisibility && !pVisibility->GetBooleanValue());
	pWndToMove->ShowWindow( bHide? SW_HIDE : SW_SHOW );
}

void CControlPane::SetFirstControlFocus() const
{
	if(!mpSourceForm)
		return;
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		TDialogControlLockedPtr pDlgControl = (*iter)->GetControlInstance();
		if( !pDlgControl )
			continue;
		CWnd* pWnd = pDlgControl->GetControlWnd();
		if( !pWnd )
			continue;
		if( (pWnd->GetStyle() & (WS_VISIBLE | WS_TABSTOP | WS_DISABLED)) != (WS_VISIBLE | WS_TABSTOP) )
			continue;
		pWnd->SetFocus();
		return;
	}
}

bool CControlPane::FindControl( HWND hwndControl, /*out*/ CString& sControlName ) const
{
	if (!hwndControl)
		return false;
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (*iter)->GetControlWnd()->m_hWnd == hwndControl )
		{
			sControlName = (*iter)->GetKeyName();
			return true;
		}
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
			{
				if( (*iterChild)->FindControl( hwndControl, sControlName ) )
					return true;
			}
		}
	}
	return false;
}

TDialogControlPtr CControlPane::FindControl( HWND hwndControl ) const
{
	if (!hwndControl)
		return false;
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (*iter)->GetControlWnd()->m_hWnd == hwndControl )
			return (*iter);
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
			{
				TDialogControlPtr pControl = (*iterChild)->FindControl( hwndControl );
				if( pControl )
					return pControl;
			}
		}
	}
	return NULL;
}

TDialogControlPtr CControlPane::FindControl( LPCTSTR pszControlName, ControlType type /*= _CtlInvalid*/ ) const
{
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (type == _CtlInvalid || (*iter)->GetControlType() == type) && (*iter)->GetKeyName() == pszControlName )
			return (*iter);
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
			{
				TDialogControlPtr pControl = (*iterChild)->FindControl( pszControlName, type );
				if( pControl )
					return pControl;
			}
		}
	}
	return NULL;
}
