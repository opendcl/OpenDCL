// ColumnsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ColumnsPage.h"
#include "PropertyObject.h"
#include "ImageListPage.h"
#include "ImageListContents.h"
#include "DropListContents.h"
#include "Workspace.h"
#include "Resource.h"
#include "StudioDialogControl.h"


CColumnData::CColumnData()
{
	m_Width = 0;	
	m_Image = 0;	
	m_Style = 0;	
	m_Alignment = 0;	
	m_DefImage = 0;	
	m_AltImage = 0;
}

CColumnData::~CColumnData()
{
}

CColumnData& CColumnData::operator=(const CColumnData &Other)
{
	m_Width = Other.m_Width;	
	m_Image = Other.m_Image;	
	m_Style = Other.m_Style;	
	m_Alignment = Other.m_Alignment;	
	m_DefImage = Other.m_DefImage;	
	m_AltImage = Other.m_AltImage;
	return *this;
}

CColumnData::CColumnData(const CColumnData &Other)
{
	m_Width = Other.m_Width;	
	m_Image = Other.m_Image;	
	m_Style = Other.m_Style;	
	m_Alignment = Other.m_Alignment;	
	m_DefImage = Other.m_DefImage;	
	m_AltImage = Other.m_AltImage;
}


/////////////////////////////////////////////////////////////////////////////
// CColumnsPage property page

CColumnsPage::CColumnsPage( TDclControlPtr pDclControl, CImageListPage* pImageListPage )
: CPropertyPage(CColumnsPage::IDD)
, mpDclControl( pDclControl )
, mpImageListPage( pImageListPage )
{
	m_bChangingIndex = false;
	m_nIndex = -1;
	m_bShowStyles = (pDclControl->GetPropertyObject(Prop::ListViewStyle) == NULL);
	m_pColCaptions = pDclControl->GetPropertyObject(Prop::ColumnCaptions);
	m_pColWidths = pDclControl->GetPropertyObject(Prop::ColumnWidths);
	m_pColImages = pDclControl->GetPropertyObject(Prop::ColumnImages);
	m_pColStyles = pDclControl->GetPropertyObject(Prop::ColumnStyles);
	m_pColAlignment = pDclControl->GetPropertyObject(Prop::ColumnAlignments);
	m_pColDefault = pDclControl->GetPropertyObject(Prop::ColumnDefaultImages);
	m_pColAlternate = pDclControl->GetPropertyObject(Prop::ColumnAlternateImages);
	m_pColListItems = pDclControl->GetPropertyObject(Prop::ColumnListItems);
	m_pColImageItems = pDclControl->GetPropertyObject(Prop::ColumnListImages);
	bUsesRowHeader = pDclControl->GetBooleanProperty(Prop::RowHeader) == TRUE;
}

CColumnsPage::~CColumnsPage()
{
}

void CColumnsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STYLETITLE, m_StyleTitle);
	DDX_Control(pDX, IDC_INDEX_EDIT, m_IndexEdit);
	DDX_Control(pDX, IDC_FILEEXT, m_FileExt);
	DDX_Control(pDX, IDC_DROPLISTBTN, m_DropListBtn);
	DDX_Control(pDX, IDC_ALTIMAGELABEL, m_AltLabel);
	DDX_Control(pDX, IDC_DEFIMAGELABEL, m_DefLabel);
	DDX_Control(pDX, IDC_DEFIMAGE, m_DefImage);
	DDX_Control(pDX, IDC_ALTIMAGE, m_AltImage);
	DDX_Control(pDX, IDC_WIDTH, m_WidthTextBox);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDC_STYLE, m_Style);
	DDX_Control(pDX, IDC_SPIN, m_Spin);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_INDEX, m_Index);
	DDX_Control(pDX, IDC_ALIGNMENT, m_Alignment);
}


BEGIN_MESSAGE_MAP(CColumnsPage, CPropertyPage)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_EN_CHANGE(IDC_TEXT, OnChangeText)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeWidth)
	ON_CBN_SELCHANGE(IDC_ALIGNMENT, OnSelchangeAlignment)
	ON_CBN_SELCHANGE(IDC_STYLE, OnSelchangeStyle)
	ON_CBN_SELCHANGE(IDC_IMAGE, OnSelchangeImage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_DEFIMAGE, OnSelchangeDefault)
	ON_CBN_SELCHANGE(IDC_ALTIMAGE, OnSelchangeAlternate)
	ON_BN_CLICKED(IDC_DROPLISTBTN, OnDroplistbtn)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_FILEEXT, OnChangeFileext)
	ON_EN_SETFOCUS(IDC_INDEX_EDIT, OnSetfocusIndexEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColumnsPage message handlers

BOOL CColumnsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if (!m_bShowStyles)
	{
		m_Style.ShowWindow(FALSE);
		m_StyleTitle.ShowWindow(FALSE);
	}

	// set the style drop height to 300
	CRect rc;
	m_Style.GetWindowRect(&rc);
	ScreenToClient(rc);
	rc.bottom = rc.top + 300;
	m_Style.MoveWindow(rc);

	size_t nCount = m_pColCaptions->size();
	m_ColData.SetSize(nCount);
	for (size_t i=0; i<nCount; i++)
	{
		if( i < m_pColCaptions->size() )
			m_ColData[i].m_Caption = m_pColCaptions->GetConstStringArrayPtr()->at(i);
		if( i < m_pColWidths->size() )
			m_ColData[i].m_Width = m_pColWidths->GetConstIntArrayPtr()->at(i);
		if( i < m_pColImages->size() )
			m_ColData[i].m_Image = m_pColImages->GetConstIntArrayPtr()->at(i);
		if( i < m_pColStyles->size() )
			m_ColData[i].m_Style = m_pColStyles->GetConstIntArrayPtr()->at(i);
		if( i < m_pColAlignment->size() )
			m_ColData[i].m_Alignment = m_pColAlignment->GetConstIntArrayPtr()->at(i);
		if( i < m_pColDefault->size() )
			m_ColData[i].m_DefImage = m_pColDefault->GetConstIntArrayPtr()->at(i);
		if( i < m_pColAlternate->size() )
			m_ColData[i].m_AltImage = m_pColAlternate->GetConstIntArrayPtr()->at(i);
		m_ColData[i].m_ListItems.RemoveAll();
		if( i < m_pColListItems->size() )
		{
			const PropVal::TCStringArray& rStr = m_pColListItems->GetConstStringArrayListPtr()->at(i);
			for( size_t idx = 0; idx < rStr.size(); idx++)
				m_ColData[i].m_ListItems.Add(rStr[idx]);
		}
		if( i < m_pColImageItems->size() )
		{
			const PropVal::TIntArray& rIntV = m_pColImageItems->GetConstIntArrayListPtr()->at(i);
			CArray< int, int > rInt;
			for(size_t idx = 0; idx < rIntV.size(); idx++)
				rInt.Add(rIntV[idx]);
			m_ColData[i].m_ImageItems.Copy(rInt);
		}
	
	}
	m_Style.SetDroppedWidth(170);
	

	CHeaderCtrl* pHeader = m_List.GetHeaderCtrl();
	int nID = pHeader->GetDlgCtrlID();
	if (pHeader->m_hWnd != NULL)
		m_HeaderCtrl.SubclassDlgItem(nID, &m_List);

	if (pHeader->GetImageList() == NULL)
		pHeader->SetImageList(&mpImageListPage->GetDclImageList()->GetImageList());

	for (INT_PTR i = 0; i< m_ColData.GetCount(); i++)
	{
		LVCOLUMN lvColumn = 
		{
			(LVCF_FMT | LVCF_TEXT | LVCF_WIDTH),
			LVCFMT_LEFT,
			m_ColData[i].m_Width,
			NULL,
			0,
			static_cast<int>( i ),
			m_ColData[i].m_Image,
		};
		if( m_ColData[i].m_Image >= 0 )
			lvColumn.mask |= LVCF_IMAGE;
		m_nIndex = i;
		m_List.InsertColumn(i, &lvColumn);
		SetColumn(i);
	}

	m_nIndex = (m_ColData.GetCount() > 0? 0 : -1);
	if (m_nIndex != -1 )
		m_IndexEdit.SetWindowText( _T("0") );
	m_Spin.SetBuddy(&m_IndexEdit);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CColumnsPage::CommitLists()
{
	m_pColCaptions->clear();
	m_pColWidths->clear();
	m_pColImages->clear();
	m_pColStyles->clear();
	m_pColAlignment->clear();
	m_pColDefault->clear();
	m_pColAlternate->clear();

	m_pColListItems->clear();
	m_pColImageItems->clear();
	
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();

	// copy all the data over to the property data holderes of the control
	for (int i=0; i < pHeader->GetItemCount(); i++)
	{
		m_pColCaptions->GetStringArrayPtr()->push_back(m_ColData[i].m_Caption);
		m_pColWidths->GetIntArrayPtr()->push_back(m_ColData[i].m_Width);
		m_pColImages->GetIntArrayPtr()->push_back(m_ColData[i].m_Image);
		m_pColStyles->GetIntArrayPtr()->push_back(m_ColData[i].m_Style);
		m_pColAlignment->GetIntArrayPtr()->push_back(m_ColData[i].m_Alignment);
		m_pColDefault->GetIntArrayPtr()->push_back(m_ColData[i].m_DefImage);
		m_pColAlternate->GetIntArrayPtr()->push_back(m_ColData[i].m_AltImage);
		PropVal::TCStringArray rStr;
		for( int idx = 0; idx < m_ColData[i].m_ListItems.GetSize(); idx++ )
			rStr.push_back(m_ColData[i].m_ListItems[idx]);
		m_pColListItems->GetStringArrayListPtr()->push_back(rStr);
		PropVal::TIntArray rInt;
		for( int idx = 0; idx < m_ColData[i].m_ImageItems.GetSize(); idx++ )
			rInt.push_back(m_ColData[i].m_ImageItems[idx]);
		m_pColImageItems->GetIntArrayListPtr()->push_back(rInt);
	}
	CStudioDialogControl::UpdateAllProperties(mpDclControl);
}

BOOL CColumnsPage::OnApply() 
{
	CommitLists();
	return CPropertyPage::OnApply();
}

void CColumnsPage::OnOK() 
{
	CommitLists();
	CPropertyPage::OnOK();
}

bool CColumnsPage::IsImageListValid()
{
	if( !mpImageListPage )
		return false;
	if( mpImageListPage->GetDclImageList()->IsNull() )
		return false;
	m_List.SetImageList( &mpImageListPage->GetDclImageList()->GetImageList(), TVSIL_NORMAL );
	m_List.SetImageList( &mpImageListPage->GetDclImageList()->GetImageList(), LVSIL_SMALL );
	return true;
}

BOOL CColumnsPage::OnSetActive() 
{
	int nSel = m_Image.GetCurSel();
	int nSel2 = m_DefImage.GetCurSel();
	int nSel3 = m_AltImage.GetCurSel();
	
	m_Image.ResetContent();
	m_DefImage.ResetContent();
	m_AltImage.ResetContent();
	if (IsImageListValid())
	{
		CImageList* pImageList = &mpImageListPage->GetDclImageList()->GetImageList();
		m_Image.SetImageList(pImageList);
		m_DefImage.SetImageList(pImageList);	
		m_AltImage.SetImageList(pImageList);
		
		for (int i=0; i < pImageList->GetImageCount(); i++)
		{
			CString sImageName;
			sImageName.Format( _T("%d"), i );
			
			COMBOBOXEXITEM cbi = 
			{
				CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE,
				i,
				sImageName.LockBuffer(),
				-1,
				i,
				i,
			};
			m_Image.InsertItem(&cbi);
			m_DefImage.InsertItem(&cbi);
			m_AltImage.InsertItem(&cbi);
		}

		CString sNone = theWorkspace.LoadResourceString( IDS_NONE );
		COMBOBOXEXITEM cbi = 
		{
			CBEIF_TEXT,
			0,
			sNone.LockBuffer(),
			-1,
		};
		m_Image.InsertItem(&cbi);
		m_DefImage.InsertItem(&cbi);
		m_AltImage.InsertItem(&cbi);
		m_Image.SetCurSel(nSel);
		m_DefImage.SetCurSel(nSel2);
		m_AltImage.SetCurSel(nSel3);
	}
	SetControls(m_nIndex);
	return CPropertyPage::OnSetActive();
}

void CColumnsPage::OnInsert() 
{
	INT_PTR nIndex = m_List.InsertColumn(m_ColData.GetCount(), CString(), LVCFMT_LEFT, 100);

	// add the data to the list arrays
	m_ColData.SetSize( nIndex + 1 );
	CColumnData& data = m_ColData[nIndex];
	CString sCaption;
	sCaption.Format( theWorkspace.LoadResourceString( IDS_NEWCOLUMNCAPTION ), nIndex );
	data.m_Caption = sCaption;
	data.m_Width = 100;	
	data.m_Image = -1;	
	data.m_Style = 0;	
	data.m_Alignment = 0;	
	data.m_DefImage = -1;	
	data.m_AltImage = -1;
	SetControls(nIndex);
	SetColumn(m_nIndex);
	m_Spin.SetPos( m_nIndex );
	m_Text.SetFocus();
	SetModified();
}

const TCHAR sDwg[] = _T("dwg;dxf");

void CColumnsPage::SetControls(INT_PTR nIndex)
{
	m_nIndex = nIndex;
	INT_PTR nColCount = m_ColData.GetCount();
	m_Spin.SetRange(0, nColCount> 0? nColCount - 1 : 0);

	m_DropListBtn.ShowWindow(FALSE);
	m_FileExt.ShowWindow(FALSE);
	m_DefLabel.ShowWindow(FALSE);
	m_DefImage.ShowWindow(FALSE);
	m_AltLabel.ShowWindow(FALSE);
	m_AltImage.ShowWindow(FALSE);
	if (nIndex < 0)
	{
		m_Spin.EnableWindow(FALSE);
		m_IndexEdit.ShowWindow(FALSE);
		m_Alignment.EnableWindow(FALSE);
		m_Image.EnableWindow(FALSE);
		m_WidthTextBox.EnableWindow(FALSE);
		m_Text.EnableWindow(FALSE);
		m_Style.EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		return;
	}

	if( nIndex >= m_ColData.GetCount() )
		nIndex = m_ColData.GetCount();
	GetDlgItem(IDC_DELETE)->EnableWindow(nColCount > 0);
	m_IndexEdit.ShowWindow(TRUE);
	m_Spin.EnableWindow(nColCount > 1);
	m_Image.EnableWindow(TRUE);
	m_Alignment.EnableWindow(TRUE);		

	// now setup the controls correctly.	
	m_bChangingIndex = true;

	if (m_ColData[nIndex].m_Style != 15 && //date style
			m_ColData[nIndex].m_Style != 16 && //time style
			m_ColData[nIndex].m_Image >= m_Image.GetCount())
		m_ColData[nIndex].m_Image = -1;

	m_Alignment.EnableWindow(TRUE);		
	m_WidthTextBox.EnableWindow(TRUE);
	m_Text.EnableWindow(TRUE);
	if (!bUsesRowHeader || nIndex > 0)
		m_Style.EnableWindow(TRUE);
	else
		m_Style.EnableWindow(FALSE);

	m_Text.SetLimitText(256);

	CString sVal;
	sVal.Format( _T("%d"), m_ColData[nIndex].m_Width );
	m_WidthTextBox.SetWindowText(sVal);

	m_Text.SetWindowText(m_ColData[nIndex].m_Caption);
	m_Style.SetCurSel(m_ColData[nIndex].m_Style);
	m_Alignment.SetCurSel(m_ColData[nIndex].m_Alignment);

	m_DefLabel.ShowWindow(TRUE);
	m_DefLabel.SetWindowText( theWorkspace.LoadResourceString( IDS_DEFICON ) );
	m_DefImage.ShowWindow(TRUE);
	m_DefImage.EnableWindow( (m_AltImage.GetCount() > 1) );
	m_DefImage.SetCurSel(m_ColData[m_nIndex].m_DefImage + 1);
	m_AltImage.ShowWindow(FALSE);
	m_AltImage.EnableWindow( (m_AltImage.GetCount() > 1) );
	m_AltImage.SetCurSel(m_ColData[m_nIndex].m_AltImage + 1);
	m_Image.SetCurSel(m_ColData[nIndex].m_Image + 1);

	switch(m_ColData[nIndex].m_Style)
	{
		case 15:
		{
			m_DefLabel.ShowWindow( SW_HIDE );
			m_DefImage.ShowWindow( SW_HIDE );
			if (m_ColData[nIndex].m_Image == -1)
				m_ColData[nIndex].m_Image = 0;
			m_DefLabel.SetWindowText( theWorkspace.LoadResourceString(IDS_DATEFORMAT) );
			break;
		}
		case 16:
		{
			m_DefLabel.ShowWindow( SW_HIDE );
			m_DefImage.ShowWindow( SW_HIDE );
			if (m_ColData[nIndex].m_Image == -1)
				m_ColData[nIndex].m_Image = 0;
			m_DefLabel.SetWindowText( theWorkspace.LoadResourceString(IDS_TIMEFORMAT) );
			break;
		}
		case 18:
		case 29:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		{
			m_DropListBtn.ShowWindow(TRUE);
			break;
		}

		case 3:
		{
			m_AltLabel.ShowWindow(TRUE);
			m_AltImage.ShowWindow(TRUE);		
			break;
		}
		
		case 35:
		{
			m_DefImage.ShowWindow( SW_HIDE );
			m_DefLabel.SetWindowText( theWorkspace.LoadResourceString(IDS_FILETYPE) );
			m_FileExt.ShowWindow(TRUE);			
			if (m_ColData[nIndex].m_ListItems.GetSize() > 0)
				m_FileExt.SetWindowText(m_ColData[nIndex].m_ListItems.GetAt(0));
			break;
		}
	}

	m_bChangingIndex = false;
}


void CColumnsPage::OnChangeText() 
{
	CString sCaption;
	m_Text.GetWindowText(sCaption);
	HDITEM hdi;
	hdi.mask = HDI_TEXT;
	hdi.pszText = sCaption.LockBuffer();
	m_List.GetHeaderCtrl()->SetItem(m_nIndex, &hdi);
	sCaption.UnlockBuffer();
	m_ColData[m_nIndex].m_Caption = sCaption;

	SetModified();
}

void CColumnsPage::OnChangeWidth() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;
	CString sText;

	m_WidthTextBox.GetWindowText(sText);
	
	if (sText.GetLength() == 0)
		m_List.SetColumnWidth(m_nIndex, 0);
	else
		m_List.SetColumnWidth(m_nIndex, _tstol(sText));
	m_ColData[m_nIndex].m_Width = _tstol(sText);
	SetColumn(m_nIndex);
	m_List.Invalidate();
	SetModified();
}


void CColumnsPage::OnColumnHeaderClicked(int nIndex) 
{
	m_Spin.SetPos(nIndex);
	m_nIndex = nIndex;
	SetControls(nIndex);
}

void CColumnsPage::ResetWidths() 
{
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();
	if (pHeader == NULL)
		return;
	if (pHeader->GetItemCount() == 0)
		return;

	for (int i=0; i<pHeader->GetItemCount(); i++)
	{		
		CRect rc;
		pHeader->GetItemRect(i, rc);
		m_ColData[i].m_Width = rc.Width();
	}
	SetControls( m_nIndex );
	SetModified();
}

void CColumnsPage::OnSelchangeAlignment() 
{
	int nSel = m_Alignment.GetCurSel();
	int nAlign;
	m_ColData[m_nIndex].m_Alignment = nSel;

	if (nSel == 0)
		nAlign = LVCFMT_LEFT;
	else if (nSel == 1)
		nAlign = LVCFMT_CENTER;
	else if (nSel == 2)
		nAlign = LVCFMT_RIGHT;
	SetColumn(m_nIndex);
	SetModified();
}

void CColumnsPage::OnSelchangeStyle() 
{
	m_ColData[m_nIndex].m_Style = m_Style.GetCurSel();

	m_FileExt.ShowWindow(FALSE);
	m_DropListBtn.ShowWindow(FALSE);
	m_DefLabel.ShowWindow(TRUE);
	m_DefLabel.SetWindowText( theWorkspace.LoadResourceString( IDS_DEFICON ) );
	m_DefImage.ShowWindow(TRUE);
	m_DefImage.EnableWindow( (m_AltImage.GetCount() > 1) );
	m_DefImage.SetCurSel(m_ColData[m_nIndex].m_DefImage + 1);
	m_AltLabel.ShowWindow(FALSE);
	m_AltImage.ShowWindow(FALSE);
	m_AltImage.EnableWindow( (m_AltImage.GetCount() > 1) );
	m_AltImage.SetCurSel(m_ColData[m_nIndex].m_AltImage + 1);					
	switch(m_Style.GetCurSel())
	{
		case 1:
		case 2:
		{
			m_ColData[m_nIndex].m_AltImage = -1;
			break;
		}
		case 3:
		{
			m_AltLabel.ShowWindow(TRUE);
			m_AltImage.ShowWindow(TRUE);		
			break;
		}
		case 15:
		{
			m_DefLabel.ShowWindow( SW_HIDE );
			m_DefImage.ShowWindow( SW_HIDE );
			if (m_ColData[m_nIndex].m_Image == -1)
			m_DefLabel.SetWindowText( theWorkspace.LoadResourceString(IDS_DATEFORMAT) );
			break;
		}
		case 16:
		{
			m_DefLabel.ShowWindow( SW_HIDE );
			m_DefImage.ShowWindow( SW_HIDE );
			if (m_ColData[m_nIndex].m_Image == -1)
				m_ColData[m_nIndex].m_Image = 0;
			m_DefLabel.SetWindowText( theWorkspace.LoadResourceString(IDS_TIMEFORMAT) );
			break;
		}
		case 18:			
		case 29:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		{
			m_DropListBtn.ShowWindow(TRUE);
			break;
		}
		case 35:
		{
			m_DefImage.ShowWindow( SW_HIDE );
			m_DefLabel.SetWindowText( theWorkspace.LoadResourceString(IDS_FILETYPE) );
			m_FileExt.SetWindowText(sDwg);
			m_FileExt.ShowWindow(TRUE);			
			break;
		}
	}
	SetModified();
}

void CColumnsPage::SetColumn( INT_PTR nIndex )
{
	CHeaderCtrl* pHdrCtrl = m_List.GetHeaderCtrl();
	if( !pHdrCtrl )
		return;
	if( nIndex >= pHdrCtrl->GetItemCount() )
		return;

	HDITEM hdi = { HDI_TEXT | HDI_IMAGE | HDI_FORMAT };
	hdi.iImage = m_ColData[m_nIndex].m_Image;

	//force a CString copy here, because calling LockBuffer() on a CArray resident CString corrupts the 
	//CString reference count mechanism and leads to an eventual crash next time a CString member function 
	//tries to do anything with the embedded string manager  2007-02-23 [ORW]
	CString sCaption = (LPCTSTR)m_ColData[m_nIndex].m_Caption;
	hdi.pszText = sCaption.LockBuffer();
	hdi.cchTextMax = -1;

	switch( m_ColData[m_nIndex].m_Alignment )
	{					
	case 1:
		hdi.fmt = HDF_CENTER | HDF_STRING;
		break;
	case 2:
		hdi.fmt = HDF_RIGHT | HDF_STRING;
		break;				
	default:
		hdi.fmt = HDF_LEFT | HDF_STRING;
		break;
	}
	
	if( hdi.iImage >= 0 )
		hdi.fmt |= HDF_IMAGE;

	pHdrCtrl->SetItem( nIndex, &hdi );
}


void CColumnsPage::OnSelchangeImage() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	int nSel = m_Image.GetCurSel();
	if( nSel >= 0 )
		--nSel;

	m_ColData[m_nIndex].m_Image = nSel;

	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();
	
	if (pHeader->GetImageList() == NULL)
		pHeader->SetImageList(&mpImageListPage->GetDclImageList()->GetImageList());
	
	SetColumn(m_nIndex);
	SetModified();
}

void CColumnsPage::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int nIndex = pNMUpDown->iPos + pNMUpDown->iDelta;
	if( (nIndex < 0 && m_ColData.GetCount() > 0) || (nIndex >= m_ColData.GetCount()) )
	{
		*pResult = 1;
		return;
	}
	SetControls( nIndex );
}

void CColumnsPage::OnDelete() 
{
	m_List.DeleteColumn(m_nIndex);

	// add the data to the list arrays
	m_ColData.RemoveAt(m_nIndex);

	m_Spin.EnableWindow(m_ColData.GetCount() > 1);
	m_Alignment.EnableWindow(m_ColData.GetCount() > 0);

	SetControls(m_nIndex >= m_ColData.GetSize()? m_ColData.GetSize() - 1 : m_nIndex);
	m_Spin.SetPos( m_nIndex );
	SetModified();
}

void CColumnsPage::OnSelchangeDefault() 
{
	int nSel = m_DefImage.GetCurSel();
	if( nSel >= 0 )
		--nSel;
	m_ColData[m_nIndex].m_DefImage = nSel;
	SetModified();
}

void CColumnsPage::OnSelchangeAlternate() 
{
	int nSel = m_AltImage.GetCurSel();
	if( nSel >= 0 )
		--nSel;
	m_ColData[m_nIndex].m_AltImage = nSel;
	SetModified();
}

void CColumnsPage::OnDroplistbtn() 
{
	if (m_ColData[m_nIndex].m_Style == 29)
	{
		CImageListContents Dlg;

    int i;	
		for (i=0; i<m_ColData[m_nIndex].m_ListItems.GetSize(); i++)
		{
			Dlg.sStrings.Add(m_ColData[m_nIndex].m_ListItems[i]);
		}
		
		for (i=0; i<m_ColData[m_nIndex].m_ImageItems.GetSize(); i++)
		{
			int n = m_ColData[m_nIndex].m_ImageItems[i];
			Dlg.sImages.Add(n);
		}
		
		Dlg.m_pImageListPage = mpImageListPage;
		
		if (Dlg.DoModal() == 1)
		{
			m_ColData[m_nIndex].m_ListItems.RemoveAll();
			if (Dlg.sStrings.GetSize() == 0)
				return;

      int i;
			for (i=0; i<Dlg.sStrings.GetSize(); i++)
			{
				m_ColData[m_nIndex].m_ListItems.Add(Dlg.sStrings[i]);
			}

			m_ColData[m_nIndex].m_ImageItems.RemoveAll();

			for (i=0; i<Dlg.sImages.GetSize(); i++)
			{
				int n = Dlg.sImages[i];
				m_ColData[m_nIndex].m_ImageItems.Add(n);
			}
		}
	}
	
	if (m_ColData[m_nIndex].m_Style == 18 || (m_ColData[m_nIndex].m_Style >= 36 && m_ColData[m_nIndex].m_Style <= 41))
	{
		CDropListContents Dlg;
		
		for (int i=0; i<m_ColData[m_nIndex].m_ListItems.GetSize(); i++)
		{
			Dlg.sStrings.Add(m_ColData[m_nIndex].m_ListItems[i]);
		}

		if (Dlg.DoModal() == 1)
		{
			m_ColData[m_nIndex].m_ListItems.RemoveAll();
			if (Dlg.sStrings.GetSize() == 0)
				return;


			for (int i=0; i<Dlg.sStrings.GetSize(); i++)
			{
				m_ColData[m_nIndex].m_ListItems.Add(Dlg.sStrings[i]);
			}
		}
	}
}

void CColumnsPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	m_ColData.RemoveAll();
}

void CColumnsPage::OnChangeFileext() 
{
	CString sText;
	m_FileExt.GetWindowText(sText);
	
	m_ColData[m_nIndex].m_ListItems.RemoveAll();
	m_ColData[m_nIndex].m_ListItems.Add(sText);

	SetModified();
}

void CColumnsPage::OnSetfocusIndexEdit() 
{
	m_Text.SetFocus();
}
