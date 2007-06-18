// GridSpacingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GridSpacingDlg.h"
#include "Project.h"
#include "DclFormObject.h"
#include "OpenDCLView.h"


/////////////////////////////////////////////////////////////////////////////
// CGridSpacingDlg dialog


CGridSpacingDlg::CGridSpacingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridSpacingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridSpacingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGridSpacingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridSpacingDlg)
	DDX_Control(pDX, IDC_GRIDSLIDER, m_Slider);
	DDX_Control(pDX, IDC_CURRENTSPACING, m_CurrentSpacing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridSpacingDlg, CDialog)
	//{{AFX_MSG_MAP(CGridSpacingDlg)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_GRIDSLIDER, OnReleasedcaptureGridslider)
	//}}AFX_MSG_MAP
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
	CProject *pProject = activeProject;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
    
	if (m_GridSpacing <= 4)
		m_GridSpacing = 0;
	
    pApp->WriteProfileInt(sProfileName, _T("nGridSpacing"), m_GridSpacing);
    

	try
	{
		// do look to ensure all open CViews are repainted with the new grid spacing.
		POSITION pos = pProject->GetDclFormList().GetHeadPosition();
		while (pos != NULL)
		{
			CDclFormObject *pDclForm = pProject->GetDclFormList().GetNext(pos);
			if (pDclForm != NULL && pDclForm->m_pChildWnd != NULL)
			{
				if (pDclForm->m_pChildWnd != NULL)
				{
					try
					{
						((COpenDCLView*)pDclForm->m_pChildWnd)->m_gridSpacing = m_GridSpacing;
						((COpenDCLView*)pDclForm->m_pChildWnd)->OnGridSpacingChanged();
					}
					catch(...)
					{
					}
				}
			}
		}
	}
	catch(...)
	{
	}
	CDialog::OnOK();
}
