// StudioControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "StudioControlPane.h"
#include "StudioDialogControl.h"
#include "AxContainerCtrl.h"
#include "ControlManager.h"


/////////////////////////////////////////////////////////////////////////////
// CStudioControlPane

CStudioControlPane::CStudioControlPane()
: CControlPane()
{
	TraceFmt( _T("> CStudioControlPane::CStudioControlPane() [this = %p]\r\n"), this );
}

CStudioControlPane::CStudioControlPane( TDclFormPtr pSourceForm, CWnd* pHostDlg )
: CControlPane( pSourceForm, pHostDlg )
{
	TraceFmt( _T("> CStudioControlPane::CStudioControlPane(%s [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)(pSourceForm? pSourceForm->GetKeyPath() : _T("<null>")), (CDclFormObject*)pSourceForm,
						(LPCTSTR)CString(pHostDlg->GetRuntimeClass()->m_lpszClassName),
						pHostDlg->m_hWnd, this );
}

CStudioControlPane::~CStudioControlPane()
{
	TraceFmt( _T("< CStudioControlPane::~CStudioControlPane() [this: %p]\r\n"), this );
}

void CStudioControlPane::SetSourceForm( TDclFormPtr pSourceForm )
{
	mpSourceForm = pSourceForm;
	mpProject = pSourceForm->GetProject();
}

TDialogControlPtr CStudioControlPane::CreateNewDialogControl( TDclControlPtr pTemplate,
																															UINT nID )
{
	bool bRecalcInProgress = mbRecalcInProgress;
	mbRecalcInProgress = true; //to prevent recalc
	TDialogControlPtr pNewCtrl = CStudioDialogControl::Create( pTemplate, this, nID );
	mbRecalcInProgress = bRecalcInProgress;
	return pNewCtrl;
}

void CStudioControlPane::ApplyPosition( TDialogControlPtr pDlgControl )
{
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( !pManager )
		__super::ApplyPosition( pDlgControl );
	else
		pManager->OnControlPositionChanged();
}

void CStudioControlPane::ZOrderFront( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
{
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( pManager )
	{
		if( hDeferred )
			DeferWindowPos( hDeferred, pManager->m_hWnd, HWND_TOP, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
		else
			pManager->SetWindowPos( &CWnd::wndTop, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
	}
	else
		__super::ZOrderFront( pDlgControl, hDeferred );
}

void CStudioControlPane::ZOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred /*= NULL*/ )
{
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( pManager )
	{
		if( hDeferred )
			DeferWindowPos( hDeferred, pManager->m_hWnd, HWND_BOTTOM, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
		else
			pManager->SetWindowPos( &CWnd::wndBottom, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS );
	}
	else
		__super::ZOrderBack( pDlgControl, hDeferred );
}
