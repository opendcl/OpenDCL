// ControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ControlPane.h"
#include "DialogObject.h"
#include "DclFormTemplate.h"
#include "DclControlTemplate.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "UndoManager.h"
#include "Project.h"
#include <algorithm>


/////////////////////////////////////////////////////////////////////////////
// CControlPane

CControlPane::CControlPane()
: mpSourceForm( NULL )
, mpProject( NULL )
, mpDlgObject( NULL )
, mpHostDlg( NULL )
, mbRecalcInProgress( false )
, mbDeferWindowPos( false )
, mbDpiChanged( false )
, mnDPI( 0 )
{
}

CControlPane::CControlPane(TDclFormPtr pSourceForm, CWnd* pHostDlg)
: mpSourceForm( pSourceForm )
, mpProject( pSourceForm? pSourceForm->GetProject() : NULL )
, mpDlgObject( pSourceForm? pSourceForm->GetFormInstance() : NULL )
, mpHostDlg( pHostDlg )
, mbRecalcInProgress( false )
, mbDeferWindowPos( false )
, mbDpiChanged( false )
, mnDPI( 0 )
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

UINT CControlPane::GetDpi() const
{
	if ( mnDPI == 0 )
	{
		TDclFormPtr pParentForm = mpSourceForm->GetParentForm();
		if( pParentForm )
		{
			CDialogObject* pParentDlg = pParentForm->GetFormInstance();
			if( pParentDlg )
			{
				CControlPane* pParentPane = pParentDlg->GetControlPane();
				return pParentPane->GetDpi();
			}
		}
		CWnd* pWnd = mpHostDlg;
		while( (pWnd && pWnd->GetStyle() & WS_CHILD) != 0 )
			pWnd = pWnd->GetParent();
		HWND hwnd = pWnd->GetSafeHwnd();
		int nDPI = (int)GetDpiForWindow( hwnd );
		if ( !hwnd )
			return nDPI;
		mnDPI = nDPI;
	}
	return mnDPI;
}

void CControlPane::TabOrderFront( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
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

void CControlPane::TabOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
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
	//ApplyTabOrder();
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
	TraceFmt( _T("CControlPane(%s)::RecalcLayout() [%d x %d / %udpi]\r\n"),
						asString( mpSourceForm ),
						mpSourceForm->GetFormSize().cx,
						mpSourceForm->GetFormSize().cy,
						GetDpi() );
	mPendingRecalc.clear();
	TDclControlList& Controls = mpSourceForm->mDclControls;
	for( TDclControlList::reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		if( (*iter)->GetType() == _CtlForm )
			continue;
		mPendingRecalc.push_front( (*iter) );
	}

	//now recalc all the control positions
	while( !mPendingRecalc.empty() )
	{
		TDclControlPtr pControl = mPendingRecalc.front();
		mPendingRecalc.pop_front();
		RecalcControlPos( pControl );
	}
	mbRecalcInProgress = false;
	mbDpiChanged = false;
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

CPoint CControlPane::GetSplitterPos( int nSplitterId )
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = (*iter);
		if( pDclControl->GetType() == CtlSplitter && pDclControl->GetID() == nSplitterId )
		{
			for( TDclControlList::iterator iterPending = mPendingRecalc.begin(); iterPending != mPendingRecalc.end(); ++iterPending )
			{
				if( (*iterPending)->GetType() == CtlSplitter && (*iterPending)->GetID() == nSplitterId )
				{
					mPendingRecalc.erase( iterPending );
					RecalcControlPos( pDclControl );
					break;
				}
			}
			return pDclControl->GetWndRect().TopLeft();
		}
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

bool CControlPane::IsModal() const
{
	TDclFormPtr pTopLevelForm = mpSourceForm;
	TDclFormPtr pParentForm = pTopLevelForm->GetParentForm();
	while( pParentForm )
	{
		pTopLevelForm = pParentForm;
		pParentForm = pTopLevelForm->GetParentForm();
	}
	return !pTopLevelForm->IsModeless();
}

bool CControlPane::IsClosing() const
{
	if( !mpDlgObject )
		return false;
	return mpDlgObject->IsClosing();
}

void CControlPane::RecalcControlPos( TDclControlPtr pDclControl )
{
	if( !pDclControl || !pDclControl->IsTabOrderAllowed() )
		return;
	CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return;

	TraceFmt( _T("CControlPane(%s)::RecalcControlPos(%s)\r\n"), asString( this ), asString( pDlgControl ) );
	bool bWasDeferWindowPos = mbDeferWindowPos;
	mbDeferWindowPos = true;
	CRect rcBefore = pDlgControl->GetWndRect();
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseLeftFromRight ) );
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseRightFromRight ) );
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseTopFromBottom ) );
	pDlgControl->ApplyProperty( pDclControl->GetPropertyObject( Prop::UseBottomFromBottom ) );
	mbDeferWindowPos = false;
	bool applyPos = mbDpiChanged;
	if ( !applyPos )
	{
		CRect rcAfter = pDlgControl->GetWndRect();
		applyPos = (TRUE == (rcAfter != rcBefore));
	}
	if ( applyPos )
		ApplyPosition( TDialogControlLockedPtr( pDlgControl ) );
	mbDeferWindowPos = bWasDeferWindowPos;
}

void CControlPane::CleanUpControls() 
{	
	TDialogControls Controls = mControls;
	mControls.clear();
	for( TDialogControls::iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		CWnd* pControl = (*iter)->GetControlWnd();
		if( pControl )
			pControl->DestroyWindow();
	}
}

void CControlPane::ApplyTabOrder()
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	HDWP hdwp = (Controls.empty()? NULL : BeginDeferWindowPos( Controls.size() ));
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDialogControlLockedPtr pDlgControl = (*iter)->GetControlInstance();
		if( !pDlgControl )
			continue;
		if( !pDlgControl->GetTemplate()->IsTabOrderAllowed() )
			continue;
		TabOrderFront( pDlgControl, hdwp );
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
	pDlgControl->FromDIP(rcNew);
	if( rcNew == rcCtrl )
		return; //no-op
	TraceFmt( _T("CControlPane(%s)::ApplyPosition(%s @ [%d,%d/%dx%d/%udpi])\r\n"),
						asString( this ),
						asString( pDlgControl ),
						rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(),
						GetDpi() );
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
	CWnd* pWndToMove = pDlgControl->GetControlWnd();
	//if the control is hosted inside another window, find the ancestor that is a child
	//of the host dialog and show or hide it instead
	assert( mpHostDlg->IsChild( pWndToMove ) );
	while( (pWndToMove->GetStyle() & WS_CHILD) && pWndToMove->GetParent() != mpHostDlg )
		pWndToMove = pWndToMove->GetParent();
	TPropertyPtr pVisibility = pDlgControl->GetTemplate()->GetPropertyObject( Prop::Visible );
	bool bHide = (pVisibility && !pVisibility->GetBooleanValue());
	if( bHide && !IsInvisibleControlAllowed( pDlgControl ) )
		bHide = false;
	pWndToMove->ShowWindow( bHide? SW_HIDE : SW_SHOW );
}

bool CControlPane::CheckDpiChanged()
{
	UINT nOldDPI = mnDPI;
	mnDPI = 0; //force recalc
	UINT nNewDPI = GetDpi();
	if ( nOldDPI == nNewDPI )
		return false;
	mbDpiChanged = true;
	if( mpSourceForm && !IsDpiAwarePerMonitorV2( mpHostDlg->GetTopLevelParent()->GetSafeHwnd() ) )
	{
		TDclControlList& Controls = mpSourceForm->mDclControls;
		for( TDclControlList::reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
		{
			ControlType type = (*iter)->GetType();
			if( type == _CtlForm )
				continue;
			CDialogControl* pDlgControl = (*iter)->GetControlInstance();
			if( pDlgControl )
				pDlgControl->HandleDpiChanged( );
		}
	}
	return (nOldDPI != mnDPI);
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
		return NULL;
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
