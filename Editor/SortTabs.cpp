// SortTabs.cpp : implementation file
//

#include "stdafx.h"
#include "SortTabs.h"


/////////////////////////////////////////////////////////////////////////////
// CSortTabs property page

IMPLEMENT_DYNCREATE(CSortTabs, CPropertyPage)

CSortTabs::CSortTabs() : CPropertyPage(CSortTabs::IDD)
{
	//{{AFX_DATA_INIT(CSortTabs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSortTabs::~CSortTabs()
{
}

void CSortTabs::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSortTabs)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSortTabs, CPropertyPage)
	//{{AFX_MSG_MAP(CSortTabs)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortTabs message handlers

BOOL CSortTabs::OnSetActive() 
{
	m_List.ResetContent();

	for (int i=0; i<m_pTabPane->m_TabList.GetCount(); i++)
	{
		POSITION pos = m_pTabPane->m_TabList.FindIndex(i);
		if (pos != NULL)
		{		
			CTabInfo *pTab = m_pTabPane->m_TabList.GetAt(pos);
			m_List.AddString(pTab->m_sCaption);
		}
	}
	
	return CPropertyPage::OnSetActive();
}

BOOL CSortTabs::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_pTabPane->Setup();
	OnSetActive();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CSortTabs::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int ni1 = m_List.GetCurSel();
	int ni2 = ni1 + pNMUpDown->iDelta;
	
	if (ni2 == m_List.GetCount())
		ni2 = 0;

	if (ni2 < 0)
		ni2 = m_List.GetCount()-1;

	CTabInfo *pTab1;
	CTabInfo *pTab2;

	POSITION pos1 = m_pTabPane->m_TabList.FindIndex(ni1);
	if (pos1 != NULL)
	{		
		pTab1 = m_pTabPane->m_TabList.GetAt(pos1);
	}

	POSITION pos2 = m_pTabPane->m_TabList.FindIndex(ni2);
	if (pos2 != NULL)
	{		
		pTab2 = m_pTabPane->m_TabList.GetAt(pos2);
	}

	m_pTabPane->m_TabList.SetAt(pos1, pTab2);
	m_pTabPane->m_TabList.SetAt(pos2, pTab1);

	OnSetActive();

	m_List.SetCurSel(ni2);
	*pResult = 0;
}

BOOL CSortTabs::OnApply() 
{
	m_pTabPane->OnApply();

	return CPropertyPage::OnApply();
}
