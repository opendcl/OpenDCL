// 3DRect.cpp : implementation file
//

#include "stdafx.h"
#include "SlideHolder.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "PropertyIds.h"

const int nReturnChar = 13;
const int nHardReturnChar = 10;


DWORD CAcadSlideControlX::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();
	return (dwStyle | BS_OWNERDRAW);
}


/////////////////////////////////////////////////////////////////////////////
// CSlideHolder

CSlideHolder::CSlideHolder( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: mControlX( pTemplate, &Pane, this )
, mpSourceControl( pTemplate )
, mpControlPane( &Pane )
{
	m_bMouseTracking = FALSE;
	m_bHasFocus = false;
	m_bSelectedRect = false;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_hbmMem = NULL;
	Create( Pane.GetHostDialog(), nID );
}

CSlideHolder::~CSlideHolder()
{
}

bool CSlideHolder::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		CButton::Create( NULL,
										 mControlX.GetWndStyle(),
										 mControlX.GetWndRect(),
										 pParentWnd,
										 nID );
	VERIFY(CWnd::SubclassDlgItem(nID, pParentWnd));

	if( bSuccess && !mControlX.ApplyPropertiesEnum() )
		bSuccess = false;

	int nColor = mpSourceControl->GetLngProperty(nAcadColor);
	mSlideCtrl.SetBackColor( GetRGBColor(nColor) );
	SetAcadColor( nColor );

	InitToolTip();
	SetToolTipEx(this, m_ToolTip, mpSourceControl);

	if( mpSourceControl->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CSlideHolder, CButton)
	//{{AFX_MSG_MAP(CSlideHolder)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSlideHolder message handlers

void CSlideHolder::OnPaint() 
{
	if (m_hbmMem != NULL)
	{
		CPaintDC dc( this ); // Device context for painting
		CDC *pdc = &dc;
        CBitmap *poldbmp; 
		CDC memdc;
    
		if (!GetParent()->IsWindowVisible())		
			return;

        // Create a compatible memory DC
        memdc.CreateCompatibleDC( pdc );
        // Select the bitmap into the DC
        poldbmp = memdc.SelectObject( CBitmap::FromHandle(m_hbmMem) );

        // Copy (BitBlt) bitmap from memory DC to screen DC
        pdc->BitBlt( 0, 0,
			GetDeviceCaps(pdc->m_hDC, HORZRES), 
			GetDeviceCaps(pdc->m_hDC, VERTRES),                 
			&memdc, 0, 0, SRCCOPY );

        memdc.SelectObject( poldbmp );
		return;
	}
	//CPaintDC dc(this); // device context for painting
	PAINTSTRUCT ps; 
	
	
	CDC* pdc = BeginPaint(&ps);

	PaintControl(pdc);
	
	EndPaint(&ps);
	
	if (m_hbmMem == NULL)
	{
		CWnd *pFocusWnd = CWnd::GetFocus();
		// if the CWnd that has focus is not this CWnd
		// then send a 0 to indicate this control does not 
		// have focus. Otherwise send a 1 to indicate that
		// it does have focus.
		
		if (!GetParent()->IsWindowVisible())		
			return;

		if (pFocusWnd != this)
			// call methods to invoke the event
			InvokeMethodInt(mpSourceControl->GetStrProperty(nEventPaint), 0, m_bInvokeWithSendString);
		else
			// call methods to invoke the event
			InvokeMethodInt(mpSourceControl->GetStrProperty(nEventPaint), 1, m_bInvokeWithSendString);
	}

	
}

void CSlideHolder::SetAcadColor(long nColor)
{	
	try
	{
		m_BkColor = GetRGBColor(nColor);
			
		CDC *pdc = CWnd::GetDC();

		PaintControl(pdc);

		pdc->Detach();
	}
	catch(...)
	{
	}
}

bool CSlideHolder::SetFileName(CString sFileName, bool slb, CString slbSldName)
{	
	bool bReturn;
	if (slb)
	{		 
		bReturn = mSlideCtrl.Load(sFileName, slb, slbSldName);	
	}
	else
	{
		bReturn = mSlideCtrl.Load(sFileName, false, CString());
	}

	if (bReturn)
		Invalidate();
	
	return bReturn;

}

void CSlideHolder::Clear()
{
	// delete the bitmap is valid
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	m_bSelectedRect = false;
	mSlideCtrl.FreeData();
	mSlideCtrl.m_FileName = CString();

	try
	{
		RedrawWindow();
	}
	catch(...)
	{
	}
}

void CSlideHolder::PaintControl(CDC *pdc)
{
	
	CRect rcCell;
	GetClientRect(&rcCell);

	int nThisWidth = rcCell.Width();
	int nThisHeight = rcCell.Height();
	
	CBrush CellBrush;
	CellBrush.CreateSolidBrush(m_BkColor);
		
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, &CellBrush);
	// delete the brush
	CellBrush.DeleteObject();
	
	if (mSlideCtrl.m_FileName.GetLength() > 0)
	{
		try
		{
			double dFactor;
			double dH;
			double dW;

			dFactor = (double)mSlideCtrl.m_Height / (double)mSlideCtrl.m_Width;
			dH = dFactor;
			dW = 1.0;

			int nDrawWidth = dW * nThisWidth;
			int nDrawHeight = dH * nDrawWidth;
			
			// if the calc height is too large
			if (nDrawHeight > nThisHeight)
			{
				dFactor = (double)mSlideCtrl.m_Width / (double)mSlideCtrl.m_Height;
				dH = 1.0;
				dW = dFactor;

				nDrawHeight = dH * nThisHeight;
				nDrawWidth = dW * nDrawHeight;

				rcCell.left = (nThisWidth - nDrawWidth) / 2;
				rcCell.right = nThisWidth - rcCell.left;
			}
			else if (nDrawHeight < nThisHeight)
			{
				rcCell.top = (nThisHeight - nDrawHeight) / 2;
				rcCell.bottom = nThisHeight - rcCell.top;
			}
			
			// if the calc width is too large
			if (nDrawWidth > nThisWidth)
			{
				dFactor = (double)mSlideCtrl.m_Height / (double)mSlideCtrl.m_Width;
				dH = dFactor;
				dW = 1.0;

				nDrawWidth = dW * nThisWidth;
				nDrawHeight = (double)nDrawWidth * dH;
				
				rcCell.left = 1;
				rcCell.right = nThisWidth;
				rcCell.top = (nThisHeight - nDrawHeight) / 2;
				rcCell.bottom = nThisHeight - rcCell.top;
			}
			// draw the slide
			mSlideCtrl.Draw(pdc, rcCell);
		}
		catch(...)
		{
		}
	}
	
	if (m_bSelectedRect)
	{
		CRect rcCell;
		GetClientRect (&rcCell);
		CSize szValue(1,1);
		rcCell.DeflateRect(szValue);
		
		CPen *pPen = new CPen(PS_SOLID, 1, m_HighlightColor);
		CPen* pOldPen = pdc->SelectObject(pPen);

		pdc->MoveTo(1, 1);
		pdc->LineTo(rcCell.Width(), 1);		
		pdc->LineTo(rcCell.Width(), rcCell.Height());		
		pdc->LineTo(1, rcCell.Height());		
		pdc->LineTo(1, 1);		
		
		pdc->SelectObject(pOldPen);			
		pPen->DeleteObject();
	
	}
	

	if (GetFocus() == this)
		// draw the solid rectangle
		pdc->DrawFocusRect(m_rcFocus);

	
}

void CSlideHolder::DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, CString sFileName)
{
	HDC hdc = ::GetDC(m_hWnd);
	
	CRect rcCell(nX, nY, nX + nSlideWidth, nY + nSlideHeight);
	CxAcadSlide tSlide;

	tSlide.Load(sFileName, false, CString());
	//tSlide.Load(sFileName);

	// draw the slide
	tSlide.Draw(hdc, rcCell);

	tSlide.FreeData();
	tSlide.m_FileName = CString();

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

}
void CSlideHolder::DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, CString sFileName, CString sLibSldName)
{
	HDC hdc = ::GetDC(m_hWnd);

	CRect rcCell(nX, nY, nX + nSlideWidth, nY + nSlideHeight);
	CxAcadSlide tSlide;

	tSlide.Load(sFileName, true, sLibSldName);
	//tSlide.Load(sFileName, sLibSldName);

	// draw the slide
	tSlide.Draw(hdc, rcCell);

	tSlide.FreeData();
	tSlide.m_FileName = CString();

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}
int CSlideHolder::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	return 0;
}


void CSlideHolder::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	 InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethod(
		mpSourceControl->GetStrProperty(nEventRDblClick),
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnRMouseEvent),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CWnd::OnRButtonDblClk(nFlags, point);
}

void CSlideHolder::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethod(
		mpSourceControl->GetStrProperty(nEventRClick),
		m_bInvokeWithSendString);	
	

	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnRMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	
	CWnd::OnRButtonUp(nFlags, point);
}

void CSlideHolder::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != nReturnChar &&
		nChar != nHardReturnChar )
	{
		CButton::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		// call methods to invoke the event
		InvokeMethod(mpSourceControl->GetStrProperty(nEventClicked), m_bInvokeWithSendString);
	}
	
}

void CSlideHolder::OnClicked() 
{
	if (mpSourceControl->m_bEventsAsAction)
	{
		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    

		CString sText = mpSourceControl->GetStrProperty(nEventClicked);
		
		list = acutBuildList(RTSTR, sText, 0);
		if (list != NULL) 
		{ 
			stat = acedInvoke(list, &result); 
			acutRelRb(list); 
			if(result != NULL) 
			{
				acutRelRb(result); 
			}
		}
		GetParent()->GetParent()->EnableWindow(FALSE);
		GetParent()->EnableWindow(TRUE);
	}
	else
	{
		// call methods to invoke the event
		InvokeMethod(mpSourceControl->GetStrProperty(nEventClicked), m_bInvokeWithSendString);	
	}
	
}

void CSlideHolder::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(mpSourceControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);	
	
	
}

void CSlideHolder::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	// delete the bitmap is valid
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	
	m_bSelectedRect = false;
	mSlideCtrl.FreeData();
	mSlideCtrl.m_FileName = CString();


	CButton::OnDestroy();
	
		
}

void CSlideHolder::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	CRect rcThis;
	GetClientRect(&rcThis);
	m_bHasFocus = true;
	m_rcFocus.left = 2;
	m_rcFocus.top = 2;
	m_rcFocus.right = rcThis.Width() - 2,
	m_rcFocus.bottom = rcThis.Height() - 2;

	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	pdc->Detach();
	
	// call methods to invoke the event
	InvokeMethod(mpSourceControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
}

void CSlideHolder::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	mpControlPane->SetGrphcBtnsParents(true);
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	pdc->Detach();	
	m_bHasFocus = false;
	// call methods to invoke the event
	InvokeMethod(mpSourceControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}

void CSlideHolder::SetHighLight(int nColorIndex)
{
	m_bSelectedRect = true;
	m_HighlightColor = GetRGBColor(nColorIndex);		
	Invalidate();
}

void CSlideHolder::RemoveHighLight()
{
	m_bSelectedRect = false;
	Invalidate();
}

void CSlideHolder::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void CSlideHolder::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

BOOL CSlideHolder::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	return CButton::PreTranslateMessage(pMsg);
}

void CSlideHolder::DrawLine(int sX, int sY, int eX, int eY, int nLineColor)
{
	HDC hdc = ::GetDC(m_hWnd);
	CPoint point;
	
	COLORREF rgb = GetRGBColor(nLineColor);
	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(hdc, pen);
	
	MoveToEx(hdc, sX, sY, &point);
	LineTo(hdc, eX, eY);		
	SetPixel(hdc, sX, sY, rgb);
	SetPixel(hdc, eX, eY, rgb);

	SelectObject(hdc, OldPen);			
	DeleteObject(pen);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

}

void CSlideHolder::DrawFillRect(int sX, int sY, int eX, int eY, int nLineColor)
{
	HDC hdc = ::GetDC(m_hWnd);

	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = eY;

	// draw the solid rectangle
	::SetBkColor(hdc, GetRGBColor(nLineColor));
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
	

}

void CSlideHolder::CopyDC() 
{
	if (!GetParent()->IsWindowVisible())		
		return;

	// delete the bitmap is valid
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	
	HDC hDC = ::GetDC(m_hWnd);
	HDC hDCmem = CreateCompatibleDC (hDC) ; 
	int nSourceX = GetDeviceCaps(hDC, HORZRES);
	int nSourceY = GetDeviceCaps(hDC, VERTRES); 
	
	if (!GetParent()->IsWindowVisible())		
	{
		DeleteDC (hDCmem); 
		return;
	}
	

	// Create a compatible bitmap for hdcSource.  
	HBITMAP hbmMem = CreateCompatibleBitmap(hDC, 
				nSourceX, 
				nSourceY); 

	if (!GetParent()->IsWindowVisible())		
	{
		DeleteDC (hDCmem); 		
		return;
	}
	
	HGDIOBJ hbmOld;
	hbmOld = SelectObject (hDCmem, hbmMem) ; 
	
	if (!GetParent()->IsWindowVisible())		
	{
		SelectObject (hDCmem, hbmOld);
		DeleteDC (hDCmem); 
		DeleteObject (hbmOld);
		return;
	}

	BitBlt(hDCmem, 0,0, 
			nSourceX,
			nSourceY,
			hDC,0, 0, SRCCOPY);

	SelectObject (hDCmem, hbmOld);
	DeleteObject (hbmOld);
	DeleteDC (hDCmem); 
	
	// delete the bitmap if valid
	if (m_hbmMem != NULL)
		DeleteObject(m_hbmMem);
	
	m_hbmMem = hbmMem;
	
		
}

void CSlideHolder::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	
	// delete the bitmap is valid
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
		
}

void CSlideHolder::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = mpSourceControl;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}



void CSlideHolder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (mpSourceControl->GetBoolProperty(nDragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		BeginDragnDrop(mpSourceControl, point, m_bInvokeWithSendString);
		// call methods to invoke the event
		InvokeMethod(mpSourceControl->GetStrProperty(nEventClicked), m_bInvokeWithSendString);	
		return;
	}
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnLMouseEvent),
		0,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	CButton::OnLButtonDown(nFlags, point);
}

void CSlideHolder::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnLMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);	

	CButton::OnLButtonUp(nFlags, point);
}

void CSlideHolder::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnMMouseEvent),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
			
	CButton::OnMButtonDblClk(nFlags, point);
}

void CSlideHolder::OnMButtonDown(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnMMouseEvent),
		0,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
		
	CButton::OnMButtonDown(nFlags, point);
}

void CSlideHolder::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnMMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CButton::OnMButtonUp(nFlags, point);
}

void CSlideHolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rcThis;
	GetWindowRect(&rcThis);
	if (point.x < 0 || 
		point.y < 0 ||
		point.x > rcThis.Width() ||
		point.y > rcThis.Height())
	{
		m_bMouseTracking = FALSE;        	
		m_bHasFocus = false;
		InvokeMethod(mpSourceControl->GetStrProperty(nEventMouseMovedOff), m_bInvokeWithSendString);
		CButton::OnMouseMove(nFlags, point);
		return;
	}

	// setup the mouse tracking event reactor
	if (!m_bMouseTracking)       
	{
		TRACKMOUSEEVENT tme;        
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;                
		if (::_TrackMouseEvent(&tme))                
			m_bMouseTracking = TRUE;
		InvokeMethod(
			mpSourceControl->GetStrProperty(nEventMouseEntered),
			m_bInvokeWithSendString);
	}

	InvokeMethodIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	
	CButton::OnMouseMove(nFlags, point);
}

BOOL CSlideHolder::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDblClick),
		nFlags,
		zDelta,
		pt.x,
		pt.y,
		m_bInvokeWithSendString);
	
	
	return CButton::OnMouseWheel(nFlags, zDelta, pt);
}

void CSlideHolder::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnRMouseEvent),
		0,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);


	
	CButton::OnRButtonDown(nFlags, point);
}

void CSlideHolder::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpSourceControl->GetStrProperty(nEventKeyDown), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSlideHolder::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpSourceControl->GetStrProperty(nEventKeyUp), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
	
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}


LRESULT CSlideHolder::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	InvokeMethod(mpSourceControl->GetStrProperty(nEventMouseMovedOff), m_bInvokeWithSendString);
	
	m_bMouseTracking = FALSE;        
	return FALSE;
	
}

void CSlideHolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nEventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	
	InvokeMethodIntIntIntInt(
		mpSourceControl->GetStrProperty(nOnLMouseEvent),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CButton::OnLButtonDblClk(nFlags, point);
}

void CSlideHolder::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	
	m_BkColor = GetRGBColor(mpSourceControl->GetLngProperty(nAcadColor));	
	
	Invalidate();	
}

void CSlideHolder::PostNcDestroy() 
{
	CButton::PostNcDestroy();
	delete this;
}
