// TabsPane.cpp : implementation file
//

#include "stdafx.h"
#include "TabsPane.h"
#include "DclControlObject.h"
#include "OpenDCLView.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "PropertyObject.h"
#include "SharedRes.h"
#include "Project.h"
#include "ProjectTreeCtrl.h"
#include "EditorWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CTabsPane dialog

CTabsPane::CTabsPane( COpenDCLView* pView,
											CDclControlObject* pControl,
											RefCountedPtr< CImageList >& pImageList )
: CPropertyPage(CTabsPane::IDD)
, mpView( pView )
, mpDclControl( pControl )
, mpImageList( pImageList )
, mnTabIndex( -1 )
{
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
	DDX_Control(pDX, IDC_FRAME, m_Frame);
	DDX_Control(pDX, IDC_CAPTION, m_Caption);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
	DDX_Control(pDX, IDC_SPIN, m_SpinBtn);
	DDX_Control(pDX, IDC_TTT, m_ToolTipTitle);
}


BEGIN_MESSAGE_MAP(CTabsPane, CPropertyPage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_EN_CHANGE(IDC_CAPTION, OnChangeCaption)
	ON_CBN_SELCHANGE(IDC_IMAGE, OnChangeImage)
	ON_EN_CHANGE(IDC_TTT, OnChangeTtt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabsPane message handlers

BOOL CTabsPane::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	Setup();

	m_SpinBtn.SetRange(0, m_TabList.GetCount());
	m_SpinBtn.EnableWindow(m_TabList.GetCount() > 1);
	bool bHasTabs = (m_TabList.GetCount() > 0);
	GetDlgItem(IDC_DELETE)->EnableWindow(bHasTabs);
	m_Caption.EnableWindow(bHasTabs);
	bool bHasImages = (mpImageList->m_hImageList && mpImageList->GetImageCount() > 0);
	m_Image.EnableWindow(bHasImages && bHasTabs);
	m_ToolTipTitle.EnableWindow(bHasTabs);

	UpdateFrame();
	UpdateTabInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CTabsPane::Setup() 
{
	if (mnTabIndex != -1)
		return;

	POSITION pos = NULL;
	m_pTabCaptions = mpDclControl->GetPropertyObject(nTabsCaption);	
	m_pTabTTT = mpDclControl->GetPropertyObject(nTabsTTT);	
	m_pTabImages = mpDclControl->GetPropertyObject(nTabsImageList);	

	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	for (size_t i = 0; i < m_pTabCaptions->GetStringArrayPtr()->size(); i++ )
	{
		CTabInfo *pTab = new CTabInfo(i);
		pTab->mpChildForm = pProject->FindDclTabChildForm(mpView->m_pThisDclForm->GetUniqueName(), i);
		pTab->msCaption = m_pTabCaptions->GetStringArrayPtr()->at(i);
		if (i < m_pTabImages->GetIntArrayPtr()->size())
			pTab->mnImageIndex = m_pTabImages->GetIntArrayPtr()->at(i);
		else
			pTab->mnImageIndex = -1;
		if (i < m_pTabTTT->GetStringArrayPtr()->size())
			pTab->msToolTipTitle = m_pTabTTT->GetStringArrayPtr()->at(i);
		m_TabList.AddTail(pTab);
	}

	mnTabIndex = 0;
}

void CTabsPane::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nIndex = pNMUpDown->iPos;
	
	mnTabIndex += pNMUpDown->iDelta;

	// if this index has been deleted, skip it.
	POSITION pos = NULL;

	// lets check our wrapping
	// if to big
	if (mnTabIndex == m_TabList.GetCount())
	{
		mnTabIndex = 0;
		m_SpinBtn.SetPos(mnTabIndex);
	}
	// if to0 small
	if (mnTabIndex == -1)
	{
		mnTabIndex = m_TabList.GetCount()-1;
		m_SpinBtn.SetPos(mnTabIndex);
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
		sText.Format( _T("%s%d"), theWorkspace.LoadResourceString(IDS_TABNO), mnTabIndex + 1 );
		m_Frame.SetWindowText( sText );
	}
	else
		m_Frame.SetWindowText( _T("") );
}

void CTabsPane::UpdateTabInfo() 
{
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(mnTabIndex);
	if (pos != NULL)
	{
		CTabInfo* pTabInfo = m_TabList.GetAt(pos);
		m_Caption.SetWindowText(pTabInfo->msCaption);
		if( mpImageList->m_hImageList && mpImageList->GetImageCount() > 0 )
		{
			if( pTabInfo->mnImageIndex >= 0 && pTabInfo->mnImageIndex < mpImageList->GetImageCount() )
				m_Image.SetCurSel(pTabInfo->mnImageIndex + 1);
			else 
				m_Image.SetCurSel(0);
			m_Image.EnableWindow(TRUE);
		}
		else
		{
			m_Image.SetCurSel(0);
			m_Image.EnableWindow(FALSE);
		}
		m_ToolTipTitle.SetWindowText(pTabInfo->msToolTipTitle);
	}
	else
	{
		mnTabIndex = 0;
		m_Caption.SetWindowText( _T("") );	
		m_Image.SetCurSel(0);
		m_Image.EnableWindow(FALSE);
		m_ToolTipTitle.SetWindowText( _T("") );
	}
}

void CTabsPane::OnAdd() 
{
	CTabInfo *pTab = new CTabInfo;

	if( m_TabList.GetCount() > 0 )
		++mnTabIndex; // increment the index to the new tab (except when adding the first tab)

	// check to see where we are going to insert the tab
	POSITION pos = m_TabList.FindIndex(mnTabIndex); 		
	if (pos != NULL)
		m_TabList.InsertBefore(pos, pTab);
	else
		m_TabList.AddTail(pTab);

	CString sCaption;
	sCaption.Format( _T("%s%d"), theWorkspace.LoadResourceString(IDS_TAB), mnTabIndex + 1 );
	m_Caption.SetWindowText( sCaption );
	m_ToolTipTitle.SetWindowText( _T("") );

	m_SpinBtn.SetRange(0, m_TabList.GetCount());
	m_SpinBtn.EnableWindow(m_TabList.GetCount() > 1);
	GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	m_Caption.EnableWindow(TRUE);
	bool bHasImages = (mpImageList->m_hImageList && mpImageList->GetImageCount() > 0);
	m_Image.SetCurSel(0);
	m_Image.EnableWindow(bHasImages);
	m_ToolTipTitle.EnableWindow(TRUE);

	UpdateFrame();
}

void CTabsPane::OnDelete() 
{
	POSITION pos = m_TabList.FindIndex(mnTabIndex);
	if (pos != NULL)
	{
		CTabInfo *pTab = m_TabList.GetAt(pos);
		
		// check to see if the user wishes to delete this tab if it has controls on it.
		if (!mpView->CanRemoveChildTabPane(pTab->mnOriginalIndex))
			return;

		m_TabList.RemoveAt(pos);
		// move the deleted tab item over to the deleted tab list for later cleanup
		if (pTab != NULL)
		{
			m_DeletedTabList.AddTail(pTab);
			if( pTab->mpChildForm )
				pTab->mpChildForm->m_bDeleted = true;
		}
	}

	// increment the index to the new tab
	if( mnTabIndex > 0 )
		--mnTabIndex;
	m_SpinBtn.SetRange(0, m_TabList.GetCount());
	m_SpinBtn.EnableWindow(m_TabList.GetCount() > 1);
	GetDlgItem(IDC_DELETE)->EnableWindow(m_TabList.GetCount() > 0);
	m_Caption.EnableWindow(m_TabList.GetCount() > 0);
	bool bHasImages = (mpImageList->m_hImageList && mpImageList->GetImageCount() > 0);
	m_Image.EnableWindow(bHasImages);
	m_ToolTipTitle.EnableWindow(m_TabList.GetCount() > 0);
	UpdateFrame();
	UpdateTabInfo();
	SetModified();
}

void CTabsPane::OnChangeCaption() 
{
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(mnTabIndex);
	if (pos != NULL)
	{
		CString sText;
		m_Caption.GetWindowText(sText);
		m_TabList.GetAt(pos)->msCaption = sText;
	}
	SetModified();
}

void CTabsPane::OnChangeImage() 
{
	POSITION pos = m_TabList.FindIndex(mnTabIndex);
	if (pos != NULL)
		m_TabList.GetAt(pos)->mnImageIndex = m_Image.GetCurSel() - 1;
	SetModified();
}

void CTabsPane::OnChangeTtt() 
{
	// here we are going to update the edit boxes
	POSITION pos = m_TabList.FindIndex(mnTabIndex);
	if (pos != NULL)
	{
		CString sText;
		m_ToolTipTitle.GetWindowText(sText);
		m_TabList.GetAt(pos)->msToolTipTitle = sText;
	}
	SetModified();
}


BOOL CTabsPane::OnApply() 
{
	try
	{
		// clean up the deleted items first
		POSITION pos;
		while ((pos = m_DeletedTabList.GetHeadPosition()) != NULL)
		{
			CTabInfo *pTabInfo = m_DeletedTabList.GetHead();
			if (pTabInfo->mpChildForm)
				mpView->RemoveChildTabPane(pTabInfo->mpChildForm);
			m_DeletedTabList.RemoveAt(pos);
			delete pTabInfo;
			pos = m_DeletedTabList.GetHeadPosition();
		}

		// reset the tab info lists
		m_pTabCaptions->clear();
		m_pTabTTT->clear();
		m_pTabImages->clear();
		theEditorWorkspace.GetProjectTreeCtrl()->RemoveChildren( mpDclControl->GetOwnerForm()->m_htiTreeItem );

		// repopulate the tab info lists
		int idxPane = -1;
		POSITION posTab = m_TabList.GetHeadPosition();
		while( posTab )
		{
			++idxPane;
			CTabInfo* pTab = m_TabList.GetNext( posTab );
			m_pTabCaptions->GetStringArrayPtr()->push_back( pTab->msCaption );
			m_pTabTTT->GetStringArrayPtr()->push_back( pTab->msToolTipTitle );
			m_pTabImages->GetIntArrayPtr()->push_back( pTab->mnImageIndex );

			pTab->mnOriginalIndex = idxPane;
			if( !pTab->mpChildForm )
				pTab->mpChildForm = mpView->AddSingleTabPane( idxPane );
			pTab->mpChildForm->SetTabIndex( idxPane );
			theEditorWorkspace.GetProjectTreeCtrl()->AddFormToTree( pTab->mpChildForm, true );
		}	
		
		// call the method to update the control itself
		mpView->RefreshChildControl(mpDclControl, (PropertyId)-2);
		mpView->ResizeChildTabPanes();
		theEditorWorkspace.GetProjectTreeCtrl()->CleanupParents();
	}
	catch(...)
	{
	}
	theWorkspace.SetModified(true);
	return CPropertyPage::OnApply();
}

BOOL CTabsPane::OnSetActive()
{
	if( !__super::OnSetActive() )
		return FALSE;
	m_Image.SetImageList(mpImageList);
	bool bHasImages = (mpImageList->m_hImageList && mpImageList->GetImageCount() > 0);
	m_Image.ResetContent();
	CString sNone = theWorkspace.LoadResourceString(IDS_NONE);
	COMBOBOXEXITEM cbi = { CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT, 0, sNone.LockBuffer(), 0, -1, -1 };
	m_Image.InsertItem( &cbi );
	if(bHasImages)
	{
		for( int i = 0; i < mpImageList->GetImageCount(); ++i )
		{
			COMBOBOXEXITEM cbi = { CBEIF_IMAGE | CBEIF_SELECTEDIMAGE, i + 1, NULL, 0, i, i };
			m_Image.InsertItem( &cbi );
		}
	}
	UpdateTabInfo();
	return TRUE;
}
