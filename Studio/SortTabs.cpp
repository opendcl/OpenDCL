// SortTabs.cpp : implementation file
//

#include "stdafx.h"
#include "SortTabs.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CSortTabs property page

CSortTabs::CSortTabs( COpenDCLView* pView, CDclControlObject* pControl, CTabsPane* pTabsPane )
: CPropertyPage(CSortTabs::IDD)
, mpView( pView )
, mpDclControl( pControl )
, mpTabsPane( pTabsPane )
{
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

	for (int i=0; i<mpTabsPane->GetTabList().GetCount(); i++)
	{
		POSITION pos = mpTabsPane->GetTabList().FindIndex(i);
		if (pos != NULL)
		{		
			CTabInfo *pTab = mpTabsPane->GetTabList().GetAt(pos);
			m_List.AddString(pTab->msCaption);
		}
	}
	m_List.SetCurSel( (mpTabsPane->GetTabList().GetCount() > 0)? 0 : -1 );
	
	return CPropertyPage::OnSetActive();
}

BOOL CSortTabs::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	mpTabsPane->Setup();
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

	CTabInfo *pTab1 = NULL;
	CTabInfo *pTab2 = NULL;

	POSITION pos1 = mpTabsPane->GetTabList().FindIndex(ni1);
	if (pos1 != NULL)
		pTab1 = mpTabsPane->GetTabList().GetAt(pos1);

	POSITION pos2 = mpTabsPane->GetTabList().FindIndex(ni2);
	if (pos2 != NULL)
		pTab2 = mpTabsPane->GetTabList().GetAt(pos2);

	if( pTab1 && pTab2 )
	{
		mpTabsPane->GetTabList().SetAt(pos1, pTab2);
		mpTabsPane->GetTabList().SetAt(pos2, pTab1);
	}

	SetModified();
	OnSetActive();

	m_List.SetCurSel(ni2);
	*pResult = 0;
}

BOOL CSortTabs::OnApply() 
{
	mpTabsPane->OnApply();
	theWorkspace.SetModified(true);
	return CPropertyPage::OnApply();
}
