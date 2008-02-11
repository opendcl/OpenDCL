// Geometry.cpp : implementation file
//

#include "stdafx.h"
#include "Geometry.h"
#include "PropertyObject.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "DclFormView.h"
#include "StudioDialogControl.h"
#include "ControlTypes.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CGeometry property page

CGeometry::CGeometry( TDclControlPtr pDclControl )
: CPropertyPage(CGeometry::IDD)
, mpDclControl( pDclControl )
, m_pUseTopFromBottom( pDclControl->GetPropertyObject( Prop::UseTopFromBottom ) )
, m_pUseBottomFromBottom( pDclControl->GetPropertyObject( Prop::UseBottomFromBottom ) )
, m_pUseLeftFromRight( pDclControl->GetPropertyObject( Prop::UseLeftFromRight ) )
, m_pUseRightFromRight( pDclControl->GetPropertyObject( Prop::UseRightFromRight ) )
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
	m_pUseTopFromBottom->SetLongValue(m_Top.GetItemData(m_Top.GetCurSel()));
	m_pUseLeftFromRight->SetLongValue(m_Left.GetItemData(m_Left.GetCurSel()));
	m_pUseBottomFromBottom->SetLongValue(m_Bottom.GetItemData(m_Bottom.GetCurSel()));
	m_pUseRightFromRight->SetLongValue(m_Right.GetItemData(m_Right.GetCurSel()));
	CStudioDialogControl::UpdateProperty( mpDclControl, Prop::Top );
	CStudioDialogControl::UpdateProperty( mpDclControl, Prop::Left );
	CStudioDialogControl::UpdateProperty( mpDclControl, Prop::Height );
	CStudioDialogControl::UpdateProperty( mpDclControl, Prop::Width );
	return CPropertyPage::OnApply();
}


BOOL CGeometry::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_Top.SetItemData(0, 0);
	m_Top.SetItemData(1, 1);

	m_Bottom.SetItemData(0, 0);
	m_Bottom.SetItemData(1, 1);

	m_Left.SetItemData(0, 0);
	m_Left.SetItemData(1, 1);
	m_Left.SetItemData(2, 2);

	m_Right.SetItemData(0, 0);
	m_Right.SetItemData(1, 1);

	TDclControlList Splitters;
	mpDclControl->GetOwnerForm()->FindControls(CtlSplitter, Splitters);
	for( TDclControlList::const_iterator iter = Splitters.begin(); iter != Splitters.end(); ++iter )
	{
		int nTheWidth = (*iter)->GetLongProperty(Prop::Width);
		int nTheHeight = (*iter)->GetLongProperty(Prop::Height);
		if (nTheWidth > nTheHeight)
		{
			m_Top.SetItemData(m_Top.AddString((*iter)->GetStringProperty(Prop::Name)), (*iter)->GetID());
			m_Bottom.SetItemData(m_Bottom.AddString((*iter)->GetStringProperty(Prop::Name)), (*iter)->GetID());
		}
		else
		{
			m_Left.SetItemData(m_Left.AddString((*iter)->GetStringProperty(Prop::Name)), (*iter)->GetID());
			m_Right.SetItemData(m_Right.AddString((*iter)->GetStringProperty(Prop::Name)), (*iter)->GetID());
		}
	}

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
}

void CGeometry::OnSelchangeTop() 
{
	m_ExampleImage.Invalidate();
	SetModified(TRUE);	
}

void CGeometry::OnSelchangeBottom() 
{
	m_ExampleImage.Invalidate();
	SetModified(TRUE);	
}

void CGeometry::OnSelchangeRight() 
{
	m_ExampleImage.Invalidate();
	SetModified(TRUE);	
}

void CGeometry::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void CGeometry::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertyPage::OnMouseMove(nFlags, point);
}
