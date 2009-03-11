// ImageListContents.cpp : implementation file
//

#include "stdafx.h"
#include "ImageListContents.h"
#include "ImageListPage.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CImageListContents dialog

CImageListContents::CImageListContents(CWnd* pParent /*=NULL*/)
	: CDialog(CImageListContents::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImageListContents)
	//}}AFX_DATA_INIT
}


void CImageListContents::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageListContents)
	DDX_Control(pDX, IDC_COMBOBOXEX, m_Image);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_THELIST, m_TheList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImageListContents, CDialog)
	//{{AFX_MSG_MAP(CImageListContents)
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_THELIST, OnClickThelist)
	ON_NOTIFY(NM_DBLCLK, IDC_THELIST, OnDblclkThelist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_THELIST, OnEndlabeleditThelist)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_THELIST, OnBeginlabeleditThelist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_THELIST, OnItemchangedThelist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_THELIST, OnKeydownThelist)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX, OnSelchangeComboboxex)
	ON_BN_CLICKED(IDUPDATE, OnUpdate)
	ON_NOTIFY(LVN_ODSTATECHANGED, IDC_THELIST, OnOdstatechangedThelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageListContents message handlers

BOOL CImageListContents::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Image.SetImageList(m_pImageListPage->GetImageList());
	m_TheList.SetImageList(m_pImageListPage->GetImageList(), LVSIL_SMALL);
	m_TheList.SetImageList(m_pImageListPage->GetImageList(), TVSIL_NORMAL);

	m_pImageListPage->GetImageList()->SetBkColor(RGB(255,255,255));

  for( int i = 0; i < m_pImageListPage->GetImageList()->GetImageCount(); ++i )
	{
		CString sValue;
		sValue.Format( _T("%u"), i );
		COMBOBOXEXITEM cbi = 
		{
			CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE,
			i,
			sValue.LockBuffer(),
			-1,
			i,
			i,
		};
		m_Image.InsertItem( &cbi );
	}
	
	//m_Image.SetCurSel(0);

	
	for( int i = 0; i < sStrings.GetSize(); ++i )
	{
		int n = 0;
		if (i>=sImages.GetSize())
		{
			sImages.Add(0);
		}
		else
		{
			n = sImages[i];
			if (n < 0)
				n = 0;
		}
		m_TheList.InsertItem(i, sStrings[i], n);
	}

	sImages.RemoveAll();
	sStrings.RemoveAll();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CImageListContents::OnClickThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;	
	
	Changed(plvdi->item.mask);
	*pResult = 0;
}

void CImageListContents::Changed(int nItem) 
{
	if (nItem == -1)
	{
		m_Image.SetCurSel(-1);
		m_Edit.SetWindowText(CString());
		return;
	}
	
	LV_ITEM lvitem;
					
	lvitem.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvitem.iItem = nItem;
	lvitem.iSubItem = 0;				
	lvitem.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
			LVIS_FOCUSED |  LVIS_SELECTED | 
			LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

	m_TheList.GetItem( &lvitem );
	
	m_Image.SetCurSel(lvitem.iImage);
	m_Edit.SetWindowText(m_TheList.GetItemText(nItem, 0));

	m_TheList.SetItem(nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);		
	
}

void CImageListContents::OnDblclkThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CImageListContents::OnEndlabeleditThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CImageListContents::OnBeginlabeleditThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CImageListContents::OnItemchangedThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CImageListContents::OnKeydownThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CImageListContents::OnChangeEdit1() 
{
	OnUpdate();	
}

void CImageListContents::OnSelchangeComboboxex() 
{
	OnUpdate();
}

void CImageListContents::OnUpdate() 
{
	if (m_TheList.GetItemCount() == 0)
		return;

	int nItem;
	POSITION pos = m_TheList.GetFirstSelectedItemPosition();
	if (pos == NULL)	
		nItem = m_TheList.GetItemCount();
	if (pos != NULL)	
		nItem = m_TheList.GetNextSelectedItem(pos);

	CString sText;
	m_Edit.GetWindowText(sText);
	int nSel = m_Image.GetCurSel();

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	lvItem.iItem = nItem;
	lvItem.iSubItem = 0;	
	lvItem.iImage = nSel;	
	lvItem.pszText = sText.LockBuffer();

	m_TheList.SetItem(&lvItem);

}

void CImageListContents::OnAdd() 
{
	int nItem;
	POSITION pos = m_TheList.GetFirstSelectedItemPosition();
	if (pos == NULL)	
	{
		nItem = m_TheList.GetItemCount();
	}
	else
	{
		nItem = m_TheList.GetNextSelectedItem(pos);
		nItem++;
	}
	

	CString sNewItem;
	sNewItem = theWorkspace.LoadResourceString(IDS_NEWITEM);
	m_Edit.SetWindowText(sNewItem);
	m_Image.SetCurSel(0);
	m_TheList.InsertItem(nItem, sNewItem, 0);

	
}

void CImageListContents::OnDelete() 
{
	int nItem;
	POSITION pos = m_TheList.GetFirstSelectedItemPosition();
	
	if (pos == NULL)	
		return;

	if (pos != NULL)	
		nItem = m_TheList.GetNextSelectedItem(pos);

	m_TheList.DeleteItem(nItem);
	if (nItem >= m_TheList.GetItemCount())
		nItem = m_TheList.GetItemCount()-1;
	
	if (nItem >= 0)
	{
		Changed(nItem);
	}
	else
	{
		m_Edit.SetWindowText(CString());
		m_Image.SetCurSel(-1);
	}

}


void CImageListContents::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CImageListContents::OnOK() 
{
	sStrings.RemoveAll();
	sImages.RemoveAll();

	for (int i=0; i<m_TheList.GetItemCount(); i++)
	{

		LV_ITEM lvitem;
					
		lvitem.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;				
		lvitem.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
				LVIS_FOCUSED |  LVIS_SELECTED | 
				LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

		m_TheList.GetItem( &lvitem );
		
		sStrings.Add(m_TheList.GetItemText(i, 0));
		sImages.Add(lvitem.iImage);

	}
	CDialog::OnOK();
}

void CImageListContents::OnOdstatechangedThelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVODSTATECHANGE* pStateChanged = (NMLVODSTATECHANGE*)pNMHDR;
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;	
	
	*pResult = 0;
}
