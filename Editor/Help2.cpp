// Help2.cpp : implementation file
//

#include "stdafx.h"
#include "Help2.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CHelp2 dialog

CHelp2::CHelp2(CWnd* pParent /*=NULL*/)
	: CDialog(CHelp2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelp2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHelp2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelp2)
	DDX_Control(pDX, IDC_RICHEDIT1, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelp2, CDialog)
	//{{AFX_MSG_MAP(CHelp2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelp2 message handlers

BOOL CHelp2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString sHelp;
	sHelp = theWorkspace.LoadResourceString(IDS_CTRLHELP);

	m_Text.SetRTF(sHelp);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
