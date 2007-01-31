// WinColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WinColorDlg.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CWinColorDlg dialog

CWinColorDlg::CWinColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinColorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWinColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinColorDlg)
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_LIST, m_WinColorListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWinColorDlg, CDialog)
	//{{AFX_MSG_MAP(CWinColorDlg)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinColorDlg message handlers

void CWinColorDlg::OnSelchangeList() 
{
	m_nColorSelection = m_WinColorListBox.GetCurSel();
}

void CWinColorDlg::OnDblclkList() 
{
	m_nColorSelection = m_WinColorListBox.GetCurSel();
	OnOK();
	
}

void CWinColorDlg::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL CWinColorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetFocus();	

	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_SCROLLBAR), GetSysColor(0));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_BACKGROUND), GetSysColor(1));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_ACTCAPTION), GetSysColor(2));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_INACTCAPT), GetSysColor(3));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_MENU), GetSysColor(4));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_WINDOW), GetSysColor(5));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_WINFRAME), GetSysColor(6));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_MENUTEXT), GetSysColor(7));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_WINTEXT), GetSysColor(8));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_CAPTEXT), GetSysColor(9));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_ACTBORDER), GetSysColor(10));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_INACTBORDER), GetSysColor(11));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_APPWORK), GetSysColor(12));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_Highlight), GetSysColor(13));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_HighlightText), GetSysColor(14));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_BTNFACE), GetSysColor(15));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_BTNSHDOW), GetSysColor(16));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_GREYTEXT), GetSysColor(17));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_BTNTEXT), GetSysColor(18));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_INACTCAPTEXT), GetSysColor(19));
	m_WinColorListBox.AddColor(theWorkspace.LoadResourceString(IDS_BTNHLIGHT), GetSysColor(20));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
