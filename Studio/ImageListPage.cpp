// ImageListPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageListPage.h"
#include "PreviewFileDlg.h"
#include "DclImageList.h"
#include "DclFormTemplate.h"
#include "PropertyObject.h"
#include "StudioDialogControl.h"
#include "Workspace.h"
#include <list>


/////////////////////////////////////////////////////////////////////////////
// CImageListPage property page

CImageListPage::CImageListPage( TDclControlPtr pDclControl )
: CPropertyPage(CImageListPage::IDD)
, mpDclControl( pDclControl )
, mpImageList( new CImageList )
, mnHeight( 0 )
, mnWidth( 0 )
{
	TImageListPtr pImageList = pDclControl->GetImageList();
	if( pImageList && pImageList->GetImageList().GetSafeHandle() )
		mpImageList->Create( &pImageList->GetImageList() );
}

CImageListPage::~CImageListPage()
{
}

void CImageListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITHEIGHT, mnHeight);
	DDX_Text(pDX, IDC_EDITWIDTH, mnWidth);
	DDX_Control(pDX, IDC_PICLIST, mPicList);
}


BEGIN_MESSAGE_MAP(CImageListPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ADDIMAGE, OnAddimage)
	ON_BN_CLICKED(IDC_REMOVEIMAGE, OnRemoveimage)
	ON_BN_CLICKED(IDC_CHANGEIMAGE, OnChangeimage)
END_MESSAGE_MAP()

BOOL CImageListPage::ImageListAddPicture(LPPICTUREDISP iPic)
{
	CPictureHolder NewPicture;
	NewPicture.SetPictureDispatch( iPic );
	if( !NewPicture.m_pPict )
		return FALSE;

	long lPicWidth;
	long lPicHeight;
	NewPicture.m_pPict->get_Width( &lPicWidth );
	NewPicture.m_pPict->get_Height( &lPicHeight );
	CSize sizePic( (int)lPicWidth, (int)lPicHeight );
	GetDC()->HIMETRICtoLP(&sizePic); //convert coordinates from units to logical units

	if( !GetImageList()->m_hImageList )
	{ // create the image list
		if( mnWidth < 0 )
			mnWidth = sizePic.cx;
		if( mnHeight < 0 )
			mnHeight = sizePic.cy;
		if( !GetImageList()->Create( mnWidth, mnHeight, ILC_COLOR32 | ILC_MASK, 1, 1 ) )
			return FALSE;
		mPicList.SetIconSpacing( mnWidth + 16 );
		mPicList.SetImageList( GetImageList(), LVSIL_NORMAL );
		mPicList.SetImageList( GetImageList(), LVSIL_SMALL );
	}

	int idxPic = -1;
	if( mnWidth == sizePic.cx && mnHeight == sizePic.cy )
	{
		switch( NewPicture.GetType() )
		{
		case PICTYPE_BITMAP:
			{
				HBITMAP hBitmap = NULL;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hBitmap );
				idxPic = GetImageList()->Add( CBitmap::FromHandle(hBitmap), RGB(192, 192, 192) ) ;
			}
			break;
		case PICTYPE_ICON:
			{
				HICON hIcon;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hIcon );
				idxPic = GetImageList()->Add( hIcon ) ;
			}
			break;
		}
	}
	if (idxPic < 0 )
	{
		CDC dcImage;
		dcImage.CreateCompatibleDC( GetDC() );
		CBitmap bitmapImage;
		bitmapImage.CreateBitmap( mnWidth, mnHeight, 1, 32, NULL );
		CBitmap* pOldBitmap = dcImage.SelectObject( &bitmapImage );
		dcImage.FillSolidRect(0, 0, mnWidth, mnHeight, RGB(192, 192, 192) );
		dcImage.SetBkColor( RGB(192, 192, 192) );
		CRect rcImage( 0, 0, mnWidth, mnHeight );
		NewPicture.Render( &dcImage, &rcImage, &rcImage );
		dcImage.SelectObject( pOldBitmap );
		idxPic = GetImageList()->Add( &bitmapImage, RGB(192, 192, 192) ) ;
	}
	if( idxPic < 0 )
		return FALSE;

	CString sIndex;
	sIndex.Format( _T("%d"), idxPic );
	LV_ITEM lvItem =
		{ LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT,
			idxPic,
			0,
			0,
			0,
			sIndex.LockBuffer(),
			-1,
			idxPic,
			NULL,
			-1,
		};
	mPicList.InsertItem( &lvItem );
	mPicList.Invalidate();
	return TRUE;
}

BOOL CImageListPage::ImageListReplacePicture( int idxPic, LPPICTUREDISP iPic )
{
	if( idxPic < 0 )
		return FALSE;
	if( !GetImageList()->m_hImageList )
		return FALSE;
	CPictureHolder NewPicture;
	NewPicture.SetPictureDispatch( iPic );
	if( !NewPicture.m_pPict )
		return FALSE;

	long lPicWidth;
	long lPicHeight;
	NewPicture.m_pPict->get_Width( &lPicWidth );
	NewPicture.m_pPict->get_Height( &lPicHeight );
	CSize sizePic( (int)lPicWidth, (int)lPicHeight );
	GetDC()->HIMETRICtoLP(&sizePic); //convert coordinates from units to logical units

	int idxReplacedPic = -1;
	if( mnWidth == sizePic.cx && mnHeight == sizePic.cy )
	{
		switch( NewPicture.GetType() )
		{
		case PICTYPE_BITMAP:
			{
				HBITMAP hBitmap = NULL;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hBitmap );
				idxReplacedPic = GetImageList()->Replace( idxPic, CBitmap::FromHandle(hBitmap), NULL ) ;
			}
			break;
		case PICTYPE_ICON:
			{
				HICON hIcon;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hIcon );
				idxReplacedPic = GetImageList()->Replace( idxPic, hIcon ) ;
			}
			break;
		}
	}
	if (idxReplacedPic < 0 )
	{
		CDC dcImage;
		dcImage.CreateCompatibleDC( GetDC() );
		CBitmap bitmapImage;
		bitmapImage.CreateBitmap( mnWidth, mnHeight, 1, 32, NULL );
		CBitmap* pOldBitmap = dcImage.SelectObject( &bitmapImage );
		dcImage.FillSolidRect(0, 0, mnWidth, mnHeight, RGB(192, 192, 192) );
		dcImage.SetBkColor( RGB(192, 192, 192) );
		CRect rcImage( 0, 0, mnWidth, mnHeight );
		NewPicture.Render( &dcImage, &rcImage, &rcImage );
		dcImage.SelectObject( pOldBitmap );
		idxReplacedPic = GetImageList()->Replace( idxPic, &bitmapImage, NULL ) ;
	}
	if( idxReplacedPic < 0 )
		return FALSE;

	CString sIndex;
	sIndex.Format( _T("%d"), idxReplacedPic );
	LV_ITEM lvItem =
		{ LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT,
			idxReplacedPic,
			0,
			0,
			0,
			sIndex.LockBuffer(),
			-1,
			idxReplacedPic,
			NULL,
			-1,
		};
	mPicList.SetItem( &lvItem );
	mPicList.Invalidate();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImageListPage message handlers

void CImageListPage::OnSelchange() 
{
	int ctSel = mPicList.GetSelectedCount();
	GetDlgItem( IDC_REMOVEIMAGE )->EnableWindow( (ctSel > 0)? TRUE : FALSE );
	GetDlgItem( IDC_CHANGEIMAGE )->EnableWindow( (ctSel == 1)? TRUE : FALSE );
}

void CImageListPage::OnAddimage() 
{
	UpdateData();
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
			CDclPicture pic( -1, sPathName );
			ImageListAddPicture( pic.GetPictureDisp() );
		}
		if( mPicList.GetItemCount() > 0 )
		{
			GetDlgItem(IDC_EDITWIDTH)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDITHEIGHT)->EnableWindow(FALSE);
		}
	}
	UpdateData(FALSE);
	SetModified();
	OnSelchange();
}

void CImageListPage::OnRemoveimage() 
{
	int nItem=-1;
	POSITION pos = mPicList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	std::list< int > listToRemove;
	while (pos)
	{
		nItem = mPicList.GetNextSelectedItem(pos);
		if (nItem != -1)
			listToRemove.push_back( nItem );
	}
	listToRemove.sort();
	for (std::list< int >::const_reverse_iterator iter = listToRemove.rbegin(); iter != listToRemove.rend(); ++iter)
		GetImageList()->Remove(*iter);
	mPicList.DeleteAllItems();
	for( int idx = 0; idx < GetImageList()->GetImageCount(); ++idx )
	{
		CString sIndex;
		sIndex.Format( _T("%d"), idx );
		LV_ITEM lvItem =
			{ LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT,
				idx,
				0,
				0,
				0,
				sIndex.LockBuffer(),
				-1,
				idx,
				NULL,
				-1,
			};
		mPicList.InsertItem( &lvItem );
	}
	
	if (mPicList.GetItemCount() == 0)
	{
		GetDlgItem(IDC_EDITWIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDITHEIGHT)->EnableWindow(TRUE);
		mpImageList = new CImageList;
		UpdateData(FALSE);
	}

	if (nItem >= mPicList.GetItemCount())
		nItem = mPicList.GetItemCount() - 1;
	if (nItem >= 0)
	{
		mPicList.SetFocus();
		mPicList.SetItem(nItem, 0, LVIF_STATE, NULL, nItem, LVIS_SELECTED, LVIS_SELECTED, 0);
		mPicList.EnsureVisible(nItem, TRUE);
		mPicList.Arrange(LVA_ALIGNLEFT);
	}

	SetModified();
	OnSelchange();
}

void CImageListPage::OnChangeimage() 
{
	int nItem=-1;
	POSITION pos = mPicList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	nItem = mPicList.GetNextSelectedItem( pos );
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
		CDclPicture pic( -1, sPathName );
		ImageListReplacePicture( nItem, pic.GetPictureDisp() );
	}

	mPicList.EnsureVisible(nItem, TRUE);
	mPicList.Arrange(LVA_ALIGNLEFT);
	SetModified();
	OnSelchange();
}

BOOL CImageListPage::OnApply() 
{
	if( mpImageList && mpImageList->m_hImageList && mpImageList->GetImageCount() > 0 )
		mpDclControl->SetImageList( new CDclImageList( mpImageList ) );
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
		ImageList_GetIconSize( GetImageList()->m_hImageList, &mnWidth, &mnHeight );
		mPicList.SetImageList( GetImageList(), TVSIL_NORMAL );
		mPicList.SetImageList( GetImageList(), LVSIL_SMALL );
		mPicList.SetIconSpacing( mnWidth + 16 );
		for( int idx = 0; idx < GetImageList()->GetImageCount(); ++idx )
		{
			CString sIndex;
			sIndex.Format( _T("%d"), idx );
			LV_ITEM lvItem =
				{ LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT,
					idx,
					0,
					0,
					0,
					sIndex.LockBuffer(),
					-1,
					idx,
					NULL,
					-1,
				};
			mPicList.InsertItem( &lvItem );
		}
		BOOL bVirgin = (GetImageList()->GetImageCount() <= 0);
		GetDlgItem(IDC_EDITWIDTH)->EnableWindow(bVirgin);
		GetDlgItem(IDC_EDITHEIGHT)->EnableWindow(bVirgin);
		UpdateData( FALSE );
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
