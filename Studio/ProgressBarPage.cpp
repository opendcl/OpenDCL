// ProgressBarPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBarPage.h"
#include "DclControlObject.h"
#include "Workspace.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CProgressBarPage dialog


CProgressBarPage::CProgressBarPage(UINT unID) : CPropertyPage(unID)
{
	//{{AFX_DATA_INIT(CProgressBarPage)
	//}}AFX_DATA_INIT
}


void CProgressBarPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressBarPage)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_TIME, m_Time);
	DDX_Control(pDX, IDC_PERCENTAGE, m_Percentage);
	DDX_Control(pDX, IDC_EDIT6, m_Second);
	DDX_Control(pDX, IDC_EDIT5, m_Seconds);
	DDX_Control(pDX, IDC_EDIT4, m_Minute);
	DDX_Control(pDX, IDC_EDIT1, m_Minutes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressBarPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgressBarPage)
	ON_BN_CLICKED(IDC_PERCENTAGE, OnPercentage)
	ON_BN_CLICKED(IDC_TIME, OnTime)
	ON_EN_CHANGE(IDC_EDIT5, OnChangeSeconds)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressBarPage message handlers

BOOL CProgressBarPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetWindowText(m_sTitle);
		
	m_pArxCtrl->GetPropertyObject(nDisplayPercentage);

	if (m_pArxCtrl->GetBoolProperty(nDisplayPercentage) == TRUE)
	{
		m_Percentage.SetCheck(TRUE);
		m_Progress.m_bPercentage = true;
	}
	else if (m_pArxCtrl->GetBoolProperty(nDisplaySeconds) == TRUE)
	{
		m_Progress.m_bTime = true;
	}
	m_Time.SetCheck(m_pArxCtrl->GetBoolProperty(nDisplaySeconds));
		
	m_Minute.SetWindowText(m_pArxCtrl->GetStrProperty(nMinuteText));
	m_Minutes.SetWindowText(m_pArxCtrl->GetStrProperty(nMinutesText));

	m_Second.SetWindowText(m_pArxCtrl->GetStrProperty(nSecondText));
	m_Seconds.SetWindowText(m_pArxCtrl->GetStrProperty(nSecondsText));
	
	m_Progress.m_sText = CString("15 ") + m_pArxCtrl->GetStrProperty(nSecondsText);

	// setup display progress bar.
	m_Progress.SetRange(0,100);
	m_Progress.SetPos(75);
	
	ShowEditFrame();
		
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CProgressBarPage::OnApply() 
{
	CString sValue;
	
	m_Minute.GetWindowText(sValue);
	m_pArxCtrl->SetStringProperty(nMinuteText, sValue);

	m_Minutes.GetWindowText(sValue);
	m_pArxCtrl->SetStringProperty(nMinutesText, sValue);

	m_Second.GetWindowText(sValue);
	m_pArxCtrl->SetStringProperty(nSecondText, sValue);
	
	m_Seconds.GetWindowText(sValue);
	m_pArxCtrl->SetStringProperty(nSecondsText, sValue);
	
	m_pArxCtrl->SetBooleanProperty(nDisplayPercentage, m_Percentage.GetCheck() != 0);
	
	m_pArxCtrl->SetBooleanProperty(nDisplaySeconds, m_Time.GetCheck() != 0);

	theWorkspace.SetModified(true);
	return CPropertyPage::OnApply();
}


void CProgressBarPage::OnPercentage() 
{
	m_Time.SetCheck(FALSE);
	
	m_Progress.m_bPercentage = (m_Percentage.GetCheck() == TRUE);
	m_Progress.m_bTime = (m_Time.GetCheck() == TRUE);
	
	ShowEditFrame();
}

void CProgressBarPage::OnTime() 
{
	m_Percentage.SetCheck(FALSE);

	m_Progress.m_bPercentage = false;
	m_Progress.m_bTime = m_Time.GetCheck() == TRUE;
	
	ShowEditFrame();
	
}

void CProgressBarPage::ShowEditFrame()
{
	BOOL bShow = m_Time.GetCheck();

	m_Progress.RedrawWindow();

	m_Minute.EnableWindow(bShow);
	m_Minutes.EnableWindow(bShow);
	m_Second.EnableWindow(bShow);
	m_Seconds.EnableWindow(bShow);

	
	GetDlgItem(IDC_STATIC1)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC2)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC3)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC4)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC5)->EnableWindow(bShow);
}

void CProgressBarPage::OnChangeSeconds() 
{
	CString sValue;

	m_Seconds.GetWindowText(sValue);

	m_Progress.m_sText = CString("15 ") + sValue;
	m_Progress.RedrawWindow();
	
}
