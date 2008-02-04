// ProgressBarPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBarPage.h"
#include "DclControlObject.h"
#include "StudioDialogControl.h"
#include "Workspace.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CProgressBarPage dialog


CProgressBarPage::CProgressBarPage( TDclControlPtr pDclControl )
: CPropertyPage( IDD )
, mpDclControl( pDclControl )
{
	//{{AFX_DATA_INIT(CProgressBarPage)
	//}}AFX_DATA_INIT
}


void CProgressBarPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_TIME, m_Time);
	DDX_Control(pDX, IDC_PERCENTAGE, m_Percentage);
	DDX_Control(pDX, IDC_EDIT6, m_Second);
	DDX_Control(pDX, IDC_EDIT5, m_Seconds);
	DDX_Control(pDX, IDC_EDIT4, m_Minute);
	DDX_Control(pDX, IDC_EDIT1, m_Minutes);
}


BEGIN_MESSAGE_MAP(CProgressBarPage, CPropertyPage)
	ON_BN_CLICKED(IDC_PERCENTAGE, OnPercentage)
	ON_BN_CLICKED(IDC_TIME, OnTime)
	ON_EN_CHANGE(IDC_EDIT5, OnChangeSeconds)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressBarPage message handlers

BOOL CProgressBarPage::OnInitDialog() 
{
	__super::OnInitDialog();
	
	SetWindowText( theWorkspace.LoadResourceString( IDS_CAPTIONTEXTOPTIONS ) );
		
	mpDclControl->GetPropertyObject(Prop::DisplayPercentage);

	if (mpDclControl->GetBooleanProperty(Prop::DisplayPercentage) == TRUE)
	{
		m_Percentage.SetCheck(TRUE);
		m_Progress.m_bPercentage = true;
	}
	else if (mpDclControl->GetBooleanProperty(Prop::DisplaySeconds) == TRUE)
	{
		m_Progress.m_bTime = true;
	}
	m_Time.SetCheck(mpDclControl->GetBooleanProperty(Prop::DisplaySeconds));
		
	m_Minute.SetWindowText(mpDclControl->GetStringProperty(Prop::MinuteText));
	m_Minutes.SetWindowText(mpDclControl->GetStringProperty(Prop::MinutesText));

	m_Second.SetWindowText(mpDclControl->GetStringProperty(Prop::SecondText));
	m_Seconds.SetWindowText(mpDclControl->GetStringProperty(Prop::SecondsText));
	
	m_Progress.m_sText = CString("15 ") + mpDclControl->GetStringProperty(Prop::SecondsText);

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
	mpDclControl->SetStringProperty(Prop::MinuteText, sValue);
	m_Minutes.GetWindowText(sValue);
	mpDclControl->SetStringProperty(Prop::MinutesText, sValue);
	m_Second.GetWindowText(sValue);
	mpDclControl->SetStringProperty(Prop::SecondText, sValue);
	m_Seconds.GetWindowText(sValue);
	mpDclControl->SetStringProperty(Prop::SecondsText, sValue);
	mpDclControl->SetBooleanProperty(Prop::DisplayPercentage, m_Percentage.GetCheck() != 0);
	mpDclControl->SetBooleanProperty(Prop::DisplaySeconds, m_Time.GetCheck() != 0);
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::MinuteText);
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::MinutesText);
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::SecondText);
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::SecondsText);
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::DisplayPercentage);
	CStudioDialogControl::UpdateProperty(mpDclControl, Prop::DisplaySeconds);

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
