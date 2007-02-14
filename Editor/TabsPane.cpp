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

	m_SpinBtn.SetRange(0, m_TabList.GetCount());
	m_SpinBtn.EnableWindow(m_TabList.GetCount() > 1);
	m_Caption.EnableWindow(m_TabList.GetCount() > 0);
	m_ToolTipText.EnableWindow(m_TabList.GetCount() > 0);
	GetDlgItem(IDC_DELETE)->EnableWindow(m_TabList.GetCount() > 0);

	UpdateFrame();
	UpdateTabInfo();

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
	// if to0 small
	if (m_nTabIndex == -1)
	{
		m_nTabIndex = m_TabList.GetCount()-1;
		m_SpinBtn.SetPos(m_nTabIndex);
	}

	UpdateFrame();
	UpdateTabInfo();
	m_Caption.SetFocus();
	*pResult = 0;
}

void CTabsPane::UpdateFrame() 
{
	// here we are going to update the frame
	if( m_TabList.GetCount() > 0 )
	{
		CString sText;
		sText.Format( _T("%s%d"), theWorkspace.LoadResourceString(IDS_TABNO), m_nTabIndex + 1 );
		m_Frame.SetWindowText( sText );
	}
	else
		m_Frame.SetWindowText( _T("") );
}

void CTabsPane::UpdateTabInfo() 
{
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		CTabInfo* pTabInfo = m_TabList.GetAt(pos);
		m_Caption.SetWindowText(pTabInfo->m_sCaption);	
		m_ToolTipText.SetWindowText(pTabInfo->m_sToolTipText);
	}
	else
	{
		m_nTabIndex = 0;
		m_Caption.SetWindowText( _T("") );	
		m_ToolTipText.SetWindowText( _T("") );
	}
}

void CTabsPane::OnAdd() 
{
	CTabInfo *pTab = new CTabInfo;

	if( m_TabList.GetCount() > 0 )
		++m_nTabIndex; // increment the index to the new tab (except when adding the first tab)

	// check to see where we are going to insert the tab
	POSITION pos = m_TabList.FindIndex(m_nTabIndex); 		
	if (pos != NULL)
		m_TabList.InsertBefore(pos, pTab);
	else
		m_TabList.AddTail(pTab);

	CString sCaption;
	sCaption.Format( _T("%s%d"), theWorkspace.LoadResourceString(IDS_TAB), m_nTabIndex + 1 );
	m_Caption.SetWindowText( sCaption );
	m_ToolTipText.SetWindowText( _T("") );

	m_SpinBtn.SetRange(0, m_TabList.GetCount());
	m_SpinBtn.EnableWindow(m_TabList.GetCount() > 1);
	m_Caption.EnableWindow(TRUE);
	m_ToolTipText.EnableWindow(TRUE);
	GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);

	UpdateFrame();
}

void CTabsPane::OnChangeCaption() 
{
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		CString sText;
		m_Caption.GetWindowText(sText);
		m_TabList.GetAt(pos)->m_sCaption = sText;
	}
	SetModified();
}

void CTabsPane::OnChangeTtt() 
{
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(m_nTabIndex);
	if (pos != NULL)
	{
		CString sText;
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
			if (pTabInfo->m_pChildForm)
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
		m_pTabCaptions->clear();
		m_pTabTTT->clear();
		//m_pTabImages->clear();
		theEditorWorkspace.GetProjectTreeCtrl()->RemoveChildren( m_pDclForm->m_htiTreeItem );


		// repopulate the tab info lists
		int idxPane = -1;
		POSITION posTab = m_TabList.GetHeadPosition();
		while( posTab )
		{
			++idxPane;
			CTabInfo* pTab = m_TabList.GetNext( posTab );
			m_pTabCaptions->GetStringArrayPtr()->push_back( pTab->m_sCaption );
			m_pTabTTT->GetStringArrayPtr()->push_back( pTab->m_sToolTipText );

			// if this is a new tab, add it to the dcl forms
			if( !pTab->m_pChildForm )
			{
				assert( pTab->m_OriginalIndex == -1 || pTab->m_OriginalIndex == idxPane );
				pTab->m_pChildForm = m_pView->AddSingleTabPane( idxPane );
				pTab->m_OriginalIndex = idxPane;
			}
			else
				theEditorWorkspace.GetProjectTreeCtrl()->AddFormToTree( pTab->m_pChildForm, false );
			pTab->m_pChildForm->SetTabIndex( idxPane );
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
		{
			m_DeletedTabList.AddTail(pTab);
			if( pTab->m_pChildForm )
				pTab->m_pChildForm->m_bDeleted = true;
		}
	}

	// increment the index to the new tab
	if( m_nTabIndex > 0 )
		--m_nTabIndex;
	m_SpinBtn.SetRange(0, m_TabList.GetCount());
	m_SpinBtn.EnableWindow(m_TabList.GetCount() > 1);
	m_Caption.EnableWindow(m_TabList.GetCount() > 0);
	m_ToolTipText.EnableWindow(m_TabList.GetCount() > 0);
	GetDlgItem(IDC_DELETE)->EnableWindow(m_TabList.GetCount() > 0);
	UpdateFrame();
	UpdateTabInfo();
	SetModified();
}
