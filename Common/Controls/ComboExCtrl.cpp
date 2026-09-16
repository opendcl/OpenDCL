#include "stdafx.h"
#include "ComboExCtrl.h"
#include "InputFilter.h"
#include "HostThemeHelper.h"
#include "ColorService.h"

namespace
{
const int knMaxInnerHooks = 32;
struct InnerHook
{
	HWND hwnd;
	CComboExCtrl* pThis;
	WNDPROC pfnOld;
};
InnerHook gInnerHooks[knMaxInnerHooks];

UINT CtlColorTypeFromMsg( UINT message )
{
	if( message == WM_CTLCOLORLISTBOX )
		return CTLCOLOR_LISTBOX;
	if( message == WM_CTLCOLORSTATIC )
		return CTLCOLOR_STATIC;
	return CTLCOLOR_EDIT;
}

WNDPROC SetWndProc( HWND hwnd, WNDPROC pfn )
{
#ifdef _WIN64
	return (WNDPROC)::SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)pfn );
#else
	return (WNDPROC)::SetWindowLong( hwnd, GWL_WNDPROC, (LONG)(LONG_PTR)pfn );
#endif
}

InnerHook* FindInnerHook( HWND hwnd )
{
	for( int i = 0; i < knMaxInnerHooks; ++i )
	{
		if( gInnerHooks[i].hwnd == hwnd )
			return &gInnerHooks[i];
	}
	return NULL;
}

InnerHook* AllocInnerHook()
{
	for( int i = 0; i < knMaxInnerHooks; ++i )
	{
		if( !gInnerHooks[i].hwnd )
			return &gInnerHooks[i];
	}
	return NULL;
}
}


/////////////////////////////////////////////////////////////////////////////
// CComboExCtrl

CComboExCtrl::CComboExCtrl()
: _TComboExBase()
, mColorService( -19L, -6L )
, mbAutoComplete( true )
, mhwndInnerCombo( NULL )
, mpfnInnerComboProc( NULL )
{
}

CComboExCtrl::CComboExCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID )
: _TComboExBase()
, mColorService( -19L, -6L )
, mbAutoComplete( true )
, mhwndInnerCombo( NULL )
, mpfnInnerComboProc( NULL )
{
	Create( pParentWnd, rectWnd, dwComboStyle, nID );
}

CComboExCtrl::~CComboExCtrl()
{
}

bool CComboExCtrl::Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID )
{
	DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL | dwComboStyle);
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	CComboBox* pComboCtrl = GetComboBoxCtrl();
	if( pComboCtrl )
		pComboCtrl->ModifyStyle( 0, CBS_HASSTRINGS | CBS_NOINTEGRALHEIGHT );
	if( bSuccess )
		SubclassInnerCombo();

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
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CComboExCtrl::OnEditchange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboExCtrl message handlers

LRESULT CComboExCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_DESTROY )
		UnsubclassInnerCombo();
	if( message == WM_CTLCOLOREDIT || message == WM_CTLCOLORLISTBOX || message == WM_CTLCOLORSTATIC )
	{
		CDC dc;
		dc.Attach( (HDC)wParam );
		HBRUSH hbr = CtlColor( &dc, CtlColorTypeFromMsg( message ) );
		dc.Detach();
		if( hbr )
			return (LRESULT)hbr;
	}
	if( message == WM_DRAWITEM )
	{
		LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
		if( lpDrawItem && DrawHostMappedItem( lpDrawItem ) )
			return TRUE;
	}
	if( message == WM_PAINT || message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		LRESULT lResult = __super::WindowProc( message, wParam, lParam );
		PaintHostMappedClosedFace( (message == WM_PAINT)? NULL : (HDC)wParam );
		return lResult;
	}
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
	__super::OnKillFocus( pNewWnd );
}

BOOL CComboExCtrl::PreTranslateMessage( MSG* pMsg )
{
	if( pMsg->message == WM_CHAR )
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
	return __super::PreTranslateMessage( pMsg );
}

HBRUSH CComboExCtrl::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	if( !IsWindowEnabled() )
		return NULL;

	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return NULL;
	pDC->SetTextColor( pColorService->GetForegroundColor() );
	pDC->SetBkColor( pColorService->GetBackgroundColor() );
	pDC->SetBkMode( OPAQUE );
	return pColorService->GetBackgroundBrush();
}


bool CComboExCtrl::DrawHostMappedItem( LPDRAWITEMSTRUCT lpDrawItem )
{
	if( !lpDrawItem || !CHostThemeHelper::HostMaps() )
		return false;
	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return false;

	const bool bEdit = ((lpDrawItem->itemState & ODS_COMBOBOXEDIT) != 0);
	const bool bSelected = ((lpDrawItem->itemState & ODS_SELECTED) != 0) && !bEdit;
	const COLORREF crBk = bSelected? OdclSysColor( COLOR_HIGHLIGHT ) : pColorService->GetBackgroundColor();
	const COLORREF crFg = bSelected? OdclSysColor( COLOR_HIGHLIGHTTEXT ) : pColorService->GetForegroundColor();

	HBRUSH hbr = ::CreateSolidBrush( crBk );
	::FillRect( lpDrawItem->hDC, &lpDrawItem->rcItem, hbr );
	::DeleteObject( hbr );

	// Editable ComboBoxEx already paints the Edit via CtlColor; only fill the item chrome.
	if( bEdit && GetEditCtrl() )
		return true;

	CString sText;
	int nItem = (int)lpDrawItem->itemID;
	if( nItem < 0 )
		nItem = GetCurSel();
	if( nItem >= 0 )
		GetLBText( nItem, sText );
	else
		GetWindowText( sText );

	CRect rcText( lpDrawItem->rcItem );
	CImageList* pImageList = GetImageList();
	if( pImageList && pImageList->GetSafeHandle() )
	{
		COMBOBOXEXITEM cbei = { 0 };
		cbei.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT;
		cbei.iItem = nItem;
		if( nItem >= 0 && GetItem( &cbei ) )
		{
			rcText.left += cbei.iIndent * ::GetSystemMetrics( SM_CXSMICON );
			const int nImage = (bSelected && cbei.iSelectedImage >= 0)? cbei.iSelectedImage : cbei.iImage;
			if( nImage >= 0 )
			{
				IMAGEINFO ii = { 0 };
				if( pImageList->GetImageInfo( nImage, &ii ) )
				{
					const int cx = ii.rcImage.right - ii.rcImage.left;
					const int cy = ii.rcImage.bottom - ii.rcImage.top;
					CPoint pt( rcText.left, rcText.top + (rcText.Height() - cy) / 2 );
					pImageList->Draw( CDC::FromHandle( lpDrawItem->hDC ), nImage, pt, ILD_TRANSPARENT );
					rcText.left += cx + 4;
				}
			}
		}
	}

	CFont* pFont = GetFont();
	HGDIOBJ hOldFont = pFont? ::SelectObject( lpDrawItem->hDC, pFont->GetSafeHandle() ) : NULL;
	::SetBkMode( lpDrawItem->hDC, TRANSPARENT );
	::SetTextColor( lpDrawItem->hDC, crFg );
	::DrawText( lpDrawItem->hDC, sText, sText.GetLength(), &rcText, HostMappedTextFormat() );
	if( hOldFont )
		::SelectObject( lpDrawItem->hDC, hOldFont );

	if( !bEdit && (lpDrawItem->itemState & ODS_FOCUS) )
		::DrawFocusRect( lpDrawItem->hDC, &lpDrawItem->rcItem );
	return true;
}

UINT CComboExCtrl::HostMappedTextFormat() const
{
	UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS;
	const DWORD dwEx = GetExStyle();
	if( dwEx & WS_EX_RTLREADING )
		nFormat |= DT_RTLREADING;
	if( dwEx & WS_EX_RIGHT )
		nFormat |= DT_RIGHT;
	return nFormat;
}

void CComboExCtrl::PaintHostMappedClosedFace( HDC hdc )
{
	if( !CHostThemeHelper::HostMaps() )
		return;
	CAcadColorService* pColorService = GetColorService();
	if( !pColorService || !m_hWnd )
		return;

	CComboBox* pInner = GetComboBoxCtrl();
	HWND hwndCombo = (pInner && pInner->m_hWnd)? pInner->m_hWnd : m_hWnd;

	HDC hdcPaint = hdc;
	if( !hdcPaint )
		hdcPaint = ::GetDC( hwndCombo );

	COMBOBOXINFO cbi = {};
	cbi.cbSize = sizeof( cbi );
	if( ::GetComboBoxInfo( hwndCombo, &cbi ) )
	{
		CRect rcBtn( cbi.rcButton );
		if( hdc && hwndCombo != m_hWnd )
			::MapWindowPoints( hwndCombo, m_hWnd, (LPPOINT)&rcBtn, 2 );
		CHostThemeHelper::PaintComboDropButton( hdcPaint, rcBtn, IsWindowEnabled() != FALSE );
	}

	if( (GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST )
	{
		CRect rc;
		::GetClientRect( hwndCombo, &rc );
		const int cxBtn = ::GetSystemMetrics( SM_CXVSCROLL );
		if( ::GetWindowLong( hwndCombo, GWL_EXSTYLE ) & 0x00400000 ) // WS_EX_LAYOUTRTL
			rc.left += cxBtn;
		else
			rc.right -= cxBtn;
		if( hdc && hwndCombo != m_hWnd )
			::MapWindowPoints( hwndCombo, m_hWnd, (LPPOINT)&rc, 2 );

		HBRUSH hbr = ::CreateSolidBrush( pColorService->GetBackgroundColor() );
		::FillRect( hdcPaint, &rc, hbr );
		::DeleteObject( hbr );

		CString sText;
		int nSel = GetCurSel();
		if( nSel >= 0 )
			GetLBText( nSel, sText );
		else
			GetWindowText( sText );

		CFont* pFont = GetFont();
		HGDIOBJ hOldFont = pFont? ::SelectObject( hdcPaint, pFont->GetSafeHandle() ) : NULL;
		::SetBkMode( hdcPaint, TRANSPARENT );
		::SetTextColor( hdcPaint, pColorService->GetForegroundColor() );
		::DrawText( hdcPaint, sText, sText.GetLength(), &rc, HostMappedTextFormat() );
		if( hOldFont )
			::SelectObject( hdcPaint, hOldFont );
	}

	if( !hdc )
		::ReleaseDC( hwndCombo, hdcPaint );
}

LRESULT CALLBACK CComboExCtrl::InnerComboSubclass( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	InnerHook* pHook = FindInnerHook( hwnd );
	if( !pHook || !pHook->pfnOld || !pHook->pThis )
		return ::DefWindowProc( hwnd, message, wParam, lParam );

	if( message == WM_CTLCOLOREDIT || message == WM_CTLCOLORLISTBOX || message == WM_CTLCOLORSTATIC )
	{
		CDC dc;
		dc.Attach( (HDC)wParam );
		HBRUSH hbr = pHook->pThis->CtlColor( &dc, CtlColorTypeFromMsg( message ) );
		dc.Detach();
		if( hbr )
			return (LRESULT)hbr;
	}

	WNDPROC pfnOld = pHook->pfnOld;
	if( message == WM_NCDESTROY )
		pHook->pThis->UnsubclassInnerCombo();
	return ::CallWindowProc( pfnOld, hwnd, message, wParam, lParam );
}

void CComboExCtrl::SubclassInnerCombo()
{
	CComboBox* pInner = GetComboBoxCtrl();
	if( !pInner || !pInner->m_hWnd )
		return;
	if( mhwndInnerCombo == pInner->m_hWnd )
		return;
	UnsubclassInnerCombo();
	InnerHook* pHook = AllocInnerHook();
	if( !pHook )
		return;
	mhwndInnerCombo = pInner->m_hWnd;
	mpfnInnerComboProc = SetWndProc( mhwndInnerCombo, InnerComboSubclass );
	pHook->hwnd = mhwndInnerCombo;
	pHook->pThis = this;
	pHook->pfnOld = mpfnInnerComboProc;
}

void CComboExCtrl::UnsubclassInnerCombo()
{
	if( !mhwndInnerCombo )
		return;
	InnerHook* pHook = FindInnerHook( mhwndInnerCombo );
	WNDPROC pfnOld = pHook? pHook->pfnOld : mpfnInnerComboProc;
	if( pfnOld && ::IsWindow( mhwndInnerCombo ) )
		SetWndProc( mhwndInnerCombo, pfnOld );
	if( pHook )
	{
		pHook->hwnd = NULL;
		pHook->pThis = NULL;
		pHook->pfnOld = NULL;
	}
	mhwndInnerCombo = NULL;
	mpfnInnerComboProc = NULL;
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
