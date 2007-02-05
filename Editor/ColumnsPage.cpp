// ColumnsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ColumnsPage.h"
#include "PropertyObject.h"
#include "ImageListPage.h"
#include "ImageListContents.h"
#include "DropListContents.h"
#include "Workspace.h"
#include "SharedRes.h"


CColumnData::CColumnData()
{
	
	m_Width = 0;	
	m_Image = 0;	
	m_Style = 0;	
	m_Alignment = 0;	
	m_Default = 0;	
	m_Alternate = 0;
	
}

CColumnData::~CColumnData()
{
}
CColumnData::CColumnData(CColumnData *pOther)
{
}
CColumnData::CColumnData(CColumnData &Other)
{
}

CColumnData CColumnData::operator=(CColumnData const & other) 
{
	m_Width = other.m_Width;	
	m_Image = other.m_Image;	
	m_Style = other.m_Style;	
	m_Alignment = other.m_Alignment;	
	m_Default = other.m_Default;	
	m_Alternate = other.m_Alternate;

	return *this;
}
/////////////////////////////////////////////////////////////////////////////
// CColumnsPage property page

IMPLEMENT_DYNCREATE(CColumnsPage, CPropertyPage)

CColumnsPage::CColumnsPage() : CPropertyPage(CColumnsPage::IDD)
{
	//{{AFX_DATA_INIT(CColumnsPage)
	//}}AFX_DATA_INIT

	m_bChangingIndex = false;

}

CColumnsPage::~CColumnsPage()
{
}

void CColumnsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColumnsPage)
	DDX_Control(pDX, IDC_STYLETITLE, m_StyleTitle);
	DDX_Control(pDX, IDC_INDEX_EDIT, m_IndexEdit);
	DDX_Control(pDX, IDC_FILEEXT, m_FileExt);
	DDX_Control(pDX, IDC_TIME, m_Times);
	DDX_Control(pDX, IDC_DATE, m_Dates);
	DDX_Control(pDX, IDC_DROPLISTBTN, m_DropListBtn);
	DDX_Control(pDX, IDC_ALTICONLABEL, m_AltLabel);
	DDX_Control(pDX, IDC_DEFICONLABEL, m_DefLabel);
	DDX_Control(pDX, IDC_DISABLED, m_Disabled);
	DDX_Control(pDX, IDC_DEFAULT, m_Default);
	DDX_Control(pDX, IDC_ALTERNATE, m_Alternate);
	DDX_Control(pDX, 1211, m_WidthTextBox);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDC_STYLE, m_Style);
	DDX_Control(pDX, IDC_SPIN, m_Spin);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_INDEX, m_Index);
	DDX_Control(pDX, IDC_ALIGNMENT, m_Alignment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColumnsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColumnsPage)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_EN_CHANGE(IDC_TEXT, OnChangeText)
	ON_EN_CHANGE(1211, OnChangeWidth)
	ON_CBN_SELCHANGE(IDC_ALIGNMENT, OnSelchangeAlignment)
	ON_CBN_SELCHANGE(IDC_STYLE, OnSelchangeStyle)
	ON_CBN_SELCHANGE(IDC_IMAGE, OnSelchangeImage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_DEFAULT, OnSelchangeDefault)
	ON_CBN_SELCHANGE(IDC_ALTERNATE, OnSelchangeAlternate)
	ON_BN_CLICKED(IDC_DROPLISTBTN, OnDroplistbtn)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_DATE, OnSelchangeDate)
	ON_CBN_SELCHANGE(IDC_TIME, OnSelchangeTime)
	ON_EN_CHANGE(IDC_FILEEXT, OnChangeFileext)
	ON_EN_CHANGE(IDC_INDEX_EDIT, OnChangeIndexEdit)
	ON_EN_SETFOCUS(IDC_INDEX_EDIT, OnSetfocusIndexEdit)
	//}}AFX_MSG_MAP
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

	size_t nCount = m_pColCaptions->GetStringArrayPtr()->size();
  size_t i;
	for (i=0; i<nCount; i++)
	{
		m_ColData[i].m_Caption = m_pColCaptions->GetStringArrayPtr()->at(i);

		// copy width data
		m_ColData[i].m_Width = m_pColWidths->GetIntArrayPtr()->at(i);
		
		// copy image data
		m_ColData[i].m_Image = m_pColImages->GetIntArrayPtr()->at(i);
		
		// copy style data
		m_ColData[i].m_Style = m_pColStyles->GetIntArrayPtr()->at(i);
		
		// copy the alignment data
		m_ColData[i].m_Alignment = m_pColAlignment->GetIntArrayPtr()->at(i);

		// copy the default data
		m_ColData[i].m_Default = m_pColDefault->GetIntArrayPtr()->at(i);

		// copy the alternate data
		m_ColData[i].m_Alternate = m_pColAlternate->GetIntArrayPtr()->at(i);
	
		// copy string array data.
		m_ColData[i].m_ListItems.RemoveAll();
		for( size_t idxL = 0; idxL < m_pColListItems->GetStringArrayListPtr()->size(); idxL++)
		{
			PropVal::TCStringArray& rStr = m_pColListItems->GetStringArrayListPtr()->at(idxL);
			for( size_t idx = 0; idx < rStr.size(); idx++)
				m_ColData[i].m_ListItems.Add(rStr[idx]);
		}
	
		// copy int array data
		for( size_t k = 0; k < m_pColImageItems->GetIntArrayListPtr()->size(); k++)
		{
			PropVal::TIntArray& rIntV = m_pColImageItems->GetIntArrayListPtr()->at(k);
			CArray< int, int > rInt;
			for(size_t idx = 0; idx < rIntV.size(); idx++)
				rInt.Add(rIntV[idx]);
			m_ColData[i].m_ImageItems.Copy(rInt);
		}
	
	}

	m_Disabled.ShowWindow(TRUE);
	m_Image.ShowWindow(FALSE);
	m_Disabled.EnableWindow(FALSE);
	m_Image.ShowWindow(TRUE);
	m_WidthTextBox.EnableWindow(FALSE);
	m_FileExt.ShowWindow(FALSE);
	m_Style.SetDroppedWidth(170);
	
	OnSetActive();

	int nID = m_List.GetHeaderCtrl()->GetDlgCtrlID();

	if (m_List.GetHeaderCtrl()->m_hWnd != NULL)
         m_HeaderCtrl.SubclassDlgItem(nID, &m_List);
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();

	if (pHeader->GetImageList() == NULL)
		pHeader->SetImageList(&m_pImageListPage->m_ImageList);


	for (size_t i=0; i<m_pColWidths->GetIntArrayPtr()->size(); i++)
	{
		m_nIndex = i;		
		m_List.InsertColumn(i, CString(), LVCFMT_LEFT, m_ColData[i].m_Width);
		SetColumn(i);
	}

	SetControls(0);

	m_Spin.SetRange(0, nCount-1);
	m_Spin.SetPos(0);
	
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
	for (int i=0; i<pHeader->GetItemCount(); i++)
	{
		m_pColCaptions->GetStringArrayPtr()->push_back(m_ColData[i].m_Caption);
	
		m_pColWidths->GetIntArrayPtr()->push_back(m_ColData[i].m_Width);
	
		m_pColImages->GetIntArrayPtr()->push_back(m_ColData[i].m_Image);
	
		m_pColStyles->GetIntArrayPtr()->push_back(m_ColData[i].m_Style);
	
		m_pColAlignment->GetIntArrayPtr()->push_back(m_ColData[i].m_Alignment);

		m_pColDefault->GetIntArrayPtr()->push_back(m_ColData[i].m_Default);
		
		m_pColAlternate->GetIntArrayPtr()->push_back(m_ColData[i].m_Alternate);
		
		PropVal::TCStringArray rStr;
		for( int idx = 0; idx < m_ColData[i].m_ListItems.GetSize(); idx++ )
			rStr.push_back(m_ColData[i].m_ListItems[idx]);
		m_pColListItems->GetStringArrayListPtr()->push_back(rStr);
	
		PropVal::TIntArray rInt;
		for( int idx = 0; idx < m_ColData[i].m_ImageItems.GetSize(); idx++ )
			rInt.push_back(m_ColData[i].m_ImageItems[idx]);
		m_pColImageItems->GetIntArrayListPtr()->push_back(rInt);
	}
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
	if (m_pImageListPage != NULL)
	{
		if (m_pImageListPage->m_hWnd == NULL)
			return false;
		
		if (m_pImageListPage->m_ImageList.m_hImageList == NULL)
			return false;
	}
	else
		return false;
	m_List.SetImageList(&m_pImageListPage->m_ImageList, TVSIL_NORMAL);
	m_List.SetImageList(&m_pImageListPage->m_ImageList, LVSIL_SMALL);
	return true;
}

BOOL CColumnsPage::OnSetActive() 
{
	int nSel = m_Image.GetCurSel();
	int nSel2 = m_Default.GetCurSel();
	int nSel3 = m_Alternate.GetCurSel();
	
	m_Image.ResetContent();
	m_Default.ResetContent();
	m_Alternate.ResetContent();
	if (!IsImageListValid())
		return CPropertyPage::OnSetActive();

	m_Image.SetImageList(&m_pImageListPage->m_ImageList);
	m_Default.SetImageList(&m_pImageListPage->m_ImageList);	
	m_Alternate.SetImageList(&m_pImageListPage->m_ImageList);
	
	m_pImageListPage->m_ImageList.SetBkColor(RGB(255,255,255));
	int i;
	for (i=0; i<m_pImageListPage->m_ImageList.GetImageCount(); i++)
	{
		TCHAR value[80];
		_ltot(i, value, 10);
		
		COMBOBOXEXITEM cbi;

		cbi.mask = CBEIF_IMAGE | //CBEIF_OVERLAY |
		 CBEIF_TEXT | CBEIF_SELECTEDIMAGE;

		cbi.iItem = i;
		cbi.pszText = value;
		cbi.cchTextMax = lstrlen(value);
		cbi.iImage = i;
		cbi.iSelectedImage = i;
		cbi.iOverlay = i;
		cbi.iIndent = 0;

		m_Image.InsertItem(&cbi);
		m_Default.InsertItem(&cbi);
		m_Alternate.InsertItem(&cbi);
	}

	COMBOBOXEXITEM cbi;

	cbi.mask = //CBEIF_OVERLAY |
	 CBEIF_TEXT ;

	CString value;
	value = theWorkspace.LoadResourceString(IDS_NONE);
	cbi.iItem = i;
	cbi.pszText = (LPTSTR)(LPCTSTR)value;
	cbi.cchTextMax = lstrlen(value);
	cbi.iImage = -1;
	cbi.iSelectedImage = -1;
	cbi.iOverlay = i;
	cbi.iIndent = 0;
	m_Image.InsertItem(&cbi);
	m_Image.SetCurSel(nSel);
	m_Default.SetCurSel(nSel2);
	m_Alternate.SetCurSel(nSel3);
	return CPropertyPage::OnSetActive();
}

void CColumnsPage::OnInsert() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();

	m_nIndex = m_List.InsertColumn(pHeader->GetItemCount(), CString(), LVCFMT_LEFT, 100);

	// add the data to the list arrays
	CColumnData data;
	int n = m_ColData.Add(data);	
	
	m_ColData[n].m_Width = 100;	
	m_ColData[n].m_Image = -1;	
	m_ColData[n].m_Style = 0;	
	m_ColData[n].m_Alignment = 0;	
	m_ColData[n].m_Default = 0;	
	m_ColData[n].m_Alternate = 0;

	if (m_nIndex == 0 && pHeader->GetItemCount() == 0)
	{
		m_Spin.EnableWindow(FALSE);
		m_Alignment.EnableWindow(FALSE);
	}
	else
	{
		m_Spin.EnableWindow(TRUE);
		m_Spin.SetBuddy(&m_IndexEdit);
		m_Alignment.EnableWindow(TRUE);		
	}

	m_Spin.SetRange(0, m_nIndex);
	m_Spin.SetPos(m_nIndex);
	
	SetControls(m_nIndex);

	m_Text.SetFocus();
	SetModified();
}

const TCHAR sDwg[] = _T("dwg;dxf");

void CColumnsPage::SetControls(int nIndex)
{
	m_nIndex = nIndex;
	m_bChangingIndex = true;
	
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();
	
	if (pHeader == NULL)
		return;

	// now setup the controls correctly.	
	m_Spin.SetPos(m_nIndex);
	if (IsImageListValid())
	{
		m_Disabled.ShowWindow(FALSE);
		m_Image.ShowWindow(TRUE);
	}
	else
	{
		m_Disabled.ShowWindow(TRUE);
		m_Image.ShowWindow(FALSE);
	}
	if (pHeader->GetItemCount() > 0)
	{
		m_Spin.EnableWindow(TRUE);
		m_Spin.SetBuddy(&m_IndexEdit);		
	}
	m_Alignment.EnableWindow(TRUE);		
	m_WidthTextBox.EnableWindow(TRUE);
	m_Text.EnableWindow(TRUE);
	if (!bUsesRowHeader || m_nIndex > 0)
	{
		m_Style.EnableWindow(TRUE);
	}
	else
	{
		m_Style.EnableWindow(FALSE);
		m_Style.SetCurSel(0);
	}
	m_Style.SetCurSel(0);
	m_Text.SetLimitText(256);

	TCHAR value[80];
	_ltot(m_nIndex, value, 10);
	if (!m_bChangingIndex)
	{
		m_IndexEdit.SetWindowText(value);
	}
	_ltot(m_ColData[m_nIndex].m_Width, value, 10);	
	

	m_WidthTextBox.SetWindowText(value);
	m_bChangingIndex = true;
	m_Text.SetWindowText(m_ColData[m_nIndex].m_Caption);
	m_bChangingIndex = false;
	m_Style.SetCurSel(m_ColData[m_nIndex].m_Style);
	m_Alignment.SetCurSel(m_ColData[m_nIndex].m_Alignment);
	if (m_ColData[m_nIndex].m_Style == 3)
	{
		if (m_ColData[m_nIndex].m_Image >= pHeader->GetImageList()->GetImageCount())
			m_ColData[m_nIndex].m_Image = -1;
	}
	m_Image.SetCurSel(m_ColData[m_nIndex].m_Image);

	m_Times.ShowWindow(FALSE);
	m_Dates.ShowWindow(FALSE);
	m_DropListBtn.ShowWindow(FALSE);
	m_DefLabel.ShowWindow(FALSE);
	m_FileExt.ShowWindow(FALSE);
	switch(m_ColData[m_nIndex].m_Style)
	{
		case 15:
		{
			if (m_ColData[m_nIndex].m_Image == -1)
			{
				m_ColData[m_nIndex].m_Image = 0;
			}
			m_Dates.SetCurSel(m_ColData[m_nIndex].m_Image);
			m_Dates.ShowWindow(TRUE);			
			CString sLoad;
			sLoad = theWorkspace.LoadResourceString(IDS_DATEFORMAT);
			m_DefLabel.SetWindowText(sLoad);
			m_DefLabel.ShowWindow(TRUE);
			break;
		}
		case 16:
		{
			if (m_ColData[m_nIndex].m_Image == -1)
			{
				m_ColData[m_nIndex].m_Image = 0;
			}
			m_Times.SetCurSel(m_ColData[m_nIndex].m_Image);
			m_Times.ShowWindow(TRUE);
			CString sLoad;
			sLoad = theWorkspace.LoadResourceString(IDS_TIMEFORMAT);
			m_DefLabel.SetWindowText(sLoad);
			m_DefLabel.ShowWindow(TRUE);
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
			if (m_Alternate.GetCount() > 0)
			{
				m_Default.ShowWindow(TRUE);
				m_Alternate.ShowWindow(TRUE);
				m_DefLabel.ShowWindow(TRUE);
				m_AltLabel.ShowWindow(TRUE);
				m_Default.SetCurSel(m_ColData[m_nIndex].m_Default);
				m_Alternate.SetCurSel(m_ColData[m_nIndex].m_Alternate);
				CString sLoad;
				sLoad = theWorkspace.LoadResourceString(IDS_DEFICON);
				m_DefLabel.SetWindowText(sLoad);
			}
			break;
		}
		
		case 35:
		{
			m_Default.ShowWindow(FALSE);
			m_Alternate.ShowWindow(FALSE);
			m_DefLabel.ShowWindow(FALSE);
			m_AltLabel.ShowWindow(FALSE);
			m_Default.SetCurSel(-1);
			m_Alternate.SetCurSel(-1);				
			CString sLoad;
			sLoad = theWorkspace.LoadResourceString(IDS_FILETYPE);
			m_DefLabel.SetWindowText(sLoad);
			m_FileExt.ShowWindow(TRUE);			
			m_DefLabel.ShowWindow(TRUE);

			if (m_ColData[m_nIndex].m_ListItems.GetSize() > 0)
			{
				m_FileExt.SetWindowText(m_ColData[m_nIndex].m_ListItems.GetAt(0));
			}
			break;
		}
		default:
		{
			m_Default.SetCurSel(-1);
			m_Alternate.SetCurSel(-1);
			m_Default.ShowWindow(FALSE);
			m_Alternate.ShowWindow(FALSE);
			m_DefLabel.ShowWindow(FALSE);
			m_AltLabel.ShowWindow(FALSE);
			break;			
		}
	}

	m_bChangingIndex = false;
}


void CColumnsPage::OnChangeText() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	CString sText;
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();
	m_Text.GetWindowText(sText);

	HDITEM hdi;
	TCHAR  lpBuffer[256];
	
	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = 256;

	pHeader->GetItem(m_nIndex, &hdi);
   
	lstrcpy(hdi.pszText, sText);
	pHeader->SetItem(m_nIndex, &hdi);
	
	m_ColData[m_nIndex].m_Caption = sText;
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
	m_nIndex = nIndex;
	//SetControls(m_nIndex);
	//SetColumn(m_nIndex);
}

void CColumnsPage::ResetWidths() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

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
	SetControls(m_nIndex);
	SetModified();
}

void CColumnsPage::OnSelchangeAlignment() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	int nSel = m_Alignment.GetCurSel();
	int nAlign;
	m_ColData[m_nIndex].m_Alignment = nSel;

	if (nSel == 0)
		nAlign = LVCFMT_LEFT;
	else if (nSel == 1)
		nAlign = LVCFMT_CENTER;
	else if (nSel == 2)
		nAlign = LVCFMT_RIGHT;

	m_List.DeleteColumn(m_nIndex);

	m_nIndex = m_List.InsertColumn(m_nIndex, CString(), nAlign, 100);

	SetColumn(m_nIndex);
	SetModified();
}

void CColumnsPage::OnSelchangeStyle() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;
	if (m_Style.GetCurSel() == 3 || m_Style.GetCurSel() == 29)
	{
		if (m_pImageListPage == NULL)
		{
			m_Style.SetCurSel(0);
			return;
		}

		if (m_pImageListPage->m_ImageList.m_hImageList == NULL)
		{
			m_Style.SetCurSel(0);
			return;
		}

		if (m_pImageListPage->m_ImageList.GetImageCount() == 0)
		{
			m_Style.SetCurSel(0);
			return;
		}
	}

	m_ColData[m_nIndex].m_Style = m_Style.GetCurSel();

	m_Times.ShowWindow(FALSE);
	m_Dates.ShowWindow(FALSE);
	m_DefLabel.ShowWindow(FALSE);
	m_FileExt.ShowWindow(FALSE);
	m_DropListBtn.ShowWindow(FALSE);
	switch(m_Style.GetCurSel())
	{
		case 15:
		{
			if (m_ColData[m_nIndex].m_Image == -1)
			{
				m_ColData[m_nIndex].m_Image = 0;
			}			
			m_Dates.SetCurSel(m_ColData[m_nIndex].m_Image);
			m_Dates.ShowWindow(TRUE);			
			CString sLoad;
			sLoad = theWorkspace.LoadResourceString(IDS_DATEFORMAT);
			m_DefLabel.SetWindowText(sLoad);
			m_DefLabel.ShowWindow(TRUE);

			break;
		}
		case 16:
		{
			if (m_ColData[m_nIndex].m_Image == -1)
			{
				m_ColData[m_nIndex].m_Image = 0;
			}
			m_Times.SetCurSel(m_ColData[m_nIndex].m_Image);			
			m_Times.ShowWindow(TRUE);			
			CString sLoad;
			sLoad = theWorkspace.LoadResourceString(IDS_TIMEFORMAT);
			m_DefLabel.SetWindowText(sLoad);
			m_DefLabel.ShowWindow(TRUE);
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
			if (m_Alternate.GetCount() > 0)
			{
				m_Default.ShowWindow(TRUE);
				m_Alternate.ShowWindow(TRUE);		
				m_DefLabel.ShowWindow(TRUE);
				m_AltLabel.ShowWindow(TRUE);
				m_Default.SetCurSel(m_ColData[m_nIndex].m_Default);
				m_Alternate.SetCurSel(m_ColData[m_nIndex].m_Alternate);					
				CString sLoad;
				sLoad = theWorkspace.LoadResourceString(IDS_DEFICON);
				m_DefLabel.SetWindowText(sLoad);				
			}
			break;
		}
		case 35:
		{
			m_Default.ShowWindow(FALSE);
			m_Alternate.ShowWindow(FALSE);
			m_DefLabel.ShowWindow(FALSE);
			m_AltLabel.ShowWindow(FALSE);
			m_Default.SetCurSel(-1);
			m_Alternate.SetCurSel(-1);				
			CString sLoad;
			sLoad = theWorkspace.LoadResourceString(IDS_FILETYPE);
			m_DefLabel.SetWindowText(sLoad);
			m_FileExt.SetWindowText(sDwg);
			m_FileExt.ShowWindow(TRUE);			
			m_DefLabel.ShowWindow(TRUE);
			break;
		}
			
		default:
		{
			m_Default.ShowWindow(FALSE);
			m_Alternate.ShowWindow(FALSE);		
			m_DefLabel.ShowWindow(FALSE);
			m_AltLabel.ShowWindow(FALSE);
			break;
		}
	}
	SetModified();
}

void CColumnsPage::SetColumn(int nIndex)
{
	
	HD_ITEM curItem;
	LPTSTR  pStrTemp;
	CHeaderCtrl* pHdrCtrl= NULL;

	// retrieve embedded header control			
	pHdrCtrl = m_List.GetHeaderCtrl();
	if (pHdrCtrl == NULL)
		return;

	if (nIndex >= pHdrCtrl->GetItemCount())
		return;


	// add bmaps to each header item
	if (!pHdrCtrl->GetItem(nIndex, &curItem))
		return;

	if (m_ColData[m_nIndex].m_Image == -1 || m_ColData[m_nIndex].m_Image >= m_Image.GetCount())
	{
		curItem.mask= HDI_TEXT | HDI_FORMAT;
		curItem.iImage = -1;
	}
	else
	{
		curItem.mask= HDI_TEXT | HDI_IMAGE | HDI_FORMAT;
		curItem.iImage= m_ColData[m_nIndex].m_Image;
	}


	pStrTemp = m_ColData[m_nIndex].m_Caption.GetBuffer(m_ColData[m_nIndex].m_Caption.GetLength());
	curItem.pszText = pStrTemp;


	switch(m_ColData[m_nIndex].m_Alignment)
	{					
	case 1:
		curItem.fmt= HDF_CENTER | HDF_STRING;
		break;
	case 2:
		curItem.fmt= HDF_RIGHT | HDF_STRING;
		break;				
	default:
		curItem.fmt= HDF_LEFT | HDF_STRING;
		break;
	}
	
	if (m_ColData[m_nIndex].m_Image == -1 || m_ColData[m_nIndex].m_Image >= m_Image.GetCount())
	{
		// do nothing
	}
	else
	{
		curItem.fmt = curItem.fmt | HDF_IMAGE;
	}

	pHdrCtrl->SetItem(nIndex, &curItem);


}


void CColumnsPage::OnSelchangeImage() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	int nSel = m_Image.GetCurSel();

	m_ColData[m_nIndex].m_Image = nSel;

	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();
	
	if (pHeader->GetImageList() == NULL)
		pHeader->SetImageList(&m_pImageListPage->m_ImageList);
	
	SetColumn(m_nIndex);
	SetModified();
}

void CColumnsPage::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	// set the changing now flag
	m_bChangingIndex = true;

	// reset the changing now flag
	m_bChangingIndex = false;
	
	*pResult = 0;
}

void CColumnsPage::OnDelete() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();

	m_List.DeleteColumn(m_nIndex);

	
	// add the data to the list arrays
	m_ColData.RemoveAt(m_nIndex);

	
	if (m_nIndex == 0 && pHeader->GetItemCount() == 0)
	{
		m_Spin.EnableWindow(FALSE);
		m_Alignment.EnableWindow(FALSE);
	}
	else
	{
		m_Spin.EnableWindow(TRUE);
		m_Spin.SetBuddy(&m_IndexEdit);
		m_Alignment.EnableWindow(TRUE);		
	}

	if (m_nIndex >= m_ColData.GetSize()) 
		m_nIndex = m_ColData.GetSize();
	
	m_Spin.SetRange(0, m_nIndex);
	m_Spin.SetPos(m_nIndex);

	SetControls(m_nIndex);
	SetModified();
}

void CColumnsPage::OnSelchangeDefault() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;
	
	m_ColData[m_nIndex].m_Default = m_Default.GetCurSel();
	SetModified();
}

void CColumnsPage::OnSelchangeAlternate() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;
	
	m_ColData[m_nIndex].m_Alternate = m_Alternate.GetCurSel();
	SetModified();
	
}

void CColumnsPage::OnDroplistbtn() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;
	

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
			Dlg.sImages.Add(m_ColData[m_nIndex].m_ImageItems[i]);
		}
		
		Dlg.m_pImageListPage = m_pImageListPage;
		
		if (Dlg.DoModal() == 1)
		{
			if (Dlg.sStrings.GetSize() == 0)
				return;

			// get the text
			m_ColData[m_nIndex].m_ListItems.RemoveAll();

      int i;
			for (i=0; i<Dlg.sStrings.GetSize(); i++)
			{
				m_ColData[m_nIndex].m_ListItems.Add(Dlg.sStrings[i]);
			}

			m_ColData[m_nIndex].m_ImageItems.RemoveAll();

			for (i=0; i<Dlg.sImages.GetSize(); i++)
			{
				int n = Dlg.sImages[i];
				m_ColData[m_nIndex].m_ImageItems.Add(Dlg.sImages[i]);
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
			if (Dlg.sStrings.GetSize() == 0)
				return;

			m_ColData[m_nIndex].m_ListItems.RemoveAll();

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

void CColumnsPage::OnSelchangeDate() 
{	
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	m_ColData[m_nIndex].m_Image = m_Dates.GetCurSel();	
	SetModified();
}

void CColumnsPage::OnSelchangeTime() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	m_ColData[m_nIndex].m_Image = m_Times.GetCurSel();	
	SetModified();
}

void CColumnsPage::OnChangeFileext() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	CString sText;
	m_FileExt.GetWindowText(sText);
	
	m_ColData[m_nIndex].m_ListItems.RemoveAll();
	m_ColData[m_nIndex].m_ListItems.Add(sText);

	SetModified();
	
}

void CColumnsPage::OnChangeIndexEdit() 
{
	// if the spin button has been clicked.
	if (m_bChangingIndex)
		// exit here now.
		return;

	
	m_bChangingIndex = true;
	CString sIndex;
	m_IndexEdit.GetWindowText(sIndex);
	m_nIndex = _tstol(sIndex);
	SetControls(m_nIndex);
	m_Text.SetFocus();

	m_bChangingIndex = false;
	
}

void CColumnsPage::OnSetfocusIndexEdit() 
{
	m_Text.SetFocus();
	
}
