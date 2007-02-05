// PropertyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyEdit.h"
#include "PropertyIds.h"
#include "UserMessageID.h"
#include "ChildFrm.h"
#include "ZOrderListCtrl.h"
#include "FormVarNameUpdate.h"
#include "PropertyObject.h"
#include "Project.h"
#include "ControlHolder.h"
#include "DclControlObject.h"
#include "ControlTypes.h"
#include "DclFormObject.h"
#include "ControlName.h"
#include "EditorWorkspace.h"
#include "ObjectDCLView.h"
#include "ProjectTreeCtrl.h"
#include "PropertyTabPane.h"
#include "MainFrm.h"
#include "AxInterfaceDescriptor.h"


static const TCHAR *sAllChars = _T("*");


static CDclControlObject* FindArxControlObject(CDclFormObject *pDclForm, CString sArxControlName, CDclControlObject *pCtrl = NULL )
{
	CDclControlObject *pRetObject = NULL;
		
	if (pDclForm == NULL)
		return NULL;

	// create a position variable to hold the converted ArxControlIndex
	POSITION ControlPos;
	
	for (int i=0;i<pDclForm->GetControlList().GetCount();i++)
	{
		// set the position variable to be equal the index to passing to the GetAt method
		ControlPos = pDclForm->GetControlList().FindIndex(i);	
		
		if (ControlPos != NULL)
		{
			CDclControlObject *pControl = pDclForm->GetControlList().GetAt(ControlPos);
			if (pControl->GetStrProperty(nName) == sArxControlName && pCtrl != pControl)
			{
				// return the object found
				return pControl;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyEdit

CPropertyEdit::CPropertyEdit()
{
	m_pDlg = NULL;
	m_sFilter = sAllChars;
	m_bAllowReturn = false; 
	m_pAxContainer = NULL;
}

CPropertyEdit::~CPropertyEdit()
{
	if (!m_pDlg)
		delete m_pDlg;
	m_pDlg = NULL;
}


BEGIN_MESSAGE_MAP(CPropertyEdit, CEdit)
	//{{AFX_MSG_MAP(CPropertyEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyEdit message handlers

void CPropertyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == nCharReturn && m_bAllowReturn == false)
	{		
		// if returns are allowed then
		if (m_bAllowReturn)
			// call CommitList to update the list
			CommitList();
		else	
			// otherwise call CommitValue to update the text value
			CommitValue();

		if (m_pView != NULL)
			m_pView->UpdateControl(m_pControl, m_pProp->GetID());
	}
	
	if (nChar == nCharEsc)
	{
		m_pProp->SetStringValue(m_OriginalValue);
		SetWindowText(m_OriginalValue);
	}
	if (m_sFilter.GetLength() == 0 ||
		m_sFilter == sAllChars ||
		nChar == nCharBackSp)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	if (nChar == nCharReturn && m_bAllowReturn == true)
	{		
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}
		
	
	char sChar = nChar;
	CString strChar = sChar;
	if (m_sFilter.FindOneOf(strChar) > nNotSet)
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else
		MessageBeep((UINT)nNotSet);

	
}

void CPropertyEdit::CommitValue() 
{
	// if returns are allowed then
	if (m_bAllowReturn)
	{
		// call CommitList to update the list
		CommitList();
		return;
	}
	
	if (theEditorWorkspace.GetActiveDocument() != NULL)
		theEditorWorkspace.GetActiveDocument()->SetModifiedFlag(TRUE);

	CString sText;
	GetWindowText(sText);
	if (m_pAxContainer != NULL)
	{
		m_pProp->GetAxInterfaceDescriptorPtr()->SetActiveXPropery(m_pAxContainer->GetActiveXCtrl(), sText);
	}
	else if (m_pControl->m_Id != CtlFormPropHolder)
	{
		// if this property is the nName property
		if (m_pProp->GetID() == nName)
		{	
			// don't set the value it is blank
			if (sText.IsEmpty())
			{
				SetWindowText(sText);
				return;
			}

			// call the method to update the VarName
			//UpdateVarName();

			// rename the zorder list control
			CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
			pZOrderList->RenameControlInList(m_pProp->GetStringValue(), sText);
		}
		
		// set the property value
		m_pProp->SetStringValue(sText);	

		// lets update the control if the property is not a position property
		if (m_pProp->GetID() != nLeft &&
			m_pProp->GetID() != nTop &&
			m_pProp->GetID() != nWidth &&
			m_pProp->GetID() != nHeight)
		{
			// and refresh the control
			m_pView->RefreshChildControl(m_pControl, m_pProp->GetID());
		}
	}
	else if (m_pControl->m_Id == CtlFormPropHolder)
	{
		if (m_pProp->GetID() == nName)
		{			
			CDclFormObject *pDclForm = activeProject->GetDclForm(sText);
			if (pDclForm != NULL)
			{
				return;
			}
			theEditorWorkspace.GetProjectTreeCtrl()->SetItemText(m_pView->m_pThisDclForm->m_htiTreeItem, sText);
			// don't set the value it is blank
			if (sText.IsEmpty())
			{
				SetWindowText(sText);
				return;
			}		
		}
		if (m_pProp->GetID() == nTitleBarText)
		{	
			CChildFrame *pChildFrm = NULL;
			try
			{
				pChildFrm = (CChildFrame*) m_pView->GetParentFrame();
			}
			catch(...){}
			if (pChildFrm)
			{
				pChildFrm->m_Title = sText;
				pChildFrm->SetWindowText(sText);
			}
		}
		m_pProp->SetStringValue(sText);	
	}
}

void CPropertyEdit::UpdateVarName()
{
	CString sNewText;
	GetWindowText(sNewText);
	if (m_OriginalValue == sNewText)
		return;

	CString sDclFormName = m_pView->m_pThisDclForm->GetKeyName();
			
	if (m_pControl->GetType() <= 0 || m_pControl->GetType() > CtlFileDlgCtrl)
	{
		// get the name for the control
		CString sControlName = GetControlName(m_pControl->GetType());

		// if the old control name was the original default control name
		if (m_OriginalValue.Left(sControlName.GetLength()) == sControlName &&
			(m_OriginalValue.GetLength() == sControlName.GetLength() + 1 ||
			m_OriginalValue.GetLength() == sControlName.GetLength() + 2))
		{
			// do a complete update of the (VarName)
			m_pControl->ForceUpdateGlobalVariable(sDclFormName);
			// and refresh the projectlist ctrl.
			theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.Invalidate();
		}
		else
		{
			if (!m_pDlg)
			{
				m_pDlg = new CFormVarNameUpdate;
				m_pDlg->Create(IDD_FORMVARNAMEUPDATE_DIALOG, &theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList);
			}
			m_pDlg->m_sDclFormName = sDclFormName;
			m_pDlg->m_pControl = m_pControl;
			m_pDlg->ShowWindow(TRUE);
			// we have to do this disabling/enabling thing because the DoModal causes an Exception error
			// and I can't figure out why. Sorry.
			theEditorWorkspace.GetMainFrame()->EnableWindow(FALSE);
			m_pDlg->EnableWindow(TRUE);
		}
	}
	else
	{
		// get the text for the msg box
		CString sMsg;
		CString sTitle;				
		sMsg = theWorkspace.LoadResourceString(IDS_UPDATEVARNAMEQ);
		sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);

		// if yes was pressed.
		if (MessageBox(sMsg, sTitle, MB_YESNO) == nAnsweredYes)
		{
			// do a complete update of the (VarName)
			m_pControl->ForceUpdateGlobalVariable(sDclFormName);
			// and refresh the projectlist ctrl.
			theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(
					m_pControl, 
					theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.m_pView);
		}	
	}
}

void CPropertyEdit::UpdateSizes()
{
	CString sText;
	GetWindowText(sText);
	
	if (m_pControl->m_Id == CtlFormPropHolder)
	{	
		if (m_pProp->GetID() == nWidth)
		{			
			int nThisValue = _tstol(sText);
			// don't set the value it is blank
			if (sText.IsEmpty() || nThisValue <= 0)
			{
				SetWindowText(sText);
				return;
			}	
			CChildFrame *pChildFrm = NULL;
			try{
			pChildFrm = (CChildFrame*) m_pView->GetParentFrame();
			}catch(...){}
			if (pChildFrm)
			{
					CRect rc;
				CRect rcClient;
				pChildFrm->GetWindowRect(&rc);
				pChildFrm->GetClientRect(&rcClient);

				int nXDiff = rc.Width() - rcClient.Width();
		
				pChildFrm->SetWindowPos(NULL, nNotSet,nNotSet,nThisValue+nXDiff, rc.Height(), SWP_NOMOVE);
			}
			
		}
		
		if (m_pProp->GetID() == nHeight)
		{			
			int nThisValue = _tstol(sText);
			// don't set the value it is blank
			if (sText.IsEmpty() || nThisValue <= 0)
			{
				SetWindowText(sText);
				return;
			}			
			CChildFrame *pChildFrm = NULL;
			try{
			pChildFrm = (CChildFrame*) m_pView->GetParentFrame();
			}catch(...){}
			if (pChildFrm)
			{
				CRect rc;
				CRect rcClient;
				pChildFrm->GetWindowRect(&rc);
				pChildFrm->GetClientRect(&rcClient);

				int nYDiff = rc.Height() - rcClient.Height();

				pChildFrm->SetWindowPos(NULL, nNotSet,nNotSet,rc.Width(), nThisValue+nYDiff, SWP_NOMOVE);
				//pChildFrm->MoveWindow(rc.left, rc.top, rc.Width(), nThisValue, TRUE);
			}
		}
	}
}

void CPropertyEdit::CommitList() 
{
	
	// clear the list
	m_pProp->clear();

	// get each list item from the CEdit and add it to the list
	for (int i=0; i<GetLineCount(); i++)
	{
		CString sText;			
		int cb;         
		TCHAR *pch; 
		pch = sText.GetBuffer(_MAX_PATH); 
		
		cb = GetLine(i, pch, _MAX_PATH); 
		pch[cb] = _T('\0'); 
		sText.ReleaseBuffer(); 
		if (sText.GetLength() > 0)
		{
			m_pProp->AddStringItem(sText);			
		}

		m_pView->RefreshChildControl(m_pControl, m_pProp->GetID());
		if (theEditorWorkspace.GetActiveDocument() != NULL)
			theEditorWorkspace.GetActiveDocument()->SetModifiedFlag(TRUE);
	}
}


void CPropertyEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CommitValue();

	// if this property is the nName property
	if (m_pProp->GetID() == nName)
	{	
		SetNameProp();
		// call the method to update the VarName
		UpdateVarName();	
	}
	else if (m_pView != NULL)
	{
		m_pView->UpdateControl(m_pControl, m_pProp->GetID());
	
		// check to update sizes
		UpdateSizes();
	}

	
	CEdit::OnKillFocus(pNewWnd);
	
}

void CPropertyEdit::OnChange() 
{
	// if returns are allowed then
	if (m_bAllowReturn)
		// call CommitList to update the list
		CommitList();
	else
	{
		// if this property is the nName property
		if (m_pProp->GetID() == nName)
			return;	
		// otherwise call CommitValue to update the text value
		CommitValue();
	}
}

void CPropertyEdit::SetNameProp()
{
	CString sText;
	CString sMsg;
	CString sTitle;
	GetWindowText(sText);

	if (m_pControl->m_Id != CtlFormPropHolder)
	{
		CDclControlObject *pFormPropHolder = FindArxControlObject(m_pView->m_pThisDclForm, sText, m_pControl);
		
		// if an identical arx control has been found and it's not this one
		if (pFormPropHolder != NULL && pFormPropHolder != m_pControl)
		{
			SetWindowText(this->m_OriginalValue);
			sMsg = theWorkspace.LoadResourceString(IDS_CTRLBADNAME);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return;
		}
		// don't set the value it is blank
		if (sText.IsEmpty())
		{
			SetWindowText(sText);
			return;
		}
		m_pProp->SetStringValue(sText);
		// rename the zorder list control
		CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
		pZOrderList->RenameControlInList(m_pProp->GetStringValue(), sText);
	}

	else if (m_pControl->m_Id == CtlFormPropHolder)
	{	
		CDclFormObject *pDclForm = activeProject->GetDclForm(sText);
		
		// if an identical form has been found and it's not this one
		if (pDclForm != NULL && pDclForm != theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.m_pDclForm)
		{
			SetWindowText(m_pProp->GetStringValue());
			sMsg = theWorkspace.LoadResourceString(IDS_FORMBADNAME);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return;
		}
		theEditorWorkspace.GetProjectTreeCtrl()->SetItemText(m_pView->m_pThisDclForm->m_htiTreeItem, sText);
		// don't set the value it is blank
		if (sText.IsEmpty())
		{
			SetWindowText(sText);
			return;
		}					
	}
}

BOOL CPropertyEdit::PreTranslateMessage(MSG* pMsg) 
{
	bool bSendMessage = false;
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN)
		{
			if (m_bAllowReturn)
			{
				return CEdit::PreTranslateMessage(pMsg);
			}
			if (!m_bAllowReturn)
			{
				CommitValue();
				
				bSendMessage = true;
				//pMsg->wParam = NULL;
				//pMsg->message = NULL;

			}
			// if this property is the nName property
			if (m_pProp->GetID() == nName)
			{	
				SetNameProp();				
			}
			else if (m_pView != NULL)
			{
				m_pView->UpdateControl(m_pControl, m_pProp->GetID());
				// check to update sizes
				UpdateSizes();
			}
		}
	}	
	BOOL bVal = CEdit::PreTranslateMessage(pMsg);

	return bVal;
}

void CPropertyEdit::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (!bShow && m_pView != NULL)
		m_pView->UpdateControl(m_pControl, m_pProp->GetID());

	CEdit::OnShowWindow(bShow, nStatus);
	
	
	
}
