#include "stdafx.h"
#include "EditEx.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CEditEx

CEditEx::CEditEx()
: _TEditBase()
{
}

CEditEx::CEditEx( CWnd* pParentWnd, const CRect& rectWnd, UINT nID )
: _TEditBase()
{
	Create( pParentWnd, rectWnd, nID );
}

CEditEx::~CEditEx()
{
}

bool CEditEx::Create( CWnd* pParentWnd, const CRect& rectWnd, UINT nID )
{
	DWORD dwStyle = (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT);
	CInputFilter* pFilter = GetInputFilter();
	if( pFilter && pFilter->GetFilter().FindOneOf( _T("\r\n") ) >= 0 )
		dwStyle |= (ES_MULTILINE | ES_WANTRETURN);
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);

	return bSuccess;
}

BEGIN_MESSAGE_MAP(CEditEx, _TEditBase)
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers

void CEditEx::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
}

void CEditEx::OnKillFocus( CWnd* pNewWnd )
{
	CInputFilter* pFilter = GetInputFilter();
	if( pFilter )
	{
		CString sText;
		GetWindowText( sText );
		if( !pFilter->OnValidateInput( sText ) )
			sText = pFilter->OnBadInput();
		SetWindowText( sText );
	}
	__super::OnKillFocus( pNewWnd );
}

BOOL CEditEx::PreTranslateMessage( MSG* pMsg )
{
	if( pMsg->message == WM_SETTEXT )
	{
		CInputFilter* pFilter = GetInputFilter();
		if( pFilter )
		{
			CString sText = (LPCTSTR)pMsg->lParam;
			if( !pFilter->OnValidateInput( sText ) )
				return TRUE;
			MSG msg = { pMsg->hwnd, WM_SETTEXT, pMsg->wParam, (LPARAM)(LPCTSTR)sText, pMsg->time, pMsg->pt.x, pMsg->pt.y };
			return __super::PreTranslateMessage( &msg );
		}
	}
	else if( pMsg->message == WM_CHAR )
	{
		CInputFilter* pFilter = GetInputFilter();
		if( pFilter )
		{
			TCHAR ch = (TCHAR)pMsg->wParam;
			if( !pFilter->FilterInput( ch ) )
			{
				pFilter->OnBadInput();
				return TRUE; //discard it
			}
			pMsg->wParam = (WPARAM)ch;
		}
	}
	return __super::PreTranslateMessage( pMsg );
}

HBRUSH CEditEx::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	if( !IsWindowEnabled() )
		return NULL;

	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return NULL;

	pDC->SetBkColor( pColorService->GetBackgroundColor() );
	pDC->SetTextColor( pColorService->GetForegroundColor() );
	return pColorService->GetBackgroundBrush();
}
