///////////////////////////////////////////////////////////////////////////
// FolderBrowseDlg.cpp: implementation of the CFolderBrowseDlg class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FolderBrowseDlg.h"
#include "shlobj.h"


// Callback function called by SHBrowseForFolder's browse control
// after initialization and when selection changes
int __stdcall CFolderBrowseDlg::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  CFolderBrowseDlg* pDirDialogObj = (CFolderBrowseDlg*)lpData;
  if (uMsg == BFFM_INITIALIZED )
  {
		::SetForegroundWindow(hwnd);
    if( ! pDirDialogObj->msInitialFolder.IsEmpty() )
      ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pDirDialogObj->msInitialFolder));
  }
  else if( uMsg == BFFM_SELCHANGED )
  {
    LPITEMIDLIST pidl = (LPITEMIDLIST) lParam;
    TCHAR selection[MAX_PATH];
    if( ! ::SHGetPathFromIDList(pidl, selection) )
      selection[0] = _T('\0');

    CString csStatusText;
    BOOL bOk = pDirDialogObj->SelChanged(selection, csStatusText);

    ::SendMessage(hwnd, BFFM_ENABLEOK, 0, bOk);
  }
  return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFolderBrowseDlg::CFolderBrowseDlg( LPCTSTR pszCaption,
												LPCTSTR pszInitialFolder /*= NULL*/,
												LPCTSTR pszRootFolder /*= NULL*/,
												DWORD dwFlags /*= BIF_RETURNONLYFSDIRS*/ )
: msCaption( pszCaption )
, msInitialFolder( pszInitialFolder )
, msRootFolder( pszRootFolder )
, mdwFlags( dwFlags )
{
}

CFolderBrowseDlg::~CFolderBrowseDlg()
{
}


BOOL CFolderBrowseDlg::DoBrowse(CWnd *pwndParent)
{

	if( !msInitialFolder.IsEmpty() )
	{
		msInitialFolder.TrimRight();
		if( msInitialFolder.Right(1) == _T('\\') || msInitialFolder.Right(1) == _T('/'))
			msInitialFolder = msInitialFolder.Left(msInitialFolder.GetLength() - 1);
	}

	LPMALLOC pMalloc;
	if (SHGetMalloc (&pMalloc)!= NOERROR)
		return FALSE;

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));

	bInfo.ulFlags = mdwFlags;

	if (!msRootFolder.IsEmpty ())
  {
    ULONG         chEaten;
    ULONG         dwAttributes;
    HRESULT       hr;
    LPSHELLFOLDER pDesktopFolder;
    //
    // Get a pointer to the Desktop's IShellFolder interface.
    //
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
    {
      //
      // IShellFolder::ParseDisplayName requires the file name be in Unicode.
      //
      //
      // Convert the path to an ITEMIDLIST.
      //
      hr = pDesktopFolder->ParseDisplayName(NULL,
                                            NULL,
                                            CStringW( msRootFolder ).LockBuffer(),
                                            &chEaten,
                                            &pidl,
                                            &dwAttributes);
      if (FAILED(hr))
      {
        pMalloc ->Free (pidl);
        pMalloc ->Release ();
        return FALSE;
      }
      bInfo.pidlRoot = pidl;
    }
  }
  bInfo.hwndOwner = pwndParent == NULL ? NULL : pwndParent->GetSafeHwnd();
	TCHAR szDisplayName[MAX_PATH];
  bInfo.pszDisplayName = szDisplayName;
	CString sTitle( msCaption );
	if (!sTitle.IsEmpty())
	{
		bInfo.ulFlags |= BIF_STATUSTEXT;
		bInfo.lpszTitle = sTitle;
	}

  bInfo.lpfn = BrowseCtrlCallback;  // address of callback function
  bInfo.lParam = (LPARAM)this;      // pass address of object to callback function

  if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
    return FALSE;

  ::SHGetPathFromIDList(pidl, msSelectedFolder.GetBuffer(MAX_PATH));
  msSelectedFolder.ReleaseBuffer();

  pMalloc ->Free(pidl);
  pMalloc ->Release();

  return TRUE;
}