// EventNameReset.cpp : implementation file
//

#include "stdafx.h"
#include "EventNameReset.h"
#include "DclControlObject.h"
#include "PropertyPane.h"
#include "DclFormObject.h"
#include "StudioFrame.h"
#include "StudioWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CEventNameReset dialog


CEventNameReset::CEventNameReset( TDclFormPtr pDclForm, CWnd* pParent /*= NULL*/ )
	: CDialog(CEventNameReset::IDD, pParent)
	, mpDclForm( pDclForm )
	, mbSetControls( true )
{
}


BEGIN_MESSAGE_MAP(CEventNameReset, CDialog)
	ON_BN_CLICKED(IDC_CTRLCHECK, OnCtrlcheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventNameReset message handlers

INT_PTR CEventNameReset::DoModal()
{
	AutoUndoGroup UndoGroup( mpDclForm->GetUndoManager(), IDS_UNDO_VARNAMEUPDATE );
	return __super::DoModal();
}

void CEventNameReset::OnOK()
{
	mpDclForm->ResetEventNames( mbSetControls );
	__super::OnOK();
}

void CEventNameReset::OnCtrlcheck()
{
	CButton* pSetControlsBtn = (CButton*)GetDlgItem( IDC_CTRLCHECK );
	mbSetControls = (pSetControlsBtn->GetCheck() == BST_CHECKED);
}

BOOL CEventNameReset::OnInitDialog()
{
	__super::OnInitDialog();
	CButton* pSetControlsBtn = (CButton*)GetDlgItem( IDC_CTRLCHECK );
	pSetControlsBtn->SetCheck( mbSetControls? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
