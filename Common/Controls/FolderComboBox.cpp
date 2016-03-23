// FolderComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "FolderComboBox.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderComboBox

CFolderComboBox::CFolderComboBox()
: mbHandleComboMessages( true )
, mnIconWidth( 16 )
, mbDropLock( false )
, mfState( fNormal )
, mbPreVistaBehavior( (LOBYTE(LOWORD(GetVersion())) < 6) ) //Vista is version 6.0
{
}

CFolderComboBox::~CFolderComboBox()
{
}

bool CFolderComboBox::Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwStyle, UINT nID )
{
	dwStyle |= (WS_CHILD | WS_VISIBLE | WS_VSCROLL);
	dwStyle |= (CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST);
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);

	if( bSuccess )
	{
		OpenTheme();
		CRect rcTree( 0, 0, rectWnd.Width(), 120 );
		DWORD dwStylePopup = (WS_POPUP | WS_BORDER | WS_VSCROLL | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | TVS_HASBUTTONS);
		bSuccess = mPopupCtrl.Create( this, rcTree, dwStylePopup, 0 );
		if( bSuccess && (dwStyle & WS_DISABLED) )
			mfState.setDisabled();
	}

	return bSuccess;
}

void CFolderComboBox::OpenTheme()
{
	mWndTheme.Attach( mWndTheme.OpenThemeData( GetSafeHwnd(), VSCLASS_COMBOBOX ), GetSafeHwnd(), true );
}

HTREEITEM CFolderComboBox::FromIndex( int idxItem, HTREEITEM htiStartAt, int& ctSearched ) const
{
	if( ctSearched >= idxItem )
		return htiStartAt;
	HTREEITEM htiSearch = mPopupCtrl.GetNextItem( htiStartAt, TVGN_CHILD );
	while( htiSearch )
	{
		++ctSearched;
		HTREEITEM htiFound = FromIndex( idxItem, htiSearch, ctSearched );
		if( htiFound )
			return htiFound;
		htiSearch = mPopupCtrl.GetNextSiblingItem( htiSearch );
	}
	return NULL;
}

HTREEITEM CFolderComboBox::FromIndex( int idxItem ) const
{
	if( !mPopupCtrl.m_hWnd )
		return NULL;
	if( idxItem < 0 || idxItem >= GetCount() )
		return NULL;
	int ctSearched = 0;
	return FromIndex( idxItem, mPopupCtrl.GetRootItem(), ctSearched );
}

int CFolderComboBox::FromHandle( HTREEITEM htiItem, HTREEITEM htiStartAt, int& ctSearched ) const
{
	if( htiItem == htiStartAt )
		return ctSearched;
	HTREEITEM htiSearch = mPopupCtrl.GetNextItem( htiStartAt, TVGN_CHILD );
	while( htiSearch )
	{
		++ctSearched;
		int idx = FromHandle( htiItem, htiSearch, ctSearched );
		if( idx >= 0 )
			return idx;
		htiSearch = mPopupCtrl.GetNextSiblingItem( htiSearch );
	}
	return CB_ERR;
}

int CFolderComboBox::FromHandle( HTREEITEM htiItem ) const
{
	if( !mPopupCtrl.m_hWnd )
		return CB_ERR;
	if( !htiItem )
		return CB_ERR;
	if( htiItem == TVI_ROOT )
		return 0;
	int ctSearched = 0;
	return FromHandle( htiItem, mPopupCtrl.GetRootItem(), ctSearched );
}

int CFolderComboBox::FindString( const CString& sSearch, HTREEITEM htiStartAt, bool bExactMatch /*= true*/ ) const
{
	CString sPath = mPopupCtrl.GetItemPath( htiStartAt );
	CString sCompare = bExactMatch? sPath : sPath.Left( sSearch.GetLength() );
	if( sCompare.CompareNoCase( sSearch ) == 0 )
		return FromHandle( htiStartAt );
	HTREEITEM htiSearch = mPopupCtrl.GetNextItem( htiStartAt, TVGN_CHILD );
	while( htiSearch )
	{
		int idx = FindString( sSearch, htiSearch, bExactMatch );
		if( idx >= 0 )
			return idx;
		htiSearch = mPopupCtrl.GetNextSiblingItem( htiSearch );
	}
	return CB_ERR;
}

void CFolderComboBox::AddPath( LPCTSTR pszPath )
{
	AddString( pszPath );
}

CString CFolderComboBox::GetSelectedPath()
{
	return mPopupCtrl.GetSelectedPath();
}

bool CFolderComboBox::SelectPath( LPCTSTR pszPath )
{
	return mPopupCtrl.SelectPath( pszPath );
}

int CFolderComboBox::AddString(LPCTSTR lpszString)
{
	HTREEITEM htiNew = mPopupCtrl.AddPath( lpszString );
	mPopupCtrl.SelectItem( htiNew );
	return FromHandle( htiNew );
}

int CFolderComboBox::DeleteString(UINT nIndex)
{
	HTREEITEM htiDelete = FromIndex( nIndex );
	if( !htiDelete )
		return CB_ERR;
	mPopupCtrl.DeleteItem( htiDelete );
	return mPopupCtrl.GetCount();
}

int CFolderComboBox::Dir(UINT attr, LPCTSTR lpszWildCard)
{
	TCHAR szPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, szPath );
	CString sPath = szPath;
	if( sPath.Right(1) != _T("\\") )
		sPath += _T('\\');

	CFileFind finder;
	BOOL bResult = finder.FindFile( sPath + lpszWildCard );
	while( bResult )
	{
		bResult = finder.FindNextFile();
		if( !finder.IsDirectory() )
			continue;
		if( finder.IsDots() )
			continue;
		mPopupCtrl.AddPath(sPath + finder.GetFileName());
	}
	finder.Close();
	mPopupCtrl.SelectPath(sPath);
	return 0;
}

int CFolderComboBox::FindString(int nStartAfter, LPCTSTR lpszString) const
{
	CString sSearch = lpszString;
	HTREEITEM htiStart = FromIndex( nStartAfter + 1 );
	int idxFound = FindString( sSearch, htiStart, false );
	if( idxFound >= 0 )
		return idxFound;
	if( nStartAfter < 0 )
		return CB_ERR;
	return FindString( sSearch, mPopupCtrl.GetRootItem(), true );;
}

int CFolderComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
{
	CString sSearch = lpszFind;
	HTREEITEM htiStart = FromIndex( nIndexStart + 1 );
	int idxFound = FindString( sSearch, htiStart, true );
	if( idxFound >= 0 )
		return idxFound;
	if( nIndexStart < 0 )
		return CB_ERR;
	return FindString( sSearch, mPopupCtrl.GetRootItem(), true );;
}

int CFolderComboBox::GetCount() const
{
	return (int)mPopupCtrl.GetCount();
}

int CFolderComboBox::GetCurSel() const
{
	return FromHandle( mPopupCtrl.GetSelectedItem() );
}

BOOL CFolderComboBox::GetDroppedState()
{
	return mPopupCtrl.IsWindowVisible();
}

int CFolderComboBox::GetLBText(int nIndex, LPTSTR lpszText) const
{
	CString sPath = mPopupCtrl.GetItemPath( FromIndex( nIndex ) );
	int cchPath = sPath.GetLength();
	lstrcpy( lpszText, sPath );
	return cchPath;
}

void CFolderComboBox::GetLBText(int nIndex, CString& rString) const
{
	rString = mPopupCtrl.GetItemPath( FromIndex( nIndex ) );
}

int CFolderComboBox::GetLBTextLen(int nIndex) const
{
	return mPopupCtrl.GetItemPath( FromIndex( nIndex ) ).GetLength();
}

int CFolderComboBox::GetTopIndex() const
{
	return FromHandle( mPopupCtrl.GetFirstVisibleItem() );
}

int CFolderComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	if( nIndex < 0 )
		return AddString( lpszString );
	HTREEITEM htiInsertAt = FromIndex( nIndex );
	if( !htiInsertAt )
		return CB_ERR;
	HTREEITEM htiInsertAfter = mPopupCtrl.GetPrevVisibleItem( htiInsertAt );
	HTREEITEM htiParent = mPopupCtrl.GetParentItem( htiInsertAfter );
	HTREEITEM htiNew = mPopupCtrl.InsertItem( lpszString, htiParent, htiInsertAfter );
	mPopupCtrl.SelectItem( htiNew );
	return FromHandle( htiNew );
}

void CFolderComboBox::ResetContent()
{
	mPopupCtrl.DeleteAllItems();
	mPopupCtrl.SelectItem( NULL );
}

int CFolderComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	int idxFound = FindString( nStartAfter, lpszString );
	if( idxFound >= 0 )
		mPopupCtrl.SelectItem( FromIndex( idxFound ) );
	return idxFound;
}

int CFolderComboBox::SetCurSel(int nSelect)
{
	if( mPopupCtrl.SelectItem( FromIndex( nSelect ) ) )
		return nSelect;
	return CB_ERR;
}

int CFolderComboBox::SetTopIndex(int nIndex)
{
	if( mPopupCtrl.SelectSetFirstVisible( FromIndex( nIndex ) ) )
		return nIndex;
	return CB_ERR;
}

void CFolderComboBox::ShowDropDown(BOOL bShowIt /*= TRUE*/)
{
	if( bShowIt )
	{
		UINT ctTotal = mPopupCtrl.GetCount();
		int nDropHeight = mPopupCtrl.GetItemHeight() * ctTotal + 3;
		CRect rcCombo;
		GetWindowRect( &rcCombo );
		CPoint ptDropdown( rcCombo.left, rcCombo.bottom );
		int nDroppedWidth = GetDroppedWidth();

		BOOL bMenuDropAlignment = TRUE;
		SystemParametersInfo( 0x001B/*SPI_GETMENUDROPALIGNMENT*/, 0, &bMenuDropAlignment, 0 );
		if( !bMenuDropAlignment )
			ptDropdown.x = rcCombo.right - nDroppedWidth;

		mPopupCtrl.SetWindowPos(NULL, ptDropdown.x, ptDropdown.y, nDroppedWidth, nDropHeight, SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
		UINT ctVisible = mPopupCtrl.GetVisibleCount();
		if( ctVisible < ctTotal )
		{ // Apparently the popup requires a horizontal scroll bar, so add space for it
			nDropHeight += GetSystemMetrics(SM_CXHSCROLL);
			mPopupCtrl.SetWindowPos(NULL, 0, 0, nDroppedWidth, nDropHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		mPopupCtrl.Open();
	}
	else
		mPopupCtrl.ShowWindow(SW_HIDE);
}


BEGIN_MESSAGE_MAP(CFolderComboBox, CComboBox)
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_MESSAGE(WM_THEMECHANGED, &CFolderComboBox::OnThemechanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFolderComboBox message handlers

LRESULT CFolderComboBox::OnCbAddString( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)AddString( (LPCTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbDeleteString( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)DeleteString( (UINT)wParam );
}

LRESULT CFolderComboBox::OnCbDir( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)Dir( (UINT)wParam, (LPCTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbFindString( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)FindString( (int)wParam, (LPCTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbFindStringExact( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)FindStringExact( (int)wParam, (LPCTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbGetCount( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)GetCount();
}

LRESULT CFolderComboBox::OnCbGetCurSel( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)GetCurSel();
}

LRESULT CFolderComboBox::OnCbGetDroppedState( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)GetDroppedState();
}

LRESULT CFolderComboBox::OnCbGetLBText( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)GetLBText( (int)wParam, (LPTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbGetLBTextLen( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)GetLBTextLen( (int)wParam );
}

LRESULT CFolderComboBox::OnCbGetTopIndex( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)GetTopIndex();
}

LRESULT CFolderComboBox::OnCbInsertString( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)InsertString( (int)wParam, (LPCTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbResetContent( WPARAM wParam, LPARAM lParam )
{
	ResetContent();
	return (LRESULT)CB_OKAY;
}

LRESULT CFolderComboBox::OnCbSelectString( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)SelectString( (int)wParam, (LPCTSTR)lParam );
}

LRESULT CFolderComboBox::OnCbSetCurSel( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)SetCurSel( (int)wParam );
}

LRESULT CFolderComboBox::OnCbSetTopIndex( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)SetTopIndex( (int)wParam );
}

LRESULT CFolderComboBox::OnCbShowDropDown( WPARAM wParam, LPARAM lParam )
{
	ShowDropDown( (BOOL)wParam );
	return (LRESULT)TRUE;
}

LRESULT CFolderComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( mbHandleComboMessages )
	{
		switch( message )
		{
			case CB_ADDSTRING: return OnCbAddString( wParam, lParam );
			case CB_DELETESTRING: return OnCbDeleteString( wParam, lParam );
			case CB_DIR: return OnCbDir( wParam, lParam );
			case CB_FINDSTRING: return OnCbFindString( wParam, lParam );
			case CB_FINDSTRINGEXACT: return OnCbFindStringExact( wParam, lParam );
			case CB_GETCOUNT: return OnCbGetCount( wParam, lParam );
			case CB_GETCURSEL: return OnCbGetCurSel( wParam, lParam );
			case CB_GETDROPPEDSTATE: return OnCbGetDroppedState( wParam, lParam );
			case CB_GETLBTEXT: return OnCbGetLBText( wParam, lParam );
			case CB_GETLBTEXTLEN: return OnCbGetLBTextLen( wParam, lParam );
			case CB_GETTOPINDEX: return OnCbGetTopIndex( wParam, lParam );
			case CB_INSERTSTRING: return OnCbInsertString( wParam, lParam );
			case CB_RESETCONTENT: return OnCbResetContent( wParam, lParam );
			case CB_SELECTSTRING: return OnCbSelectString( wParam, lParam );
			case CB_SETCURSEL: return OnCbSetCurSel( wParam, lParam );
			case CB_SETTOPINDEX: return OnCbSetTopIndex( wParam, lParam );
			case CB_SHOWDROPDOWN: return OnCbShowDropDown( wParam, lParam );
		}
	}
	return __super::WindowProc(message, wParam, lParam);
}

BOOL CFolderComboBox::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_RIGHT:
		case VK_DOWN:
			ShowDropDown( TRUE );
			return TRUE;
		case VK_LEFT:
		case VK_UP:
			ShowDropDown( FALSE );
			return TRUE;
		}
		break;
	case WM_SYSKEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_RIGHT:
		case VK_DOWN:
			ShowDropDown( TRUE );
			return TRUE;
		case VK_LEFT:
		case VK_UP:
			ShowDropDown( FALSE );
			return TRUE;
		}
		break;
	}
	return __super::PreTranslateMessage(pMsg);
}

BOOL CFolderComboBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = __super::OnCommand(wParam, lParam);
	if( lParam ) //child control notification?
	{
		switch( HIWORD(wParam) )
		{
		case CBN_CLOSEUP:
			mfState.setClosed();
			mbDropLock = true;
			PostMessage( WM_CANCELMODE );
			break;
		case CBN_SELCHANGE:
			Invalidate();
			break;
		case CBN_DROPDOWN:
			mfState.setOpen();
			Invalidate();
			break;
		case CBN_SELENDOK:
		case CBN_SELENDCANCEL:
			break;
		}
	}
	return bResult;
}

void CFolderComboBox::OnCancelMode()
{
	mbDropLock = false;
	//mUIState = kNormal;
	mPopupCtrl.EnableTracking( true );
	Invalidate();
}

void CFolderComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	mPopupCtrl.EnableTracking( false );
	mfState.setPressed();
	SetFocus();
	BOOL bDropped = GetDroppedState();
	if( bDropped )
		ShowDropDown(FALSE);
	else if( !mbDropLock )
		ShowDropDown();
}

void CFolderComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	mfState.setUnpressed();
	mPopupCtrl.EnableTracking( true );
	Invalidate();
}

void CFolderComboBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CFolderComboBox::OnDestroy()
{
	if( mPopupCtrl.m_hWnd )
		mPopupCtrl.DestroyWindow();
	__super::OnDestroy();
}

void CFolderComboBox::OnKillFocus(CWnd* pNewWnd)
{
	if( pNewWnd == &mPopupCtrl )
		return; //skip default processing
	__super::OnKillFocus(pNewWnd);
}

void CFolderComboBox::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);
	Invalidate();
}

void CFolderComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !mfState.isHot() && !mfState.isOpen() )
	{
		mfState.setHot();
		Invalidate();
		if( mbPreVistaBehavior )
		{
			TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
			_TrackMouseEvent( &tm );
		}
	}
	__super::OnMouseMove(nFlags, point);
}

void CFolderComboBox::OnMouseLeave()
{
	if( mfState.isHot() )
	{
		mfState.setCold();
		Invalidate();
	}
	__if_exists(CWnd::OnMouseLeave)
	{
	__super::OnMouseLeave();
	}
}

void CFolderComboBox::OnEnable(BOOL bEnable)
{
	__super::OnEnable(bEnable);
	if( bEnable )
		mfState.setEnabled();
	else
		mfState.setDisabled();
}

LRESULT CFolderComboBox::OnThemechanged(WPARAM wParam, LPARAM lParam)
{
	OpenTheme();
	return 0;
}

void CFolderComboBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.SaveDC();

	CRect rcClient;
	GetClientRect( &rcClient );
	int nButtonWidth = GetSystemMetrics(SM_CXHSCROLL);
	int nNCBorder = mbPreVistaBehavior? 1 : 0;
	int nBorder = 2;
	int nGap = 2;
	CRect rcButton( rcClient.right - nNCBorder - nButtonWidth, rcClient.top + nNCBorder, rcClient.right - nNCBorder, rcClient.bottom - nNCBorder );
	CRect rcItem( nBorder, nBorder, rcButton.left - nBorder, rcClient.bottom - nBorder );
	CRect rcIcon( rcItem.left + nGap, (rcClient.bottom - mnIconWidth) / 2, rcItem.left + nGap + mnIconWidth, ((rcClient.bottom - mnIconWidth) / 2) + mnIconWidth );
	CRect rcLabel( rcIcon.right + nGap, rcItem.top, rcItem.right, rcItem.bottom );
	rcLabel.DeflateRect( nGap, nGap, nGap, nGap );

	int idxIcon = -1;
	CString sLabel;
	HTREEITEM htiSelected = mPopupCtrl.GetSelectedItem();
	if( htiSelected != NULL )
	{
		idxIcon = mPopupCtrl.GetItemImageIndex( htiSelected );
		sLabel = mPopupCtrl.GetItemDisplayName( htiSelected );
	}

	CWnd* pFocusWnd = GetFocus();
	bool bFocused = (pFocusWnd == this);
	bool bHighlighted = false;

	DcTheme Theme( GetFolderComboBoxTheme(), dc.GetSafeHdc() );
	if( Theme )
	{
		int nTBState = (mfState.isDisabled()? CBTBS_DISABLED : (bFocused? CBTBS_FOCUSED : (mfState.isHot()? CBTBS_HOT : CBTBS_NORMAL)));
		Theme.DrawThemeBackground( CP_BACKGROUND, nTBState, &rcClient, NULL );
		int nBState = (mfState.isDisabled()? CBB_DISABLED : (bFocused? CBB_FOCUSED : (mfState.isHot()? CBB_HOT : CBB_NORMAL)));
		Theme.DrawThemeBackground( CP_BORDER, nBState, &rcClient, NULL );
		int nROState = (mfState.isDisabled()? CBRO_DISABLED : (mfState.isOpen()? CBRO_PRESSED : (mfState.isHot()? CBRO_HOT : CBRO_NORMAL)));
		Theme.DrawThemeBackground( CP_READONLY, nROState, &rcClient, NULL );

		if( !mbPreVistaBehavior )
		{
			int nDBRState = (mfState.isDisabled()? CBXSR_DISABLED : (mfState.isOpen()? CBXSR_PRESSED : (mfState.isHot()? CBXSR_HOT : CBXSR_NORMAL)));
			Theme.DrawThemeBackground( CP_DROPDOWNBUTTONRIGHT, nDBRState, &rcButton, NULL );
		}
		else
		{
			int nDBState = (mfState.isDisabled()? CBXS_DISABLED : (mfState.isPressed()? CBXS_PRESSED : (mfState.isHot()? CBXS_HOT : CBXS_NORMAL)));
			Theme.DrawThemeBackground( CP_DROPDOWNBUTTON, nDBState, &rcButton, NULL );
		}
		if( !sLabel.IsEmpty() || idxIcon >= 0 )
		{
			CRect rcBackground = rcItem;
			COLORREF clrBackground;
			COLORREF clrForeground;
			if( bFocused && mbPreVistaBehavior )
			{
				bHighlighted = true;
				clrBackground = GetSysColor( COLOR_HIGHLIGHT );
				clrForeground = GetSysColor( COLOR_HIGHLIGHTTEXT );
				rcBackground.DeflateRect(1, 1, 0, 1);
			}
			else
			{
				clrBackground = GetSysColor( mfState.isDisabled()? COLOR_3DFACE : COLOR_WINDOW );
				clrForeground = GetSysColor( mfState.isDisabled()? COLOR_GRAYTEXT : COLOR_BTNTEXT );
				rcBackground.DeflateRect(0, 0, -1, 0);
			}
			if( mbPreVistaBehavior )
				dc.FillSolidRect( &rcBackground, clrBackground );
			dc.SetTextColor( clrForeground );
			dc.SetBkMode(TRANSPARENT);
			dc.SelectObject(GetFont());
			dc.DrawText( sLabel, &rcLabel, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );
			//	int nState = (mfState.isDisabled()? CBXS_DISABLED : (mfState.isPressed()? CBXS_PRESSED : (mfState.isHot()? CBXS_HOT : CBXS_NORMAL)));
			//	Theme.DrawThemeText( 0, nState, sLabel, sLabel.GetLength(), DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER, 0, &rcLabel );
		}
	}
	else
	{
		rcButton.DeflateRect(-1, 1, 1, 1);

		dc.FillSolidRect( &rcClient, GetSysColor( mfState.isDisabled()? COLOR_3DFACE : COLOR_WINDOW ) );
		if( bFocused )
		{
			bHighlighted = true;
			CRect rcHighlight = rcItem;
			rcHighlight.DeflateRect(1, 1, mbPreVistaBehavior? 1 : 0, 1);
			dc.FillSolidRect( &rcHighlight, GetSysColor( COLOR_HIGHLIGHT ) );
		}

		UINT nBtnState = 0;
		if( mfState.isDisabled() )
			nBtnState |= DFCS_INACTIVE;
		if( mfState.isPressed() )
			nBtnState |= DFCS_PUSHED;
		//if( mfState.isHot() )
		//	nBtnState |= 0x1000/*DFCS_HOT*/;
		dc.DrawEdge( &rcClient, EDGE_SUNKEN, BF_RECT );
		dc.DrawFrameControl( &rcButton, DFC_SCROLL, DFCS_SCROLLCOMBOBOX | nBtnState );
		if( !sLabel.IsEmpty() )
		{
			COLORREF clrForeground;
			if( bHighlighted )
				clrForeground = GetSysColor( COLOR_HIGHLIGHTTEXT );
			else
				clrForeground = GetSysColor( mfState.isDisabled()? COLOR_GRAYTEXT : COLOR_BTNTEXT );
			dc.SetTextColor( clrForeground );
			dc.SetBkMode(TRANSPARENT);
			dc.SelectObject(GetFont());
			//dc.ExtTextOut( rcLabel.left, rcLabel.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, sLabel, (UINT)sLabel.GetLength(), NULL );
			dc.DrawText( sLabel, &rcLabel, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );
		}
	}

	if( idxIcon >= 0 )
	{
		CImageList* pImageList = mPopupCtrl.GetImageList(TVSIL_NORMAL);
		if( pImageList )
			pImageList->Draw( &dc, idxIcon, CPoint( rcIcon.left, rcIcon.top ), ILD_TRANSPARENT | (mfState.isDisabled()? ILD_BLEND50 : (bHighlighted? ILD_SELECTED : 0)) );
	}

	if( bFocused && !bHighlighted )
		dc.DrawFocusRect( rcItem );

	dc.RestoreDC(-1);
}
