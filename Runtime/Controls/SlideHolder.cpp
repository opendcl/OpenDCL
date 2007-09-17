// 3DRect.cpp : implementation file
//

#include "stdafx.h"
#include "SlideHolder.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "PropertyIds.h"

const int nReturnChar = 13;
const int nHardReturnChar = 10;


/////////////////////////////////////////////////////////////////////////////
// CSlideHolder

CSlideHolder::CSlideHolder( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: CArxDialogControl( pTemplate, &Pane, this )
{
	m_bMouseTracking = FALSE;
	m_bHasFocus = false;
	m_bSelectedRect = false;
	m_hbmMem = NULL;
	Create( Pane.GetHostDialog(), nID );
}

CSlideHolder::~CSlideHolder()
{
	if(m_hbmMem)
		::DeleteObject(m_hbmMem);
}

bool CSlideHolder::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (CButton::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	int nColor = mpTemplate->GetLongProperty(Prop::BackgroundColor);
	mSlideCtrl.SetBackColor( GetRGBColor(nColor) );
	SetAcadColor( nColor );

	if( mpTemplate->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

DWORD CSlideHolder::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();
	return (dwStyle | BS_OWNERDRAW);
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
	if (!GetParent()->IsWindowVisible())		
		return;
	CPaintDC dc( this ); // Device context for painting
	if (m_hbmMem != NULL)
	{
		CDC memdc;
		// Create a compatible memory DC
		memdc.CreateCompatibleDC( &dc );
		// Select the bitmap into the DC
		CBitmap *poldbmp = memdc.SelectObject( CBitmap::FromHandle(m_hbmMem) );

		// Copy (BitBlt) bitmap from memory DC to screen DC
		dc.BitBlt( 0, 0,
			GetDeviceCaps(dc.m_hDC, HORZRES), 
			GetDeviceCaps(dc.m_hDC, VERTRES),                 
			&memdc, 0, 0, SRCCOPY );

    memdc.SelectObject( poldbmp );
		return;
	}
	PaintControl(&dc);
	if (m_hbmMem == NULL)
	{
		CWnd *pFocusWnd = CWnd::GetFocus();
		// if the CWnd that has focus is not this CWnd
		// then send a 0 to indicate this control does not 
		// have focus. Otherwise send a 1 to indicate that
		// it does have focus.

		if (pFocusWnd != this)
			// call methods to invoke the event
			InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventPaint), 0, m_bInvokeWithSendString);
		else
			// call methods to invoke the event
			InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventPaint), 1, m_bInvokeWithSendString);
	}
}

void CSlideHolder::SetAcadColor(long nColor)
{	
	m_BkColor = GetRGBColor(nColor);
	Invalidate();
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
		
		CPen Pen(PS_SOLID, 1, m_HighlightColor);
		CPen* pOldPen = pdc->SelectObject(&Pen);

		pdc->MoveTo(1, 1);
		pdc->LineTo(rcCell.Width(), 1);		
		pdc->LineTo(rcCell.Width(), rcCell.Height());		
		pdc->LineTo(1, rcCell.Height());		
		pdc->LineTo(1, 1);		
		
		pdc->SelectObject(pOldPen);
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
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventRDblClick),
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnRMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventRClick),
		m_bInvokeWithSendString);	
	

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnRMouseEvent),
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
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), m_bInvokeWithSendString);
	}
	
}

void CSlideHolder::OnClicked() 
{
	if (mpTemplate->m_bEventsAsAction)
	{
		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    

		CString sText = mpTemplate->GetStringProperty(Prop::EventClicked);
		
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
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), m_bInvokeWithSendString);	
	}
	
}

void CSlideHolder::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), m_bInvokeWithSendString);	
	
	
}

void CSlideHolder::OnDestroy() 
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
	ReleaseDC(pdc);
	
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), m_bInvokeWithSendString);
	
}

void CSlideHolder::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);
	m_bHasFocus = false;
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), m_bInvokeWithSendString);
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

BOOL CSlideHolder::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
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

	// Create a compatible bitmap for hdcSource.  
	HBITMAP hbmMem = CreateCompatibleBitmap(hDC, 
				nSourceX, 
				nSourceY); 
	
	HGDIOBJ hbmOld;
	hbmOld = SelectObject (hDCmem, hbmMem) ; 

	BitBlt(hDCmem, 0,0, 
			nSourceX,
			nSourceY,
			hDC,0, 0, SRCCOPY);

	SelectObject (hDCmem, hbmOld);
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
		m_DropTarget.m_pThisArxControl = mpTemplate;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}



void CSlideHolder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		BeginDragnDrop(mpTemplate, point, m_bInvokeWithSendString);
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), m_bInvokeWithSendString);	
		return;
	}
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnLMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnLMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnMMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnMMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnMMouseEvent),
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
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventMouseMovedOff), m_bInvokeWithSendString);
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
			mpTemplate->GetStringProperty(Prop::EventMouseEntered),
			m_bInvokeWithSendString);
	}

	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	
	CButton::OnMouseMove(nFlags, point);
}

BOOL CSlideHolder::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
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
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnRMouseEvent),
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
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSlideHolder::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
	
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}


LRESULT CSlideHolder::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventMouseMovedOff), m_bInvokeWithSendString);
	
	m_bMouseTracking = FALSE;        
	return FALSE;
	
}

void CSlideHolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnLMouseEvent),
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
	
	m_BkColor = GetRGBColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));	
	
	Invalidate();	
}

void CSlideHolder::PostNcDestroy() 
{
	CButton::PostNcDestroy();
	delete this;
}
