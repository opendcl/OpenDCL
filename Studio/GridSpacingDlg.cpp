// GridSpacingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GridSpacingDlg.h"
#include "EditorWorkspace.h"
#include "DclFormObject.h"
#include "OpenDCLView.h"


/////////////////////////////////////////////////////////////////////////////
// CGridSpacingDlg dialog


CGridSpacingDlg::CGridSpacingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridSpacingDlg::IDD, pParent)
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
	CDialog::OnInitDialog();
	
	
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
    
	
    m_GridSpacing = pApp->GetProfileInt(sProfileName, _T("nGridSpacing"), 8);
    

	m_Slider.SetRange(0, 16, TRUE);
	m_Slider.SetPos(m_GridSpacing-4);
	
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
	m_GridSpacing = m_Slider.GetPos() + 4;
	
	
	if (m_GridSpacing == 4)
	{
		m_GridSpacing = 0;
		sValue = theWorkspace.LoadResourceString(IDS_NOGRID);
	}
	else
	{
		sValue = theWorkspace.LoadResourceString(IDS_AGRID);
		CString sSpacing;
		sSpacing.Format(_T("%d"), m_GridSpacing);
		CString sWillBe;
		sWillBe = theWorkspace.LoadResourceString(IDS_WILLBE);
		sValue += sSpacing + sWillBe;
	}

	m_CurrentSpacing.SetWindowText(sValue);


}

void CGridSpacingDlg::OnOK() 
{
	TEditorProjectPtr pProject = activeProject;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
    
	if (m_GridSpacing <= 4)
		m_GridSpacing = 0;
	
    pApp->WriteProfileInt(sProfileName, _T("nGridSpacing"), m_GridSpacing);
    

	// do look to ensure all open CViews are repainted with the new grid spacing.
	const TDclFormList& Forms = pProject->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( (*iter)->m_pChildWnd )
		{
			((COpenDCLView*)(*iter)->m_pChildWnd)->m_gridSpacing = m_GridSpacing;
			((COpenDCLView*)(*iter)->m_pChildWnd)->OnGridSpacingChanged();
		}
	}
	CDialog::OnOK();
}
