// FormVarNameUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "FormVarNameUpdate.h"
#include "DclControlTemplate.h"
#include "PropertyPane.h"
#include "DclFormTemplate.h"
#include "StudioFrame.h"
#include "StudioWorkspace.h"


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
	ON_BN_CLICKED(IDC_CTRLCHECK, OnCtrlcheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate message handlers

INT_PTR CFormVarNameUpdate::DoModal()
{
	AutoUndoGroup UndoGroup( mpDclForm->GetUndoManager(), IDS_UNDO_VARNAMEUPDATE );
	return __super::DoModal();
}

void CFormVarNameUpdate::OnOK()
{
	if( msFormName.IsEmpty() )
		mpDclForm->ClearGlobalVariableName( mbSetControls );
	else
	{
		mpDclForm->GetControlProperties()->AddStringProperty( Prop::Name, PropString, msFormName );
		mpDclForm->SetGlobalVariableName( NULL, mbSetControls );
	}
	__super::OnOK();
}

void CFormVarNameUpdate::OnCtrlcheck()
{
	CButton* pSetControlsBtn = (CButton*)GetDlgItem( IDC_CTRLCHECK );
	mbSetControls = (pSetControlsBtn->GetCheck() == BST_CHECKED);
}

BOOL CFormVarNameUpdate::OnInitDialog()
{
	__super::OnInitDialog();
	CButton* pSetControlsBtn = (CButton*)GetDlgItem( IDC_CTRLCHECK );
	pSetControlsBtn->SetCheck( mbSetControls? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
