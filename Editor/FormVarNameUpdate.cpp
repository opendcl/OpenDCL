// FormVarNameUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "FormVarNameUpdate.h"
#include "DclControlObject.h"
#include "PropertyTabPane.h"
#include "DclFormObject.h"
#include "MainFrm.h"
#include "EditorWorkspace.h"


inline CPropertyTabPane* GetPropertyTabs()
{
	return theEditorWorkspace.GetPropertyTabs();
}


/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate dialog


CFormVarNameUpdate::CFormVarNameUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CFormVarNameUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormVarNameUpdate)
	m_FormCheckBox = TRUE;
	m_CtrlCheck = FALSE;
	//}}AFX_DATA_INIT
	m_pControl = NULL;	
	m_sDclFormName = "";
}


void CFormVarNameUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormVarNameUpdate)
	DDX_Check(pDX, IDC_FORMSCHECK, m_FormCheckBox);
	DDX_Check(pDX, IDC_CTRLCHECK, m_CtrlCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormVarNameUpdate, CDialog)
	//{{AFX_MSG_MAP(CFormVarNameUpdate)
	ON_BN_CLICKED(IDC_CTRLCHECK, OnCtrlcheck)
	ON_BN_CLICKED(IDC_FORMSCHECK, OnFormscheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate message handlers


void CFormVarNameUpdate::OnOK() 
{
	if (m_pControl != NULL)
	{
		// this is important. The var name of this dialog must be update first
		if (m_FormCheckBox == TRUE)
		{
			// do a complete update of the (VarName)
			m_pControl->ForceUpdateGlobalVariable(m_sDclFormName);
			
		}
		// then we must update the controls on the form, if required.
		if (m_CtrlCheck == TRUE)
		{
			GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.m_pDclForm->ForceUpdateGlobalVariableName(m_sDclFormName);
		}
		// Now finally we must refresh the property list box. 
		// if we place this above drawing errors can occure.
		if (m_FormCheckBox == TRUE)
		{
			try
			{
				// and refresh the projectlist ctrl.
				GetPropertyTabs()->DisplaySelectedControlProperties(
					m_pControl, 
					GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.m_pView);
			}
			catch(...)
			{
			}
		}
		m_pControl = NULL;	
		m_sDclFormName = "";
	}
	theEditorWorkspace.GetMainFrame()->EnableWindow(TRUE);
	CDialog::OnOK();

	// and refresh the projectlist ctrl.
	GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.SetFocus();
}

void CFormVarNameUpdate::OnCancel() 
{
	m_sDclFormName = "";
	m_pControl = NULL;	
	theEditorWorkspace.GetMainFrame()->EnableWindow(TRUE);
	// and refresh the projectlist ctrl.
	GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.SetFocus();
	CDialog::OnCancel();
}

void CFormVarNameUpdate::OnCtrlcheck() 
{
	CButton *btn = (CButton*)GetDlgItem(IDC_CTRLCHECK);
	if (btn->GetCheck() == 1)
		m_CtrlCheck	= TRUE;
	else
		m_CtrlCheck	= FALSE;
}

void CFormVarNameUpdate::OnFormscheck() 
{
	CButton *btn = (CButton*)GetDlgItem(IDC_FORMSCHECK);
	if (btn->GetCheck() == 1)
		m_FormCheckBox	= TRUE;
	else
		m_FormCheckBox	= FALSE;
	
}
