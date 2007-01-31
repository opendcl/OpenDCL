// ComboBoxPage.cpp : implementation file
//

#include "stdafx.h"
#include "ComboBoxPage.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "PurchaseMode.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CComboBoxPage dialog

CComboBoxPage::CComboBoxPage() : CPropertyPage(CComboBoxPage::IDD)
{
	//{{AFX_DATA_INIT(CComboBoxPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CComboBoxPage::~CComboBoxPage()
{
}


void CComboBoxPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComboBoxPage)
	DDX_Control(pDX, IDC_COMBOBOXEX, m_ComboBoxEx);
	DDX_Control(pDX, IDC_OPTIONLIST, m_OptionList);
	DDX_Control(pDX, IDC_DESCRIPTION, m_Desc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComboBoxPage, CPropertyPage)
	//{{AFX_MSG_MAP(CComboBoxPage)
	ON_LBN_SELCHANGE(IDC_OPTIONLIST, OnSelchangeOptionlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxPage message handlers

BOOL CComboBoxPage::OnApply() 
{
	m_pStyle->SetLongValue(m_SelectedStyle);
	
	return CPropertyPage::OnApply();
}

BOOL CComboBoxPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString sDesc;
	
	int nCount = 14;

	if (m_nCtrl == CtlImageComboBox)
		nCount = 3;

	for (int i=0; i<nCount; i++)
	{
		sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + i);
		switch (nCurrentPurchaseMode)
		{
		case nPurchasedLT:
			if (i < 3)		
				m_OptionList.AddString(sDesc);
			break;			
		case nPurchasedStd:
			if (i < 12)		
				m_OptionList.AddString(sDesc);
			break;
		case nPurchasedR14Pro:
			if (i < 3 )
				m_OptionList.AddString(sDesc);
			if (i > 7 && i != 10 && i != 11)
				m_OptionList.AddString(sDesc);
			break;
		default:
			m_OptionList.AddString(sDesc);
			break;
		}
	}

	m_SelectedStyle = m_pStyle->GetLongValue();
	m_OptionList.SetCurSel(m_pStyle->GetLongValue());

	DisplayDesc(m_pStyle->GetLongValue());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CComboBoxPage::DisplayDesc(int nSetting) 
{
	CString sDesc;
	CComboBox *pCombo0 = (CComboBox*)GetDlgItem(IDC_COMBO0);
	CComboBox *pCombo1 = (CComboBox*)GetDlgItem(IDC_COMBO1);
	CComboBoxEx *pComboEx = (CComboBoxEx*)GetDlgItem(IDC_COMBOBOXEX);
	switch (nSetting)
	{
	case 0:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC0);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Editable"));
		pCombo0->SetCurSel(0);
		break;
	case 1:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC1);
		pCombo0->ShowWindow(FALSE);
		pCombo1->ShowWindow(TRUE);
		pComboEx->ShowWindow(FALSE);
		pCombo1->ResetContent();
		pCombo1->AddString(_T("(List)"));
		pCombo1->SetCurSel(-1);
		break;
	case 2:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC2);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Read Only"));
		pCombo0->SetCurSel(0);
		break;
	case 3:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC3);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Closed Filled"));
		pCombo0->SetCurSel(0);
		break;
	case 4:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC4);
		
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Red"));
		pCombo0->SetCurSel(0);
		break;
	case 5:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC5);
		
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("0.13 mm"));
		pCombo0->SetCurSel(0);

		break;
	case 6:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC6);
		
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Plot Style Names"));
		pCombo0->SetCurSel(0);
		break;
	case 7:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC7);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Plot Style Tables"));
		pCombo0->SetCurSel(0);
		break;
	case 8:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC8);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Arial"));
		pCombo0->SetCurSel(0);
		break;
	case 9:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC9);
		pCombo0->ShowWindow(FALSE);
		pCombo1->ShowWindow(TRUE);
		pComboEx->ShowWindow(FALSE);
		pCombo1->ResetContent();
		pCombo1->AddString(_T("Arial"));
		pCombo1->AddString(_T("Arial Black"));
		pCombo1->AddString(_T("Arial Narrow"));
		pCombo1->AddString(_T("Arial Rounded MT Bold"));
		pCombo1->AddString(_T("Aurora Cn BT"));
		pCombo1->SetCurSel(-1);
		break;
	case 10:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC10);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Plotters"));
		pCombo0->SetCurSel(0);
		break;
	case 11:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC11);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Paper Sizes"));
		pCombo0->SetCurSel(0);
		break;
	case 12:
		sDesc = theWorkspace.LoadResourceString(IDS_COMBODESC12);
		pCombo0->ShowWindow(TRUE);
		pCombo1->ShowWindow(FALSE);
		pComboEx->ShowWindow(FALSE);
		pCombo0->ResetContent();
		pCombo0->AddString(_T("Folder"));
		pCombo0->SetCurSel(0);
	
	}

	m_SelectedStyle = nSetting;
	m_Desc.SetWindowText(sDesc);
}


void CComboBoxPage::OnSelchangeOptionlist() 
{
	DisplayDesc(m_OptionList.GetCurSel());	
	SetModified(TRUE);
	
}
