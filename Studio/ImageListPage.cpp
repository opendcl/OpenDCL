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
, mnHeight( 0 )
, mnWidth( 0 )
{
	TImageListPtr pImageList = pDclControl->GetImageList();
	if( pImageList && pImageList->GetImageList().GetSafeHandle() )
		mpImageList = new CDclImageList( *pImageList );
	else
		mpImageList = new CDclImageList();
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

BOOL CImageListPage::ImageListAddPicture(LPPICTUREDISP iPic)
{
	bool bCreating = (mpImageList->IsNull());
	int idxPic = mpImageList->AddPicture( GetDC(), iPic );
	if( bCreating && !mpImageList->IsNull() )
	{
		const CSize& sizeImage = mpImageList->GetSize();
		mnWidth = sizeImage.cx;
		mnHeight = sizeImage.cy;
		mPicList.SetIconSpacing( mnWidth + 16 );
		mPicList.SetImageList( &mpImageList->GetImageList(), LVSIL_NORMAL );
		mPicList.SetImageList( &mpImageList->GetImageList(), LVSIL_SMALL );
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
	if( mpImageList->IsNull() )
		return FALSE;
	if( !mpImageList->ReplacePicture( idxPic, GetDC(), iPic ) )
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
	mPicList.SetItem( &lvItem );
	mPicList.Invalidate();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CImageListPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ADDIMAGE, OnAddimage)
	ON_BN_CLICKED(IDC_REMOVEIMAGE, OnRemoveimage)
	ON_BN_CLICKED(IDC_CHANGEIMAGE, OnChangeimage)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

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
		mpImageList->DeletePicture(*iter);
	mPicList.DeleteAllItems();
	for( int idx = 0; idx < mpImageList->GetCount(); ++idx )
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
		mpImageList = new CDclImageList;
		mPicList.SetImageList( &mpImageList->GetImageList(), LVSIL_NORMAL );
		mPicList.SetImageList( &mpImageList->GetImageList(), LVSIL_SMALL );
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
	if( mpImageList && mpImageList->GetCount() > 0 )
		mpDclControl->SetImageList( mpImageList );
	else
		mpDclControl->SetImageList( NULL );
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::ImageList);
	return CPropertyPage::OnApply();
}

BOOL CImageListPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if( !mpImageList->IsNull() )
	{
		CSize sizeImage = mpImageList->GetSize();
		mnWidth = sizeImage.cx;
		mnHeight = sizeImage.cy;
		mPicList.SetIconSpacing( mnWidth + 16 );
		mPicList.SetImageList( &mpImageList->GetImageList(), LVSIL_NORMAL );
		mPicList.SetImageList( &mpImageList->GetImageList(), LVSIL_SMALL );
		for( int idx = 0; idx < mpImageList->GetCount(); ++idx )
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
		GetDlgItem(IDC_EDITWIDTH)->EnableWindow( (mnWidth < 0) );
		GetDlgItem(IDC_EDITHEIGHT)->EnableWindow( (mnHeight < 0) );
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

void CImageListPage::OnDropFiles(HDROP hDropInfo)
{
	UINT ctFiles = DragQueryFile( hDropInfo, (UINT)-1, NULL, 0 );
	if( ctFiles == 0 )
		return;

	for( UINT nIdx = 0; nIdx < ctFiles; ++nIdx )
	{
		TCHAR szFile[MAX_PATH + 1];
		UINT cchFilename = DragQueryFile( hDropInfo, nIdx, szFile, MAX_PATH );
		if( cchFilename > 0 )
		{
			CDclPicture pic( -1, szFile );
			if( !pic.IsValid() )
				continue;
			ImageListAddPicture( pic.GetPictureDisp() );
		}
	}
	if( mPicList.GetItemCount() > 0 )
	{
		GetDlgItem(IDC_EDITWIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITHEIGHT)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
	SetModified();
	OnSelchange();
}
