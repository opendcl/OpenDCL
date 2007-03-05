// VdclGroupBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclGroupBox.h"
#include "DclControlObject.h"
//#include "PropertyObject.h"
//#include "ToolTips.h"
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
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	// get the caption
	CString Caption = pControl->GetStrProperty(nCaption);
	
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;	
	RetVal =  CStatic::Create(CString(), dwStyle, ArxRect, pParentWnd, nID );
	
	VERIFY(CStatic::SubclassDlgItem(nID, pParentWnd));

	CRect rcFrame(0,0, ArxRect.Width(), ArxRect.Height());
	dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_GROUPBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	
	m_Frame.Create(Caption, dwStyle, rcFrame, this, nFrameId);
	VERIFY(m_Frame.SubclassDlgItem(nFrameId, this));
	m_Frame.SetFont(GetFont());
	m_Frame.MoveWindow(rcFrame, TRUE);

	
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


void VdclGroupBox::SetTooltipText(CString* spText, BOOL bActivate)
{
	
} // End of SetTooltipText
void VdclGroupBox::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip

BOOL VdclGroupBox::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	return CStatic::PreTranslateMessage(pMsg);
}
