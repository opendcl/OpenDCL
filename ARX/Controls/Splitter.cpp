// 3DRect.cpp : implementation file
//

#include "stdafx.h"
#include "Splitter.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "Project.h"


/////////////////////////////////////////////////////////////////////////////
// CSplitter

CSplitter::CSplitter()
{
	m_nStyle = 0;
	hCursor = NULL;
	
	// Min and Max range of the splitter.
	m_nMin = m_nMax = -1;

	// Mouse is pressed down or not ?
	m_bIsPressed = FALSE;	

	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      NULL,
      NULL,
      NULL);

}

CSplitter::~CSplitter()
{
	if (hCursor != NULL)
		DeleteObject(hCursor);
}


BEGIN_MESSAGE_MAP(CSplitter, CWnd)
	//{{AFX_MSG_MAP(CSplitter)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitter message handlers

BOOL CSplitter::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	m_pLeft = m_ArxControl->GetPropertyObject(nLeft);
	m_pTop = m_ArxControl->GetPropertyObject(nTop);

	CPoint pt = ReadPlacement();

	if (pControl->m_pWidth->GetLongValue() > pControl->m_pHeight->GetLongValue())
	{
		// get the rectangle of the new control
		if (pt.y > -1)
		{
			pControl->m_pTop->SetLongValue(pt.y);
		}			
	}
	else
	{
		// get the rectangle of the new control
		if (pt.x > -1)
		{
			pControl->m_pLeft->SetLongValue(pt.x);
		}			
	}
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = ArxRect.top + pControl->m_pHeight->GetLongValue();
	ArxRect.right = ArxRect.left + pControl->m_pWidth->GetLongValue();
	
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;	
	
	RetVal =  CWnd::Create(
		m_ClassName,
		m_ClassName,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		ArxRect,
		pParentWnd,
		nID+2500,
		NULL);

	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	
	m_nStyle = m_ArxControl->GetLngProperty(nSplitterStyle);

	if (ArxRect.Width() < ArxRect.Height())
	{
		hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		m_nType = SPS_VERTICAL;
	}
	else
	{
		hCursor = ::LoadCursor(NULL, IDC_SIZENS);
		m_nType = SPS_HORIZONTAL;
	}

	
	
	m_pClosestInside = m_ArxControl->GetPropertyObject(nSplitterMin);
	m_pClosestOutside = m_ArxControl->GetPropertyObject(nSplitterMax);

	if (m_nType == SPS_VERTICAL)
		SetRange(m_pClosestInside->GetLongValue(),
				 ArxRect.Height() - m_pClosestOutside->GetLongValue());
	else
		SetRange(m_pClosestInside->GetLongValue(),
				 ArxRect.Width() - m_pClosestOutside->GetLongValue());

	return RetVal;
}

void CSplitter::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	PAINTSTRUCT ps; 
	
	
	CDC* pdc = BeginPaint(&ps);
    
	
	CBrush *pCellBrush = new CBrush;	
	pCellBrush->CreateSysColorBrush(COLOR_BTNFACE);
			
	
	CRect rcThis;
	GetWindowRect(&rcThis);

	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());


	switch (m_nStyle)
	{
	case Splitter_DoubleRaised:
		{
		// draw the Window background for the cell				
		pdc->FillRect(rcCell, pCellBrush);
		
		if (rcCell.Width() >= rcCell.Height())
		{
			CRect rc1 = rcCell;
			rc1.bottom = rc1.top + 3;
			CRect rc2 = rc1;
			rc2.top = rc1.top + 3;
			rc2.bottom = rc2.top + 3;

			// draw the solid rectangle
			::DrawEdge(pdc->m_hDC, &rc1, BDR_RAISEDINNER, BF_RECT);		
			// draw the solid rectangle
			::DrawEdge(pdc->m_hDC, &rc2, BDR_RAISEDINNER, BF_RECT);		
		}
		else
		{
			CRect rc1 = rcCell;
			rc1.right = rc1.left + 3;
			CRect rc2 = rc1;
			rc2.left = rc1.left + 3;
			rc2.right = rc2.left + 3;

			// draw the solid rectangle
			::DrawEdge(pdc->m_hDC, &rc1, BDR_RAISEDINNER, BF_RECT);		
			// draw the solid rectangle
			::DrawEdge(pdc->m_hDC, &rc2, BDR_RAISEDINNER, BF_RECT);		
		}
		break;
		
		}
	case Splitter_Raised:
		{
		// draw the solid rectangle
		::DrawEdge(pdc->m_hDC, &rcCell, BDR_RAISEDINNER, BF_RECT);
		
		rcCell.DeflateRect(1,1);
		
		// draw the Window background for the cell				
		pdc->FillRect(rcCell, pCellBrush);
		break;
		}
	case Splitter_Sunken:
		{
		// draw the Window background for the cell				
		pdc->FillRect(rcCell, pCellBrush);
		
		CRect rc1 = rcCell;
			
		if (rcCell.Width() >= rcCell.Height())
		{
			rc1.top++; 
			rc1.top++; 
			rc1.left--;
			rc1.left--; 
			rc1.right++;
			rc1.right++;
			rc1.bottom++; 
			rc1.bottom++; 
		}
		else
		{
			rc1.top--; 
			rc1.top--; 
			rc1.left++;
			rc1.left++; 
			rc1.right++;
			rc1.right++;
			rc1.bottom++; 
			rc1.bottom++; 
		}
		
		// draw the solid rectangle
		::DrawEdge(pdc->m_hDC, &rc1, EDGE_ETCHED, BF_RECT);
		

		rcCell.top = rc1.bottom+1;
		
		// draw the Window background for the cell				
		pdc->FillRect(rcCell, pCellBrush);
		break;
		}

	default:
		// draw the Window background for the cell				
		pdc->FillRect(rcCell, pCellBrush);
		break;
	}
	
	// delete the brush
	pCellBrush->DeleteObject();
	delete pCellBrush;

	EndPaint(&ps);
	
	// Do not call CWnd::OnPaint() for painting messages
}


void CSplitter::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	m_bIsPressed = TRUE;
	SetCapture();
	CRect rcWnd;
	GetWindowRect(rcWnd);
	
	if (m_nType == SPS_VERTICAL)
		m_nX = rcWnd.left + rcWnd.Width() / 2;		
	else
		m_nY = rcWnd.top  + rcWnd.Height() / 2;
	
	if (m_nType == SPS_VERTICAL)
		m_nSavePos = m_nX;
	else
		m_nSavePos = m_nY;

	CWindowDC dc(NULL);
	DrawLine(&dc, m_nX, m_nY);
}

void CSplitter::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCursor(hCursor);
	ShowCursor(TRUE);

	if (m_bIsPressed)
	{
		CWindowDC dc(NULL);
		DrawLine(&dc, m_nX, m_nY);
		
		CPoint pt = point;
		ClientToScreen(&pt);
		GetParent()->ScreenToClient(&pt);

		if (pt.x < m_pClosestInside->GetLongValue())
			pt.x = m_pClosestInside->GetLongValue();
		if (pt.y < m_pClosestInside->GetLongValue())
			pt.y = m_pClosestInside->GetLongValue();

		CRect rc;
		GetParent()->GetClientRect(&rc);			

		if (pt.x > rc.Width() - m_pClosestOutside->GetLongValue())
			pt.x = rc.Width() - m_pClosestOutside->GetLongValue();
		if (pt.y > rc.Height() - m_pClosestOutside->GetLongValue())
			pt.y = rc.Height() - m_pClosestOutside->GetLongValue();

		GetParent()->ClientToScreen(&pt);
		m_nX = pt.x;
		m_nY = pt.y;
		DrawLine(&dc, m_nX, m_nY);
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CSplitter::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsPressed)
	{
		ClientToScreen(&point);
		CWindowDC dc(NULL);

		DrawLine(&dc, m_nX, m_nY);
		CPoint pt(m_nX, m_nY);
		m_bIsPressed = FALSE;
		CWnd *pOwner = GetOwner();
		if (pOwner && IsWindow(pOwner->m_hWnd))
		{
			pOwner->ScreenToClient(&pt);
			MoveWindowTo(pt);
				
			int delta;
			if (m_nType == SPS_VERTICAL)
			{
				delta = m_nX - m_nSavePos;
				m_ArxControl->m_pLeft->SetLongValue(m_ArxControl->m_pLeft->GetLongValue() + delta );
				m_ArxControl->m_pOffsetLeft->SetLongValue(m_ArxControl->m_pOffsetLeft->GetLongValue() + delta );
				m_ArxControl->m_pOffsetRight->SetLongValue(m_ArxControl->m_pOffsetRight->GetLongValue() - delta );
			}
			else
			{
				delta = m_nY - m_nSavePos;
				m_ArxControl->m_pTop->SetLongValue(m_ArxControl->m_pTop->GetLongValue() + delta );
				m_ArxControl->m_pOffsetTop->SetLongValue(m_ArxControl->m_pOffsetTop->GetLongValue() + delta );
				m_ArxControl->m_pOffsetBottom->SetLongValue(m_ArxControl->m_pOffsetBottom->GetLongValue() - delta );
			}
			
			SPC_NMHDR nmsp;
		
			nmsp.hdr.hwndFrom = m_hWnd;
			nmsp.hdr.idFrom   = GetDlgCtrlID();
			nmsp.hdr.code     = SPN_SIZED;
			nmsp.delta = delta;

			pOwner->SendMessage(WM_NOTIFY, nmsp.hdr.idFrom, (LPARAM)&nmsp);

			CRect rcThis;
			GetWindowRect(&rcThis);
			GetParent()->ScreenToClient(&rcThis);
			InvokeMethodIntIntIntInt(
				m_ArxControl->GetStrProperty(nEventSplitterMoved),
				rcThis.left,
				rcThis.top,
				rcThis.Width(),
				rcThis.Height(),
				m_bInvokeWithSendString);
	
		}
		Invalidate();
		
		m_pControlPane->RecalcLayout( true );

		SavePlacement();
	}

	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();
	
}

void CSplitter::DrawLine(CDC* pDC, int x, int y)
{
	int nRop = pDC->SetROP2(R2_NOTXORPEN);

	CRect rcWnd;
	int d = 1;
	GetWindowRect(rcWnd);
	CPen  pen;
	pen.CreatePen(0, 1, RGB(200, 200, 200));
	CPen *pOP = pDC->SelectObject(&pen);
	
	if (m_nType == SPS_VERTICAL)
	{
		pDC->MoveTo(m_nX - d, rcWnd.top);
		pDC->LineTo(m_nX - d, rcWnd.bottom);

		pDC->MoveTo(m_nX + d, rcWnd.top);
		pDC->LineTo(m_nX + d, rcWnd.bottom);
	}
	else // m_nType == SPS_HORIZONTAL
	{
		pDC->MoveTo(rcWnd.left, m_nY - d);
		pDC->LineTo(rcWnd.right, m_nY - d);
		
		pDC->MoveTo(rcWnd.left, m_nY + d);
		pDC->LineTo(rcWnd.right, m_nY + d);
	}
	pDC->SetROP2(nRop);
	pDC->SelectObject(pOP);
}

void CSplitter::MoveWindowTo(CPoint pt)
{
	CRect rc;
	GetWindowRect(rc);
	CWnd* pParent;
	pParent = GetParent();
	if (!pParent || !::IsWindow(pParent->m_hWnd))
		return;

	pParent->ScreenToClient(rc);
	if (m_nType == SPS_VERTICAL)
	{	
		int nMidX = (rc.left + rc.right) / 2;
		int dx = pt.x - nMidX;
		rc.OffsetRect(dx, 0);
	}
	else
	{	
		int nMidY = (rc.top + rc.bottom) / 2;
		int dy = pt.y - nMidY;
		rc.OffsetRect(0, dy);
	}
	MoveWindow(rc);
}
BOOL CSplitter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		SetCursor(hCursor);
		return FALSE;
	}
	else
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
void CSplitter::SetRange(int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;
}

// Set splitter range from (nRoot - nSubtraction) to (nRoot + nAddition)
// If (nRoot < 0)
//		nRoot =  <current position of the splitter>
void CSplitter::SetRange(int nSubtraction, int nAddition, int nRoot)
{
	if (nRoot < 0)
	{
		CRect rcWnd;
		GetWindowRect(rcWnd);
		if (m_nType == SPS_VERTICAL)
			nRoot = rcWnd.left + rcWnd.Width() / 2;
		else // if m_nType == SPS_HORIZONTAL
			nRoot = rcWnd.top + rcWnd.Height() / 2;
	}
	m_nMin = nRoot - nSubtraction;
	m_nMax = nRoot + nAddition;
}

const TCHAR sTopLeftX[] = _T("nTopLeftX");
const TCHAR sTopLeftY[] = _T("nTopLeftY");
const TCHAR sUnderScore[] = _T("_");

void CSplitter::SavePlacement()
{
	CWinApp* pApp = AfxGetApp();

	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + m_ArxControl->GetKeyPath(); 

	if (IsWindow(m_hWnd) && m_pLeft != NULL)
	{
		CRect rectCurrent;
		GetWindowRect( &rectCurrent );
		GetParent()->ScreenToClient( &rectCurrent );
		pApp->WriteProfileInt(sProfileName, sTopLeftX, rectCurrent.left);
		pApp->WriteProfileInt(sProfileName, sTopLeftY, rectCurrent.top);
	}
    
}


CPoint CSplitter::ReadPlacement()
{	
	CPoint szRet;
	CWinApp* pApp = AfxGetApp();
	
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + m_ArxControl->GetKeyPath(); 
    
	szRet.x = pApp->GetProfileInt(sProfileName, sTopLeftX, -1);
	szRet.y = pApp->GetProfileInt(sProfileName, sTopLeftY, -1);

	return szRet;
}