// FolderComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "FolderComboBox.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderComboBox

CFolderComboBox::CFolderComboBox()
: m_iconWidth( 16 )
, m_droppedHeight( 100 )
, m_droppedWidth( 50 )
{
}

CFolderComboBox::~CFolderComboBox()
{
}

bool CFolderComboBox::Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwStyle, UINT nID )
{
	dwStyle |= (WS_CHILD | WS_VISIBLE | WS_VSCROLL);
	dwStyle |= (CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED);
	bool bSuccess = (__super::Create( dwStyle, rectWnd, pParentWnd, nID ) != FALSE);

	if( bSuccess )
	{
		CRect rcTree( 0, 0, rectWnd.Width(), 120 );
		DWORD dwStyle = (WS_POPUP | WS_BORDER | WS_VSCROLL | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS);
		bSuccess = m_treeCtrl.Create( this, rcTree, dwStyle, 0 );
		SetDroppedWidth( rectWnd.Width() );
		SetDroppedHeight( 120 );
	}

	return bSuccess;
}

BOOL CFolderComboBox::GetDroppedState( )
{
	return m_treeCtrl.IsWindowVisible();
}

void CFolderComboBox::AddPath( LPCTSTR pszPath )
{
	m_treeCtrl.AddPath( pszPath );
	Invalidate();
}

CString CFolderComboBox::GetSelectedPathDisplayName()
{
	return m_treeCtrl.GetSelectedDisplayName();
}

CString CFolderComboBox::GetSelectedPath()
{	
	return m_treeCtrl.GetSelectedPath();
}

bool CFolderComboBox::SelectPath( LPCTSTR pszPath )
{	
	if( !m_treeCtrl.SelectPath( pszPath ) )
		return false;
	ResetContent();
	AddString( GetSelectedPathDisplayName() );
	SetCurSel( 0 );
	Invalidate();
	return true;
}

bool CFolderComboBox::SelectFolder( LPCTSTR pszFolderName )
{	
	ResetContent();
	AddString( pszFolderName );
	SetCurSel( 0 );
	Invalidate();
	return m_treeCtrl.SelectFolder( pszFolderName );
}

void CFolderComboBox::DisplayTree()
{
	int nDropHeight = m_treeCtrl.GetItemHeight() * m_treeCtrl.GetCount() + 3;
	CRect rc;
	GetWindowRect( &rc );
	rc.top = rc.bottom + 1;
	rc.bottom = rc.top + nDropHeight;
	rc.right = rc.left + GetDroppedWidth();
	
	m_treeCtrl.Display(rc);
}

int CFolderComboBox::GetDroppedHeight()
{
	return m_droppedHeight;
}

void CFolderComboBox::SetDroppedHeight(int height)
{
	m_droppedHeight = height;
}

int CFolderComboBox::GetDroppedWidth2()
{
	return m_droppedWidth;
}

void CFolderComboBox::SetDroppedWidth(int width)
{
	m_droppedWidth = width;
}


BEGIN_MESSAGE_MAP(CFolderComboBox, CComboBox)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_MESSAGE(CB_DIR, OnCbDir)
	ON_REGISTERED_MESSAGE(CFolderTreeCtrl::refWM_SELCHANGE(), OnSelchange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFolderComboBox message handlers

void CFolderComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	CDC dc;

	if(!dc.Attach(pDIStruct -> hDC ))
		return;

	CRect rc(pDIStruct->rcItem);
	CRect rc2;
	GetClientRect(rc2);
	CPoint p;
	p.x = 4;
	p.y = (rc2.bottom - m_iconWidth)/2 ;

	HTREEITEM item = m_treeCtrl.GetSelectedItem();
	if (item != NULL)
	{
		int idxImage = m_treeCtrl.GetItemImageIndex(item);
		if (idxImage >= 0)
		{
			CImageList* imageList = m_treeCtrl.GetImageList(TVSIL_NORMAL);
			if (imageList)
			{
				if (idxImage == 0)
					imageList->Draw(&dc, 1, p, ILD_NORMAL);
				else
					imageList->Draw(&dc, idxImage, p, ILD_NORMAL);
			}
		}
		CString sDisplayName = m_treeCtrl.GetItemDisplayName(item);
		rc.top += 1;
		rc.bottom -= 1;
		rc.left += m_iconWidth + 2;
		CSize size = dc.GetTextExtent(sDisplayName);
		rc.right = rc.left + size.cx + 4;
		if(pDIStruct -> itemState & ODS_SELECTED)
		{	dc.FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT) );
			dc.DrawFocusRect(rc);
			dc.SetTextColor((0x00FFFFFF & ~(GetSysColor(COLOR_WINDOWTEXT))));
		}
		else
			dc.SetTextColor( GetSysColor(COLOR_WINDOWTEXT) );
		rc.left += 2;
		dc.DrawText(sDisplayName, rc, DT_SINGLELINE | DT_VCENTER);
	}
	dc.Detach();
}

BOOL CFolderComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_DOWN || pMsg->wParam==VK_RIGHT))
	{	m_treeCtrl.SelectNextItem(TRUE);
		Invalidate();
		return TRUE;
	}
	else if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_UP || pMsg->wParam==VK_LEFT))
	{	m_treeCtrl.SelectNextItem(FALSE);
		Invalidate();
		return TRUE;
	}
	else if (pMsg->message==WM_SYSKEYDOWN && pMsg->wParam==VK_DOWN)
	{	
		DisplayTree();
		return TRUE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CFolderComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//UINT nItemHeight = 16;
}

void CFolderComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_treeCtrl.IsWindowVisible())
		m_treeCtrl.ShowWindow(SW_HIDE);
	else
		DisplayTree ();
}

void CFolderComboBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
}

LRESULT CFolderComboBox::OnSelchange( WPARAM wParam, LPARAM lParam )
{
	CString sPath = m_treeCtrl.GetItemPath((HTREEITEM)lParam);
	ResetContent();
	AddString( sPath );
	SetCurSel( 0 );
	SetFocus();
	Invalidate();
	return 0;
}

LRESULT CFolderComboBox::OnCbDir( WPARAM wParam, LPARAM lParam )
{
	TCHAR szPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, szPath );
	CString sPath = szPath;
	ResetContent();
	AddString( sPath );
	SetCurSel( 0 );
	SetFocus();
	Invalidate();
	m_treeCtrl.AddPath(szPath);
	return 0;
}

void CFolderComboBox::OnDestroy() 
{
	if( m_treeCtrl.m_hWnd )
		m_treeCtrl.DestroyWindow();
	__super::OnDestroy();
}
