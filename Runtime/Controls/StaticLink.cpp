// StaticLink.cpp : implementation file
//

#include "stdafx.h"
#include "StaticLink.h"
#include "AcadColorTable.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "SharedRes.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// CStaticLink

COLORREF CStaticLink::g_colorUnvisited = RGB(0,0,255);		 // blue
COLORREF CStaticLink::g_colorVisited   = RGB(128,0,128);		 // purple
HCURSOR	CStaticLink::g_hCursorLink = NULL;

IMPLEMENT_DYNAMIC(CStaticLink, CStatic)

CStaticLink::CStaticLink()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_BkColor = GetRGBColor(-16);
	m_color = 0;
}

CStaticLink::~CStaticLink()
{
}

BEGIN_MESSAGE_MAP(CStaticLink, CStatic)
	//{{AFX_MSG_MAP(CStaticLink)
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticLink message handlers

void CStaticLink::SetAcadColor(long nColor)
{
	m_BkColor = GetRGBColor(nColor);
}


void CStaticLink::SetForeColor(long nColor)
{
	m_color = GetRGBColor(nColor);
}


BOOL CStaticLink::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	CString lpszWindowName;// = "Default";
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	m_link = lpszWindowName;	
	int nColor = pControl->GetLongProperty(Prop::ForegroundColor);
	m_color = GetRGBColor(nColor);

	BOOL bCreated = CStatic::Create(
		lpszWindowName,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		ArxRect, 
		pParentWnd,
		nID);

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
	return bCreated;
}

__LRESULT CStaticLink::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

HBRUSH CStaticLink::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return NULL;

	pDC->SetTextColor(m_color);
	pDC->SetBkMode(TRANSPARENT);
	return CreateSolidBrush(m_BkColor);		
}


void CStaticLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Call ShellExecute to run the file. For an URL, this will run the browser.
	//
	HINSTANCE h;
	if (m_ArxControl->GetLongProperty(Prop::URLLinkType) == 0)
		h = m_link.Navigate(m_ArxControl->GetStrProperty(Prop::URLAddress));
	else
		h = m_link.Navigate(CString(_T("mailto:")) + m_ArxControl->GetStrProperty(Prop::URLAddress));

	if ((UINT_PTR)h > 32) 
	{						 // success!
		m_color = g_colorVisited;			 // change color
		Invalidate();							 // repaint 
	}
	else 
	{
		MessageBeep(0);		// unable to execute file!
		TRACE(_T("*** WARNING: CStaticLink: unable to navigate link %s\n"),
		(LPCTSTR)m_link);
	}	
}

BOOL CStaticLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	::SetCursor(::LoadCursor(_hdllInstance, MAKEINTRESOURCE(IDC_HAND)));
	return TRUE;
}

int CStaticLink::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CStaticLink::SetLinkText(CString sNewText) 
{
	m_link = sNewText;
	SetWindowText(sNewText);
	Refresh();
}

void CStaticLink::Refresh()
{
	CDC *pdc = GetDC();
	if (pdc == NULL)
		return;
	Refresh(pdc);
	ReleaseDC(pdc);
}

void CStaticLink::Refresh(CDC *pdc) 
{
	HBRUSH hbr = NULL;
	
	// prepare to erase the back ground
	CRect rcThis;	
	CWnd::GetWindowRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
	
	CBrush CellBrush;	
	CellBrush.CreateSysColorBrush(COLOR_BTNFACE);
	
	
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, &CellBrush);
	// delete the brush
	CellBrush.DeleteObject();

	// use underline font and visited/unvisited colors
	CFont* pOldFont = pdc->SelectObject(GetFont());
	pdc->SetTextColor(m_color);
	CString sText;
	GetWindowText(sText);
	pdc->SetBkMode(TRANSPARENT);
	pdc->TextOut(1,1,sText);
	pdc->SelectObject(pOldFont);
}

void CStaticLink::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Refresh(&dc);
}

void CStaticLink::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CStatic::OnDestroy();
}

void CStaticLink::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CStatic::OnMouseMove(nFlags, point);
}

BOOL CStaticLink::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

