// TreeCtrlFolder.cpp : implementation file
//

#include "stdafx.h"
#include "FolderTreeCtrl.h"
#include "Resource.h"
#include "Workspace.h"
#include "FolderComboBox.h"

// multi-monitor stubs to fake support for multiple monitors in Win95 and WinNT
//#define COMPILE_MULTIMON_STUBS //stubs are already defined by PPToolTip.cpp
#include "MultiMon.h"

#undef SubclassWindow

#define FOLDER_ICON			0
#define FOLDER_EXPAND_ICON	1
#define NETWORK_ICON		2
#define DESKTOP_ICON		3
#define MYCOMPUTER_ICON		4
#define FLOPPY_ICON			5
#define HARDDRIVE_ICON		6
#define CDROM_ICON			7
#define CONTROLPANEL_ICON	8
#define MY_DOCUMENT_ICON	9

class CFolder  
{
public:
	CFolder() : mnImg( -1 ), mnImgExpanded( -1 ) {}
	CFolder( LPCTSTR pszDisplayName, LPCTSTR pszPath, int nImg = 0, int nImgExpanded = 1 )
		: mnImg( nImg )
		, mnImgExpanded( nImgExpanded )
		, msDisplayName( pszDisplayName )
		, msPath( pszPath )
		{}
	virtual ~CFolder() {}

public:
	int mnImg;
	int mnImgExpanded;
	CString msDisplayName;
	CString msPath;
};


/////////////////////////////////////////////////////////////////////////////
// CFolderTreeCtrl

CFolderTreeCtrl::CFolderTreeCtrl()
: mhtiSelected( NULL )
, mpFolderCombo( NULL )
{
	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_FOLDER );
	mImageList.Create( 16, 16, ILC_COLOR24, 10, 5 );
	mImageList.Add( &bitmap, RGB(0,0,0) );
}

CFolderTreeCtrl::~CFolderTreeCtrl()
{
}

bool CFolderTreeCtrl::Create( CFolderComboBox* pFolderCombo, const CRect& rectWnd, DWORD dwStyle, UINT nID )
{
	mpFolderCombo = pFolderCombo;

	//CTreeCtrl::Create() calls CWnd::Create(), which causes an assert when creating a WS_POPUP window.
	//The workaround is to create the tree control using the Windows API, then subclass it - 2007-09-23 [ORW]
	INITCOMMONCONTROLSEX ICC = { sizeof(INITCOMMONCONTROLSEX), ICC_TREEVIEW_CLASSES };
	InitCommonControlsEx( &ICC );
	HWND hwndTreeCtrl =
		::CreateWindow( _T("SysTreeView32"), _T(""), dwStyle, rectWnd.left, rectWnd.top, rectWnd.Width(), rectWnd.Height(),
										pFolderCombo->GetParent()->m_hWnd, NULL, theWorkspace.GetThisModule(), NULL );
	if( !hwndTreeCtrl )
		return false;
#ifdef _UNICODE
	BOOL bUnicode = TRUE;
#else
	BOOL bUnicode = FALSE;
#endif
	::SendMessage( hwndTreeCtrl, CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	if( !SubclassWindow( hwndTreeCtrl ) )
		return false;

	SetImageList( &mImageList, TVSIL_NORMAL );
	EnumFolders();
	return true;
}

//static
const UINT& CFolderTreeCtrl::refWM_SELCHANGE()
{
	static const UINT WM_SELCHANGE = RegisterWindowMessage( _T("OpenDCL.FolderTree.SelChange") );
	return WM_SELCHANGE;
}

void CFolderTreeCtrl::FreeMemory()
{	
	FreeMemory(GetRootItem());
	DeleteAllItems();
}

HTREEITEM CFolderTreeCtrl::FreeMemory(HTREEITEM hItem)
{
	if(!hItem)
		return NULL;
	if( !m_hWnd )
		return NULL;

	CFolder* folder = (CFolder*)GetItemData(hItem);
	if (folder!=NULL)
		delete folder;
	
	HTREEITEM hRet = NULL;
	if (ItemHasChildren(hItem))
		hRet = FreeMemory(GetChildItem(hItem));

	if(hRet == NULL)
		hRet = FreeMemory(GetNextSiblingItem(hItem));

	return hRet;
}

void CFolderTreeCtrl::Hide()
{
	ShowWindow(SW_HIDE);
	if( mpFolderCombo )
		mpFolderCombo->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_CLOSEUP), (LPARAM)m_hWnd );
	// collapse the child when selected the parent?
	// Expand(m_selectedItem, TVE_COLLAPSE);
}

void CFolderTreeCtrl::Inform()
{
	if( mpFolderCombo )
	{
		mpFolderCombo->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_SELENDOK), (LPARAM)m_hWnd );
		mpFolderCombo->SendMessage( refWM_SELCHANGE(), 0, (LPARAM)mhtiSelected );
	}
}

HTREEITEM CFolderTreeCtrl::GetSelectedItem()
{
	return mhtiSelected;
}

BOOL CFolderTreeCtrl::SelectItem(HTREEITEM item)
{
	mhtiSelected = item;
	return __super::SelectItem(item);
}

CString CFolderTreeCtrl::GetItemDisplayName(HTREEITEM item)
{
	if( !m_hWnd )
		return CString();
	CFolder* pFolder = (CFolder*)GetItemData( item );
	if( !pFolder )
		return CString();
	return pFolder->msDisplayName;
}

CString CFolderTreeCtrl::GetItemPath(HTREEITEM item)
{
	if( !m_hWnd )
		return CString();
	CFolder* pFolder = (CFolder*)GetItemData( item );
	if( !pFolder )
		return CString();
	return pFolder->msPath;
}

int CFolderTreeCtrl::GetItemImageIndex(HTREEITEM item)
{
	if( !m_hWnd )
		return -1;
	CFolder* pFolder = (CFolder*)GetItemData( item );
	if( !pFolder )
		return -1;
	return pFolder->mnImg;
}


HTREEITEM CFolderTreeCtrl::AddFolder(CFolder* folder, HTREEITEM parent, bool bCurrentDir)
{
	if (folder == NULL)
		return NULL;

	int imageIndex = folder->mnImg;

	if (bCurrentDir)
		imageIndex = 1;
	HTREEITEM item = InsertItem(folder->msDisplayName, 
							imageIndex, imageIndex, parent);
	SetItemData(item, (DWORD_PTR)folder);
	return item;
}

void CFolderTreeCtrl::AddPath( LPCTSTR pszPath )
{
	HTREEITEM item, parent;
	CStringArray foldersArray, foldersDescArray;
	CString desc, tmp, str;

	CString dir = pszPath;
	
	int len = dir.GetLength();
	if (len > 0 && dir[len-1] != _T('\\'))
		dir = dir + _T("\\");

	int n = dir.Find(_T('\\'));
	while (n != -1)
	{	desc = dir.Left(n);
		str = str + desc + _T("\\");
		tmp = str;
		if (str.GetLength()>0 && foldersArray.GetSize()>0)
			tmp = str.Left(str.GetLength()-1);
		foldersArray.Add(tmp);
		foldersDescArray.Add(desc);
		dir = dir.Mid(n+1);
		n = dir.Find(_T('\\'));
	}

	int cnt = foldersArray.GetSize();
	if (cnt <= 0)
		return;

	item = GetRootItem();
	item = SearchPath(item, foldersArray.GetAt(0));
	parent = item;
	int i = 0;
	while (item && i<cnt)
	{	item = SearchChildOneLevel(item, foldersArray.GetAt(i));
		if (item != NULL)
		{	parent = item;
			item = GetChildItem(item);
		}
		else 
			break;
		i++;
	}

	cnt = foldersArray.GetSize();
	while (i < cnt)
	{	
		if (i == cnt -1)
			parent = AddFolder(new CFolder(foldersDescArray.GetAt(i), foldersArray.GetAt(i)), parent, true);
		else
			parent = AddFolder(new CFolder(foldersDescArray.GetAt(i), foldersArray.GetAt(i)), parent);
		i++;
	}
	
	item = parent;
	SelectItem(item);
}

HTREEITEM CFolderTreeCtrl::SearchPath( HTREEITEM hItem, LPCTSTR pszPath )
{
	if( hItem == TVI_ROOT )
		hItem = GetRootItem();
	if( !hItem )
		return NULL;
	if( !m_hWnd )
		return NULL;
	CFolder* pFolder = (CFolder*)GetItemData( hItem );
	if( pFolder && pFolder->msPath.CompareNoCase( pszPath ) == 0 )
		return hItem;
	hItem = GetNextItem( hItem, TVGN_CHILD );
	while( hItem )
	{
		HTREEITEM htiFound = SearchPath( hItem, pszPath );
		if( htiFound )
			return htiFound;
		hItem = GetNextSiblingItem( hItem );
	}
	return NULL;
}

HTREEITEM CFolderTreeCtrl::SearchFolder( HTREEITEM hItem, LPCTSTR pszFolderName )
{
	if( hItem == TVI_ROOT )
		hItem = GetRootItem();
	if( !hItem )
		return NULL;
	if( !m_hWnd )
		return NULL;
	CFolder* pFolder = (CFolder*)GetItemData( hItem );
	if( pFolder && pFolder->msDisplayName.CompareNoCase( pszFolderName ) == 0 )
		return hItem;
	hItem = GetNextItem( hItem, TVGN_CHILD );
	while( hItem )
	{
		HTREEITEM htiFound = SearchFolder( hItem, pszFolderName );
		if( htiFound )
			return htiFound;
		hItem = GetNextSiblingItem( hItem );
	}
	return NULL;
}

HTREEITEM CFolderTreeCtrl::SearchChildOneLevel( HTREEITEM hItem, LPCTSTR pszPath )
{
	if( !m_hWnd )
		return NULL;
	CFolder* folder;
	while (hItem)
	{	
		folder = (CFolder*)GetItemData(hItem);
		if (folder!=NULL && folder->msPath.CompareNoCase(pszPath)==0) 
			return hItem;

		hItem = GetNextSiblingItem(hItem);		
	}
	return NULL;
}

CString CFolderTreeCtrl::GetSelectedDisplayName()
{
	CString sPath;
	if( m_hWnd && mhtiSelected )
	{
		CFolder* pFolder = (CFolder*)GetItemData( mhtiSelected );
		if( pFolder )
			sPath = pFolder->msDisplayName;
	}
	return sPath;
}

CString CFolderTreeCtrl::GetSelectedPath()
{
	CString sPath;
	if( m_hWnd && mhtiSelected )
	{
		CFolder* pFolder = (CFolder*)GetItemData( mhtiSelected );
		if( pFolder )
			sPath = pFolder->msPath;
	}
	return sPath;
}

bool CFolderTreeCtrl::SelectPath( LPCTSTR pszPath )
{
	HTREEITEM hti = SearchPath( TVI_ROOT, pszPath );
	SelectItem( hti );
	return (hti != NULL);
}

bool CFolderTreeCtrl::SelectFolder( LPCTSTR pszFolderName )
{
	HTREEITEM hti = SearchFolder( TVI_ROOT, pszFolderName );
	SelectItem( hti );
	return (hti != NULL);
}

void CFolderTreeCtrl::AddDrives(HTREEITEM myComputer)
{
	// load drives
	DWORD drives = GetLogicalDrives();
	if (drives != 0)
	{	
		CString str;
		CString driveDesc;
		int iconID = HARDDRIVE_ICON;
		TCHAR driveName[] = _T("A:\\");
		char driveLetter = _T('A');
		long bits = 1;
		SHFILEINFO sfi;
		UINT flags = SHGFI_DISPLAYNAME;
		int size = sizeof(drives) * 8;
		for (int i=0; i<size; i++)
		{			
			driveName[0] = driveLetter;
			driveDesc.Format(_T("Drive (%c:)"), driveLetter);
						 
			if (SHGetFileInfo(driveName, 0, &sfi, sizeof(sfi), flags))
				driveDesc = sfi.szDisplayName;
			if (GetDriveType(driveName)==DRIVE_REMOVABLE)
				iconID = FLOPPY_ICON;
			else if (GetDriveType(driveName)==DRIVE_CDROM)
				iconID = CDROM_ICON;
			else
				iconID = HARDDRIVE_ICON;

			if (drives & bits)
				AddFolder(new CFolder(driveDesc, driveName, iconID, iconID), myComputer);
			bits = bits << 1;
			driveLetter += 1;
		}
	}
}

void CFolderTreeCtrl::EnumFolders()
{
	HTREEITEM myComputer;
		LPMALLOC pMalloc;
		LPITEMIDLIST pidlItems = NULL;
		IShellFolder *psfDeskTop = NULL;
		LPENUMIDLIST ppenum = NULL;
		ULONG celtFetched;
		HRESULT hr;
		STRRET strDispName;
		TCHAR pszDisplayName[MAX_PATH];
		ULONG uAttr;
	LPITEMIDLIST pidl = NULL;
		CString sMyComputer;
	CString sRecycle;
	CString sMyDocuments;
	CString sSharedDocuments;
	CString sNetwork;

		CoInitialize( NULL );

	
	hr = SHGetMalloc(&pMalloc);

	hr = SHGetDesktopFolder(&psfDeskTop);
	
	hr = psfDeskTop->GetDisplayNameOf(NULL, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	TCHAR szPath[MAX_PATH] = _T("");
	CComQIPtr<IPersistFolder2> ipf2_desktop(psfDeskTop);
	if( ipf2_desktop )
	{
		LPITEMIDLIST pidlDesktop = NULL;
		if (SUCCEEDED(ipf2_desktop->GetCurFolder( &pidlDesktop )))
			SHGetPathFromIDList( pidlDesktop, szPath );
	}
	if( !szPath[0] )
		lstrcpyn( szPath, pszDisplayName, MAX_PATH );

	HTREEITEM desktop = AddFolder(new CFolder(pszDisplayName, szPath, DESKTOP_ICON, DESKTOP_ICON));
			
		psfDeskTop->Release();

	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_PERSONAL, &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	sMyDocuments = pszDisplayName;
	
	if (!SUCCEEDED(SHGetPathFromIDList( pidl, szPath )))
		lstrcpyn( szPath, pszDisplayName, MAX_PATH );
	
	AddFolder(new CFolder(pszDisplayName, szPath, MY_DOCUMENT_ICON, MY_DOCUMENT_ICON), desktop);		

	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_BITBUCKET , &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	sRecycle = pszDisplayName;
	
		hr = psfDeskTop->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ppenum);
	

	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_DRIVES, &pidl);
				
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
	
	sMyComputer = pszDisplayName;
	
	if (!SUCCEEDED(SHGetPathFromIDList( pidl, szPath )))
		lstrcpyn( szPath, pszDisplayName, MAX_PATH );
	
	myComputer = AddFolder(new CFolder(pszDisplayName, szPath, FOLDER_ICON, FOLDER_EXPAND_ICON), desktop);
	
	AddDrives(myComputer);


	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_COMMON_DOCUMENTS , &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
	
	sSharedDocuments = pszDisplayName;
	
	if (!SUCCEEDED(SHGetPathFromIDList( pidl, szPath )))
		lstrcpyn( szPath, pszDisplayName, MAX_PATH );
	
	AddFolder(new CFolder(pszDisplayName, szPath, FOLDER_ICON, FOLDER_EXPAND_ICON), myComputer);


	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_PERSONAL, &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
	
	if (!SUCCEEDED(SHGetPathFromIDList( pidl, szPath )))
		lstrcpyn( szPath, pszDisplayName, MAX_PATH );
	
	AddFolder(new CFolder(pszDisplayName, szPath, FOLDER_ICON, FOLDER_EXPAND_ICON), myComputer);


	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_NETWORK, &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	sNetwork = pszDisplayName;
	
	if (!SUCCEEDED(SHGetPathFromIDList( pidl, szPath )))
		lstrcpyn( szPath, pszDisplayName, MAX_PATH );

	AddFolder(new CFolder(pszDisplayName, szPath, NETWORK_ICON, NETWORK_ICON), desktop);	

	
	
	// look through the enum list and extract the appropriate directory names.
		while( (hr = ppenum->Next(1, &pidlItems, &celtFetched), hr) == S_OK && celtFetched == 1)
		{
				psfDeskTop->GetDisplayNameOf(pidlItems, SHGDN_INFOLDER, &strDispName);
				StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

		uAttr = SFGAO_FOLDER;
				psfDeskTop->GetAttributesOf(1, (LPCITEMIDLIST *) &pidlItems, &uAttr);
				if(uAttr & SFGAO_FOLDER)
				{
			if (pszDisplayName != sMyComputer &&
				pszDisplayName != sRecycle &&
				pszDisplayName != sMyDocuments &&
				pszDisplayName != sSharedDocuments &&
				pszDisplayName != sNetwork)
			{
	
				if (!SUCCEEDED(SHGetPathFromIDList( pidlItems, szPath )))
					lstrcpyn( szPath, pszDisplayName, MAX_PATH );
				AddFolder(new CFolder(pszDisplayName, szPath, FOLDER_ICON, FOLDER_EXPAND_ICON), desktop);
			}
			}
				
		pMalloc->Free(pidlItems);
		}

		
		ppenum->Release();
	
		CoUninitialize();
	
	Expand(desktop, TVE_EXPAND);
	Expand(myComputer, TVE_EXPAND);
	
	SelectItem(desktop);
}

void CFolderTreeCtrl::SelectNextItem(BOOL selectNext)
{
	HTREEITEM item = GetSelectedItem();
	if (selectNext)
		item = GetNextVisibleItem(item);
	else
		item = GetPrevVisibleItem(item);
	if (item != NULL)
	{
		SelectItem(item);
		Inform();
		mpFolderCombo->SetFocus();
	}
}

void CFolderTreeCtrl::Display( const CRect& rcTree )
{
	__super::SelectItem(mhtiSelected);
	CRect rc( rcTree );
	HMONITOR hDisplay = ::MonitorFromWindow( m_hWnd, MONITOR_DEFAULTTONEAREST );
	MONITORINFO DisplayInfo = { sizeof(MONITORINFO) };
	if( ::GetMonitorInfo( hDisplay, &DisplayInfo ) )
	{
		RECT& rectDisplay = DisplayInfo.rcWork;
		if( rc.right > rectDisplay.right )
			rc.right = rectDisplay.right;
		if( rc.bottom > rectDisplay.bottom )
			rc.bottom = rectDisplay.bottom;
	}
	SetWindowPos(&wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);

	CWnd* pTopParent = GetParent()->GetParentOwner();
	if (pTopParent != NULL)
	{
		pTopParent->SendMessage( WM_NCACTIVATE, TRUE );
		pTopParent->SetRedraw( TRUE );
	}
}


BEGIN_MESSAGE_MAP(CFolderTreeCtrl, CTreeCtrl)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillFocus)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFolderTreeCtrl message handlers

void CFolderTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT flags ;
	HTREEITEM item = HitTest(point, &flags);
	if (item != NULL)
		__super::SelectItem(item);
		
	__super::OnMouseMove(nFlags, point);
}

void CFolderTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hti = pNMTreeView->itemNew.hItem;
	int imageIndex = 0;
	int imageIndexExpanded = 1;
	CFolder* folder = (CFolder*) GetItemData(hti);
	if (folder != NULL)
	{	
		imageIndex = folder->mnImg;
		imageIndexExpanded = folder->mnImgExpanded;
	}
	if (pNMTreeView->action == TVE_EXPAND)
		SetItemImage(hti, imageIndexExpanded, imageIndexExpanded);
	else
		SetItemImage(hti, imageIndex, imageIndex);
	
	*pResult = 0;
}

void CFolderTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	UINT flags;
	HTREEITEM item = HitTest( point, &flags );
	if( flags != TVHT_ONITEMBUTTON )
	{
		mhtiSelected = item;
		if( !item )
			mhtiSelected = __super::GetSelectedItem();
		Hide();
		Inform();
		return;
	}
	__super::OnLButtonDown(nFlags, point);
}

void CFolderTreeCtrl::OnKillFocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Hide();
}

BOOL CFolderTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE &&
		pMsg->hwnd == m_hWnd)
	{
		ShowWindow(SW_HIDE);
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_hWnd)
	{
		mhtiSelected = __super::GetSelectedItem();
		Hide();
		Inform();
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CFolderTreeCtrl::OnDestroy() 
{
	FreeMemory();
	__super::OnDestroy();
}
