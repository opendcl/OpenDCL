// ArxControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlPane.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "XPStyleButtonST.h"
#include "Resource.h"
#include "AxContainer.h"
#include "VdclAngleEdit.h"
#include "OdclEdit.h"
#include "VdclNumericEdit.h"
#include "VdclSymbolEdit.h"
#include "VdclComboBoxEx.h"
#include "VdclComboBox.h"
#include "VdclArrowHeadComboBox.h"
#include "VdclColorComboBox.h"
#include "VdclLineWeightComboBox.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "FontCombo.h"
#include "PrinterComboBox.h"
#include "ComboBoxFolder.h"
#include "OdclLayerCombo.h"
#include "PropertyObject.h"
#include "Splitter.h"
#include "DwgDirList.h"
#include "OptionListBox.h"
#include "VdclStatic.h"
#include "GsPreviewCtrl.h"
#include "SpreadSheet.h"
#include "OdclListCtrl.h"
#include "VdclCheckBox.h"
#include "VdclGroupBox.h"
#include "VdclListBox.h"
#include "DwgPreviewCtrl.h"
#include "VdclRadioButton.h"
#include "OdclMonth.h"
#include "PictureBox.h"
#include "ProgressTimeToComplete.h"
#include "RoundSliderCtrl.h"
#include "VdclSliderCtrl.h"
#include "VdclScrollBar.h"
#include "VdclSpinButton.h"
#include "SlideHolder.h"
#include "StaticLink.h"
#include "HtmlCtrl.h"
#include "VdclTree.h"
#include "VdclTextButton.h"
#include "VdclTab.h"
#include "TabPage.h"
#include "DclFormObject.h"
#include "Project.h"
#include "ToolTips.h"

static const TCHAR *sFilterName = _T("IDB_FILTER";)


/////////////////////////////////////////////////////////////////////////////
// CArxControlPane

CArxControlPane::CArxControlPane()
: CControlPane()
{
	TraceFmt( _T("> CArxControlPane::CArxControlPane() [this = %08X]\r\n"), (long)this );
}

CArxControlPane::CArxControlPane(CDclFormObject* pSourceForm, CWnd* pHostDlg)
: CControlPane( pSourceForm, pHostDlg )
{
	TraceFmt( _T("> CArxControlPane::CArxControlPane(%s [%08X], %s [HWND: %08X]) [this: %08X]\r\n"),
						pSourceForm->GetKeyPath(), pSourceForm, CString(pHostDlg->GetRuntimeClass()->m_lpszClassName),
						pHostDlg->m_hWnd, (long)this );
}

CArxControlPane::~CArxControlPane()
{
	TraceFmt( _T("< CArxControlPane::~CArxControlPane() [this: %08X]\r\n"), (long)this );
}

TDialogControlPtr CArxControlPane::CreateNewDialogControl( CDclControlObject* pTemplate,
																												 CControlPane* pPane,
																												 UINT nID )
{
	return CArxDialogControl::Create( pTemplate, pPane, nID );
}


void AddTab(VdclTab *pControl, const CDclFormObject *pDclTab, int nTabIndex)
{
	// set counter for ArxControls
	int nCount = pDclTab->GetControlList().GetCount();
	
	
	CString sText;
	TC_ITEM TabCtrlItem;
	CString sTTT;
	TabCtrlItem.mask = TCIF_TEXT;
	
	// delete all previos tabs
	//pControl->m_Child.DeleteAllItems();

	// get the tab's lists
	RefCountedPtr< CPropertyObject > pTabsCaptionProperty = pControl->m_ArxControl->GetPropertyObject(nTabsCaption);
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = pControl->m_ArxControl->GetPropertyObject(nTabsTTT);
	RefCountedPtr< CPropertyObject > pTabsImageProperty = pControl->m_ArxControl->GetPropertyObject(nTabsImageList);

	int nTabQty = pTabsCaptionProperty->CountList();
	//for (int i = 0; i < nTabQty; i++)
	//{
	//	// get the tag caption
	//	TabCtrlItem.pszText = pTabsCaptionProperty->GetStringItem(i).GetBuffer(256);
	//	// add the new tab
	//	//pControl->m_Child.InsertItem( i, &TabCtrlItem );
	//}
	
	
	pControl->InitToolTip();
	pControl->m_Child.SetToolTips(&pControl->m_ToolTip);

	// set start position for navigating ArxControls
	POSITION pos = pDclTab->GetControlList().GetHeadPosition();
	
	// do loop to navigate Arx Controls
	while (pos != NULL)
	{
		// get current ArxControlObject
		CDclControlObject* pTemplate = pDclTab->GetControlList().GetNext(pos);

		if (pTemplate->GetType() == CtlTabStrip)
		{
			// get the tag caption
			TabCtrlItem.pszText = pTabsCaptionProperty->GetStringItem(nTabIndex).GetBuffer(256);
			
			int i = pControl->m_Child.GetItemCount();
			//// set the image list item number is required
			//if (pTabsImageProperty)
			//	TabCtrlItem.iImage = (int)atof(pTabsImageProperty->GetStringItem(nTabIndex));

			//// add the new tab
			//i = pControl->m_Child.InsertItem(i, &TabCtrlItem );
			//
			//CRect r;
			//POSITION pos = pTabsTTTProperty->m_stringList.FindIndex(nTabIndex);		
			//
			//if (pos != NULL)
			//{
			//	sText = pTabsTTTProperty->m_stringList.GetAt(pos);
			//	pControl->m_Child.GetItemRect(i, &r);
			//	pControl->m_ToolTip.AddTool(&pControl->m_Child, sText, &r, i);
			//}
		}

		// increment counter
		nCount--;
	}
	pControl->SetupTabs();
	//for (i = 0; i < nTabQty; i++)
	//{
	//	CRect r;
	//	POSITION pos = pTabsTTTProperty->m_stringList.FindIndex(i);		
	//	if (pos != NULL)
	//		sText = pTabsTTTProperty->m_stringList.GetAt(pos);

	//	m_Child.GetItemRect(i, &r);
	//	VERIFY(m_ToolTip.AddTool(&m_Child, sText, &r, i));
	//}

   	// Activate the tooltip control.
   	pControl->m_ToolTip.Activate(TRUE);
	
}

bool CArxControlPane::CreateTabPanes( CDclFormObject* pDclForm, CWnd *pTabControl, UINT& nId, CRect rcTab )
{
	bool bFailed = false;
	int nCount = 0;
	int nTabCount = 0;
	
	POSITION pos = mpProject->GetDclFormList().GetHeadPosition();
	while (pos)
	{
		CDclFormObject *pOtherDclForm = mpProject->GetDclFormList().GetNext(pos);
		pOtherDclForm->EnsureIsLoaded();

		// if mpHostDlg other dcl form's parent name is the name of pDclForm dcl form then load it as a tab pane
		if (pOtherDclForm->GetParentName() == pDclForm->GetUniqueName() && pOtherDclForm->GetParentName().GetLength() > 0)
		{
			// Create the tab pane itself that the controls actually sit on.
			CTabPage *pActualTabPane = new CTabPage(pOtherDclForm, pTabControl);
			pActualTabPane->Create( IDD_TABPAGE2, pTabControl);
			pActualTabPane->MoveWindow(rcTab, TRUE);

			// set the project pointer
			pActualTabPane->GetControlPane().GetPaneWindowRect().right = rcTab.right;
			pActualTabPane->GetControlPane().GetPaneWindowRect().bottom = rcTab.bottom;
			
			nId++;
			// call method to create the controls
			if (!pActualTabPane->GetControlPane().CreateControls(pOtherDclForm, nId))
				bFailed = true;
			
			pActualTabPane->ShowWindow(FALSE);
			
			CTabTracker *pTabTracker = new CTabTracker;
			pTabTracker->m_pDclForm = pOtherDclForm;
			pTabTracker->m_pTabPage = pActualTabPane;
			pTabTracker->m_nIndex = pOtherDclForm->GetTabIndex();
			((VdclTab*)pTabControl)->m_ArxTabsList.AddTail(pTabTracker);
			AddTab((VdclTab*)pTabControl, pDclForm, pOtherDclForm->GetTabIndex());
			nTabCount++;			
		}
	}
	return !bFailed;
}

void CArxControlPane::CleanUpControls() 
{	
	for (int idx = mControls.size() - 1; idx >= 0; --idx)
	{
		TDialogControlPtr pControl = mControls[idx];
		if( pControl->GetControlType() == CtlActiveX )
			((CAxContainer*)&*(pControl->GetControl()))->CloseWindow();
	}
	CControlPane::CleanUpControls();
}

CWnd* CArxControlPane::FindControl(CString sControlName, int nControlType) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter 
	int nCount = mpSourceForm->GetControlList().GetCount();
	while(nCount < mpSourceForm->GetControlList().GetCount())
	{
		// get position
		pos = mpSourceForm->GetControlList().FindIndex(nCount);
		if (pos != NULL)
		{
			// get current property
			CDclControlObject* pArxControlForm = mpSourceForm->GetControlList().GetAt(pos);
			
			// if mpHostDlg control's name is a match to the requested control
			if (sControlName == pArxControlForm->GetStrProperty(nName))
			{
				// return a pointer to the CWnd control
				CWnd *pControl = pArxControlForm->GetWindow(); //mpHostDlg->GetDlgItem(pArxControlForm->m_Id);
				if (pArxControlForm->GetType() != nControlType)
				{
					pControl = NULL;
					theWorkspace.DisplayAlert(theWorkspace.LoadResourceString(IDS_WRONGCTRLTYPE));
				}
				
				return pControl;
			}

			if (pArxControlForm->GetType() == CtlTabStrip)
			{
				// get the tab control
				CWnd *pControl = pArxControlForm->GetWindow(); //mpHostDlg->GetDlgItem(pArxControlForm->m_Id);

				// get the child ControlPane id count
				int nTabCount = ((VdclTab *)pControl)->m_ArxTabsList.GetCount();
				if (nTabCount > 0)
				{
					for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
					{
						POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
						if (pos != NULL)
						{
							CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
						
							// do loop to call mpHostDlg method for each child ControlPane
							for (int i=0; i < pTab->m_pDclForm->GetControlList().GetCount(); i++)
							{
								CArxControlPane ctrlPane;
								ctrlPane.mpSourceForm = pTab->m_pDclForm;
								ctrlPane.mpHostDlg = mpHostDlg;
								// get the child ControlPane
								CWnd *pChildControl = ctrlPane.FindControl(sControlName, nControlType);
								if (pChildControl != NULL)
									return pChildControl;
							}
						}
					}
				}
			}
		}
		// increment counter
		nCount++;
	}

	return NULL;
}


bool CArxControlPane::FindControl(HWND hwndControl, /*out*/ CString& sControlName) const
{
	if (!hwndControl)
		return false;

	// create a position variable to hold the counter increment
	POSITION pos = mpSourceForm->GetControlList().GetHeadPosition();	
	while(pos != NULL)
	{
		// get current property
		CDclControlObject* pArxControlForm = mpSourceForm->GetControlList().GetNext(pos);
		if (pArxControlForm != NULL)
		{
			// if mpHostDlg control's name is a match to the requested control
			if (pArxControlForm->GetWindow()->m_hWnd == hwndControl)
			{
				sControlName = pArxControlForm->GetStrProperty(nName);
				return true;
			}

			if (pArxControlForm->GetType() == CtlTabStrip)
			{
				// get the tab control
				CWnd *pControl = pArxControlForm->GetWindow(); 
				// get the child ControlPane id count
				int nTabCount = ((VdclTab *)pControl)->m_ArxTabsList.GetCount();
				if (nTabCount > 0)
				{
					for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
					{
						POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
						if (pos != NULL)
						{
							CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
						
							// do loop to call mpHostDlg method for each child ControlPane
							for (int i=0; i < pTab->m_pDclForm->GetControlList().GetCount(); i++)
							{
								CArxControlPane ctrlPane(pTab->m_pDclForm, pControl);
								// get the child ControlPane
								if (ctrlPane.FindControl(hwndControl, sControlName))
									return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}


TDialogControlPtr CArxControlPane::FindControl(HWND hwndControl) const
{
	if (!hwndControl)
		return false;
	size_t ctMax = mControls.size();
	for( size_t idx = 0; idx < ctMax; ++idx )
	{
		TDialogControlPtr pControl = mControls[idx];
		if( pControl->GetControl()->m_hWnd == hwndControl )
			return pControl;
		if (pControl->GetControlType() == CtlTabStrip)
		{
			POSITION posTab = ((VdclTab*)&*(pControl->GetControl()))->m_ArxTabsList.GetHeadPosition();
			if (posTab != NULL)
			{
				CTabTracker *pTab = ((VdclTab*)&*(pControl->GetControl()))->m_ArxTabsList.GetNext(posTab);
				// do loop to call mpHostDlg method for each child ControlPane
				CArxControlPane ctrlPane(pTab->m_pDclForm, pControl->GetControl());
				TDialogControlPtr pNestedControl = ctrlPane.FindControl(hwndControl);
				if( pNestedControl )
					return pNestedControl;
			}
		}
	}
	return NULL;
}


CDclControlObject *CArxControlPane::FindArxObject(CString sControlName) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = 0;
	int nTotal = mpSourceForm->GetControlList().GetCount();
	while(nCount < mpSourceForm->GetControlList().GetCount())
	{
		// get position
		pos = mpSourceForm->GetControlList().FindIndex(nCount);
		// get current property
		CDclControlObject* pArxObject = mpSourceForm->GetControlList().GetAt(pos);
		
		// if mpHostDlg control's name is a match to the requested control
		if (sControlName == pArxObject->GetStrProperty(nName))
		{
			return pArxObject;
		}

		if (pArxObject->GetType() == CtlTabStrip)
		{
			// get the tab control
			CWnd *pControl = pArxObject->GetWindow(); //mpHostDlg->GetDlgItem(pArxObject->m_Id);

			for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
			{
				POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
				if (pos != NULL)
				{
					CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
				
					CArxControlPane ctrlPane;
					ctrlPane.mpHostDlg = mpHostDlg;
					ctrlPane.mpSourceForm = pTab->m_pDclForm;
					// get the child ControlPane
					CDclControlObject* pChildArxObject = ctrlPane.FindArxObject(sControlName);
					if (pChildArxObject != NULL)
						return pChildArxObject;
				}
			}
		}
		// increment counter
		nCount++;
	}

	return NULL;
}


CDclFormObject *CArxControlPane::FindDclObject(CString sControlName) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = 0;
	int nTotal = mpSourceForm->GetControlList().GetCount();
	while(nCount < mpSourceForm->GetControlList().GetCount())
	{
		// get position
		pos = mpSourceForm->GetControlList().FindIndex(nCount);
		// get current property
		CDclControlObject* pArxObject = mpSourceForm->GetControlList().GetAt(pos);
		
		// if mpHostDlg control's name is a match to the requested control
		if (sControlName == pArxObject->GetStrProperty(nName))
		{
			return mpSourceForm;
		}

		if (pArxObject->GetType() == CtlTabStrip)
		{
			// get the tab control
			CWnd *pControl = pArxObject->GetWindow(); //mpHostDlg->GetDlgItem(pArxObject->m_Id);

			for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
			{
				POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
				if (pos != NULL)
				{
					CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
				
					// do loop to call mpHostDlg method for each child ControlPane
					for (int i=0; i < pTab->m_pDclForm->GetControlList().GetCount(); i++)
					{
						CArxControlPane ctrlPane;
						ctrlPane.mpHostDlg = mpHostDlg;
						ctrlPane.mpSourceForm = pTab->m_pDclForm;
						// get the child ControlPane
						const CDclControlObject* pChildArxObject = ctrlPane.FindArxObject(sControlName);
						if (pChildArxObject != NULL)
							return pTab->m_pDclForm;
					}
				}
			}
		}
		// increment counter
		nCount++;
	}

	return NULL;
}
