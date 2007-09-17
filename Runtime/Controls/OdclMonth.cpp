// OdclMonth.cpp : implementation file
//

#include "stdafx.h"
#include "OdclMonth.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// OdclMonth

OdclMonth::OdclMonth()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

OdclMonth::~OdclMonth()
{
}


BEGIN_MESSAGE_MAP(OdclMonth, CMonthCalCtrl)
	//{{AFX_MSG_MAP(OdclMonth)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(MCN_GETDAYSTATE, OnGetdaystate)
	ON_NOTIFY_REFLECT(MCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(MCN_SELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OdclMonth message handlers

BOOL OdclMonth::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

	long lMaxSel = pControl->GetLongProperty(Prop::MultiSelection);
	if ( lMaxSel > 1 )
		dwStyle = dwStyle | MCS_MULTISELECT;

	if (pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CMonthCalCtrl::Create(dwStyle, ArxRect, pParentWnd, nID );
	if( lMaxSel > 1 )
		SetMaxSelCount( lMaxSel );
	
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	CMonthCalCtrl::SizeMinReq(TRUE);
	CTime tToday;
	CMonthCalCtrl::GetToday(tToday);
	CMonthCalCtrl::SetCurSel(tToday);

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

void OdclMonth::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CMonthCalCtrl::OnDestroy();
}

void OdclMonth::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CMonthCalCtrl::OnMouseMove(nFlags, point);
}


void OdclMonth::OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventGetDayState), m_bInvokeWithSendString);
	*pResult = 0;
}

void OdclMonth::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventSelChanged), m_bInvokeWithSendString);
	*pResult = 0;
}

void OdclMonth::OnSelect(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventSelect), m_bInvokeWithSendString);
	*pResult = 0;
}

BOOL OdclMonth::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CMonthCalCtrl::PreTranslateMessage(pMsg);
}
