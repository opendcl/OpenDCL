// ArxAcadSlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxAcadSlideCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAcadSlideCtrl

CArxAcadSlideCtrl::CArxAcadSlideCtrl( CControlPane& Pane, TDclControlPtr pTemplate,
																			UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, &Pane, this )
, mArxServices( this )
, mbTrackingMouse( false )
{
	m_bHasFocus = false;
	m_bSelectedRect = false;
	m_hbmMem = NULL;

	if( bCreate )
		Create( Pane.GetHostDialog(), nID );
}

CArxAcadSlideCtrl::~CArxAcadSlideCtrl()
{
	if(m_hbmMem)
		::DeleteObject(m_hbmMem);
}

bool CArxAcadSlideCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CArxAcadSlideCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	return (dwStyle | BS_OWNERDRAW | BS_NOTIFY);
}

bool CArxAcadSlideCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	mSlideCtrl.SetBackColor( GetRGBColor( pProp->GetLongValue() ) );
	return true;
}


BEGIN_MESSAGE_MAP(CArxAcadSlideCtrl, CButton)
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
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxAcadSlideCtrl message handlers

void CArxAcadSlideCtrl::OnPaint() 
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
			InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventPaint), 0, IsAsyncEvents());
		else
			// call methods to invoke the event
			InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventPaint), 1, IsAsyncEvents());
	}
}

bool CArxAcadSlideCtrl::SetFileName( LPCTSTR pszFilename, LPCTSTR pszSlide )
{	
	if( !mSlideCtrl.Load( theWorkspace.FindFile( pszFilename ), pszSlide ) )
		return false;
	Invalidate();
	return true;
}

void CArxAcadSlideCtrl::Clear()
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

void CArxAcadSlideCtrl::PaintControl(CDC *pdc)
{
	
	CRect rcCell;
	GetClientRect(&rcCell);

	int nThisWidth = rcCell.Width();
	int nThisHeight = rcCell.Height();
	
	CBrush CellBrush;
	CellBrush.CreateSolidBrush(mSlideCtrl.m_BkColor);
		
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

void CArxAcadSlideCtrl::DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, LPCTSTR pszFilename, LPCTSTR pszSlideName)
{
	HDC hdc = ::GetDC(m_hWnd);

	CRect rcCell(nX, nY, nX + nSlideWidth, nY + nSlideHeight);
	CxAcadSlide tSlide;

	tSlide.Load( theWorkspace.FindFile( pszFilename ), pszSlideName);
	//tSlide.Load(sFileName, sLibSldName);

	// draw the slide
	tSlide.Draw(hdc, rcCell);

	tSlide.FreeData();
	tSlide.m_FileName = CString();

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}


void CArxAcadSlideCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	 InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventRightDblClick),
		IsAsyncEvents());
	
	CWnd::OnRButtonDblClk(nFlags, point);
}

void CArxAcadSlideCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventRightClick),
		IsAsyncEvents());	
	

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventRMouse),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	
	CWnd::OnRButtonUp(nFlags, point);
}

void CArxAcadSlideCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar != _T('\n') && nChar != _T('\r') )
	{
		__super::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
	}
	
}

void CArxAcadSlideCtrl::OnClicked() 
{
	CString sEvent = mpTemplate->GetStringProperty(Prop::EventClicked);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		resbuf rbEvent = { NULL, RTSTR };
		rbEvent.resval.rstring = sEvent.LockBuffer();
		resbuf* prbResult = NULL;
		acedInvoke( &rbEvent, &prbResult ); 
		if( prbResult ) 
			acutRelRb( prbResult ); 
		GetParent()->GetParent()->EnableWindow( FALSE );
		GetParent()->EnableWindow( TRUE );
	}
	else
		InvokeMethod( sEvent, IsAsyncEvents() );	
}

void CArxAcadSlideCtrl::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());	
	
	
}

void CArxAcadSlideCtrl::OnDestroy() 
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
	__super::OnDestroy();
}

void CArxAcadSlideCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	
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
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
	
}

void CArxAcadSlideCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);
	m_bHasFocus = false;
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxAcadSlideCtrl::SetHighlight(const COLORREF& rgb)
{
	m_bSelectedRect = true;
	m_HighlightColor = rgb;		
	Invalidate();
}

void CArxAcadSlideCtrl::RemoveHighlight()
{
	m_bSelectedRect = false;
	Invalidate();
}

BOOL CArxAcadSlideCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxAcadSlideCtrl::DrawLine(int sX, int sY, int eX, int eY, const COLORREF& rgb)
{
	HDC hdc = ::GetDC(m_hWnd);
	CPoint point;
	
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

void CArxAcadSlideCtrl::DrawFillRect(int sX, int sY, int eX, int eY, const COLORREF& rgb)
{
	HDC hdc = ::GetDC(m_hWnd);

	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = eY;

	// draw the solid rectangle
	::SetBkColor(hdc, rgb);
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
	

}

void CArxAcadSlideCtrl::CopyDC() 
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

void CArxAcadSlideCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	
	// delete the bitmap is valid
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
		
}

void CArxAcadSlideCtrl::SetDragnDrop(BOOL bRegister)
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



void CArxAcadSlideCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == MK_LBUTTON)
	{
		BeginDragnDrop(mpTemplate, point, IsAsyncEvents());
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());	
		return;
	}
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventLMouse),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnLButtonDown(nFlags, point);
}

void CArxAcadSlideCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventLMouse),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());	

	__super::OnLButtonUp(nFlags, point);
}

void CArxAcadSlideCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMMouse),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
			
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxAcadSlideCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMMouse),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
		
	__super::OnMButtonDown(nFlags, point);
}

void CArxAcadSlideCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMMouse),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	__super::OnMButtonUp(nFlags, point);
}

void CArxAcadSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);

	if( !mbTrackingMouse )
	{
		InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseEntered ), IsAsyncEvents() );
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		if( _TrackMouseEvent( &tm ) )
			mbTrackingMouse = true;
	}

	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

BOOL CArxAcadSlideCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseWheel),
		nFlags,
		zDelta,
		pt.x,
		pt.y,
		IsAsyncEvents());
	
	
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CArxAcadSlideCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventRMouse),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());


	
	__super::OnRButtonDown(nFlags, point);
}

void CArxAcadSlideCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
	
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxAcadSlideCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
	
	
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}


LRESULT CArxAcadSlideCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseMovedOff ), IsAsyncEvents() );
	mbTrackingMouse = false;        
	return FALSE;
}

void CArxAcadSlideCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventLMouse),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	__super::OnLButtonDblClk(nFlags, point);
}

void CArxAcadSlideCtrl::OnSysColorChange() 
{
	__super::OnSysColorChange();
	OnApplyBackgroundColor( mpTemplate->GetPropertyObject( Prop::BackgroundColor ) );
}

void CArxAcadSlideCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
