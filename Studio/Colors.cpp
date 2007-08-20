// Colors.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDCL.h"
#include "Colors.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}

/////////////////////////////////////////////////////////////////////////////
// CColors property page


CColors::CColors(UINT unID) : CPropertyPage(unID)
{
	//{{AFX_DATA_INIT(CColors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CColors::~CColors()
{
}

void CColors::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColors)
	DDX_Control(pDX, IDC_COLORCOMBO, m_ColorCB);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_SYSTEMCOLORLIST, m_SystemColors);
	DDX_Control(pDX, IDC_COLORPATCH, m_Color);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColors, CPropertyPage)
	//{{AFX_MSG_MAP(CColors)
	ON_LBN_SELCHANGE(IDC_SYSTEMCOLORLIST, OnSelchangeSystemcolorlist)	
	ON_CBN_SELCHANGE(IDC_COLORCOMBO, OnSelchangeColorcombo)
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	ON_EN_KILLFOCUS(IDC_EDIT, OnKillfocusEdit)	
	ON_BN_CLICKED(IDC_TRUEBTN, OnTruebtn)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColors message handlers

BOOL CColors::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetWindowText(m_sTitle);
	m_SystemColors.AddColor(_T("Scroll Bar"), GetSysColor(0));
	m_SystemColors.AddColor(_T("Background"), GetSysColor(1));
	m_SystemColors.AddColor(_T("Active Caption"), GetSysColor(2));
	m_SystemColors.AddColor(_T("Inactive Caption"), GetSysColor(3));
	m_SystemColors.AddColor(_T("Menu"), GetSysColor(4));
	m_SystemColors.AddColor(_T("Window"), GetSysColor(5));
	m_SystemColors.AddColor(_T("Window Frame"), GetSysColor(6));
	m_SystemColors.AddColor(_T("Menu Text"), GetSysColor(7));
	m_SystemColors.AddColor(_T("Window Text"), GetSysColor(8));
	m_SystemColors.AddColor(_T("Caption Text"), GetSysColor(9));
	m_SystemColors.AddColor(_T("Active Border"), GetSysColor(10));
	m_SystemColors.AddColor(_T("Inactive Border"), GetSysColor(11));
	m_SystemColors.AddColor(_T("App Workspace"), GetSysColor(12));
	m_SystemColors.AddColor(_T("Highlight"), GetSysColor(13));
	m_SystemColors.AddColor(_T("Highlight Text"), GetSysColor(14));
	m_SystemColors.AddColor(_T("Button Face"), GetSysColor(15));
	m_SystemColors.AddColor(_T("Button Shadow"), GetSysColor(16));
	m_SystemColors.AddColor(_T("Gray Text"), GetSysColor(17));
	m_SystemColors.AddColor(_T("Button Text"), GetSysColor(18));
	m_SystemColors.AddColor(_T("Inactive Caption Text"), GetSysColor(19));
	m_SystemColors.AddColor(_T("Button Highlight"), GetSysColor(20));
	m_SystemColors.AddColor(_T("Acad Model Space"), RGB(0,0,0));
	m_SystemColors.AddColor(_T("Acad Layout Space"), RGB(255,255,255));

	m_ColorCB.ResetContent();
	m_ColorCB.AddColor(_T("Red"), RGB(255,0,0));
	m_ColorCB.AddColor(_T("Yellow"), RGB(255,255,0));
	m_ColorCB.AddColor(_T("Green"), RGB(0,255,0));
	m_ColorCB.AddColor(_T("Cyan"), RGB(0,255,255));
	m_ColorCB.AddColor(_T("Blue"), RGB(0,0,255));
	m_ColorCB.AddColor(_T("Magenta"), RGB(255,0,255));
	m_ColorCB.AddColor(_T("White"), RGB(255,255,255));
	
	


	m_Edit.SetLimitText(3);

	m_Edit.SetWindowText(LTOA(m_pColor->GetLongValue()));

	if (m_pColor->GetLongValue() < 0)
		m_SystemColors.SetCurSel((m_pColor->GetLongValue() * -1)-1);
	else
		m_ColorCB.SetCurSel(m_pColor->GetLongValue()-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CColors::OnApply() 
{
	CString sValue;
	m_Edit.GetWindowText(sValue);
	int nThisValue = _tstol(sValue);
	m_pColor->SetLongValue(nThisValue);
	theWorkspace.SetModified(true);
	return CPropertyPage::OnApply();
}

BEGIN_EVENTSINK_MAP(CColors, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CColors)
	//ON_EVENT(CColors, IDC_ACADCOLORCTRL, 1 /* OnSelChange */, OnOnSelChangeAcadcolorctrl, VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CColors::OnSelchangeSystemcolorlist() 
{
	int nCurSel = m_SystemColors.GetCurSel();
	nCurSel = (nCurSel * -1) - 1;
	TCHAR Value[80];
	_ltot(nCurSel, Value, 10);
	m_Edit.SetWindowText(Value);
	SetModified(TRUE);

	m_ColorCB.SetCurSel(-1);
	DisplayColor();
}


void CColors::OnSelchangeColorcombo() 
{
	TCHAR Value[80];
	_ltot(m_ColorCB.GetCurSel()+1, Value, 10);
	m_Edit.SetWindowText(Value);
	SetModified(TRUE);

	m_SystemColors.SetCurSel(-1);
	DisplayColor();
}

void CColors::OnChangeEdit() 
{
	CString sValue;
	m_Edit.GetWindowText(sValue);
	int nThisValue = _tstol(sValue);
	if (nThisValue < 0)
	{
		m_SystemColors.SetCurSel((nThisValue * -1)-1);
	}
	else if (nThisValue > 0 && nThisValue < 8)
	{
		m_ColorCB.SetCurSel(nThisValue-1);
	}
	
	DisplayColor();
	SetModified(TRUE);
	
}

void CColors::OnKillfocusEdit() 
{
	CString sValue;
	m_Edit.GetWindowText(sValue);
	int nValueTest = _tstol(sValue);
	TCHAR value[80];
	if (nValueTest < -23)
	{
		_ltot(-23, value, 10);		
		m_Edit.SetWindowText(value);
	}
	if (nValueTest > 255)
	{
		_ltot(255, value, 10);
		m_Edit.SetWindowText(value);
	}
}



void CColors::OnTruebtn() 
{
	
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) // The user selected the "OK" button
	{
		  COLORREF crColor = colorDlg.GetColor(); 
		  TCHAR value[80];
		  _ltot(crColor, value, 10);
		  m_Edit.SetWindowText(value);
	}
	DisplayColor();
}


void CColors::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

}

void CColors::DisplayColor() 
{
	CString sValue;
	m_Edit.GetWindowText(sValue);

	if (sValue.GetLength() > 0)
	{
		if (_tstol(sValue)  == -22)
		{
			m_Color.m_color = RGB(0,0,0);
		}
		else if (_tstol(sValue)  == -23)
		{
			m_Color.m_color = RGB(255,255,255);
		}
		else if (_tstol(sValue) < 0)
		{
			int n = _tstol(sValue);
			n = n*-1;
			n = n-1;
			m_Color.m_color = GetSysColor(n);
		}
		else if (_tstol(sValue) > 255)
			m_Color.m_color = _tstol(sValue);
		else 
		{
			int n = _tstol(sValue);
			m_Color.m_color = RGB(acadcol[n][0], acadcol[n][1], acadcol[n][2]);
		}
	}
	m_Color.Invalidate();

}