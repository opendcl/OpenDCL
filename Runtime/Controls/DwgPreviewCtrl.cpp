// DwgPreviewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DwgPreviewCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "ToolTips.h"
#include "ErrorLexicon.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "Workspace.h"

const int nReturnChar = 13;
const int nHardReturnChar = 10;

//needed until this control is derived from CDialogObject
#define IsAsyncEvents() (m_ArxControl->GetLongProperty( Prop::EventInvoke ) == 1)


/////////////////////////////////////////////////////////////////////////////
// CDwgPreviewCtrl

CDwgPreviewCtrl::CDwgPreviewCtrl()
: mbrushBackground( RGB(0,0,0) )
{
	m_bSelectedRect = false;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_BackColor = RGB(0,0,0);
}

CDwgPreviewCtrl::~CDwgPreviewCtrl()
{
	mbrushBackground.DeleteObject();
}


BEGIN_MESSAGE_MAP(CDwgPreviewCtrl, CButton)
	//ON_WM_PAINT()
	//{{AFX_MSG_MAP(CDwgPreviewCtrl)
	ON_WM_SIZE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDwgPreviewCtrl message handlers

BOOL CDwgPreviewCtrl::Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID)
					 
{	
	BOOL RetVal;
	CRect ArxRect;
	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_GROUP|BS_OWNERDRAW;

	if (pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CButton::Create(
		_T(""),
		dwStyle,
		ArxRect,
		pParentWnd,
		nID);
	
	int nColor = m_ArxControl->GetLongProperty(Prop::BackgroundColor);
	COLORREF rgb = GetRGBColor(nColor);
	
	m_BkColor = rgb;
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	return RetVal;
}

void CDwgPreviewCtrl::LoadDwg(CString sFileName)
{
	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
	{
		theWorkspace.DisplayAlert(ErrorFileNotFound);
		return;
	}	
	m_filename = sPath;
	RedrawWindow();
}

void CDwgPreviewCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(m_BackColor);
	PaintCtrl(pDC);
}

/*
void CDwgPreviewCtrl::OnPaint() 
{
	PAINTSTRUCT ps; 
	CDC* pdc = BeginPaint(&ps);

	PaintCtrl(pdc);
	
	EndPaint(&ps);

}*/


void CDwgPreviewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CWnd::OnMouseMove(nFlags, point);
}

void CDwgPreviewCtrl::PaintCtrl(CDC *pdc) 
{
	CRect rcCell;
	CWnd::GetClientRect(&rcCell);
	
	CBrush CellBrush;
	CellBrush.CreateSolidBrush(m_BackColor);
			
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, &CellBrush);
	// delete the brush
	CellBrush.DeleteObject();		
	

	if(m_filename != CString())
	{
		TCHAR fileName[512];
		_tcscpy(fileName, m_filename);
		
		// static frame works the best for this function
		acdbDisplayPreviewFromDwg(fileName, (void*)m_hWnd, &m_BackColor);
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
		Pen.DeleteObject();
	}

	if (GetFocus() == this)
		// draw the solid rectangle
		pdc->DrawFocusRect(m_rcFocus);

}

void CDwgPreviewCtrl::SetHighLight(int nColorIndex)
{
	m_bSelectedRect = true;
	m_HighlightColor = GetRGBColor(nColorIndex);		
	Invalidate();
}

void CDwgPreviewCtrl::RemoveHighLight()
{
	m_bSelectedRect = false;
	Invalidate();
}


void CDwgPreviewCtrl::SetAcadColor(long nColor)
{
	m_BackColor = GetRGBColor(nColor);
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(m_BackColor);
}

void CDwgPreviewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	if(m_filename != "")
	{
		SetWindowText(m_filename);
		SetWindowText(_T(""));
		TCHAR fileName[512];
		_tcscpy(fileName, m_filename);
		ULONG Color = RGB(0,0,0);
		// static frame works the best for this function
		acdbDisplayPreviewFromDwg(fileName, (void*)m_hWnd, &Color);
	}
	else
	{
		CDC *pdc = GetDC();
		Refresh(pdc);
		pdc->DeleteDC();
	}	
}

void CDwgPreviewCtrl::Refresh(CDC *pdc)
{
	CRect rcThis;
	CWnd::GetClientRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
	CBrush CellBrush(COLOR_BTNFACE);
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, &CellBrush);
	// delete the brush
	CellBrush.DeleteObject();
}


void CDwgPreviewCtrl::OnClicked() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
	
}

void CDwgPreviewCtrl::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());	
}	

void CDwgPreviewCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != nReturnChar &&
		nChar != nHardReturnChar )
	{
		CButton::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
	}
	
}

void CDwgPreviewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CRect rcThis;
	GetClientRect(&rcThis);

	m_rcFocus.left = 2;
	m_rcFocus.top = 2;
	m_rcFocus.right = rcThis.Width() - 2,
	m_rcFocus.bottom = rcThis.Height() - 2;

	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
	
}

void CDwgPreviewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);

	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
	
}

BOOL CDwgPreviewCtrl::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CButton::PreTranslateMessage(pMsg);
}

void CDwgPreviewCtrl::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CButton::OnDestroy();
}

void CDwgPreviewCtrl::SetDragnDrop(BOOL bRegister)
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

void CDwgPreviewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (m_ArxControl->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == MK_LBUTTON)
		BeginDragnDrop(m_ArxControl, point, IsAsyncEvents());
		
	CButton::OnLButtonDown(nFlags, point);
}

HBRUSH CDwgPreviewCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return (HBRUSH)::GetStockObject(NULL_BRUSH); 
}

HBRUSH CDwgPreviewCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return NULL;
	pDC->SetBkColor(m_BackColor);	
	return mbrushBackground;
}

