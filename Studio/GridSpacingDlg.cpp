// GridSpacingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GridSpacingDlg.h"
#include "OpenDCL.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CGridSpacingDlg dialog


CGridSpacingDlg::CGridSpacingDlg()
	: CDialog(CGridSpacingDlg::IDD, AfxGetApp()->GetMainWnd())
{
}


void CGridSpacingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRIDSLIDER, m_Slider);
	DDX_Control(pDX, IDC_CURRENTSPACING, m_CurrentSpacing);
}


BEGIN_MESSAGE_MAP(CGridSpacingDlg, CDialog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_GRIDSLIDER, OnReleasedcaptureGridslider)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridSpacingDlg message handlers

BOOL CGridSpacingDlg::OnInitDialog() 
{
	GetDlgItemText( IDC_CURRENTSPACING, sCurrentSettingFmt );
	ASSERT( !sCurrentSettingFmt.IsEmpty() ); //getting the format string from the dialog resource!
	CDialog::OnInitDialog();
	m_GridSpacing = theApp.GetGridSpacing();
	m_Slider.SetRange( 0, 17, TRUE );
	m_Slider.SetPos( m_GridSpacing > 0? m_GridSpacing - 3 : 0 );
	SetDispaySetting();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CGridSpacingDlg::OnReleasedcaptureGridslider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetDispaySetting();
	*pResult = 0;
}

void CGridSpacingDlg::SetDispaySetting()
{
	CString sValue;
	int nSetting = m_Slider.GetPos();
	if( nSetting == 0 )
	{
		m_GridSpacing = 0;
		sValue = theWorkspace.LoadResourceString( IDS_NONE );
	}
	else
	{
		m_GridSpacing = nSetting + 3;
		sValue.Format( theWorkspace.LoadResourceString( IDS_GRIDSPACING ), m_GridSpacing );
	}
	ASSERT( !sCurrentSettingFmt.IsEmpty() ); //getting the format string from the dialog resource!
	CString sDisplayText;
	sDisplayText.Format( sCurrentSettingFmt, (LPCTSTR)sValue );
	m_CurrentSpacing.SetWindowText( sDisplayText );
}

void CGridSpacingDlg::OnOK() 
{
	theApp.SetGridSpacing( m_GridSpacing );
	CDialog::OnOK();
}
