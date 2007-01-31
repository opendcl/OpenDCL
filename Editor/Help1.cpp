// Help1.cpp : implementation file
//

#include "stdafx.h"
#include "Help1.h"


/////////////////////////////////////////////////////////////////////////////
// CHelp1 dialog

CHelp1::CHelp1(CWnd* pParent /*=NULL*/)
	: CDialog(CHelp1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelp1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHelp1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelp1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelp1, CDialog)
	//{{AFX_MSG_MAP(CHelp1)
	ON_BN_CLICKED(IDC_CTRLS, OnCtrls)
	ON_BN_CLICKED(IDC_GENERAL, OnGeneral)
	ON_BN_CLICKED(IDC_GENERAL2, OnGeneral2)
	ON_BN_CLICKED(IDC_BONUS, OnBonus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelp1 message handlers

void CHelp1::OnCtrls() 
{
	m_nType = 1;
	
	OnOK();
}

void CHelp1::OnGeneral() 
{
	m_nType = 2;
	OnOK();
}

BOOL CHelp1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nType	= 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CHelp1::OnGeneral2() 
{
	m_nType = 3;
	OnOK();
	
}

void CHelp1::OnBonus() 
{
	m_nType = 4;
	OnOK();
}
