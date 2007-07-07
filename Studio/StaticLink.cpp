// StaticLink.cpp : implementation file
//

#include "stdafx.h"
#include "StaticLink.h"
#include "Resource.h"
#include "AcadColorTable.h"
#include "Workspace.h"

COLORREF CStaticLink::g_colorVisited   = RGB(128,0,128);		 // purple
HCURSOR	CStaticLink::g_hCursorLink = NULL;


IMPLEMENT_DYNAMIC(CStaticLink, CStatic)

/////////////////////////////////////////////////////////////////////////////
// CStaticLink

CStaticLink::CStaticLink()
{
	//m_UrlLink ;//= "www.objectdcl.com";
	m_pStaticBrush = new CBrush();
	//m_pStaticBrush->CreateStockObject(HOLLOW_BRUSH);	
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nButtonFace));

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
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticLink message handlers

void CStaticLink::SetAcadColor(long nColorArg)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nColorArg));
}


void CStaticLink::SetForeColor(long nColorArg)
{
	m_color = GetRGBColor(nColorArg);
}

BOOL CStaticLink::Create(LPCTSTR lpszWindowName, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	m_link = lpszWindowName;	
	m_bDeleteOnDestroy = TRUE;
	//SetForeColor(m_pControl->GetColorProperty(nForeColor));
	
	BOOL bCreated = CStatic::Create(
		lpszWindowName,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rect, 
		pParentWnd,
		nID);

	return bCreated;
}

LRESULT CStaticLink::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

HBRUSH CStaticLink::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(m_color);
	pDC->SetBkMode(TRANSPARENT);

	pDC->SelectObject(m_pStaticBrush);
	return *m_pStaticBrush;		
}

void CStaticLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_link.IsEmpty()) 
	{
		// no link: try to load from resource string or window text 
		m_link = theWorkspace.LoadResourceString(GetDlgCtrlID());
		if (m_link.IsEmpty())
		{
			GetWindowText(m_link);
			if (m_link.IsEmpty())
				return;
		}
	}

	// Call ShellExecute to run the file. For an URL, this will run the browser.
	//
	HINSTANCE h = m_link.Navigate(m_UrlLink);
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
	if (g_hCursorLink == NULL) {
		CWinApp* pApp = AfxGetApp();
		g_hCursorLink = CopyCursor(pApp->LoadCursor(IDC_HANDICON));		
	}
	if (g_hCursorLink) {
		::SetCursor(g_hCursorLink);
		return TRUE;
	}

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
	
	//CFont *pFont = CStatic::GetFont();
	//pFont->lfUnderline = TRUE;

	return 0;
}

void CStaticLink::SetLinkText(CString sNewText) 
{
	m_link = sNewText;
	CStatic::SetWindowText(sNewText);
	
	Invalidate();
	
}
void CStaticLink::RePaint() 
{
	Invalidate();
	
}

void CStaticLink::Refresh(CDC *pdc) 
{
	HBRUSH hbr = NULL;

	// prepare to erase the back ground
	CRect rcThis;	
	GetWindowRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
	
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, m_pStaticBrush);
	

	// use underline font and visited/unvisited colors
	//pdc->SelectObject(&m_font);
	pdc->SelectObject(CWnd::GetFont());
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
	CStatic::OnDestroy();
	
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}
