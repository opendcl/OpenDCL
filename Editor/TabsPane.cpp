// TabsPane.cpp : implementation file
//

#include "stdafx.h"
#include "TabsPane.h"
#include "DclControlObject.h"
#include "ObjectDCLView.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "PropertyObject.h"
#include "Project.h"
#include "ProjectTreeCtrl.h"
#include "EditorWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CTabsPane dialog

IMPLEMENT_DYNCREATE(CTabsPane, CPropertyPage)


CTabsPane::CTabsPane() : CPropertyPage(CTabsPane::IDD)
{
	//{{AFX_DATA_INIT(CTabsPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDclForm = NULL;
	m_pControl = NULL;
	m_nTabIndex = -1;
}


CTabsPane::~CTabsPane()
{
	POSITION pos = m_TabList.GetHeadPosition();
	while (pos != NULL)
	{
		CTabInfo *pTabInfo = m_TabList.GetHead();
		m_TabList.RemoveAt(pos);
		pos = m_TabList.GetHeadPosition();
		delete pTabInfo;
	}
	
	pos = m_DeletedTabList.GetHeadPosition();
	while (pos != NULL)
	{
		try
		{
			// get the tab info
			CTabInfo *pTabInfo = m_DeletedTabList.GetHead();
			// remove it from the list
			m_DeletedTabList.RemoveAt(pos);
			// get next head position
			pos = m_DeletedTabList.GetHeadPosition();
			// delete the item.
			delete pTabInfo;
		}
		catch(...)
		{
		}
	}
}


void CTabsPane::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabsPane)
	DDX_Control(pDX, IDC_FRAME, m_Frame);
	DDX_Control(pDX, IDC_CAPTION, m_Caption);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
	DDX_Control(pDX, IDC_SPIN, m_SpinBtn);
	DDX_Control(pDX, IDC_TTT, m_ToolTipText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabsPane, CPropertyPage)
	//{{AFX_MSG_MAP(CTabsPane)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_EN_CHANGE(IDC_CAPTION, OnChangeCaption)
	ON_EN_CHANGE(IDC_TTT, OnChangeTtt)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabsPane message handlers

BOOL CTabsPane::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	Setup();

	m_SpinBtn.SetRange(-1, m_TabList.GetCount());
	
	UpdateCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CTabsPane::Setup() 
{
	if (m_nTabIndex != -1)
		return;

	POSITION pos = NULL;
	m_pTabCaptions = m_pControl->GetPropertyObject(nTabsCaption);	
	m_pTabTTT = m_pControl->GetPropertyObject(nTabsTTT);	
	m_pTabImages = m_pControl->GetPropertyObject(nTabsImageList);	

	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	for (size_t i=0; i<m_pTabCaptions->GetStringArrayPtr()->size(); i++)
	{
		CTabInfo *pTab = new CTabInfo(i);
		
		// get a pointer to the child dcl form pane for later use
		pTab->m_pChildForm = pProject->GetDclTabChildForm(m_pView->m_pThisDclForm->GetUniqueName(), i);

		// add the caption
		pTab->m_sCaption = m_pTabCaptions->GetStringArrayPtr()->at(i);

		// add the tool tip text
		if (i < m_pTabTTT->GetStringArrayPtr()->size())
			pTab->m_sToolTipText = m_pTabTTT->GetStringArrayPtr()->at(i);

		m_TabList.AddTail(pTab);
	}

	m_nTabIndex = 0;
}
void CTabsPane::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nIndex = pNMUpDown->iPos;
	
	m_nTabIndex += pNMUpDown->iDelta;

	// if this index has been deleted, skip it.
	POSITION pos = NULL;

	// lets check our wrapping
	// if to big
	if (m_nTabIndex == m_TabList.GetCount())
	{
		m_nTabIndex = 0;
		m_SpinBtn.SetPos(m_nTabIndex);
	}
	// if to small
	if (m_nTabIndex == -1)
	{
		m_nTabIndex = m_TabList.GetCount()-1;
		m_SpinBtn.SetPos(m_nTabIndex);
	}

	
	UpdateCtrls();
	m_Caption.SetFocus();
	*pResult = 0;
}

void CTabsPane::UpdateCtrls() 
{
	// here we are going to update the frame
	char value[10];
	_ltoa(m_nTabIndex, value, 10);
	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_TABNO);
	m_Frame.SetWindowText(sText + value);
		
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		m_Caption.SetWindowText(m_TabList.GetAt(pos)->m_sCaption);	
		sText = m_TabList.GetAt(pos)->m_sToolTipText;
		m_ToolTipText.SetWindowText(sText);
	}
	
}

void CTabsPane::OnAdd() 
{
	char value[10];
	_ltoa(m_nTabIndex, value, 10);
	CString sText;

	sText = theWorkspace.LoadResourceString(IDS_TAB);
	CTabInfo *pTab = new CTabInfo;
		
	pTab->m_sCaption = sText + value;
	pTab->m_sToolTipText = "";

	// check to see where we are going to insert the tab
	if (m_nTabIndex == m_TabList.GetCount()-1)
	{
		m_TabList.AddTail(pTab);
	}
	else
	{
		POSITION pos = m_TabList.FindIndex(m_nTabIndex); 		
		if (pos != NULL)
			m_TabList.InsertAfter(pos, pTab);
		else
			m_TabList.AddTail(pTab);
	}
	// increment the index to the new tab
	m_nTabIndex++;
	m_SpinBtn.SetRange(-1, m_TabList.GetCount());

	UpdateCtrls();
	SetModified();
}

void CTabsPane::OnChangeCaption() 
{
	CString sText;
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		m_Caption.GetWindowText(sText);
		m_TabList.GetAt(pos)->m_sCaption = sText;
	}
	SetModified();
	
}

void CTabsPane::OnChangeTtt() 
{
	CString sText;
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		m_ToolTipText.GetWindowText(sText);
		m_TabList.GetAt(pos)->m_sToolTipText = sText;
	}
	SetModified();
}


BOOL CTabsPane::OnApply() 
{
	try
	{
		// clean up the deleted items first
		POSITION pos = m_DeletedTabList.GetHeadPosition();
		while (pos != NULL)
		{
			// get the tab info
			CTabInfo *pTabInfo = m_DeletedTabList.GetHead();
			
			// if the tab to be delete has a CDclFormObject associated with it, 
			// we must delete it to.
			if (pTabInfo->m_OriginalIndex > -1)
			{
				// make the call to the view to delete the existing tab's CDclFormObject
				m_pView->RemoveChildTabPane(pTabInfo->m_pChildForm);
			}

			// remove it from the list
			m_DeletedTabList.RemoveAt(pos);
			
			// get next head position
			pos = m_DeletedTabList.GetHeadPosition();
			
			// delete the item.
			delete pTabInfo;
		}

		// reset the tab info lists
		m_pTabCaptions->GetStringArrayPtr()->clear();
		m_pTabTTT->GetStringArrayPtr()->clear();
		//m_pTabImages->clear();

		// repopulate the tab info lists
		for (int i=0; i<m_TabList.GetCount(); i++)
		{
			// add the caption
			POSITION pos = m_TabList.FindIndex(i);
			if (pos != NULL)
			{
				CTabInfo *pTab = m_TabList.GetAt(pos);
				m_pTabCaptions->GetStringArrayPtr()->push_back(pTab->m_sCaption);
				m_pTabTTT->GetStringArrayPtr()->push_back(pTab->m_sToolTipText);

				// if this is a new tab, add it to the dcl forms
				if (!pTab->m_pChildForm)
				{
					assert( pTab->m_OriginalIndex == -1 || pTab->m_OriginalIndex == i );
					pTab->m_pChildForm = m_pView->AddSingleTabPane(i);
					pTab->m_OriginalIndex = i;
				}
				pTab->m_pChildForm->SetTabIndex(i);
			}	
		}
		
		// call the method to update the control itself
		m_pView->RefreshChildControl(m_pControl, (PropertyId)-2);
		m_pView->ResizeChildTabPanes();
		theEditorWorkspace.GetProjectTreeCtrl()->CleanupParents();

	}
	catch(...)
	{
	}
	return CPropertyPage::OnApply();
}


void CTabsPane::OnDelete() 
{
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		CTabInfo *pTab = m_TabList.GetAt(pos);
		
		// check to see if the user wishes to delete this tab if it has controls on it.
		if (!m_pView->CanRemoveChildTabPane(pTab->m_OriginalIndex))
			return;

		m_TabList.RemoveAt(pos);
		// move the deleted tab item over to the deleted tab list for later cleanup
		if (pTab != NULL)
			m_DeletedTabList.AddTail(pTab);
		
	}

	// increment the index to the new tab
	m_nTabIndex--;
	m_SpinBtn.SetRange(-1, m_TabList.GetCount());

		
	UpdateCtrls();
	SetModified();
}
