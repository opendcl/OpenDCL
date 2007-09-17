// Geometry.cpp : implementation file
//

#include "stdafx.h"
#include "Geometry.h"
#include "PropertyObject.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "OpenDCLView.h"
#include "ControlTypes.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CGeometry property page

IMPLEMENT_DYNCREATE(CGeometry, CPropertyPage)

CGeometry::CGeometry() : CPropertyPage(CGeometry::IDD)
{
	//{{AFX_DATA_INIT(CGeometry)
	//}}AFX_DATA_INIT
}

CGeometry::~CGeometry()
{
}

void CGeometry::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeometry)
	DDX_Control(pDX, IDC_RIGHT, m_Right);
	DDX_Control(pDX, IDC_BOTTOM, m_Bottom);
	DDX_Control(pDX, IDC_TOP, m_Top);
	DDX_Control(pDX, IDC_LEFT, m_Left);
	DDX_Control(pDX, IDC_EXAMPLEIMAGE, m_ExampleImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeometry, CPropertyPage)
	//{{AFX_MSG_MAP(CGeometry)
	ON_CBN_SELCHANGE(IDC_LEFT, OnSelchangeLeft)
	ON_CBN_SELCHANGE(IDC_TOP, OnSelchangeTop)
	ON_CBN_SELCHANGE(IDC_BOTTOM, OnSelchangeBottom)
	ON_CBN_SELCHANGE(IDC_RIGHT, OnSelchangeRight)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeometry message handlers



BOOL CGeometry::OnApply() 
{
	m_pUseTopFromBottom->SetType(PropLong);
	m_pUseTopFromBottom->SetLongValue(m_Top.GetItemData(m_Top.GetCurSel()));

	m_pUseLeftFromRight->SetType(PropLong);
	m_pUseLeftFromRight->SetLongValue(m_Left.GetItemData(m_Left.GetCurSel()));

	m_pUseBottomFromBottom->SetType(PropLong);
	m_pUseBottomFromBottom->SetLongValue(m_Bottom.GetItemData(m_Bottom.GetCurSel()));

	
	m_pUseRightFromRight->SetType(PropLong);
	m_pUseRightFromRight->SetLongValue(m_Right.GetItemData(m_Right.GetCurSel()));

	
	COpenDCLView *pView = (COpenDCLView *)m_pDclForm->m_pChildWnd;
	
	pView->CalcAllOffsets();
	
	return CPropertyPage::OnApply();
}



void CGeometry::ShowSplitter(CString sName) 
{
	CDclControlObject *pCtrl = m_pDclForm->FindControl(sName, CtlSplitter);
	if (pCtrl)
	{
		int nTheWidth = pCtrl->GetLongProperty(Prop::Width);
		int nTheHeight = pCtrl->GetLongProperty(Prop::Height);
		int nTheLeft = pCtrl->GetLongProperty(Prop::Left);
		int nTheTop = pCtrl->GetLongProperty(Prop::Top);
		
		if (nTheWidth > nTheHeight)
		{
			if (m_pControl->GetLongProperty(Prop::Top) < nTheTop)
				m_BottomSplitter.ShowWindow(TRUE);
			else
				m_TopSplitter.ShowWindow(TRUE);
		}
		else
		{
			if (m_pControl->GetLongProperty(Prop::Left) < nTheLeft)
				m_RightSplitter.ShowWindow(TRUE);
			else
				m_LeftSplitter.ShowWindow(TRUE);
		}
	}
}

BOOL CGeometry::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_LeftSplitter.Create(WS_CHILD, CRect (10,10,90,90), &m_ExampleImage, 10101);
	m_TopSplitter.Create(WS_CHILD, CRect (10,10,90,90), &m_ExampleImage, 10102);
	m_RightSplitter.Create(WS_CHILD, CRect (10,10,90,90), &m_ExampleImage, 10103);
	m_BottomSplitter.Create(WS_CHILD, CRect (10,10,90,90), &m_ExampleImage, 10104);

	m_LeftSplitter.MoveWindow(50-10, 79+2-9, 6, 81);
	m_TopSplitter.MoveWindow(37-9, 70+2-9, 141, 6);
	m_RightSplitter.MoveWindow(37+141-6-13-9, 79+2-9, 6, 81);
	m_BottomSplitter.MoveWindow(37-9, 166-1-9, 141, 6);

	m_Top.SetItemData(0, 0);
	m_Top.SetItemData(1, 1);

	m_Bottom.SetItemData(0, 0);
	m_Bottom.SetItemData(1, 1);

	m_Left.SetItemData(0, 0);
	m_Left.SetItemData(1, 1);
	m_Left.SetItemData(2, 2);

	m_Right.SetItemData(0, 0);
	m_Right.SetItemData(1, 1);

	CList< CDclControlObject* > Splitters;
	m_pDclForm->FindControls(CtlSplitter, Splitters);
	POSITION pos = Splitters.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = Splitters.GetNext(pos);
		if (!pCtrl->IsDeleted())
		{
			int nTheWidth = pCtrl->GetLongProperty(Prop::Width);
			int nTheHeight = pCtrl->GetLongProperty(Prop::Height);
			if (nTheWidth > nTheHeight)
			{
				int n;
				n = m_Top.AddString(pCtrl->GetStringProperty(Prop::Name));
				m_Top.SetItemData(n, pCtrl->GetID());
				n = m_Bottom.AddString(pCtrl->GetStringProperty(Prop::Name));
				m_Bottom.SetItemData(n, pCtrl->GetID());
			}
			else
			{
				int n;
				n = m_Left.AddString(pCtrl->GetStringProperty(Prop::Name));
				m_Left.SetItemData(n, pCtrl->GetID());
				n = m_Right.AddString(pCtrl->GetStringProperty(Prop::Name));
				m_Right.SetItemData(n, pCtrl->GetID());
			}
		}
	}
	
	m_LeftSplitter.ShowWindow(FALSE);
	m_TopSplitter.ShowWindow(FALSE);
	m_RightSplitter.ShowWindow(FALSE);
	m_BottomSplitter.ShowWindow(FALSE);


	if (m_pUseRightFromRight->GetType() == PropLong)
	{
		for (int i=0; i<m_Right.GetCount(); i++)
		{
			if (m_Right.GetItemData(i) == m_pUseRightFromRight->GetLongValue())
				m_Right.SetCurSel(i);
		}
	}
	else
		m_Right.SetCurSel(m_pUseRightFromRight->GetBooleanValue() == TRUE);
	

	if (m_pUseTopFromBottom->GetType() == PropLong)
	{
		for (int i=0; i<m_Top.GetCount(); i++)
		{
			if (m_Top.GetItemData(i) == m_pUseTopFromBottom->GetLongValue())
				m_Top.SetCurSel(i);
		}
	}
	else
		m_Top.SetCurSel(m_pUseTopFromBottom->GetBooleanValue() == TRUE);


	if (m_pUseBottomFromBottom->GetType() == PropLong)
	{
		for (int i=0; i<m_Bottom.GetCount(); i++)
		{
			if (m_Bottom.GetItemData(i) == m_pUseBottomFromBottom->GetLongValue())
				m_Bottom.SetCurSel(i);
		}
	}
	else
		m_Bottom.SetCurSel(m_pUseBottomFromBottom->GetBooleanValue() == TRUE);


	if (m_pUseTopFromBottom->GetType() == PropLong)
	{
		for (int i=0; i<m_Left.GetCount(); i++)
		{
			if (m_Left.GetItemData(i) == m_pUseLeftFromRight->GetLongValue())
				m_Left.SetCurSel(i);
		}
	}
	else
		m_Left.SetCurSel(m_pUseLeftFromRight->GetBooleanValue() == TRUE);
	

	ShowAllSplitters();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}



void CGeometry::OnSelchangeLeft() 
{
	if (m_Left.GetCurSel() == 2)
	{
		if (m_Right.GetCurSel() == 1)
		{
			m_Right.SetCurSel(0);
		}
		m_Right.EnableWindow(FALSE);
	}
	else
		m_Right.EnableWindow(TRUE);

	m_ExampleImage.Invalidate();
	SetModified(TRUE);
	

	ShowAllSplitters();
}

void CGeometry::ShowAllSplitters() 
{
	CString sSplitterName;

	m_LeftSplitter.ShowWindow(FALSE);
	m_TopSplitter.ShowWindow(FALSE);
	m_RightSplitter.ShowWindow(FALSE);
	m_BottomSplitter.ShowWindow(FALSE);

	if (m_Left.GetCurSel() > 2)
	{
		m_Left.GetLBText(m_Left.GetCurSel(), sSplitterName);
		ShowSplitter(sSplitterName);
	}
	
	if (m_Top.GetCurSel() > 1)
	{
		m_Top.GetLBText(m_Top.GetCurSel(), sSplitterName);
		ShowSplitter(sSplitterName);
		m_TopSplitter.Invalidate();
	}

	if (m_Bottom.GetCurSel() > 1)
	{
		m_Bottom.GetLBText(m_Bottom.GetCurSel(), sSplitterName);
		ShowSplitter(sSplitterName);
		m_BottomSplitter.Invalidate();
	}

	if (m_Right.GetCurSel() > 1)
	{
		m_Right.GetLBText(m_Right.GetCurSel(), sSplitterName);
		ShowSplitter(sSplitterName);
		m_RightSplitter.Invalidate();
	}

}

void CGeometry::OnSelchangeTop() 
{
	m_ExampleImage.Invalidate();
	SetModified(TRUE);	

	ShowAllSplitters();
	
}

void CGeometry::OnSelchangeBottom() 
{
	m_ExampleImage.Invalidate();
	SetModified(TRUE);	

	ShowAllSplitters();

}

void CGeometry::OnSelchangeRight() 
{
	m_ExampleImage.Invalidate();
	SetModified(TRUE);	

	ShowAllSplitters();
	
	
}

void CGeometry::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	m_TopSplitter.Invalidate();
	m_LeftSplitter.Invalidate();
	m_RightSplitter.Invalidate();
	m_BottomSplitter.Invalidate();
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void CGeometry::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertyPage::OnMouseMove(nFlags, point);
}
