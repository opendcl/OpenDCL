// StaticLink.cpp : implementation file
//

#include "stdafx.h"
#include "StaticLink.h"
#include "AcadColorTable.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "Resource.h"
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

	m_pStaticBrush = new CBrush();
	//m_pStaticBrush->CreateStockObject(HOLLOW_BRUSH);	
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(-16));

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
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nColor));
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
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	m_link = lpszWindowName;	
	m_bDeleteOnDestroy = TRUE;
	int nColor = pControl->GetLngProperty(nForeColor);
	m_color = GetRGBColor(nColor);

	BOOL bCreated = CStatic::Create(
		lpszWindowName,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		ArxRect, 
		pParentWnd,
		nID);

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	switch (m_ArxControl->GetLngProperty(nEventInvoke))
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

#if (_ACADTARGET == 16)
UINT CStaticLink::OnNcHitTest(CPoint point) 
#else
LRESULT CStaticLink::OnNcHitTest(CPoint point) 
#endif
{
	return HTCLIENT;
}

HBRUSH CStaticLink::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return NULL;

	pDC->SetTextColor(m_color);
	pDC->SetBkMode(TRANSPARENT);

	pDC->SelectObject(m_pStaticBrush);
	return *m_pStaticBrush;		
}


void CStaticLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Call ShellExecute to run the file. For an URL, this will run the browser.
	//
	HINSTANCE h;
	if (m_ArxControl->GetLngProperty(nURLLinkType) == 0)
		h = m_link.Navigate(m_ArxControl->GetStrProperty(nURLAddress));
	else
	{
		h = m_link.Navigate(CString ("mailto:") + m_ArxControl->GetStrProperty(nURLAddress));
	}

	if ((UINT)h > 32) 
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
	HMODULE hModule = _hdllInstance;	
	::SetCursor(::LoadCursor(hModule, MAKEINTRESOURCE(IDC_HAND)));
	return TRUE;
	
}

void CStaticLink::PostNcDestroy()
{
	//if (m_bDeleteOnDestroy)
		//delete this;
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
	CStatic::SetWindowText(sNewText);
	
	CDC *pdc = CStatic::GetDC();
	if (pdc == NULL)
		return;
	CStaticLink::Refresh(pdc);
	pdc->Detach();
	
}
void CStaticLink::Refresh()
{
	CDC *pdc = CStatic::GetDC();
	if (pdc == NULL)
		return;
	CStaticLink::Refresh(pdc);
	pdc->Detach();

}

void CStaticLink::Refresh(CDC *pdc) 
{
	HBRUSH hbr = NULL;
	
	// prepare to erase the back ground
	CRect rcThis;	
	CWnd::GetWindowRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
	
	CBrush *pCellBrush = new CBrush;	
	pCellBrush->CreateSysColorBrush(COLOR_BTNFACE);
	
	
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, pCellBrush);
	// delete the brush
	pCellBrush->DeleteObject();
	delete pCellBrush;
	

	// use underline font and visited/unvisited colors
	pdc->SelectObject(GetFont());
	pdc->SetTextColor(m_color);
	CString sText;
	CStatic::GetWindowText(sText);
	pdc->SetBkMode(TRANSPARENT);
	
	pdc->TextOut(1,1,sText);

	// return hollow brush to preserve parent background color
	hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
}

void CStaticLink::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	
	PAINTSTRUCT ps; 
	CDC* pdc = BeginPaint(&ps);
    
	CStaticLink::Refresh(pdc);

	EndPaint(&ps);	
}

void CStaticLink::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CStatic::OnDestroy();
	
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
	
}


void CStaticLink::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CStatic::OnMouseMove(nFlags, point);
}

BOOL CStaticLink::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}

