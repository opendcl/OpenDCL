// VdclProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VdclProgressCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "PropertyIds.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VdclProgressCtrl

VdclProgressCtrl::VdclProgressCtrl()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclProgressCtrl::~VdclProgressCtrl()
{
}


BEGIN_MESSAGE_MAP(VdclProgressCtrl, CProgressCtrl)
	//{{AFX_MSG_MAP(VdclProgressCtrl)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclProgressCtrl message handlers

BOOL VdclProgressCtrl::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE | WS_CLIPSIBLINGS;

	if (pControl->GetBoolProperty(nSmoothProgress) == TRUE)
		dwStyle = dwStyle | PBS_SMOOTH;

	if (pControl->GetLngProperty(nOrientation) == 1)
		dwStyle = dwStyle | PBS_VERTICAL ;

	RetVal = CProgressCtrl::Create(dwStyle, ArxRect, pParentWnd, nID);
	
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	return RetVal;
}

BOOL VdclProgressCtrl::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CProgressCtrl::PreTranslateMessage(pMsg);
}

void VdclProgressCtrl::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CProgressCtrl::OnDestroy();
	
}
