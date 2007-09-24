// FormVarNameUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "FormVarNameUpdate.h"
#include "DclControlObject.h"
#include "PropertyTabPane.h"
#include "DclFormObject.h"
#include "MainFrm.h"
#include "EditorWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate dialog


CFormVarNameUpdate::CFormVarNameUpdate( TDclFormPtr pDclForm,
																				LPCTSTR pszFormName /*= NULL*/,
																				CWnd* pParent /*= NULL*/ )
	: CDialog(CFormVarNameUpdate::IDD, pParent)
	, mpDclForm( pDclForm )
	, msFormName( pszFormName )
	, mbSetControls( true )
{
}


BEGIN_MESSAGE_MAP(CFormVarNameUpdate, CDialog)
	//{{AFX_MSG_MAP(CFormVarNameUpdate)
	ON_BN_CLICKED(IDC_CTRLCHECK, OnCtrlcheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate message handlers


void CFormVarNameUpdate::OnOK() 
{
	if( msFormName.IsEmpty() )
		mpDclForm->ClearGlobalVariableName( mbSetControls );
	else
	{
		mpDclForm->GetControlProperties()->AddStringProperty( Prop::Name, PropString, msFormName );
		mpDclForm->SetGlobalVariableName( NULL, mbSetControls );
	}
	CDialog::OnOK();
}

void CFormVarNameUpdate::OnCtrlcheck() 
{
	CButton* pSetControlsBtn = (CButton*)GetDlgItem( IDC_CTRLCHECK );
	mbSetControls = (pSetControlsBtn->GetCheck() == BST_CHECKED);
}

BOOL CFormVarNameUpdate::OnInitDialog()
{
	CDialog::OnInitDialog();
	CButton* pSetControlsBtn = (CButton*)GetDlgItem( IDC_CTRLCHECK );
	pSetControlsBtn->SetCheck( mbSetControls? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
