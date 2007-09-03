// VdclScrollBar.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "VdclScrollBar.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclScrollBar

VdclScrollBar::VdclScrollBar()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclScrollBar::~VdclScrollBar()
{
}


BEGIN_MESSAGE_MAP(VdclScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(VdclScrollBar)
	ON_WM_HSCROLL_REFLECT()	
	ON_WM_SETFOCUS()
	ON_WM_VSCROLL_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclScrollBar message handlers

BOOL VdclScrollBar::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID) 
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
	
	m_nLargeChange = pControl->GetLongProperty(Prop::LargeChange);
	m_nSmallChange = pControl->GetLongProperty(Prop::SmallChange);

	dwStyle = WS_CHILD | SBS_BOTTOMALIGN | WS_CLIPSIBLINGS | WS_GROUP;

	if (pControl->GetLongProperty(Prop::Orientation) == 0)
		dwStyle = dwStyle | SBS_HORZ;
	else
		dwStyle = dwStyle | SBS_VERT;

	RetVal = CScrollBar::Create(dwStyle, ArxRect, pParentWnd, nID );

	VERIFY(CScrollBar::SubclassDlgItem(nID, pParentWnd));
	CScrollBar::SetScrollRange(
		pControl->GetLongProperty(Prop::MinValue),
		pControl->GetLongProperty(Prop::MaxValue),
		TRUE);
	m_hPos = pControl->GetLongProperty(Prop::Value);
	CScrollBar::SetScrollPos(m_hPos, TRUE);
	CScrollBar::ShowScrollBar(TRUE);
	CScrollBar::SetWindowPos(NULL, ArxRect.left, ArxRect.top, ArxRect.Width(), ArxRect.Height(), NULL);
	
	m_pValueProp = pControl->GetPropertyObject(Prop::Value);

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	switch (m_ArxControl->GetLongProperty(Prop::EventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	return RetVal;
}


void VdclScrollBar::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollBar::OnSetFocus(pOldWnd);
	
	pOldWnd->SetFocus();
	
}


void VdclScrollBar::HScroll(UINT nSBCode, UINT nPos) 
{
	VdclScrollBar::OnScroll(nSBCode, nPos);
}

void VdclScrollBar::OnScroll(UINT nSBCode, UINT nPos) 
{
	int MinPos;
	int MaxPos;
	int nWhichEvent = 0;
	
	CScrollBar::GetScrollRange(&MinPos, &MaxPos);
	
	switch (nSBCode)
	{
	case 0: // lesser or min arrow click
		{
			if (m_hPos > MinPos)
				m_hPos = m_hPos - m_nSmallChange;
			break;
		}
	case 1: // greater or max arrow click
		{
			if (m_hPos < MaxPos)
				m_hPos = m_hPos + m_nSmallChange;
			break;
		}		
	case 2: // user has clicked scroll area left/above of Scroll Button indicator
		{
			int nThisPos = m_hPos - m_nLargeChange;
			if (nThisPos < MinPos)
				m_hPos = MinPos;
			else
				m_hPos = nThisPos;
			break;
		}
	case 3: // user has click scroll area right/below of Scroll Button indicator
		{
			int nThisPos = m_hPos + m_nLargeChange;
			if (nThisPos > MaxPos)
				m_hPos = MaxPos;
			else
				m_hPos = nThisPos;
			break;
		}
	case 5: // user is scrolling the scrollbar
		{
			m_hPos = nPos;
			break;
		}
	case 4: // user is done scrolling the scrollbar
	case 8: // user is done scrolling the scrollbar
		{
			nWhichEvent = 1;
			break;
		}
	default:
		{
			nSBCode = nSBCode;
			break;
		}
	}

	if (m_hPos < MinPos)
		m_hPos = MinPos;
	if (m_hPos > MaxPos)
		m_hPos = MaxPos;
	
	CScrollBar::SetScrollPos(m_hPos, TRUE);	
	
	m_pValueProp->SetLongValue(m_hPos);
	
	if (nWhichEvent == 0)
		// call methods to invoke the event
		InvokeMethodInt(m_ArxControl->GetStrProperty(Prop::EventScroll), m_hPos, m_bInvokeWithSendString);
	else
		// call methods to invoke the event
		InvokeMethodInt(m_ArxControl->GetStrProperty(Prop::EventScrolled), m_hPos, m_bInvokeWithSendString);
	
	
}


void VdclScrollBar::VScroll(UINT nSBCode, UINT nPos) 
{
	VdclScrollBar::OnScroll(nSBCode, nPos);
	
}

void VdclScrollBar::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CScrollBar::OnDestroy();
	
}

BOOL VdclScrollBar::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	
	return CScrollBar::PreTranslateMessage(pMsg);
}
