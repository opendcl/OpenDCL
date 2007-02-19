///////////////////////////////////////////////////////////////////////////
// DirDialog.cpp: implementation of the CDirDialog class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirDialog.h"
#include "shlobj.h"


// Callback function called by SHBrowseForFolder's browse control
// after initialization and when selection changes
int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    CDirDialog* pDirDialogObj = (CDirDialog*)lpData;
    if (uMsg == BFFM_INITIALIZED )
    {
        if( ! pDirDialogObj->m_strSelDir.IsEmpty() )
            ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pDirDialogObj->m_strSelDir));
        if( ! pDirDialogObj->m_strWindowTitle.IsEmpty() )
            ::SetWindowText(hwnd, (LPCTSTR) pDirDialogObj->m_strWindowTitle);
    }
    else if( uMsg == BFFM_SELCHANGED )
    {
        LPITEMIDLIST pidl = (LPITEMIDLIST) lParam;
        TCHAR selection[MAX_PATH];
        if( ! ::SHGetPathFromIDList(pidl, selection) )
            selection[0] = '\0';

        CString selectionCString = selection;

        CString csStatusText;
        BOOL bOk = pDirDialogObj->SelChanged((LPCSTR)(LPCTSTR)selectionCString, csStatusText);

        if( pDirDialogObj->m_bStatus )
            ::SendMessage(hwnd, BFFM_SETSTATUSTEXT , 0, (LPARAM)(LPCSTR)(LPCTSTR)csStatusText);

        ::SendMessage(hwnd, BFFM_ENABLEOK, 0, bOk);
    }
  return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirDialog::CDirDialog()
{
	m_bStatus = FALSE;
}

CDirDialog::~CDirDialog()
{
}

const TCHAR sSlash1[] = _T("\\");
const TCHAR sSlash2[] = _T("//");


BOOL CDirDialog::DoBrowse(CWnd *pwndParent)
{

	if( ! m_strSelDir.IsEmpty() )
	{
		m_strSelDir.TrimRight();
		if( m_strSelDir.Right(1) == sSlash1 || m_strSelDir.Right(1) == sSlash2)
				m_strSelDir = m_strSelDir.Left(m_strSelDir.GetLength() - 1);
	}

	LPMALLOC pMalloc;
	if (SHGetMalloc (&pMalloc)!= NOERROR)
		return FALSE;

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));

	if (!m_strInitDir.IsEmpty ())
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
                                            CStringW( m_strInitDir ).LockBuffer(),
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
  bInfo.pszDisplayName = m_strPath.GetBuffer (MAX_PATH);
	if (!m_strTitle.IsEmpty())
		bInfo.lpszTitle = m_strTitle;
	bInfo.ulFlags = BIF_RETURNFSANCESTORS |
                  BIF_RETURNONLYFSDIRS |
                  (!m_strTitle.IsEmpty() ? BIF_STATUSTEXT : 0);

  bInfo.lpfn = BrowseCtrlCallback;  // address of callback function
  bInfo.lParam = (LPARAM)this;      // pass address of object to callback function

  if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
    return FALSE;
  m_strPath.ReleaseBuffer();
  m_iImageIndex = bInfo.iImage;

  if (::SHGetPathFromIDList(pidl, m_strPath.GetBuffer(MAX_PATH)) == FALSE)
  {
    pMalloc ->Free(pidl);
    pMalloc ->Release();
    return FALSE;
  }

  m_strPath.ReleaseBuffer();

  pMalloc ->Free(pidl);
  pMalloc ->Release();

  return TRUE;
}