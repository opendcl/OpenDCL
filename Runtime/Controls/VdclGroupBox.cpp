// VdclGroupBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclGroupBox.h"
#include "DclControlObject.h"
#include "ToolTips.h"
#include "PropertyObject.h"
#include "PropertyIds.h"

const int nFrameId = 78;


/////////////////////////////////////////////////////////////////////////////
// VdclGroupBox

VdclGroupBox::VdclGroupBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
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

BOOL VdclGroupBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	// get the caption
	CString Caption = pControl->GetStringProperty(Prop::Caption);
	
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;	
	RetVal =  CStatic::Create(CString(), dwStyle, ArxRect, pParentWnd, nID );

	CRect rcFrame(0,0, ArxRect.Width(), ArxRect.Height());
	dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_GROUPBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	
	m_Frame.Create(Caption, dwStyle, rcFrame, this, nFrameId);
	VERIFY(m_Frame.SubclassDlgItem(nFrameId, this));
	m_Frame.SetFont(GetFont());
	m_Frame.MoveWindow(rcFrame, TRUE);

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);
	return RetVal;
}

void VdclGroupBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CStatic::OnDestroy();
	
	
}

void VdclGroupBox::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	CRect rcFrame(0,0,cx,cy);
	m_Frame.MoveWindow(rcFrame, TRUE);
}

BOOL VdclGroupBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}
