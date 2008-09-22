// ButtonStyles.cpp : implementation file
//

#include "stdafx.h"
#include "ButtonStyles.h"
#include "PreviewFileDlg.h"
#include "PictureObject.h"
#include "Workspace.h"
#include "Project.h"
#include "PropertyObject.h"
#include "StudioDialogControl.h"
#include "Resource.h"
#include "DclFormView.h"


/////////////////////////////////////////////////////////////////////////////
// CButtonStyles property page

CButtonStyles::CButtonStyles( TDclControlPtr pControl )
: CPropertyPage(CButtonStyles::IDD)
, mpControl( pControl )
{
	m_pStyle = pControl->GetPropertyObject(Prop::ButtonStyle);
	m_pPicture = pControl->GetPropertyObject(Prop::Picture);
	m_nHighestId = 99;
}

CButtonStyles::~CButtonStyles()
{
}

void CButtonStyles::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonStyles)
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Control(pDX, IDC_PICLIST, m_PicList);
	DDX_Control(pDX, IDC_DESC, m_Desc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CButtonStyles, CPropertyPage)
	//{{AFX_MSG_MAP(CButtonStyles)
	ON_BN_CLICKED(IDC_STYLE0, OnStyle0)
	ON_BN_CLICKED(IDC_STYLE1, OnStyle1)
	ON_BN_CLICKED(IDC_STYLE2, OnStyle2)
	ON_BN_CLICKED(IDC_STYLE3, OnStyle3)
	ON_BN_CLICKED(IDC_STYLE4, OnStyle4)
	ON_BN_CLICKED(IDC_STYLE5, OnStyle5)
	ON_BN_CLICKED(IDC_STYLE6, OnStyle6)
	ON_LBN_SELCHANGE(IDC_PICLIST, OnSelchangePiclist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonStyles message handlers

void CButtonStyles::OnStyle0() 
{
	DisplayDesc(0);
	SetModified();	
}

void CButtonStyles::OnStyle1() 
{
	DisplayDesc(1);
	SetModified();
	
}

void CButtonStyles::OnStyle2() 
{
	DisplayDesc(2);
	SetModified();
	
}

void CButtonStyles::OnStyle3() 
{
	DisplayDesc(3);
	SetModified();
	
}

void CButtonStyles::OnStyle4() 
{
	DisplayDesc(4);
	SetModified();
	
}

void CButtonStyles::OnStyle5() 
{
	DisplayDesc(5);	
	SetModified();
}

void CButtonStyles::OnStyle6() 
{
	DisplayDesc(6);	
	SetModified();
}

void CButtonStyles::OnSelchangePiclist() 
{
	int nSel = m_PicList.GetCurSel();
	if (nSel == 1)
	{
		CString sFilter;

		sFilter = theWorkspace.LoadResourceString(IDS_IMAGEFILEFILTER);
	
		// create the open dialog box
		CPreviewFileDlg BrowseWnd(
			TRUE, 
			NULL,
			NULL, 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
			sFilter,
			CWnd::GetActiveWindow());

		// proceed to setup the file buffer size
		BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
		TCHAR* pc = new TCHAR[MAX_PATH];
		BrowseWnd.m_ofn.lpstrFile = pc;
		BrowseWnd.m_ofn.lpstrFile[0] = _T('\0');

		// call method to invoke the file dialog box
		if (BrowseWnd.DoModal() == IDOK)
		{
			CString sFile = BrowseWnd.GetPathName();
			m_nHighestId++;
			mpControl->GetOwnerProject()->LoadPictureFile(sFile, m_nHighestId);
			TCHAR Value[80];
			_ui64tot(m_nHighestId, Value, 10);
			int n = m_PicList.AddString(Value);
			m_PicList.SetItemData(n, m_nHighestId);
			m_PicList.SetCurSel(n);
			m_SelectedPic = m_nHighestId;
		}
		delete[] pc;
	}
	else if (nSel == 0)
		m_SelectedPic = 0;
	else
		m_SelectedPic = m_PicList.GetItemData(nSel);
	
	if (m_SelectedPic > 0)
		m_Picture.SetPicture(mpControl->GetOwnerProject()->FindPicture(m_SelectedPic));
	SetModified(TRUE);
}

BOOL CButtonStyles::OnApply() 
{
	m_pStyle->SetLongValue(m_SelectedStyle);
	m_pPicture->SetLongValue(static_cast< long >(m_SelectedPic));
	CStudioDialogControl::UpdateProperty(mpControl, Prop::ButtonStyle);
	CStudioDialogControl::UpdateProperty(mpControl, Prop::Picture);
	return CPropertyPage::OnApply();
}

void CButtonStyles::DisplayDesc(int nSetting)
{
	CString sDesc;
	
	switch (nSetting)
	{
	case 0:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE0);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 1:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE1);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 2:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE2);
		m_PicList.ShowWindow(FALSE);
		m_Picture.ShowWindow(FALSE);
		GetDlgItem(IDC_PICK)->ShowWindow(TRUE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 3:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE3);
		m_PicList.ShowWindow(FALSE);
		m_Picture.ShowWindow(FALSE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 4:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE4);
		m_PicList.ShowWindow(FALSE);
		m_Picture.ShowWindow(FALSE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(TRUE);
		break;
	case 5:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE5);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 6:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE6);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	}

	m_SelectedStyle = nSetting;
	m_Desc.SetWindowText(sDesc);
	SetModified(TRUE);
}


BOOL CButtonStyles::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CButton *pButton = NULL;
	m_SelectedStyle = m_pStyle->GetLongValue();
	m_SelectedPic = m_pPicture->GetLongValue();
	switch (m_pStyle->GetLongValue())
	{
	case 0:
		pButton = (CButton*)GetDlgItem(IDC_STYLE0);
		break;
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_STYLE1);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_STYLE2);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_STYLE3);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_STYLE4);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_STYLE5);
		break;
	case 6:
		pButton = (CButton*)GetDlgItem(IDC_STYLE6);
		break;
	}
	pButton->SetCheck(TRUE);

	DisplayDesc(m_pStyle->GetLongValue());

	CString sNone;
	sNone = theWorkspace.LoadResourceString(IDS_NONE);
	m_PicList.AddString(sNone);
	CString sAdd;
	sAdd = theWorkspace.LoadResourceString(IDS_ADD);
	m_PicList.AddString(sAdd);
	TProjectPtr pProject = mpControl->GetOwnerProject();
	const TPictureMap& Pictures = pProject->GetPictureMap();
	for( TPictureMap::const_iterator iter = Pictures.begin(); iter != Pictures.end(); ++iter )
	{
		UINT nId = iter->first;
		CString sID;
		sID.Format( _T("%u"), nId );
		int idx = m_PicList.AddString( sID );
		m_PicList.SetItemData( idx, nId );
		if( m_SelectedPic == nId )
			m_PicList.SetCurSel( idx );
		if( nId > m_nHighestId )
			m_nHighestId = nId;
	}
	OnSelchangePiclist();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
