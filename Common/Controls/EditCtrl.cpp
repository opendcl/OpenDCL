#include "stdafx.h"
#include "EditCtrl.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CEditCtrl

CEditCtrl::CEditCtrl()
: _TEditBase()
, mColorService( RGB(0,0,0), RGB(255,255,255) )
{
}

CEditCtrl::CEditCtrl( CWnd* pParentWnd, const CRect& rectWnd, UINT nID,
											DWORD dwStyle /*= GetDefaultWndStyle()*/, CInputFilter* pFilter /*= NULL*/ )
: _TEditBase()
, mColorService( RGB(0,0,0), RGB(255,255,255) )
{
	Create( pParentWnd, rectWnd, nID, dwStyle, pFilter );
}

CEditCtrl::~CEditCtrl()
{
}

bool CEditCtrl::Create( CWnd* pParentWnd, const CRect& rectWnd, UINT nID,
												DWORD dwStyle /*= GetDefaultWndStyle()*/, CInputFilter* pFilter /*= NULL*/ )
{
	bool bPassword = false;
	if( pFilter )
	{
		if( pFilter->GetFilter().FindOneOf( _T("\r\n") ) >= 0 )
			dwStyle |= (ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN);
		else
			dwStyle |= (ES_AUTOHSCROLL);
		bPassword = pFilter->IsMaskedInput();
		if( bPassword )
			dwStyle |= ES_PASSWORD;
	}
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);

	if( bSuccess && bPassword )
		SetPasswordChar( _T('*') );

	return bSuccess;
}

void CEditCtrl::GetRawWindowText( CString& sText )
{
	int cchResult = __super::WindowProc( WM_GETTEXTLENGTH, 0, 0 );
	if( cchResult > 0 )
	{
		cchResult = __super::WindowProc( WM_GETTEXT, (WPARAM)(cchResult + 1), (LPARAM)sText.GetBuffer( cchResult + 1 ) );
		sText.ReleaseBuffer( cchResult );
	}
	else
		sText.Empty();
}


BEGIN_MESSAGE_MAP(CEditCtrl, _TEditBase)
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditCtrl message handlers

LRESULT CEditCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_SETTEXT )
	{
		CInputFilter* pFilter = GetInputFilter();
		if( pFilter )
		{
			CString sText = (LPCTSTR)lParam;
			if( !pFilter->OnValidateInput( sText ) )
				return FALSE;
			return __super::WindowProc( message, wParam, (LPARAM)(LPCTSTR)sText );
		}
	}
	else if( message == WM_GETTEXT )
	{
		CInputFilter* pFilter = GetInputFilter();
		if( pFilter )
		{
			CString sText;
			GetRawWindowText( sText );
			pFilter->ConvertForDisplay( sText );
			int cchBuf = (int)wParam;
			lstrcpyn( (LPTSTR)lParam, (LPCTSTR)sText, cchBuf );
			int cchText = sText.GetLength();
			if( cchText >= cchBuf )
				return cchBuf;
			return cchText;
		}
	}
	else if( message == WM_GETTEXTLENGTH )
	{
		CInputFilter* pFilter = GetInputFilter();
		if( pFilter )
		{
			CString sText;
			GetRawWindowText( sText );
			pFilter->ConvertForDisplay( sText );
			return sText.GetLength();
		}
	}
	return __super::WindowProc(message, wParam, lParam);
}

void CEditCtrl::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
}

void CEditCtrl::OnKillFocus( CWnd* pNewWnd )
{
	__super::OnKillFocus( pNewWnd );
	CInputFilter* pFilter = GetInputFilter();
	if( pFilter )
	{
		CString sText;
		GetRawWindowText( sText );
		if( !pFilter->OnValidateInput( sText ) )
			sText = pFilter->GetLastValidInput();
		SetWindowText( sText ); //validate and set the input
	}
}

BOOL CEditCtrl::PreTranslateMessage( MSG* pMsg )
{
	if( pMsg->message == WM_CHAR && (TCHAR)pMsg->wParam >= _T(' ') )
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

HBRUSH CEditCtrl::CtlColor( CDC* pDC, UINT nCtlColor ) 
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
