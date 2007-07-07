// VdclGroupBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclGroupBox.h"

const int nFrameId = 78;


/////////////////////////////////////////////////////////////////////////////
// VdclGroupBox

VdclGroupBox::VdclGroupBox()
{
	m_bCreated = false;
}

VdclGroupBox::~VdclGroupBox()
{
}


BEGIN_MESSAGE_MAP(VdclGroupBox, CStatic)
	//{{AFX_MSG_MAP(VdclGroupBox)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclGroupBox message handlers

BOOL VdclGroupBox::Create(CString sCaption, CRect rcRect, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |WS_GROUP;	
	RetVal =  CStatic::Create(_T(""), dwStyle, rcRect, pParentWnd, nID );
	
	//VERIFY(CStatic::SubclassDlgItem(nID, pParentWnd));

	CRect rcFrame(0,0, rcRect.Width(), rcRect.Height());
	dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_GROUPBOX  | WS_CLIPSIBLINGS;	
	m_bCreated = m_Frame.Create(sCaption, dwStyle, rcFrame, this, nFrameId);
	//VERIFY(m_Frame.SubclassDlgItem(nFrameId, this));
	m_Frame.MoveWindow(rcFrame, TRUE);
	return RetVal;
}
void VdclGroupBox::SetCaption(CString sCaption)
{
	m_Frame.SetWindowText(sCaption);
}
void VdclGroupBox::OnDestroy() 
{

	CStatic::OnDestroy();
	
	
}

void VdclGroupBox::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	if (m_bCreated)
	{
		CRect rcFrame(0,0,cx,cy);
		m_Frame.MoveWindow(rcFrame, TRUE);	
	}
}


