// ImageListPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageListPage.h"
#include "PreviewFileDlg.h"
#include "ImageListObject.h"
#include "DclFormObject.h"
#include "PropertyObject.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CImageListPage property page

//IMPLEMENT_DYNCREATE(CImageListPage, CPropertyPage)

CImageListPage::CImageListPage( CDclControlObject* pDclControl )
: CPropertyPage(CImageListPage::IDD)
, mpDclControl( pDclControl )
, mpImageList( new CImageList )
, nCurrentWidth( 0 )
, nCurrentHeight( 0 )
{
	//{{AFX_DATA_INIT(CImageListPage)
	//}}AFX_DATA_INIT
	//m_bImageListCreated = false;
	nCurrentWidth = 0;
	nCurrentHeight = 0;
	RefCountedPtr< CImageListObject > pImageList = pDclControl->GetImageList();
	if( pImageList )
	{
		mpImageList->Create( &pImageList->m_ImageList );
		nCurrentWidth = pImageList->m_ImageSize.cx;
		nCurrentHeight = pImageList->m_ImageSize.cy;
	}
}

CImageListPage::~CImageListPage()
{
}

void CImageListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageListPage)
	DDX_Control(pDX, IDC_EDITHEIGHT, m_DispHeight);
	DDX_Control(pDX, IDC_EDITWIDTH, m_DispWidth);
	DDX_Control(pDX, IDC_PICLIST, m_PicList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImageListPage, CPropertyPage)
	//{{AFX_MSG_MAP(CImageListPage)
	ON_BN_CLICKED(IDC_ADDIMAGE, OnAddimage)
	ON_BN_CLICKED(IDC_REMOVEIMAGE, OnRemoveimage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageListPage message handlers

void CImageListPage::OnAddimage() 
{
	CString sFilter;
	CStringList m_FileList;

	sFilter = theWorkspace.LoadResourceString(IDS_FILTER);

	m_FileList.RemoveAll();

	// create the open dialog box
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		sFilter,
		CWnd::GetActiveWindow());

	// proceed to setup the file buffer size
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	TCHAR* pc = new TCHAR[MAX_PATH];
	BrowseWnd.m_ofn.lpstrFile = pc;
	BrowseWnd.m_ofn.lpstrFile[0] = _T('\0');

	// call method to invoke the file dialog box
	int iReturn = BrowseWnd.DoModal();
		
	
	if(iReturn == IDOK)   
	{
		CString szPathName;
		POSITION pos;

		// do loop to get all selected files
		for (pos = BrowseWnd.GetStartPosition(); pos != NULL; )
		{
			// get the file name 
			szPathName = BrowseWnd.GetNextPathName(pos);
	
			// add the file name to the file list
			m_FileList.AddTail(szPathName);				
			LoadPictureFile(szPathName);
		}
	}

	delete [] pc; 
	
	int nCount = m_FileList.GetCount();
	SetModified();
	GetDlgItem( IDC_REMOVEIMAGE )->EnableWindow( TRUE );
}

void CImageListPage::LoadPictureFile(LPCTSTR szFile)
{
	LPPICTURE		lpPicture;
	lpPicture		= NULL;
	CPictureHolder	phPicture;
	
	// open file
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_ASSERTE(-1 != dwFileSize);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	_ASSERTE(FALSE != bRead);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	_ASSERTE(SUCCEEDED(hr) && pstm);
	
	// Create IPicture from image file
	if (lpPicture)
		lpPicture->Release();
	hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&lpPicture);
	_ASSERTE(SUCCEEDED(hr) && lpPicture);	

	IPicture *ipOld = phPicture.m_pPict;
	phPicture.m_pPict = lpPicture;

	// chad this has to be fixed later
	ImageListAddPicture(phPicture.GetPictureDispatch());
	phPicture.m_pPict = ipOld;
	

	pstm->Release();

}

BOOL CImageListPage::ImageListAddPicture(LPPICTUREDISP iPic)
{
	BOOL bRetVal = TRUE;

	CPictureHolder NewPicture;
	NewPicture.SetPictureDispatch(iPic);
	long lPicHeight = 0;
	long lPicWidth = 0;
	CDC * cdc = GetDC();
	CSize sizePic;
	int nRetVal;


	if (NULL == NewPicture.m_pPict)
	{
		return FALSE;
	}

	// if picture is a bitmap
	if (PICTYPE_BITMAP == NewPicture.GetType())
	{
		HBITMAP hBitmap = NULL;

		// get handle of the bitmap
		NewPicture.m_pPict->get_Handle((OLE_HANDLE FAR *) &hBitmap);

		// get dimensions of bitmap
		NewPicture.m_pPict->get_Width(&lPicWidth);
		NewPicture.m_pPict->get_Height(&lPicHeight);

		sizePic.cx = (int)lPicWidth;
		sizePic.cy = (int)lPicHeight;

		// convert coordinates from units to logical units
		cdc->HIMETRICtoLP(&sizePic);

		// if image list has not been created
		if (NULL == GetImageList().m_hImageList)
		{			
			if (nCurrentWidth == 0)
			{
				nCurrentWidth = sizePic.cx;
				nCurrentHeight = sizePic.cy;
			}

			// create the image list
			bRetVal = GetImageList().Create(sizePic.cx, sizePic.cy, ILC_COLOR8 | ILC_MASK, 0, 1);
			m_PicList.SetImageList(&GetImageList(), TVSIL_NORMAL);
			m_PicList.SetImageList(&GetImageList(), LVSIL_SMALL);
			TCHAR Value[80];
			_ltot(sizePic.cx, Value, 10);
			m_DispWidth.SetWindowText(Value);
			_ltot(sizePic.cy, Value, 10);
			m_DispHeight.SetWindowText(Value);
			// set the background color of the image list
			GetImageList().SetBkColor(RGB(255,255,255));		
			
		}

		// create a temp bitmap
		//CBitmap * TempBmp = CBitmap::FromHandle(hBitmap);

		if (bRetVal)
		{
			// add bitmap to imagelist; mask is ignored in this sample
			nRetVal = GetImageList().Add(
				CBitmap::FromHandle(hBitmap),
				RGB(192, 192, 192) ) ;
			bRetVal = (nRetVal != -1);			
		}
		DeleteObject(hBitmap);
	}
	// else if picture is an icon
	else if (PICTYPE_ICON == NewPicture.GetType())
	{
		HICON hIcon;

		// get handle of the icon
		NewPicture.m_pPict->get_Handle((OLE_HANDLE FAR *) &hIcon);

		// get dimensions of icon
		NewPicture.m_pPict->get_Width(&lPicWidth);
		NewPicture.m_pPict->get_Height(&lPicHeight);

		CDC * cdc = GetDC();

		sizePic.cx = (int)lPicWidth;
		sizePic.cy = (int)lPicHeight;

		// convert coordinates from units to logical units
		cdc->HIMETRICtoLP(&sizePic);

		// if image list has not been created
		if (NULL == GetImageList().m_hImageList)
		{
			if (nCurrentWidth == 0)
			{
				nCurrentWidth = sizePic.cx;
				nCurrentHeight = sizePic.cy;
			}
			
			// create the image list
			bRetVal = GetImageList().Create(sizePic.cx, sizePic.cy, ILC_COLOR8 | ILC_MASK, 1, 1);
			m_PicList.SetImageList(&GetImageList(), TVSIL_NORMAL);
			m_PicList.SetImageList(&GetImageList(), LVSIL_SMALL);
			TCHAR Value[80];
			_ltot(sizePic.cx, Value, 10);
			m_DispWidth.SetWindowText(Value);
			_ltot(sizePic.cy, Value, 10);
			m_DispHeight.SetWindowText(Value);
			// set the background color of the image list
			GetImageList().SetBkColor(RGB(255,255,255));		
			
		}
		
		if (bRetVal)
		{
			// add icon to image list
			nRetVal = GetImageList().Add(hIcon);
			bRetVal = (nRetVal != -1);
		}
		//DestroyIcon(hIcon);		
	}
	else
	{
		bRetVal = FALSE;
		return bRetVal;
	}
	
	int nImage = GetImageList().GetImageCount()-1;
	int nPicIndex = m_PicList.GetItemCount();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT;
	lvItem.iItem = nImage;
	lvItem.iSubItem = 0;
	TCHAR sValue [256];
	_ltot(nPicIndex, sValue, 10);
	lvItem.pszText = sValue;
	lvItem.iImage = nImage;
	lvItem.iIndent = -1;

	// insert the row item
	int nInsertIndex = m_PicList.InsertItem(&lvItem);
	m_PicList.SetItemData(nPicIndex, nImage);
		
	
	
	m_PicList.SetIconSpacing(nCurrentWidth + 16);
	return (bRetVal);
}



void CImageListPage::OnRemoveimage() 
{
	int nItem=-1;
	POSITION pos = m_PicList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	while (pos)
	{
		nItem = m_PicList.GetNextSelectedItem(pos);
		if (nItem != -1)
			m_PicList.DeleteItem(nItem);
		GetImageList().Remove(nItem);
	}
	

	m_PicList.Arrange(LVA_ALIGNLEFT);
	for (int i=0; i < m_PicList.GetItemCount(); i++)
	{
		TCHAR Value[80];
		_ltot(i, Value, 10);

		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;	
		lvItem.iImage = i;	
		lvItem.pszText = Value;

		m_PicList.SetItem(&lvItem);		
	}
	
	if (m_PicList.GetItemCount() == 0)
	{
		m_DispHeight.SetWindowText(_T(""));
		m_DispWidth.SetWindowText(_T(""));
		mpImageList = new CImageList;
		GetDlgItem( IDC_REMOVEIMAGE )->EnableWindow( FALSE );
	}

	if (nItem >= m_PicList.GetItemCount())
		nItem = m_PicList.GetItemCount() - 1;
	if (nItem >= 0)
	{
		m_PicList.SetFocus();
		m_PicList.SetItem(nItem, 0, LVIF_STATE, NULL, nItem, LVIS_SELECTED, LVIS_SELECTED, 0);
		m_PicList.EnsureVisible(nItem, TRUE);
		m_PicList.Arrange(LVA_ALIGNLEFT);
	}

	SetModified();
}

BOOL CImageListPage::OnApply() 
{
	if( mpImageList && mpImageList->m_hImageList && mpImageList->GetImageCount() > 0 )
	{
		CImageListObject* pNewImageList = new CImageListObject;
		pNewImageList->m_ImageSize.SetSize( nCurrentWidth, nCurrentHeight );
		pNewImageList->m_ImageList.Attach( ImageList_Duplicate( mpImageList->m_hImageList ) );
		mpDclControl->SetImageList( pNewImageList );
	}
	else
		mpDclControl->SetImageList( NULL );

	return CPropertyPage::OnApply();
}

BOOL CImageListPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (GetImageList().m_hImageList != NULL)
	{
		TCHAR sValue [80];
		_ltot(nCurrentWidth, sValue, 10);
		m_DispWidth.SetWindowText(sValue);
		_ltot(nCurrentHeight, sValue, 10);
		m_DispHeight.SetWindowText(sValue);

		m_PicList.SetImageList(&GetImageList(), TVSIL_NORMAL);
		m_PicList.SetImageList(&GetImageList(), LVSIL_SMALL);
		
		//m_PicList.SetIconSpacing(16);
		for (int i=0; i < GetImageList().GetImageCount(); i++)
		{
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT;
			lvItem.iItem = i;
			lvItem.iSubItem = 0;
			
			_ltot(i, sValue, 10);
			lvItem.pszText = sValue;
			lvItem.iImage = i;
			lvItem.iIndent = -1;

			// insert the row item
			int nInsertIndex = m_PicList.InsertItem(&lvItem);
			m_PicList.SetItemData(i, i);
		}
	}
	else
		GetDlgItem( IDC_REMOVEIMAGE )->EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
