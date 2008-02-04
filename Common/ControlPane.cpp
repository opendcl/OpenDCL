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
, mhDeferredPos( NULL )
{
}

CControlPane::CControlPane(TDclFormPtr pSourceForm, CWnd* pHostDlg)
: mpSourceForm( pSourceForm )
, mpProject( pSourceForm? pSourceForm->GetProject() : NULL )
, mpDlgObject( pSourceForm? pSourceForm->GetFormInstance() : NULL )
, mpHostDlg( pHostDlg )
, mbRecalcInProgress( false )
, mhDeferredPos( NULL )
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
			DeferWindowPos( hDeferred, pControlWnd->m_hWnd, HWND_TOP, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE );
		else
			pControlWnd->SetWindowPos( &CWnd::wndTop, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE );
	}
}

void CControlPane::ZOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
{
	CWnd* pControlWnd = pDlgControl->GetControlWnd();
	if( pControlWnd )
	{
		if( hDeferred )
			DeferWindowPos( hDeferred, pControlWnd->m_hWnd, HWND_BOTTOM, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE );
		else
			pControlWnd->SetWindowPos( &CWnd::wndBottom, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE );
	}
}

void CControlPane::AddControl( TDialogControlPtr pDlgControl )
{
	mControls.push_back( pDlgControl );
}

void CControlPane::RemoveControl( TDialogControlPtr pDlgControl )
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
	TDclControlList& Controls = mpSourceForm->mDclControls;
	if( Controls.empty() )
		return true;
	bool bFailed = false;
	bool bVisible = (mpHostDlg->IsWindowVisible() != FALSE);
	if( bVisible )
		mpHostDlg->SetRedraw( FALSE );
	for( TDclControlList::iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = (*iter);
		if (pDclControl->GetType() <= _CtlForm)
			continue;
		if (pDclControl->GetType() == CtlFileDlgCtrl)
			continue;
		UINT idDlg = pDclControl->GetID();
		if( idDlg <= 0 || pDclControl->GetType() != CtlSplitter )
			idDlg = nId++;
		TDialogControlPtr pControl = CreateNewDialogControl( pDclControl, idDlg );
		assert( pControl != NULL );
		if( pControl )
			mControls.push_back( pControl );
		else
			bFailed = true;
	}
	if( bVisible )
	{
		mpHostDlg->RedrawWindow( NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN );
		mpHostDlg->SetRedraw( TRUE );
	}
	return !bFailed;
}

void CControlPane::RecalcLayout()
{
	if( !mpSourceForm )
		return;

	if( mpSourceForm->GetControlCount() == 0 )
		return;

	DisableUndoManager DisableUndo( mpSourceForm->GetUndoManager() ); //no need to record undo during recalc

	mbRecalcInProgress = true;
	TDclControlList& Controls = mpSourceForm->mDclControls;
	assert( mhDeferredPos == NULL );
	mhDeferredPos = BeginDeferWindowPos( 2 );

	// first lets calc all the control positions for any splitter controls.
	for( TDclControlList::iterator iter = ++Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() == CtlSplitter )
			ResetControlsPos( (*iter) );
	}

	EndDeferWindowPos( mhDeferredPos );
	mhDeferredPos = BeginDeferWindowPos( Controls.size() );

	//next lets calc all the control positions for all NON-splitter controls.
	for( TDclControlList::iterator iter = ++Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() != CtlSplitter )
			ResetControlsPos( (*iter) );
	}

	EndDeferWindowPos( mhDeferredPos );
	mhDeferredPos = NULL;
	InvalidateControls();
	mbRecalcInProgress = false;
	//mpHostDlg->Invalidate(); //can't do this: it paints over the controls in Windows XP with Window Classic theme
}

void CControlPane::InvalidateControls()
{
	if( !mpSourceForm )
		return;

	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() != CtlBlockView && (*iter)->GetType() != CtlHatch )
		{
			CWnd* pWnd = (*iter)->GetWindow();
			if( pWnd )
				pWnd->Invalidate();
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
			return pDclControl->GetWndRect().TopLeft();
	}
	return CPoint( 0, 0 );	
}

void CControlPane::ResetControlsPos( TDclControlPtr pDclControl )
{
	if( !pDclControl )
		return;
	CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return;

	bool bDeferred = (mhDeferredPos != NULL);
	if( !bDeferred )
		mhDeferredPos = BeginDeferWindowPos( 1 );
	pDlgControl->OnApplyProperty( pDclControl->GetPropertyObject( Prop::UseLeftFromRight ) );
	pDlgControl->OnApplyProperty( pDclControl->GetPropertyObject( Prop::UseRightFromRight ) );
	pDlgControl->OnApplyProperty( pDclControl->GetPropertyObject( Prop::UseTopFromBottom ) );
	pDlgControl->OnApplyProperty( pDclControl->GetPropertyObject( Prop::UseBottomFromBottom ) );
	if( !bDeferred )
	{
		EndDeferWindowPos( mhDeferredPos );
		pDlgControl->GetControlWnd()->RedrawWindow();
	}

	//if( (pDclControl->GetType() == CtlComboBox || pDclControl->GetType() == CtlImageComboBox) &&
	//		((pControl->GetStyle() & CBS_DROPDOWN) != 0) )
	//	rcControl.bottom += pDclControl->GetLongProperty( Prop::DropDownHeight );
	
	//if (pDclControl->GetType() == CtlMonth)
	//	((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
}

void CControlPane::CleanUpControls() 
{	
	for (int idx = mControls.size() - 1; idx >= 0; --idx)
	{
		CWnd* pControl = mControls[idx]->GetControlWnd();
		if( pControl )
			pControl->DestroyWindow();
	}
	mControls.clear();
}

void CControlPane::ApplyZOrder()
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	HDWP hdwp = (Controls.empty()? NULL : BeginDeferWindowPos( Controls.size() ));
	for( TDclControlList::const_reverse_iterator iter = Controls.rbegin();
			 iter != Controls.rend();
			 ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if( !pDclControl->IsZOrderAllowed() )
			continue;
		TDialogControlLockedPtr pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		ZOrderFront( pDlgControl, hdwp );
	}
	if( hdwp )
		EndDeferWindowPos( hdwp );
}

void CControlPane::ApplyPosition( TDialogControlPtr pDlgControl )
{
	CWnd* pWndToMove = pDlgControl->GetControlWnd();
	//if the control is hosted inside another window, find the ancestor that is a child
	//of the host dialog and move it instead
	assert( mpHostDlg->IsChild( pWndToMove ) );
	while( (pWndToMove->GetControlUnknown() || (pWndToMove->GetStyle() & WS_CHILD)) &&
				 pWndToMove->GetParent() != mpHostDlg )
		pWndToMove = pWndToMove->GetParent();
	CRect rcNew = pDlgControl->GetWndRect();
	if( mhDeferredPos )
		DeferWindowPos( mhDeferredPos, pWndToMove->m_hWnd, NULL, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE );
	else
		pWndToMove->MoveWindow( &rcNew, TRUE );
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
	TDclControlList::const_iterator iter = Controls.end();
	while( iter != Controls.begin() )
	{
		--iter;
		CWnd* pWnd = (*iter)->GetWindow();
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
