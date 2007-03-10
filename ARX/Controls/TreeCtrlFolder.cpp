// TreeCtrlFolder.cpp : implementation file
//

#include "stdafx.h"
#include "TreeCtrlFolder.h"
#include "Resource.h"
#include "ComboBoxFolder.h"

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


/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlFolder

CTreeCtrlFolder::CTreeCtrlFolder()
{
	m_selectedItem = NULL;
	m_comboBoxFolder = FALSE;
}

CTreeCtrlFolder::~CTreeCtrlFolder()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlFolder, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlFolder)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlFolder message handlers

void CTreeCtrlFolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT flags ;
	HTREEITEM item = HitTest(point, &flags);
	if (item != NULL)
		CTreeCtrl::SelectItem(item);
		
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CTreeCtrlFolder::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hti = pNMTreeView->itemNew.hItem;
	int imageIndex = 0;
	int imageIndexExpanded = 1;
	CFolder* folder = (CFolder*) GetItemData(hti);
	if (folder != NULL)
	{	
		imageIndex = folder->m_imageIndex;
		imageIndexExpanded = folder->m_imageIndexExpanded;
	}
	if (pNMTreeView->action == TVE_EXPAND)
		SetItemImage(hti, imageIndexExpanded, imageIndexExpanded);
	else
		SetItemImage(hti, imageIndex, imageIndex);
	
	*pResult = 0;
}

void CTreeCtrlFolder::CreateImageList()
{
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_FOLDER);
	m_imageList.Create(16, 16, ILC_COLOR24, 10, 5);
	m_imageList.Add(&bitmap, RGB(0,0,0));
	SetImageList(&m_imageList, TVSIL_NORMAL);
}

void CTreeCtrlFolder::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	UINT flags;
	HTREEITEM item = HitTest(point, &flags);
	if (flags!=TVHT_ONITEMBUTTON)
	{	m_selectedItem = item;
		if (item == NULL)
			m_selectedItem = CTreeCtrl::GetSelectedItem();
		HideAndInform();
		return;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTreeCtrlFolder::OnKillFocus(CWnd* pNewWnd) 
{
	CPoint point;
	GetCursorPos(&point);
	CWnd* wnd = WindowFromPoint(point);
	if (wnd->GetSafeHwnd() != m_comboBoxFolder->GetSafeHwnd())
		ShowWindow(SW_HIDE);

	CTreeCtrl::OnKillFocus(pNewWnd);
}

BOOL CTreeCtrlFolder::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE &&
		pMsg->hwnd == m_hWnd)
	{	ShowWindow(SW_HIDE);
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN &&
		pMsg->hwnd == m_hWnd)
	{	m_selectedItem = CTreeCtrl::GetSelectedItem();
		HideAndInform();
		return TRUE;
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CTreeCtrlFolder::Display(CRect rc)
{	
	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// lets make sure that the drop list is not shown partially off screen
	if (nScreenHeight < rc.top + rc.bottom/* actually height*/)
		rc.bottom = nScreenHeight - rc.top -1;
	
	if (nScreenWidth < rc.left + rc.right/* actually height*/)
		rc.left = nScreenWidth - rc.right -1;

	SetWindowPos(&wndNoTopMost, rc.left, rc.top, rc.right, rc.bottom, SWP_SHOWWINDOW);

	CWnd* pTopParent = GetParent()->GetParentOwner();
    if (pTopParent != NULL)
	{  pTopParent->SendMessage( WM_NCACTIVATE, TRUE );
	   pTopParent->SetRedraw( TRUE );
	}
}

void CTreeCtrlFolder::OnDestroy() 
{
	FreeMemory();
	CTreeCtrl::OnDestroy();
}

void CTreeCtrlFolder::FreeMemory()
{	
	FreeMemory(GetRootItem());
	DeleteAllItems();
}

HTREEITEM CTreeCtrlFolder::FreeMemory(HTREEITEM hItem)
{
	if(!hItem)
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

void CTreeCtrlFolder::HideAndInform()
{
	ShowWindow(SW_HIDE);
	Inform();

	// collapse the child when selected the parent?
	// Expand(m_selectedItem, TVE_COLLAPSE);
}

void CTreeCtrlFolder::Inform()
{
	CWnd* parent = GetParent();
	if (parent != NULL)
	{	CFolder* folder = (CFolder*) GetItemData(m_selectedItem);
		m_comboBoxFolder->SendMessage(WM_SELECTITEM_CHANGE, (WPARAM)folder);
		m_comboBoxFolder->SetFocus();
	}
}

HTREEITEM CTreeCtrlFolder::GetSelectedItem()
{	return m_selectedItem;
}

int CTreeCtrlFolder::SelectItem(HTREEITEM item)
{
	m_selectedItem = item;
	return CTreeCtrl::SelectItem(item);
}


HTREEITEM CTreeCtrlFolder::AddFolder(CFolder* folder, HTREEITEM parent, bool bCurrentDir)
{
	if (folder == NULL)
		return NULL;

	int imageIndex = folder->m_imageIndex;

	if (bCurrentDir)
		imageIndex = 1;
	HTREEITEM item = InsertItem(folder->m_pathDescription, 
							imageIndex, imageIndex, parent);
	SetItemData(item, (DWORD)folder);
	return item;
}

void CTreeCtrlFolder::AddPath(CString path)
{
	HTREEITEM item, parent;
	CStringArray foldersArray, foldersDescArray;
	CString desc, tmp, str;

	CString dir = path;
	
	int len = path.GetLength();
	if (len > 0 && path[len-1] != '\\')
		dir = dir + "\\";

	int n = dir.Find('\\');
	while (n != -1)
	{	desc = dir.Left(n);
		str = str + desc + "\\";
		tmp = str;
		if (str.GetLength()>0 && foldersArray.GetSize()>0)
			tmp = str.Left(str.GetLength()-1);
		foldersArray.Add(tmp);
		foldersDescArray.Add(desc);
		dir = dir.Mid(n+1);
		n = dir.Find('\\');
	}

	int cnt = foldersArray.GetSize();
	if (cnt <= 0)
		return;

	item = GetRootItem();
	item = Search(item, foldersArray.GetAt(0));
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

HTREEITEM CTreeCtrlFolder::Search(HTREEITEM hItem, CString path)
{
	if(!hItem)
		return NULL;

	CFolder* folder = (CFolder*)GetItemData(hItem);
	if (folder!=NULL && folder->m_path.CollateNoCase(path)==0) 
		return hItem;
	
	HTREEITEM hRet = NULL;
	if (ItemHasChildren(hItem))
		hRet = Search(GetChildItem(hItem), path);

	if(hRet == NULL)
		hRet = Search(GetNextSiblingItem(hItem), path);

	return hRet;
}

HTREEITEM CTreeCtrlFolder::SearchChildOneLevel(HTREEITEM item, CString path)
{
	CFolder* folder;
	while (item)
	{	
		folder = (CFolder*)GetItemData(item);
		if (folder!=NULL && folder->m_path.CollateNoCase(path)==0) 
			return item;

		item = GetNextSiblingItem(item);		
	}
	return NULL;
}

CString CTreeCtrlFolder::GetSelectedPath()
{
	CString path;
	CFolder* folder = (CFolder*) GetItemData(m_selectedItem);
	if (folder != NULL)
		path = folder->m_path;
	return path;
}


void CTreeCtrlFolder::AddDrives(HTREEITEM myComputer)
{
	// load drives
	DWORD drives = GetLogicalDrives();
	if (drives != 0)
	{	
		CString str;
		CString driveDesc;
		int iconID = HARDDRIVE_ICON;
    TCHAR driveName[] = _T("A:\\");
		char driveLetter = 'A';
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

void CTreeCtrlFolder::EnumFolders()
{
	HTREEITEM myComputer;
    LPMALLOC pMalloc;
    LPITEMIDLIST pidlProgFiles = NULL;
    LPITEMIDLIST pidlItems = NULL;
    IShellFolder *psfFirstFolder = NULL;
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

	
	HTREEITEM desktop = AddFolder(new CFolder(pszDisplayName, pszDisplayName, DESKTOP_ICON, DESKTOP_ICON));
			
    psfDeskTop->Release();

	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_PERSONAL, &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	sMyDocuments = pszDisplayName;
	
	
	AddFolder(new CFolder(pszDisplayName, pszDisplayName, MY_DOCUMENT_ICON, MY_DOCUMENT_ICON), desktop);		
	

	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_BITBUCKET , &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	sRecycle = pszDisplayName;
	
    hr = psfDeskTop->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ppenum);
	

	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_DRIVES, &pidl);
				
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
	
	sMyComputer = pszDisplayName;
	
	myComputer = AddFolder(new CFolder(pszDisplayName, pszDisplayName, FOLDER_ICON, FOLDER_EXPAND_ICON), desktop);
	
	AddDrives(myComputer);


	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_COMMON_DOCUMENTS , &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
	
	sSharedDocuments = pszDisplayName;
	
	AddFolder(new CFolder(pszDisplayName, pszDisplayName, FOLDER_ICON, FOLDER_EXPAND_ICON), myComputer);


	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_PERSONAL, &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
	
	AddFolder(new CFolder(pszDisplayName, pszDisplayName, FOLDER_ICON, FOLDER_EXPAND_ICON), myComputer);


	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_NETWORK, &pidl);
	
	hr = psfDeskTop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDispName);
	StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);

	sNetwork = pszDisplayName;

	AddFolder(new CFolder(pszDisplayName, pszDisplayName, NETWORK_ICON, NETWORK_ICON), desktop);	

	
	
	// look through the enum list and extract the appropriate directory names.
    while( hr = ppenum->Next(1, &pidlItems, &celtFetched) == S_OK && (celtFetched) == 1)
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
				AddFolder(new CFolder(pszDisplayName, pszDisplayName, FOLDER_ICON, FOLDER_EXPAND_ICON), desktop);
			}
	    }
        
		pMalloc->Free(pidlItems);
    }

    
    ppenum->Release();
	
    CoUninitialize();

	TCHAR szPath[MAX_PATH];
	GetSystemDirectory(szPath, MAX_PATH);
	
	Expand(desktop, TVE_EXPAND);
	Expand(myComputer, TVE_EXPAND);
	
	SelectItem(desktop);
}

void CTreeCtrlFolder::Init(CComboBoxFolder* comboBoxFolder)
{
	m_comboBoxFolder = comboBoxFolder;

	char szPath[MAX_PATH];

	EnumFolders();
	return;

/*	char szPath[MAX_PATH];

	HTREEITEM desktop = AddFolder(new CFolder("Desktop", "Desktop", DESKTOP_ICON, DESKTOP_ICON));
	HTREEITEM myComputer = AddFolder(new CFolder("My Computer", "My Computer", MYCOMPUTER_ICON, MYCOMPUTER_ICON), desktop);

	// load drives
	DWORD drives = GetLogicalDrives();
	if (drives != 0)
	{	
		CString str;
		CString driveDesc;
		int iconID = HARDDRIVE_ICON;
		char driveName[] = "A:\\";
		char driveLetter = 'A';
		long bits = 1;
		SHFILEINFO sfi;
		UINT flags = SHGFI_DISPLAYNAME;
		int size = sizeof(drives) * 8;
		for (int i=0; i<size; i++)
		{			
			driveName[0] = driveLetter;
			driveDesc.Format("Drive (%c:)", driveLetter);
             
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
	AddFolder(new CFolder("Control Panel", "Control Panel", CONTROLPANEL_ICON, CONTROLPANEL_ICON), myComputer);
	AddFolder(new CFolder("My Network Places", "My Network Places", NETWORK_ICON, NETWORK_ICON), desktop);
	//SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE);
	//SHGetFolderPath(*this, CSIDL_PERSONAL, NULL, SHGFP_TYPE_DEFAULT, szPath);
	AddFolder(new CFolder("My Documents", "My Documents", MY_DOCUMENT_ICON, MY_DOCUMENT_ICON), desktop);
	GetSystemDirectory(szPath, MAX_PATH);
	
	Expand(desktop, TVE_EXPAND);
	Expand(myComputer, TVE_EXPAND);
	
	SelectItem(desktop);
*/

}


void CTreeCtrlFolder::SelectNextItem(BOOL selectNext)
{
	HTREEITEM item = GetSelectedItem();
	if (selectNext)
		item = GetNextVisibleItem(item);
	else
		item = GetPrevVisibleItem(item);
	if (item != NULL)
	{	SelectItem(item);
		Inform();
	}
}
