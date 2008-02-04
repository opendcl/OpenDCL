#include "stdafx.h"
#include "ComboExCtrl.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CComboExCtrl

CComboExCtrl::CComboExCtrl()
: _TComboExBase()
, mbAutoComplete( true )
{
}

CComboExCtrl::CComboExCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID )
: _TComboExBase()
, mbAutoComplete( true )
{
	Create( pParentWnd, rectWnd, dwComboStyle, nID );
}

CComboExCtrl::~CComboExCtrl()
{
}

bool CComboExCtrl::Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID )
{
	DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_VSCROLL | dwComboStyle);
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);

	CComboBox* pComboCtrl = GetComboBoxCtrl();
	if( pComboCtrl )
		pComboCtrl->ModifyStyle( 0, CBS_HASSTRINGS | CBS_NOINTEGRALHEIGHT );

	return bSuccess;
}

void CComboExCtrl::GetRawWindowText( CString& sText )
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

BEGIN_MESSAGE_MAP(CComboExCtrl, _TComboExBase)
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboExCtrl message handlers

LRESULT CComboExCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CComboExCtrl::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
}

void CComboExCtrl::OnKillFocus( CWnd* pNewWnd )
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
		CEdit* pEditCtrl = GetEditCtrl();
		if( pEditCtrl )
			pEditCtrl->SetSel( nSelStart, nSelEnd, TRUE );
	}
}

BOOL CComboExCtrl::PreTranslateMessage( MSG* pMsg )
{
	if( pMsg->message == WM_CHAR )
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

HBRUSH CComboExCtrl::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	if( !IsWindowEnabled() )
		return NULL;

	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return NULL;
	return pColorService->CtlColor( pDC, nCtlColor );
}

void CComboExCtrl::OnEditchange()
{
	if( !mbAutoComplete ) 
		return;
	CEdit* pEditCtrl = GetEditCtrl();
	if( !pEditCtrl )
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
			pEditCtrl->SetSel( cchText, -1 );
			break;
		}
	}
}
