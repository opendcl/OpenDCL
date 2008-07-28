#include "stdafx.h"
#include "ComboCtrl.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CComboCtrl

CComboCtrl::CComboCtrl()
: _TComboBase()
, mbAutoComplete( true )
, mColorService( (long)-19, (long)-6 )
{
}

CComboCtrl::CComboCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID )
: _TComboBase()
, mbAutoComplete( true )
, mColorService( (long)-19, (long)-6 )
{
	Create( pParentWnd, rectWnd, dwComboStyle, nID );
}

CComboCtrl::~CComboCtrl()
{
}

bool CComboCtrl::Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID )
{
	DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_HASSTRINGS | dwComboStyle);
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);

	return bSuccess;
}

void CComboCtrl::GetRawWindowText( CString& sText )
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

BEGIN_MESSAGE_MAP(CComboCtrl, _TComboBase)
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboCtrl message handlers

LRESULT CComboCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CComboCtrl::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
}

void CComboCtrl::OnKillFocus( CWnd* pNewWnd )
{
	__super::OnKillFocus( pNewWnd );
	CInputFilter* pFilter = GetInputFilter();
	if( pFilter )
	{
		DWORD dwCurSel = GetEditSel(); // Currently selected range
		int nSelStart = LOWORD(dwCurSel);
		int nSelEnd   = HIWORD(dwCurSel);
		CString sText;
		GetRawWindowText( sText );
		if( !pFilter->OnValidateInput( sText ) )
		{
			sText = pFilter->GetLastValidInput();
			nSelStart = -1;
			nSelEnd = -1;
		}
		SetWindowText( sText ); //validate and set the input
		SetEditSel( nSelStart, nSelEnd );
	}
}

BOOL CComboCtrl::PreTranslateMessage( MSG* pMsg )
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
			if( ch >= _T(' ') || ch == _T('\n') || ch == _T('\r') ) //ignore control characters (except CR)
			{
				if( !pFilter->FilterInput( ch ) )
				{
					pFilter->OnBadInput();
					return TRUE; //discard it
				}
				pMsg->wParam = (WPARAM)ch;
			}
		}
	}
	else if( pMsg->message == WM_KEYDOWN )
	{
		mbAutoComplete = true;
		WPARAM nVirtKey = pMsg->wParam;
		if( nVirtKey == VK_DELETE || nVirtKey == VK_BACK )
			mbAutoComplete = false;
	}
	return __super::PreTranslateMessage( pMsg );
}

HBRUSH CComboCtrl::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	if( !IsWindowEnabled() )
		return NULL;

	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return NULL;
	return pColorService->CtlColor( pDC, nCtlColor );	
}

void CComboCtrl::OnEditchange()
{
	if( !mbAutoComplete ) 
		return;
	CString sText;
	GetWindowText( sText );
	if( GetCurSel() >= 0 )
	{
		CString sLBText;
		GetLBText( GetCurSel(), sLBText );
		if( sLBText == sText )
			return; //no-op
	}

	int cchText = sText.GetLength();
	DWORD dwCurSel = GetEditSel(); // Currently selected range
	WORD dStart = LOWORD(dwCurSel);
	WORD dEnd   = HIWORD(dwCurSel);

	for( int idx = 0; idx < GetCount(); ++idx )
	{
		CString sLBText;
		GetLBText( idx, sLBText );
		if( sLBText.Left( cchText ).CompareNoCase( sText ) == 0 )
		{
			mbAutoComplete = false;
			SetCurSel( idx );
			mbAutoComplete = true;
			SetEditSel( cchText, -1 );
			break;
		}
	}
}
