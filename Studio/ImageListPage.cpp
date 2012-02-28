// ImageListPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageListPage.h"
#include "PreviewFileDlg.h"
#include "ImageListObject.h"
#include "DclFormObject.h"
#include "PropertyObject.h"
#include "StudioDialogControl.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CImageListPage property page

CImageListPage::CImageListPage( TDclControlPtr pDclControl )
: CPropertyPage(CImageListPage::IDD)
, mpDclControl( pDclControl )
, mpImageList( new CImageList )
, nCurrentWidth( 0 )
, nCurrentHeight( 0 )
{
	TImageListPtr pImageList = pDclControl->GetImageList();
	if( pImageList && pImageList->GetImageList().GetSafeHandle() )
	{
		mpImageList->Create( &pImageList->GetImageList() );
		const CSize& szImage = pImageList->GetSize();
		nCurrentWidth = szImage.cx;
		nCurrentHeight = szImage.cy;
	}
}

CImageListPage::~CImageListPage()
{
}

void CImageListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITHEIGHT, m_DispHeight);
	DDX_Control(pDX, IDC_EDITWIDTH, m_DispWidth);
	DDX_Control(pDX, IDC_PICLIST, m_PicList);
}


BEGIN_MESSAGE_MAP(CImageListPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ADDIMAGE, OnAddimage)
	ON_BN_CLICKED(IDC_REMOVEIMAGE, OnRemoveimage)
	ON_BN_CLICKED(IDC_CHANGEIMAGE, OnChangeimage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageListPage message handlers

void CImageListPage::OnSelchange() 
{
	int nSelCount = m_PicList.GetSelectedCount();
	GetDlgItem( IDC_REMOVEIMAGE )->EnableWindow( (nSelCount > 0)? TRUE : FALSE );
	GetDlgItem( IDC_CHANGEIMAGE )->EnableWindow( (nSelCount == 1)? TRUE : FALSE );
}

void CImageListPage::OnAddimage() 
{
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		theWorkspace.LoadResourceString(IDS_IMAGEFILEFILTER),
		this );

	TCHAR szFileBuf[8192] = _T("");
	BrowseWnd.m_ofn.nMaxFile = _elements(szFileBuf);
	BrowseWnd.m_ofn.lpstrFile = szFileBuf;

	if( IDOK == BrowseWnd.DoModal() )   
	{
		POSITION pos = BrowseWnd.GetStartPosition();
		while( pos )
		{
			CString sPathName = BrowseWnd.GetNextPathName( pos );
			CPictureObject pic( -1, sPathName );
			ImageListAddPicture( pic.GetPictureDisp() );
		}
	}
	SetModified();
	OnSelchange();
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
		if (NULL == GetImageList()->m_hImageList)
		{			
			if (nCurrentWidth == 0)
			{
				nCurrentWidth = sizePic.cx;
				nCurrentHeight = sizePic.cy;
			}

			// create the image list
			bRetVal = GetImageList()->Create( sizePic.cx, sizePic.cy, ILC_COLOR32 | ILC_MASK, 1, 1 );
			m_PicList.SetImageList(GetImageList(), TVSIL_NORMAL);
			m_PicList.SetImageList(GetImageList(), LVSIL_SMALL);
			TCHAR Value[80];
			_ltot(sizePic.cx, Value, 10);
			m_DispWidth.SetWindowText(Value);
			_ltot(sizePic.cy, Value, 10);
			m_DispHeight.SetWindowText(Value);
		}
		if (bRetVal)
		{
			// add bitmap to imagelist; mask is ignored in this sample
			nRetVal = GetImageList()->Add( CBitmap::FromHandle(hBitmap), RGB(192, 192, 192) ) ;
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
		if (NULL == GetImageList()->m_hImageList)
		{
			if (nCurrentWidth == 0)
			{
				nCurrentWidth = sizePic.cx;
				nCurrentHeight = sizePic.cy;
			}
			
			// create the image list
			bRetVal = GetImageList()->Create( sizePic.cx, sizePic.cy, ILC_COLOR32 | ILC_MASK, 1, 1 );
			m_PicList.SetImageList(GetImageList(), TVSIL_NORMAL);
			m_PicList.SetImageList(GetImageList(), LVSIL_SMALL);
			TCHAR Value[80];
			_ltot(sizePic.cx, Value, 10);
			m_DispWidth.SetWindowText(Value);
			_ltot(sizePic.cy, Value, 10);
			m_DispHeight.SetWindowText(Value);
		}
		
		if (bRetVal)
		{
			// add icon to image list
			nRetVal = GetImageList()->Add(hIcon);
			bRetVal = (nRetVal != -1);
		}
		//DestroyIcon(hIcon);		
	}
	else
	{
		bRetVal = FALSE;
		return bRetVal;
	}
	
	int nImage = GetImageList()->GetImageCount()-1;
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
	m_PicList.InsertItem(&lvItem);
	m_PicList.SetItemData(nPicIndex, nImage);
	m_PicList.SetIconSpacing(nCurrentWidth + 16);
	return (bRetVal);
}

BOOL CImageListPage::ImageListReplacePicture( int idxPic, LPPICTUREDISP iPic )
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
		if (NULL == GetImageList()->m_hImageList)
			return FALSE;
		nRetVal = GetImageList()->Replace( idxPic, CBitmap::FromHandle( hBitmap ), NULL ) ;
		bRetVal = (nRetVal != -1);			
		DeleteObject( hBitmap );
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
		if (NULL == GetImageList()->m_hImageList)
			return FALSE;
		
		// add icon to image list
		nRetVal = GetImageList()->Replace( idxPic, hIcon );
		bRetVal = (nRetVal != -1);
		//DestroyIcon(hIcon);		
	}
	else
	{
		bRetVal = FALSE;
		return bRetVal;
	}

	LV_ITEM lvItem;
	lvItem.mask = LVIF_IMAGE;
	lvItem.iItem = idxPic;
	lvItem.iSubItem = 0;
	lvItem.iImage = idxPic;
	m_PicList.SetItem( &lvItem );
	m_PicList.Invalidate();
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
		GetImageList()->Remove(nItem);
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
	OnSelchange();
}

void CImageListPage::OnChangeimage() 
{
	int nItem=-1;
	POSITION pos = m_PicList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	nItem = m_PicList.GetNextSelectedItem( pos );
	if( nItem < 0 )
		return;

	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		theWorkspace.LoadResourceString(IDS_IMAGEFILEFILTER),
		this );


	TCHAR szFileBuf[8192] = _T("");
	BrowseWnd.m_ofn.nMaxFile = _elements(szFileBuf);
	BrowseWnd.m_ofn.lpstrFile = szFileBuf;

	if( IDOK == BrowseWnd.DoModal() )   
	{
		POSITION pos = BrowseWnd.GetStartPosition();
		CString sPathName = BrowseWnd.GetNextPathName( pos );
		CPictureObject pic( -1, sPathName );
		ImageListReplacePicture( nItem, pic.GetPictureDisp() );
	}

	m_PicList.EnsureVisible(nItem, TRUE);
	m_PicList.Arrange(LVA_ALIGNLEFT);
	SetModified();
	OnSelchange();
}

BOOL CImageListPage::OnApply() 
{
	if( mpImageList && mpImageList->m_hImageList && mpImageList->GetImageCount() > 0 )
		mpDclControl->SetImageList( new CImageListObject( mpImageList ) );
	else
		mpDclControl->SetImageList( NULL );
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::ImageList);
	return CPropertyPage::OnApply();
}

BOOL CImageListPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (GetImageList()->m_hImageList != NULL)
	{
		TCHAR sValue [80];
		_ltot(nCurrentWidth, sValue, 10);
		m_DispWidth.SetWindowText(sValue);
		_ltot(nCurrentHeight, sValue, 10);
		m_DispHeight.SetWindowText(sValue);

		m_PicList.SetImageList(GetImageList(), TVSIL_NORMAL);
		m_PicList.SetImageList(GetImageList(), LVSIL_SMALL);
		
		//m_PicList.SetIconSpacing(16);
		for (int i=0; i < GetImageList()->GetImageCount(); i++)
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
			m_PicList.InsertItem(&lvItem);
			m_PicList.SetItemData(i, i);
		}
	}
	OnSelchange();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CImageListPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( wParam == IDC_PICLIST && ((NMHDR*)lParam)->code == LVN_ITEMCHANGED )
		OnSelchange();

	return __super::OnNotify(wParam, lParam, pResult);
}
