// TreeCtrlFolder.cpp : implementation file
//

#include "stdafx.h"
#include "FolderTreeCtrl.h"
#include "Resource.h"
#include "Workspace.h"
#include "FolderComboBox.h"
#include <shellapi.h>
#include <comdef.h>
#include <vector>

// multi-monitor stubs to fake support for multiple monitors in Win95 and WinNT
//#define COMPILE_MULTIMON_STUBS //stubs are already defined by PPToolTip.cpp
#include "MultiMon.h"

#undef SubclassWindow

#ifndef WM_ACAD_KEEPFOCUS
#define WM_ACAD_KEEPFOCUS (0x0400+0x6D01)
#endif

enum _Icons
{
	FOLDER_ICON = 0,
	FOLDER_OPEN_ICON = 1,
	NETWORK_ICON = 2,
	DESKTOP_ICON = 3,
	MYCOMPUTER_ICON = 4,
	FLOPPY_ICON = 5,
	HARDDRIVE_ICON = 6,
	CDROM_ICON = 7,
	CONTROLPANEL_ICON = 8,
	MY_DOCUMENT_ICON = 9,
};


/////////////////////////////////////////////////////////////////////////////
// CFolderTreeCtrl

CFolderTreeCtrl::CFolderTreeCtrl()
: mhtiSelected( NULL )
, mdwLastCharTime( 0 )
, mpFolderCombo( NULL )
, mbTracking( true )
, mpActiveWnd( NULL )
{
	CBitmap bmpIList;
	bmpIList.LoadBitmap( IDB_FOLDER ); //161 x 18, 24-bit
	mImageList.Create( 16, 16, ILC_MASK | ILC_COLOR24, 10, 5 );
	mImageList.Add( &bmpIList, RGB(255,255,255) );
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
		::CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_TOPMOST, _T("SysTreeView32"), _T(""), WS_CHILDWINDOW | dwStyle, rectWnd.left, rectWnd.top, rectWnd.Width(), rectWnd.Height(),
											pFolderCombo->m_hWnd, NULL, theWorkspace.GetThisModule(), NULL );
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

	CoInitialize( NULL );
	AddMyComputer();
	CoUninitialize();
	return true;
}

HTREEITEM CFolderTreeCtrl::GetSelectedItem() const
{
	return mhtiSelected;
}

BOOL CFolderTreeCtrl::SelectItem( HTREEITEM hItem )
{
	if( !__super::SelectItem( hItem ) )
		return FALSE;
	Expand( hItem, TVE_EXPAND );
	CommitCurrentItem();
	return TRUE;
}

CString CFolderTreeCtrl::GetItemDisplayName( HTREEITEM hItem ) const
{
	CString sName;
	if( hItem && m_hWnd )
		sName = GetItemText( hItem );
	return sName;
}

CString CFolderTreeCtrl::GetItemPath( HTREEITEM hItem ) const
{
	CString sPath;
	if( hItem && m_hWnd )
	{
		CString* pCString = (CString*)GetItemData( hItem );
		if( pCString )
			sPath = *pCString;
	}
	return sPath;
}

int CFolderTreeCtrl::GetItemImageIndex( HTREEITEM hItem ) const
{
	if( !hItem || !m_hWnd )
		return -1;
	int idxNormal = -1;
	int idxSelected = -1;
	GetItemImage( hItem, idxNormal, idxSelected );
	return idxNormal;
}


HTREEITEM CFolderTreeCtrl::AddFolder( LPCTSTR pszDisplayName, LPCTSTR pszPath, int nImg, int nImgExpanded, HTREEITEM htiParent )
{
	if (!pszDisplayName || !*pszDisplayName)
		return NULL;

	HTREEITEM htiNewFolder = InsertItem( pszDisplayName, nImg, nImgExpanded, htiParent );
	if( htiNewFolder && pszPath && *pszPath )
		SetItemData(htiNewFolder, (DWORD_PTR)new CString(pszPath));
	return htiNewFolder;
}

HTREEITEM CFolderTreeCtrl::AddPath( LPCTSTR pszPath )
{
	if( !pszPath || !pszPath[0] )
	{
		CoInitialize( NULL );
		HTREEITEM htiNew = AddMyComputer();
		CoUninitialize();
		CommitCurrentItem();
		return htiNew;
	}
	if( *pszPath == _T('\\') && !pszPath[1] )
	{
		CoInitialize( NULL );
		HTREEITEM htiNew = AddDesktop();
		CoUninitialize();
		CommitCurrentItem();
		return htiNew;
	}
	TCHAR szFullPath[MAX_PATH] = _T("");
	GetFullPathName( pszPath, MAX_PATH, szFullPath, NULL );
	CString sPathRemaining = szFullPath;
	CString sPathPrefix;
	HTREEITEM htiFolder = NULL;
	while( !sPathRemaining.IsEmpty() )
	{
		CString sFolder = sPathRemaining.SpanExcluding(_T("\\/"));
		sPathRemaining = sPathRemaining.Mid( sFolder.GetLength() ).TrimLeft(_T("\\/"));
		if( !sPathPrefix.IsEmpty() && sPathPrefix.GetAt( sPathPrefix.GetLength() - 1 ) != _T('\\') )
			sPathPrefix += _T('\\');
		else if( sFolder.GetLength() == 2 && sFolder.GetAt(1) == _T(':') )
			sFolder += _T('\\');
		sPathPrefix += sFolder;
		HTREEITEM htiSubFolder = NULL;
		if( !htiFolder )
		{
			htiFolder = GetRootItem();
			htiSubFolder = SearchPath( htiFolder, sFolder );
		}
		else
			htiSubFolder = SearchChildOneLevel( htiFolder, sPathPrefix );
		if( htiSubFolder )
			htiFolder = htiSubFolder;
		else
		{
			htiFolder = AddFolder( sFolder, sPathPrefix, FOLDER_ICON, FOLDER_OPEN_ICON, htiFolder );
			if( htiFolder )
				Expand( htiFolder, TVE_EXPAND );
		}
	}
	return htiFolder;
}

HTREEITEM CFolderTreeCtrl::SearchPath( HTREEITEM hItem, LPCTSTR pszPath, bool bExactMatch /*= true*/ ) const
{
	if( hItem == TVI_ROOT )
		hItem = GetRootItem();
	if( !hItem )
		return NULL;
	if( !m_hWnd )
		return NULL;
	CString sPath = pszPath;
	sPath = sPath.TrimRight(_T("\\/"));
	if( sPath.GetLength() == 2 && sPath.GetAt(1) == _T(':') )
		sPath += _T('\\');
	CString* pCString = (CString*)GetItemData( hItem );
	if( pCString && pCString->GetLength() >= sPath.GetLength() )
	{
		CString sCompare = (bExactMatch? *pCString : pCString->Left(sPath.GetLength()));
		if( sCompare.CompareNoCase( sPath ) == 0 )
			return hItem;
	}
	hItem = GetNextItem( hItem, TVGN_CHILD );
	while( hItem )
	{
		HTREEITEM htiFound = SearchPath( hItem, sPath, bExactMatch );
		if( htiFound )
			return htiFound;
		hItem = GetNextSiblingItem( hItem );
	}
	return NULL;
}

HTREEITEM CFolderTreeCtrl::SearchChildOneLevel( HTREEITEM hItem, LPCTSTR pszPath ) const
{
	if( !hItem )
		return NULL;
	if( !m_hWnd )
		return NULL;
	hItem = GetChildItem( hItem );
	while( hItem )
	{	
		CString* pCString = (CString*)GetItemData( hItem );
		if( pCString && pCString->CompareNoCase( pszPath ) == 0 )
			return hItem;
		hItem = GetNextSiblingItem( hItem );		
	}
	return NULL;
}

CString CFolderTreeCtrl::GetSelectedDisplayName() const
{
	CString sName;
	if( m_hWnd && mhtiSelected )
		sName = GetItemText( mhtiSelected );
	return sName;
}

CString CFolderTreeCtrl::GetSelectedPath() const
{
	return GetItemPath( mhtiSelected );
}

bool CFolderTreeCtrl::SelectPath( LPCTSTR pszPath )
{
	HTREEITEM hti = SearchPath( TVI_ROOT, pszPath );
	SelectItem( hti );
	return (hti != NULL);
}

//Machinery to map CSIDL to absolute PIDL in order to map special fodlers to icons in the tree control's image list
typedef int CSIDL;
typedef int ICONINDEX;
struct T_data
{
	LPITEMIDLIST pidl;
	ICONINDEX iconNormal;
	ICONINDEX iconOpened;
	T_data() : pidl(NULL), iconNormal(-1), iconOpened(-1) {}
	T_data(LPITEMIDLIST _pidl, ICONINDEX _iconNormal, ICONINDEX _iconOpened)
		: pidl(_pidl), iconNormal(_iconNormal), iconOpened(_iconOpened)
		{}
};
class SpecialFolderIconMap
{
	IShellFolder* mpSfRoot;
	typedef std::vector< T_data > T_map;
	T_map mMap;
public:
	SpecialFolderIconMap(IShellFolder* pSfRoot) : mpSfRoot( pSfRoot )
	{
		add(CSIDL_DESKTOP, DESKTOP_ICON);
		add(CSIDL_PERSONAL, MY_DOCUMENT_ICON);
		add(CSIDL_BITBUCKET, FOLDER_ICON, FOLDER_OPEN_ICON);
		add(CSIDL_DRIVES, MYCOMPUTER_ICON);
		add(CSIDL_COMMON_DOCUMENTS, FOLDER_ICON, FOLDER_OPEN_ICON);
		add(CSIDL_NETWORK, NETWORK_ICON);
	}
	~SpecialFolderIconMap()
	{
		for( T_map::iterator iter = mMap.begin(); iter != mMap.end(); ++iter )
		{
			LPITEMIDLIST pidl = iter->pidl;
			if( pidl )
				CoTaskMemFree( pidl );
		}
	}
	CSIDL lookupIcons(const LPITEMIDLIST pidl, int& iconNormal, int& iconOpened) const
	{
		iconNormal = -1;
		iconOpened = -1;
		if( !mpSfRoot )
			return -1;
		CSIDL csidl = 0;
		for( T_map::const_iterator iter = mMap.begin(); iter != mMap.end(); ++iter, ++csidl )
		{
			LPITEMIDLIST pidlTest = iter->pidl;
			if( pidl )
			{
				HRESULT hr = mpSfRoot->CompareIDs( 0, pidl, pidlTest );
				if( HRESULT_CODE(hr) == 0 )
				{
					iconNormal = iter->iconNormal;
					iconOpened = iter->iconOpened;
					return csidl;
				}
			}
		}
		return -1;
	}
protected:
	HRESULT add(CSIDL csidl, int idxNormalIcon, int idxOpenedIcon = -1)
	{
		if( csidl < 0 )
			return E_INVALIDARG;
		LPITEMIDLIST pidl;
		HRESULT hr = SHGetSpecialFolderLocation(NULL, csidl, &pidl);
		if( FAILED(hr) )
			return hr;
		if( mMap.size() <= T_map::size_type(csidl) )
			mMap.resize( T_map::size_type(csidl) + 1 );
		mMap[csidl] = T_data( pidl, idxNormalIcon, idxOpenedIcon >= 0? idxOpenedIcon : idxNormalIcon );
		return S_OK;
	}
};

HRESULT CFolderTreeCtrl::AddSubfolders( IShellFolder* pSfRoot, IShellFolder* pSfParent, HTREEITEM htiParent )
{
	CComPtr< IEnumIDList > pEidlSubfolders;
	HRESULT hr = pSfParent->EnumObjects( NULL, SHCONTF_FOLDERS, &pEidlSubfolders );
	if( FAILED(hr) )
		return hr;
	if( !pEidlSubfolders )
		return S_OK;

	static SpecialFolderIconMap IconMap( pSfRoot );

	while( 1 )
	{
		ULONG ctFetched = 0;
		LPITEMIDLIST pidlRelSubfolder = NULL;
		hr = pEidlSubfolders->Next( 1, &pidlRelSubfolder, &ctFetched );
		if( FAILED(hr) )
			return hr;
		if( hr == S_FALSE || ctFetched == 0 )
			break;

		CComPtr< IShellFolder > pSfSubfolder;
		hr = pSfParent->BindToObject( pidlRelSubfolder, NULL, IID_IShellFolder, (void**)&pSfSubfolder );
		if( SUCCEEDED(hr) )
		{
			STRRET strDisplayName;
			hr = pSfParent->GetDisplayNameOf( pidlRelSubfolder, SHGDN_NORMAL, &strDisplayName );
			if( SUCCEEDED(hr) )
			{
				STRRET strFolderPath;
				hr = pSfParent->GetDisplayNameOf( pidlRelSubfolder, SHGDN_FORPARSING, &strFolderPath );
				if( SUCCEEDED(hr) )
				{
					LPTSTR pszSubfolderPath = NULL;
					hr = StrRetToStr( &strFolderPath, NULL, &pszSubfolderPath );
					if( SUCCEEDED(hr) )
					{
						if( SUCCEEDED(hr) )
						{
							CComQIPtr< IPersistFolder2 > pPfSubfolder( pSfSubfolder );
							if( pPfSubfolder )
							{
								LPITEMIDLIST pidlAbsSubfolder = NULL;
								hr = pPfSubfolder->GetCurFolder( &pidlAbsSubfolder );
								if( SUCCEEDED(hr) )
								{
									int idxNormalIcon = -1;
									int idxOpenIcon = -1;
									CSIDL csidl = IconMap.lookupIcons(pidlAbsSubfolder, idxNormalIcon, idxOpenIcon);
									CoTaskMemFree(pidlAbsSubfolder);
									if( csidl < 0 )
									{
										switch( GetDriveType(pszSubfolderPath) )
										{
										case DRIVE_UNKNOWN:
										case DRIVE_NO_ROOT_DIR:
											break;
										case DRIVE_REMOVABLE:
											idxNormalIcon = FLOPPY_ICON;
											idxOpenIcon = FLOPPY_ICON;
											break;
										case DRIVE_CDROM:
											idxNormalIcon = CDROM_ICON;
											idxOpenIcon = CDROM_ICON;
											break;
										default:
											idxNormalIcon = HARDDRIVE_ICON;
											idxOpenIcon = HARDDRIVE_ICON;
											break;
										}
									}
									if( idxNormalIcon < 0 )
										idxNormalIcon = 0;
									if( idxOpenIcon < 0 )
										idxOpenIcon = 1;

									LPTSTR pszSubfolderName;
									hr = StrRetToStr( &strDisplayName, NULL, &pszSubfolderName );
									if( SUCCEEDED(hr) )
									{
										HTREEITEM htiSubfolder = AddFolder( pszSubfolderName, pszSubfolderPath, idxNormalIcon, idxOpenIcon, htiParent );
										CoTaskMemFree( pszSubfolderName );
										if( htiSubfolder )
										{
											switch( csidl )
											{
											case CSIDL_DRIVES:
												hr = AddSubfolders( pSfRoot, pSfSubfolder, htiSubfolder );
												Expand(htiSubfolder, TVE_EXPAND);
												break;
											}
										}
									}
								}
							}
						}
						CoTaskMemFree( pszSubfolderPath );
					}
				}
			}
		}
		CoTaskMemFree( pidlRelSubfolder );
	};
	return S_OK;
}

HTREEITEM CFolderTreeCtrl::AddMyComputer()
{
	CComPtr< IShellFolder > pSfDeskTop;
	HRESULT hr = SHGetDesktopFolder( &pSfDeskTop );
	if( FAILED(hr) )
		return NULL;

	LPITEMIDLIST pidlMyComputer;
	hr = SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidlMyComputer);
	if( FAILED(hr) )
		return NULL;

	CComPtr< IShellFolder > pSfMyComputer;
	hr = pSfDeskTop->BindToObject( pidlMyComputer, NULL, IID_IShellFolder, (void**)&pSfMyComputer );
	if( FAILED(hr) )
	 return NULL;

	STRRET strDisplayName;
	hr = pSfDeskTop->GetDisplayNameOf( pidlMyComputer, SHGDN_NORMAL, &strDisplayName );
	if( FAILED(hr) )
		return NULL;

	STRRET strPath;
	hr = pSfDeskTop->GetDisplayNameOf( pidlMyComputer, SHGDN_FORPARSING, &strPath );
	if( FAILED(hr) )
	{
		CoTaskMemFree( strDisplayName.pOleStr );
		return NULL;
	}

	LPTSTR pszDisplayName;
	hr = StrRetToStr( &strDisplayName, NULL, &pszDisplayName );
	CoTaskMemFree( strDisplayName.pOleStr );
	if( FAILED(hr) )
	{
		CoTaskMemFree( strPath.pOleStr );
		return NULL;
	}

	HTREEITEM htiMyComputer = NULL;

	LPTSTR pszPath;
	hr = StrRetToStr( &strPath, NULL, &pszPath );
	CoTaskMemFree( strPath.pOleStr );
	if( SUCCEEDED(hr) )
	{
		htiMyComputer = AddFolder( pszDisplayName, pszPath, MYCOMPUTER_ICON, MYCOMPUTER_ICON, TVI_ROOT );
		if( htiMyComputer )
		{
			hr = AddSubfolders( pSfDeskTop, pSfMyComputer, htiMyComputer );
			Expand( htiMyComputer, TVE_EXPAND );
			__super::SelectItem( htiMyComputer );
		}
		CoTaskMemFree( pszPath );
	}
	CoTaskMemFree( pszDisplayName );

	return htiMyComputer;
}

HTREEITEM CFolderTreeCtrl::AddDesktop()
{
	CComPtr< IShellFolder > pSfDeskTop;
	HRESULT hr = SHGetDesktopFolder( &pSfDeskTop );
	if( FAILED(hr) )
		return NULL;

	STRRET strDisplayName;
	hr = pSfDeskTop->GetDisplayNameOf( NULL, SHGDN_NORMAL, &strDisplayName );
	if( FAILED(hr) )
		return NULL;

	STRRET strPath;
	hr = pSfDeskTop->GetDisplayNameOf( NULL, SHGDN_FORPARSING, &strPath );
	if( FAILED(hr) )
	{
		CoTaskMemFree( strDisplayName.pOleStr );
		return NULL;
	}

	LPTSTR pszDisplayName;
	hr = StrRetToStr( &strDisplayName, NULL, &pszDisplayName );
	CoTaskMemFree( strDisplayName.pOleStr );
	if( FAILED(hr) )
	{
		CoTaskMemFree( strPath.pOleStr );
		return NULL;
	}

	HTREEITEM htiDesktop = NULL;

	LPTSTR pszPath;
	hr = StrRetToStr( &strPath, NULL, &pszPath );
	CoTaskMemFree( strPath.pOleStr );
	if( SUCCEEDED(hr) )
	{
		htiDesktop = AddFolder( pszDisplayName, pszPath, DESKTOP_ICON, DESKTOP_ICON, TVI_ROOT );
		if( htiDesktop )
		{
			hr = AddSubfolders( pSfDeskTop, pSfDeskTop, htiDesktop );
			Expand( htiDesktop, TVE_EXPAND );
			__super::SelectItem( htiDesktop );
		}
		CoTaskMemFree( pszPath );
	}
	CoTaskMemFree( pszDisplayName );

	return htiDesktop;
}

void CFolderTreeCtrl::CommitCurrentItem()
{
	mhtiSelected = __super::GetSelectedItem();
	if( mpFolderCombo )
		mpFolderCombo->Invalidate();
}

void CFolderTreeCtrl::NotifySelChange() const
{
	if( mpFolderCombo )
	{
		mpFolderCombo->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_SELENDOK), (LPARAM)mpFolderCombo->m_hWnd );
		mpFolderCombo->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_SELCHANGE), (LPARAM)mpFolderCombo->m_hWnd );
	}
}

void CFolderTreeCtrl::Open()
{
	__super::SelectItem(mhtiSelected);
	msAutoSearch.Empty();
	mdwLastCharTime = 0;
	CRect rc;
	GetWindowRect( &rc );
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
	mpActiveWnd = GetActiveWindow();

	typedef BOOL (WINAPI *F_AnimateWindow)(HWND,DWORD,DWORD);
	F_AnimateWindow pfAnimateWindow = NULL;;
	BOOL bComboBoxAnimation = FALSE;
	SystemParametersInfo( 0x1004/*SPI_GETCOMBOBOXANIMATION*/, 0, &bComboBoxAnimation, 0 );
	if( bComboBoxAnimation )
		pfAnimateWindow = (F_AnimateWindow)GetProcAddress(GetModuleHandle(_T("USER32.DLL")), "AnimateWindow");
	if( pfAnimateWindow )
	{
		bool bDown = true;
		if( mpFolderCombo )
		{
			CRect rcCombo;
			mpFolderCombo->GetWindowRect( &rcCombo );
			bDown = (rc.top >= rcCombo.bottom);
		}
		SetWindowPos(&wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), 0);
		pfAnimateWindow( m_hWnd, 200, 0x20000/*AW_ACTIVATE*/ | 0x40000/*AW_SLIDE*/ | (bDown? 0x4/*AW_VER_POSITIVE*/ : 0x8/*AW_VER_NEGATIVE*/) );
	}
	else
		SetWindowPos(&wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
	EnsureVisible(mhtiSelected);
	if( mpFolderCombo )
		mpFolderCombo->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_DROPDOWN), (LPARAM)mpFolderCombo->m_hWnd );
}


BEGIN_MESSAGE_MAP(CFolderTreeCtrl, CTreeCtrl)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CFolderTreeCtrl::OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &CFolderTreeCtrl::OnDeleteitem)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CFolderTreeCtrl::OnAcadKeepFocus)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFolderTreeCtrl message handlers

void CFolderTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( mbTracking )
	{
		UINT flags ;
		HTREEITEM item = HitTest(point, &flags);
		if (item != NULL)
			__super::SelectItem(item);
	}

	__super::OnMouseMove(nFlags, point);
}

void CFolderTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	switch( pNMTreeView->itemNew.iImage )
	{
	case FOLDER_ICON:
	case FOLDER_OPEN_ICON:
		{
			int idxIcon = (pNMTreeView->action == TVE_EXPAND? FOLDER_OPEN_ICON : FOLDER_ICON);
			SetItemImage( pNMTreeView->itemNew.hItem, idxIcon, idxIcon );
		}
		break;
	};
	
	*pResult = 0;
}

void CFolderTreeCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if( pNMTreeView->itemOld.mask & TVIF_PARAM )
		delete (CString*)pNMTreeView->itemOld.lParam;
	if( pNMTreeView->itemOld.hItem == mhtiSelected )
	{
		__super::SelectItem( NULL );
		CommitCurrentItem();
	}
	*pResult = 0;
}

void CFolderTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	if( mbTracking )
	{
		UINT flags ;
		HTREEITEM item = HitTest(point, &flags);
		if (item != NULL)
			__super::SelectItem(item);
		if( flags & TVHT_ONITEMBUTTON )
			mbTracking = false;
		__super::OnLButtonDown(nFlags, point);
	}
}

void CFolderTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	if( mbTracking )
	{
		UINT flags;
		HTREEITEM hti = HitTest( point, &flags );
		if( flags & (TVHT_ONITEM | TVHT_ONITEMINDENT | TVHT_ONITEMRIGHT) )
		{
			if( hti )
				__super::SelectItem(hti);
			ShowWindow(SW_HIDE);
			if( hti )
			{
				CommitCurrentItem();
				NotifySelChange();
			}
			return;
		}
	}
	else
		mbTracking = true;
}

BOOL CFolderTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	switch( pMsg->message )
	{
	case WM_CHAR:
		if( pMsg->wParam > 32 && pMsg->wParam < 128 )
		{
			int nDelay = 3;
			SystemParametersInfo( SPI_GETKEYBOARDDELAY, 0, &nDelay, 0 );
			DWORD nDelayMilliseconds = 250 + (nDelay * 250);
			if( mdwLastCharTime == 0 || ((pMsg->time > mdwLastCharTime? (pMsg->time - mdwLastCharTime) : (pMsg->time + (DWORD(-1) - mdwLastCharTime))) > nDelayMilliseconds) )
				msAutoSearch = (CHAR)pMsg->wParam;
			else
				msAutoSearch += (CHAR)pMsg->wParam;
			mdwLastCharTime = pMsg->time;
			HTREEITEM htiFound = SearchPath( TVI_ROOT, msAutoSearch, false );
			if( htiFound )
			{
				SelectItem( htiFound );
				NotifySelChange();
			}
			return TRUE;
		}
		break;
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		//case VK_DOWN:
		//	CommitNextItem( true, false );
		//	return TRUE;
		//case VK_RIGHT:
		//	CommitNextItem( true, true );
		//	return TRUE;
		//case VK_UP:
		//	CommitNextItem( false, false );
		//	return TRUE;
		//case VK_LEFT:
		//	CommitNextItem( false, true );
		//	return TRUE;
		case VK_RETURN:
			ShowWindow(SW_HIDE);
			CommitCurrentItem();
			NotifySelChange();
			return TRUE;
		case VK_ESCAPE:
			ShowWindow(SW_HIDE);
			return TRUE;
		case VK_TAB:
			ShowWindow(SW_HIDE);
			if( mpActiveWnd )
			{
				pMsg->hwnd = ::GetParent(m_hWnd);
				mpActiveWnd->IsDialogMessage(pMsg);
			}
			return TRUE;
		}
		break;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CFolderTreeCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	if( !bShow )
	{
		if( mpFolderCombo )
			mpFolderCombo->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_CLOSEUP), (LPARAM)mpFolderCombo->m_hWnd );
	}
}

LRESULT CFolderTreeCtrl::OnAcadKeepFocus(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)TRUE;
}

void CFolderTreeCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	switch( nState )
	{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if( mpActiveWnd && pWndOther == mpActiveWnd )
				mpActiveWnd->SendMessage( WM_NCACTIVATE, (WPARAM)TRUE, NULL );
			mpFolderCombo->Invalidate();
			break;
		case WA_INACTIVE:
			if( mpActiveWnd && pWndOther != mpActiveWnd )
				mpActiveWnd->SendMessage( WM_NCACTIVATE, (WPARAM)FALSE, NULL );
			break;
	};
}

BOOL CFolderTreeCtrl::OnNcActivate(BOOL bActive)
{
	if( !bActive )
	{
		ShowWindow(SW_HIDE);
		return TRUE;
	}

	return __super::OnNcActivate(bActive);
}
