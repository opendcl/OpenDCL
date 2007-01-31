// PropertyButton.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyButton.h"
//#include "UserMessageID.h"
#include "PropertyListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyButton

CPropertyButton::CPropertyButton()
{
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      NULL,
      ::GetSysColorBrush(COLOR_BTNFACE),
      NULL);
	
}

CPropertyButton::~CPropertyButton()
{
}


BEGIN_MESSAGE_MAP(CPropertyButton, CWnd)
	//{{AFX_MSG_MAP(CPropertyButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyButton message handlers

BOOL CPropertyButton::Create(const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	m_ButtonImage = nDropDownImage;
	
	return CWnd::Create(m_ClassName, _T("CPropertyButton"), WS_CHILD|WS_CLIPSIBLINGS, rect, pParentWnd, nID, NULL);	
}

void CPropertyButton::OnPaint() 
{
	PAINTSTRUCT ps; 
	try
	{
		CDC* pdc = BeginPaint(&ps);
    
		CWnd *pThisWnd = this;
		CRect rcBounds;
		pThisWnd->GetWindowRect(&rcBounds);
		
		DrawButtonUp(&rcBounds, pdc);
		
		EndPaint(&ps);
	}
	catch(...)
	{
	}
}

void CPropertyButton::Refresh() 
{	
	CDC* pdc = GetDC();
    
	
	CRect rcBounds;
	GetWindowRect(&rcBounds);
	
	DrawButtonUp(&rcBounds, pdc);
	
	pdc->Detach();
	
}

void CPropertyButton::SetButtonStyle(int nStyle)
{
	m_ButtonImage = nStyle;
}
int CPropertyButton::GetButtonStyle()
{
	return m_ButtonImage;
}
void CPropertyButton::DrawOuterRectUp(CRect *rect, CDC *pdc) 
{
	CPen* pOldPen;
	CPen *pPenLite = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNFACE));
	pOldPen = pdc->SelectObject(pPenLite);

	// draw the line
	pdc->MoveTo(rect->Width() - 2, 0);
	pdc->LineTo(0, 0);
	pdc->LineTo(0, rect->Height() - 1);
	
	pdc->SelectObject(pOldPen);			
	pPenLite->DeleteObject();
	delete pPenLite;

	CPen *pPenDark = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));
	pOldPen = pdc->SelectObject(pPenDark);

	// draw the line
	pdc->MoveTo(rect->Width() - 1, 0);
	pdc->LineTo(rect->Width() - 1, rect->Height() - 1);
	pdc->LineTo(nOffWindow, rect->Height() - 1);

	
	pdc->SelectObject(pOldPen);			
	pPenDark->DeleteObject();
	delete pPenDark;

}

void CPropertyButton::DrawInnerRectUp(CRect *rect, CDC *pdc) 
{
	CPen* pOldPen;
	CPen *pPenLite = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
	pOldPen = pdc->SelectObject(pPenLite);

	// draw the line
	pdc->MoveTo(rect->Width() - 2, 1);
	pdc->LineTo(1, 1);
	pdc->LineTo(1, rect->Height() - 2);
	
	pdc->SelectObject(pOldPen);			
	pPenLite->DeleteObject();
	delete pPenLite;

	CPen *pPenDark = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
	pOldPen = pdc->SelectObject(pPenDark);

	// draw the line
	pdc->MoveTo(rect->Width() - 2, 1);
	pdc->LineTo(rect->Width() - 2, rect->Height() - 2);
	pdc->LineTo(0, rect->Height() - 2);
	
	pdc->SelectObject(pOldPen);			
	pPenDark->DeleteObject();
	delete pPenDark;

}

void CPropertyButton::DrawCenterUp(CRect *rect, CDC *pdc) 
{
	CBrush *pCellBrush = new CBrush;
	CRect rcCell(2,2,rect->Width() - 2, rect->Height() - 2);

	pCellBrush->CreateSysColorBrush(COLOR_BTNFACE);
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, pCellBrush);
	// delete the brush
	pCellBrush->DeleteObject();
	delete pCellBrush;

}

void CPropertyButton::DrawButtonUp(CRect *rect, CDC *pdc)
{
	DrawOuterRectUp(rect, pdc);
	DrawInnerRectUp(rect, pdc);
	DrawCenterUp(rect, pdc);
	DrawButtonImage(true, rect, pdc);
}

void CPropertyButton::DrawButtonImage(bool bButtonUp, CRect *rect, CDC *pdc)
{
	int nImageOffset;
	CPen* pOldPen;

	// decide the image offset
	if (bButtonUp)
		nImageOffset = 0;
	else
		nImageOffset = 1;

	CPen *pPenDark = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));
	pOldPen = pdc->SelectObject(pPenDark);

	// draw the correct image
	switch (m_ButtonImage)
	{
		case nDropDownImage:
		{			
			int nStartTop = rect->Height() / 2;
			
			// draw the down arrow
			pdc->MoveTo(5, nStartTop - 2);
			pdc->LineTo(10, nStartTop - 2);
			pdc->MoveTo(6, nStartTop - 1);
			pdc->LineTo(9, nStartTop - 1);
			pdc->MoveTo(7, nStartTop);
			pdc->LineTo(8, nStartTop);			
			
			break;
		}
		case nDotsImage:
		{
	
			// draw the first dot
			pdc->MoveTo(3, rect->Height() - 6);
			pdc->LineTo(5, rect->Height() - 6);
			pdc->MoveTo(3, rect->Height() - 7);
			pdc->LineTo(5, rect->Height() - 7);
			
			// draw the second dot
			pdc->MoveTo(6, rect->Height() - 6);
			pdc->LineTo(8, rect->Height() - 6);
			pdc->MoveTo(6, rect->Height() - 7);
			pdc->LineTo(8, rect->Height() - 7);
			
			// draw the third dot
			pdc->MoveTo(9, rect->Height() - 6);
			pdc->LineTo(11, rect->Height() - 6);
			pdc->MoveTo(9, rect->Height() - 7);
			pdc->LineTo(11, rect->Height() - 7);
			
			break;
		}
	}

	pdc->SelectObject(pOldPen);			
	pPenDark->DeleteObject();
	delete pPenDark;

}

void CPropertyButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	CWnd::OnLButtonDown(nFlags, point);
}


void CPropertyButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);
	((CPropertyListCtrl*)GetParent())->OnButtonPressed();
	
}

BOOL CPropertyButton::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN)
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
		}
	}		
		
	return CWnd::PreTranslateMessage(pMsg);
}
