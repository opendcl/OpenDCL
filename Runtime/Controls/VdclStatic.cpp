// VdclStatic.cpp : implementation file
//

#include "stdafx.h"
#include "VdclStatic.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// VdclStatic

VdclStatic::VdclStatic()
: mbrushBackground( GetRGBColor(-16) )
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_BkColor = GetRGBColor(-16);
	m_ForeColor = GetRGBColor(-19);
}

VdclStatic::~VdclStatic()
{
	mbrushBackground.DeleteObject();
}


BEGIN_MESSAGE_MAP(VdclStatic, CStatic)
	//{{AFX_MSG_MAP(VdclStatic)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	//ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void VdclStatic::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	UpdateWindow();
	CStatic::OnNcCalcSize(bCalcValidRects, lpncsp);
}


BOOL VdclStatic::OnEraseBkgnd(CDC* pDC) 
{
	CRect cr;
	GetClientRect(cr); 
	pDC->FillRect(&cr, &mbrushBackground);
	return TRUE; //CStatic::OnEraseBkgnd(pDC);
}

void VdclStatic::SetAcadColor(long nColor)
{
	m_BkColor = GetRGBColor(nColor);
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(m_BkColor);
}


void VdclStatic::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}


/////////////////////////////////////////////////////////////////////////////
// VdclStatic message handlers

BOOL VdclStatic::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	
	dwStyle = WS_CHILD | WS_VISIBLE | SS_LEFT | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	

	RetVal = CStatic::Create(Caption, dwStyle, ArxRect, pParentWnd, nID);
	VERIFY(CStatic::SubclassDlgItem(nID, pParentWnd));

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	return RetVal;
}

void VdclStatic::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CStatic::OnDestroy();
}

void VdclStatic::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CStatic::OnMouseMove(nFlags, point);
}

BOOL VdclStatic::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}

void VdclStatic::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = m_ArxControl;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}

void VdclStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_ArxControl->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		BeginDragnDrop(m_ArxControl, point, m_bInvokeWithSendString);
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void VdclStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	int nDCState = dc.SaveDC();

	dc.SetTextColor(m_ForeColor);
	dc.SetBkColor(m_BkColor);
	dc.SetBkMode(OPAQUE);
	CBrush* pOldBrush = dc.SelectObject(&mbrushBackground);

	// setup the font			
	CFont *pOldFont = dc.SelectObject(GetFont());

	DWORD dwJustification;
	switch (m_ArxControl->GetLongProperty(Prop::Justification))
	{
	case 0:
		dwJustification = DT_TOP|DT_WORDBREAK|DT_LEFT;
		break;
	case 1:
		dwJustification = DT_TOP|DT_WORDBREAK|DT_CENTER;
		break;
	case 2:
		dwJustification = DT_TOP|DT_WORDBREAK|DT_RIGHT;
		break;	
	}

	CRect rcThis;
	GetClientRect(&rcThis);
	CString sText;
	GetWindowText(sText);
	LPCTSTR lpszString;
	lpszString = sText;

	dc.FillRect(rcThis, &mbrushBackground);
	if (IsWindowEnabled() == FALSE)
    {
      dc.SetTextColor(GetSysColor(COLOR_3DHIGHLIGHT));

      CRect rect = rcThis;

      rect.left   += 1;
      rect.top    += 1;
      rect.right  += 1;
      rect.bottom += 1;

	  dc.DrawText(lpszString, sText.GetLength(), &rect, dwJustification);
	  dc.SetBkColor(m_BkColor);
		dc.SetTextColor(GetSysColor(COLOR_3DSHADOW));
	  dc.DrawText(lpszString, sText.GetLength(), &rcThis, dwJustification);
    }
	else
	{
		dc.SetBkColor(m_BkColor);
		dc.SetTextColor(m_ForeColor);
		dc.DrawText(lpszString, sText.GetLength(), &rcThis, dwJustification);
	}
	
	// select old font
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldBrush);
	dc.RestoreDC(nDCState);
}

HBRUSH VdclStatic::CtlColor ( CDC* pDC, UINT nCtlColor )
{
	if (!IsWindowEnabled())
		return NULL;
	
	//pDC->SetBkMode(TRANSPARENT);	
	pDC->SetTextColor(m_ForeColor);
	pDC->SetBkColor(m_BkColor); // Setting the Color of the Text Background to the one passed by the Dialog
	return mbrushBackground;
	// for tranparent call this next line.
	return (HBRUSH)::GetStockObject(HOLLOW_BRUSH); 
}

void VdclStatic::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	RedrawWindow();
}
