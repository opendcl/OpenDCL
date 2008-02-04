// TextBoxFilters.cpp : implementation file
//

#include "stdafx.h"
#include "TextBoxFilters.h"
#include "PropertyObject.h"
#include "StudioDialogControl.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CTextBoxFilters property page

CTextBoxFilters::CTextBoxFilters( TDclControlPtr pDclControl )
: CPropertyPage(CTextBoxFilters::IDD)
, mpDclControl ( pDclControl )
{
	m_pStyle = pDclControl->GetPropertyObject(Prop::FilterStyle);
}

CTextBoxFilters::~CTextBoxFilters()
{
}

void CTextBoxFilters::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextBoxFilters)
	DDX_Control(pDX, IDC_DESC, m_Desc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextBoxFilters, CPropertyPage)
	//{{AFX_MSG_MAP(CTextBoxFilters)
	ON_BN_CLICKED(IDC_FILTER0, OnFilter0)
	ON_BN_CLICKED(IDC_FILTER1, OnFilter1)
	ON_BN_CLICKED(IDC_FILTER2, OnFilter2)
	ON_BN_CLICKED(IDC_FILTER3, OnFilter3)
	ON_BN_CLICKED(IDC_FILTER4, OnFilter4)
	ON_BN_CLICKED(IDC_FILTER5, OnFilter5)
	ON_BN_CLICKED(IDC_FILTER6, OnFilter6)
	ON_BN_CLICKED(IDC_FILTER7, OnFilter7)
	ON_BN_CLICKED(IDC_FILTER8, OnFilter8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextBoxFilters message handlers

void CTextBoxFilters::DisplayDesc(int nSetting)
{
	CString sDesc;
	switch (nSetting)
	{
	case 0:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER0);
		break;
	case 1:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER1);
		break;
	case 2:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER2);
		break;
	case 3:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER3);
		break;
	case 4:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER4);
		break;
	case 5:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER5);
		break;
	case 6:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER6);
		break;
	case 7:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER7);
		break;
	case 8:
		sDesc = theWorkspace.LoadResourceString(IDS_TEXTBOXFILTER8);
		break;
	}

	m_SelectedStyle = nSetting;
	m_Desc.SetWindowText(sDesc);
	SetModified();
}

BOOL CTextBoxFilters::OnApply() 
{
	m_pStyle->SetLongValue(m_SelectedStyle);
	CStudioDialogControl::UpdateProperty(mpDclControl, m_pStyle->GetID());
	return CPropertyPage::OnApply();
}

BOOL CTextBoxFilters::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CButton *pButton = NULL;
	switch (m_pStyle->GetLongValue())
	{
	case 0:
		pButton = (CButton*)GetDlgItem(IDC_FILTER0);
		break;
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_FILTER1);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_FILTER2);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_FILTER3);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_FILTER4);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_FILTER5);
		break;
	case 6:
		pButton = (CButton*)GetDlgItem(IDC_FILTER6);
		break;
	case 7:
		pButton = (CButton*)GetDlgItem(IDC_FILTER7);
		break;
	case 8:
		pButton = (CButton*)GetDlgItem(IDC_FILTER8);
		break;
	}
	pButton->SetCheck(TRUE);

	m_SelectedStyle = m_pStyle->GetLongValue();
	
	DisplayDesc(m_SelectedStyle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CTextBoxFilters::OnFilter0() 
{
	DisplayDesc(0);
	
}

void CTextBoxFilters::OnFilter1() 
{
	DisplayDesc(1);
	
}

void CTextBoxFilters::OnFilter2() 
{
	DisplayDesc(2);
	
}

void CTextBoxFilters::OnFilter3() 
{
	DisplayDesc(3);
	
}

void CTextBoxFilters::OnFilter4() 
{
	DisplayDesc(4);
	
}

void CTextBoxFilters::OnFilter5() 
{
	DisplayDesc(5);
	
}

void CTextBoxFilters::OnFilter6() 
{
	DisplayDesc(6);
	
}

void CTextBoxFilters::OnFilter7() 
{
	DisplayDesc(7);
	
}

void CTextBoxFilters::OnFilter8() 
{
	DisplayDesc(8);
	
}
