// PictureBox.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PictureBox.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "Project.h"
#include "PictureObject.h"
#include "ToolTips.h"

#define HIMETRIC_INCH	2540

const int nClientBorderSize = 4;
const int nStaticBorderSize = 2;

const int nReturnChar = 13;
const int nHardReturnChar = 10;
const int nLightGrey = 192;

const TCHAR sTL[] = _T("TL");
const TCHAR sTC[] = _T("TC");
const TCHAR sTR[] = _T("TR");
const TCHAR sBL[] = _T("BL");
const TCHAR sBC[] = _T("BC");
const TCHAR sBR[] = _T("BR");

const TCHAR sL[] = _T("L");
const TCHAR sR[] = _T("R");
const TCHAR sC[] = _T("C");

//needed until this control is derived from CDialogObject
#define IsAsyncEvents() (m_ArxControl->GetLongProperty( Prop::EventInvoke ) == 1)


CRect CalcFitRect(int nPicWidth, int nPicHeight, int nCtrlWidth, int nCtrlHeight)
{
	CRect rcCell(0, 0, nCtrlWidth, nCtrlHeight);
	double dFactor;
	double dH;
	double dW;

	dFactor = (double)nPicHeight / (double)nPicWidth;
	dH = dFactor;
	dW = 1.0;

	int nDrawWidth = int(dW * nCtrlWidth);
	int nDrawHeight = int(dH * nDrawWidth);
	
	// if the calc height is too large
	if (nDrawHeight > nCtrlHeight)
	{
		dFactor = (double)nPicWidth / (double)nPicHeight;
		dH = 1.0;
		dW = dFactor;

		nDrawHeight = int(dH * nCtrlHeight);
		nDrawWidth = int(dW * nDrawHeight);

		rcCell.left = (nCtrlWidth - nDrawWidth) / 2;
		rcCell.right = nCtrlWidth - rcCell.left;
	}
	else if (nDrawHeight < nCtrlHeight)
	{
		rcCell.top = (nCtrlHeight - nDrawHeight) / 2;
		rcCell.bottom = nCtrlHeight - rcCell.top;
	}
	
	// if the calc width is too large
	if (nDrawWidth > nCtrlWidth)
	{
		dFactor = (double)nPicHeight / (double)nPicWidth;
		dH = dFactor;
		dW = 1.0;

		nDrawWidth = int(dW * nCtrlWidth);
		nDrawHeight = int((double)nDrawWidth * dH);
		
		rcCell.left = 1;
		rcCell.right = nCtrlWidth;
		rcCell.top = (nCtrlHeight - nDrawHeight) / 2;
		rcCell.bottom = nCtrlHeight - rcCell.top;
	}

	return rcCell;
}


/////////////////////////////////////////////////////////////////////////////
// CPictureBox

CPictureBox::CPictureBox()
{
	m_bMouseTracking = FALSE;
	m_PictureID = 0;
	m_pPictureHolder = NULL;	
	
	m_bHasFocus = false;
	m_pPicture = NULL;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_hbmMem = NULL;
	m_AutoSize = false;
	m_BkColor = 0;
	m_bLoadPicture = false;
	m_bStretchLoadedPicture = false;
	
	
}

CPictureBox::~CPictureBox()
{
	if (m_pPictureHolder != NULL)
	{
		m_pPictureHolder->Release();
		m_pPictureHolder = NULL;
		//delete m_pPictureHolder;
	}
	
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
}


BEGIN_MESSAGE_MAP(CPictureBox, CButton)
	//{{AFX_MSG_MAP(CPictureBox)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ENABLE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureBox message handlers

BOOL CPictureBox::Create(TDclControlPtr pControl, TProjectPtr pProject, CWnd* pParentWnd, UINT nID) 
{
	BOOL bReturn;
	m_bMouseTracking = FALSE;        
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	CRect ArxRect;
	// set the project pointer so this control can load pictures from it
	m_pProject = pProject;
	// set the arx control pointer
    m_ArxControl = pControl;
	
	// set the default size
	m_cxIcon = 16;
	m_cyIcon = 16;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	// set the back color
	m_BkColor = GetRGBColor(m_ArxControl->GetLongProperty(Prop::BackgroundColor));
	m_brBackground.CreateSolidBrush(m_BkColor);

	if (pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	bReturn = CButton::Create(
		CString(),
		dwStyle | BS_OWNERDRAW,
		ArxRect,
		pParentWnd, 
		nID);

	SetAllProperties();
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	return bReturn;
}

void CPictureBox::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	
	m_BkColor = GetRGBColor(m_ArxControl->GetLongProperty(Prop::BackgroundColor));	
	
	Invalidate();
	
}

void CPictureBox::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	
	Invalidate();
}

void CPictureBox::SetPictureID(int nPictureID)
{
	m_PictureID = nPictureID;

	if (nPictureID == 0 && m_pPicture != NULL && m_bLoadPicture)
	{
		m_pPicture = NULL;
		delete m_pPicture;
	}
	if (m_pPictureHolder)
	{
		m_pPictureHolder->Release();
		m_pPictureHolder = NULL;
	}
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}

	m_pPicture = m_pProject->FindPicture( nPictureID );
	if( m_pPicture )
	{
		m_cxIcon = m_pPicture->GetWidth();
		m_cyIcon = m_pPicture->GetHeight();
		AutoSize();
	}
	else
		SetPictureBlank();

	CDC *pdc = GetDC();
	Refresh(pdc);
	ReleaseDC(pdc);
}

void CPictureBox::AutoSize()
{
	BOOL bAutoSize = m_ArxControl->GetBooleanProperty(Prop::AutoSize);
	if (!bAutoSize)
		return;
	
	if (m_pPictureHolder == NULL && m_PictureID == 0)
		return;

	CRect rcThis;
	rcThis.left = m_ArxControl->GetPropertyObject(Prop::Left)->GetLongValue();
	rcThis.top = m_ArxControl->GetPropertyObject(Prop::Top)->GetLongValue();
	
	switch (m_ArxControl->GetLongProperty(Prop::BorderStyle))
	{		
	case 0:
		{
		rcThis.right = rcThis.left + m_cxIcon;
		rcThis.bottom = rcThis.top + m_cyIcon;
		break;
		}
	case 1:
		{
		rcThis.right = rcThis.left + m_cxIcon + nClientBorderSize;
		rcThis.bottom = rcThis.top + m_cyIcon + nClientBorderSize;
		break;
		}
	case 2:
		{
		rcThis.right = rcThis.left + m_cxIcon + nStaticBorderSize;
		rcThis.bottom = rcThis.top + m_cyIcon + nStaticBorderSize;
		break;
		}		
	}
	
	CRect rcControl;
	GetWindowRect(&rcControl);
	// if the control's size is not the same, resize it.
	if (rcControl.Width() != rcThis.Width() && rcControl.Height() != rcThis.Height())
		MoveWindow(rcThis, TRUE);
}

void CPictureBox::Clear()
{
	m_pPicture = NULL;
	m_PictureID = 0;
	m_ArxControl->SetLongProperty(Prop::Picture, m_PictureID);
	// set the picture ID to a value that indicates it's blank
	
	if (m_pPictureHolder)
	{
		m_pPictureHolder->Release();
		m_pPictureHolder = NULL;
		//delete m_pPictureHolder;
	}

	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
		
	}
	

	SetPictureBlank();
	
	//RedrawWindow();
	
	CRect rcCell;	
	GetClientRect(&rcCell);

	HDC hdc = ::GetDC(m_hWnd);

	HBRUSH hBrush = ::CreateSolidBrush(m_BkColor);

	// draw the Window background for the cell				
	FillRect(hdc, rcCell, hBrush);	
	// delete the brush
	DeleteObject(hBrush);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}

void CPictureBox::Refresh()
{
	CDC *pdc = GetDC();
	CPictureBox::Refresh(pdc);
	ReleaseDC(pdc);
}

void CPictureBox::Refresh(CDC *pdc)
{	
	AutoSize();

	CRect rcCell;	
	GetClientRect(&rcCell);
	
	
	CBrush CellBrush(m_BkColor);
	
	
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, &CellBrush);
	// delete the brush
	CellBrush.DeleteObject();
		
	if (m_pPicture)
	{
		CSize IconSize(m_pPicture->GetWidth(), m_pPicture->GetHeight());

		int nPicWidth = m_cxIcon;
		int nPicHeight = m_cyIcon;

		// Center the icon horizontally
		int nPicLeft = ((rcCell.Width() - nPicWidth)/2);
		
		// Center the icon vertically
		int nPicTop = ((rcCell.Height() - nPicHeight)/2);           
		if (m_AutoSize == TRUE)
		{
			nPicTop = 0;
			nPicLeft = 0;
		}

		CRect rcPic(
			nPicLeft,
			nPicTop,
			nPicLeft + nPicWidth,
			nPicTop + nPicHeight
			);
		if (PICTYPE_BITMAP == m_pPicture->GetPicType())
		{			
			if (Prop::Enabled)
			{
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								m_pPicture->GetBitmap(), 
								DSS_NORMAL, 
								NULL);				
			}
			else
			{			
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								m_pPicture->GetBitmap(), 
								DSS_DISABLED, 
								NULL);				
			}
		}
		// else if picture is an icon
		else if (PICTYPE_ICON == m_pPicture->GetPicType())
		{
			if (Prop::Enabled)
			{
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								m_pPicture->CloneIcon(), 
								DSS_NORMAL, 
								(CBrush*)NULL);
			}
			else
			{				
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								m_pPicture->CloneIcon(), 
								DSS_DISABLED, 
								(CBrush*)NULL);
			}
		}
		else
		{
			// get width and height of picture
			long lWidth = m_pPicture->GetWidth();
			long lHeight = m_pPicture->GetHeight();
			
			m_cxIcon = lWidth;
			m_cyIcon = lHeight;

			// Center the picture horizontally
			int nPicLeft = ((rcCell.Width() - lWidth)/2);
			
			// Center the picture vertically
			int nPicTop = ((rcCell.Height() - lHeight)/2);           
			if (m_AutoSize == TRUE)
			{
				nPicTop = 0;
				nPicLeft = 0;
			}

			if (m_pPicture->GetPicType() == PICTYPE_METAFILE ||
				m_pPicture->GetPicType() == PICTYPE_ENHMETAFILE)
			{
				rcCell = CalcFitRect(lWidth, lHeight,rcCell.Width(), rcCell.Height());

				// display picture using IPicture::Render
				m_pPicture->Render(pdc, rcCell.left, rcCell.top, rcCell);				
			}
			else if (lWidth > rcCell.Width() || lHeight > rcCell.Height() || m_bStretchLoadedPicture)
			{
				// display picture using IPicture::Render
				m_pPicture->Render(pdc, 0, 0, rcCell);
			}		
			else
			{
				// display picture using IPicture::Render
				m_pPicture->Render(pdc, nPicLeft, nPicTop, rcCell);
			}
		}
	}

	CRect rc;
	rc = rcCell;

	// if the picture holder object has been set
	// draw the picture holder object	
	if (m_pPictureHolder != NULL)
	{
		// get width and height of picture
		long hmWidth;
		long hmHeight;
		m_pPictureHolder->get_Width(&hmWidth);
		m_pPictureHolder->get_Height(&hmHeight);
		
		// convert himetric to pixels
		int nPicWidth	= MulDiv(hmWidth, pdc->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
		int nPicHeight	= MulDiv(hmHeight, pdc->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);		
		
		m_cxIcon = nPicWidth;
		m_cyIcon = nPicHeight;
		AutoSize();

		// Center the picture horizontally
		int nPicLeft = ((rc.Width() - nPicWidth)/2);
		
		// Center the picture vertically
		int nPicTop = ((rc.Height() - nPicHeight)/2);           
		if (m_ArxControl->GetBooleanProperty(Prop::AutoSize))
		{
			nPicTop = 0;
			nPicLeft = 0;
		}

		SHORT nPicType;
		m_pPictureHolder->get_Type(&nPicType);
	
		if (nPicType == PICTYPE_METAFILE ||
			nPicType == PICTYPE_ENHMETAFILE)
		{
			rcCell = CalcFitRect(nPicWidth, nPicHeight,rcCell.Width(), rcCell.Height());

			// display picture using IPicture::Render
			m_pPictureHolder->Render(pdc->m_hDC, rcCell.left, rcCell.top, rcCell.Width(), rcCell.Height(), 0, hmHeight, hmWidth, -hmHeight, &rc);
				
			// display picture using IPicture::Render
			//m_pPictureHolder->Render(pdc->m_hDC, 0, 0, rcThis.Width(), rcThis.Height(), 0, hmHeight, hmWidth, -hmHeight, &rc);
		}
		else if (nPicWidth <= rc.Width() && nPicHeight <= rc.Height() && !m_bStretchLoadedPicture)
		{
			// display picture using IPicture::Render
			m_pPictureHolder->Render(pdc->m_hDC, nPicLeft, nPicTop, nPicWidth, nPicHeight, 0, hmHeight, hmWidth, -hmHeight, &rc);
		}
		else if (nPicWidth > rc.Width() || nPicHeight > rc.Height() || m_bStretchLoadedPicture)
		{
			double dFactor;
			double dH;
			double dW;
			rcCell = rc;

			dFactor = (double)nPicHeight / (double)nPicWidth;
			dH = dFactor;
			dW = 1.0;

			int nDrawWidth = dW * rc.Width();
			int nDrawHeight = dH * nDrawWidth;
			
			// if the calc height is too large
			if (nDrawHeight > rc.Height())
			{
				dFactor = (double)nPicWidth / (double)nPicHeight;
				dH = 1.0;
				dW = dFactor;

				nDrawHeight = dH * rc.Height();
				nDrawWidth = dW * nDrawHeight;

				rcCell.left = (rc.Width() - nDrawWidth) / 2;
				rcCell.right = rc.Width() - rcCell.left;
			}
			else if (nDrawHeight < rc.Height())
			{
				rcCell.top = (rc.Height() - nDrawHeight) / 2;
				rcCell.bottom = rc.Height() - rcCell.top;
			}
			
			// if the calc width is too large
			if (nDrawWidth > rc.Width())
			{
				dFactor = (double)nPicHeight / (double)nPicWidth;
				dH = dFactor;
				dW = 1.0;

				nDrawWidth = dW * rc.Width();
				nDrawHeight = (double)nDrawWidth * dH;
				
				rcCell.left = 1;
				rcCell.right = rc.Width();
				rcCell.top = (rc.Height() - nDrawHeight) / 2;
				rcCell.bottom = rc.Height() - rcCell.top;
			}
			m_pPictureHolder->Render(pdc->m_hDC, rcCell.left, rcCell.top, rcCell.Width(), rcCell.Height(), 0, hmHeight, hmWidth, -hmHeight, &rc);
		}
		else
		{
			// display picture using IPicture::Render
			m_pPictureHolder->Render(pdc->m_hDC, nPicLeft, nPicTop, nPicWidth, nPicHeight, 0, hmHeight, hmWidth, -hmHeight, &rc);
		}
	}
	
}


int CPictureBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetPictureBlank();
	return 0;
}

void CPictureBox::SetPictureBlank() 
{
	m_pPicture = NULL;
}

void CPictureBox::SetAllProperties()
{
	SetProperty(Prop::Picture);
}

void CPictureBox::SetProperty(int nID)
{
	switch(nID)
	{
		case Prop::Picture:
		{
			SetPictureID(m_ArxControl->GetLongProperty(Prop::Picture));
		}
	}
}

void CPictureBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (m_ArxControl->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		BeginDragnDrop(m_ArxControl, point, IsAsyncEvents());
	}

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnLMouseEvent),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CPictureBox::SetAcadColor(long nColor)
{
	m_BkColor = GetRGBColor(nColor);

	//RedrawWindow();
	CDC *pdc = GetDC();
	CPictureBox::Refresh(pdc);
	ReleaseDC(pdc);
}

void CPictureBox::DrawLine(int sX, int sY, int eX, int eY, COLORREF rgb)
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

void CPictureBox::DrawPoint(int nX, int nY, COLORREF rgb)
{
	HDC hdc = ::GetDC(m_hWnd);
	
	SetPixel(hdc, nX, nY, rgb);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}

void CPictureBox::DrawArc(int sX, int sY, int eX, int eY, int saX, int saY, int eaX, int eaY, COLORREF rgb)
{
	HDC hdc = ::GetDC(m_hWnd);
	CPoint point;
	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(hdc, pen);
	
	MoveToEx(hdc, sX, sY, &point);
	Arc(hdc, sX, sY, eX, eY, saX, saY, eaX, eaY);		

	SelectObject(hdc, OldPen);			
	DeleteObject(pen);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
	
}
void CPictureBox::DrawCircle(int sX, int sY, int eX, int eY, COLORREF rgb)
{
	HDC hdc = ::GetDC(m_hWnd);
	CPoint point;
	int cY = eY - (sY / 2);
	
	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(hdc, pen);
	
	MoveToEx(hdc, sX, sY, &point);
	Arc(hdc, sX, sY, eX, eY, sX, cY, eX, cY);
	Arc(hdc, sX, sY, eX, eY, eX, cY, sX, cY);

	SelectObject(hdc, OldPen);			
	DeleteObject(pen);
		
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}



void CPictureBox::DrawText(int sX, int sY, int nForeColor, int nBackColor, CString sText, CString sJustification, int nEnabled)
{
	
	HDC hdc = ::GetDC(m_hWnd);

	// setup the font			
	HGDIOBJ pOldFont = SelectObject(hdc, GetFont()->m_hObject);

	CPoint ptText;		
	CSize szExtent;
	::GetTextExtentPoint32(hdc, sText, sText.GetLength(), &szExtent);
	

	DWORD dwJustification;
	sJustification.MakeUpper();
	if (sJustification == sTL)
	{
		dwJustification = TA_TOP|TA_LEFT;
		ptText = CPoint(sX, sY);
	}
	else if (sJustification == sTC)
	{
		dwJustification = TA_TOP|TA_CENTER;		
		ptText = CPoint(sX - (szExtent.cx / 2), sY);
	}
	else if (sJustification == sTR)
	{
		dwJustification = TA_TOP|TA_RIGHT;
		ptText = CPoint(sX - szExtent.cx, sY);
	}
	else if (sJustification == sBL)
	{
		dwJustification = TA_BASELINE|TA_LEFT;
		ptText = CPoint(sX, sY - szExtent.cy);
	}
	else if (sJustification == sBC)
	{
		dwJustification = TA_BASELINE|TA_CENTER;
		ptText = CPoint(sX - (szExtent.cx / 2), sY - szExtent.cy);
	}
	else if (sJustification == sBR)
	{
		dwJustification = TA_BASELINE|TA_RIGHT;
		ptText = CPoint(sX - szExtent.cx, sY - szExtent.cy);
	}

	// set the text aligment
	SetTextAlign(hdc, dwJustification);
	
	// setup the color for the text
	SetTextColor(hdc, GetRGBColor(nForeColor));

	// check if we are to use the preset background color
	if (nBackColor == nUseBackColor)
		SetBkColor(hdc, m_BkColor);	
	else // or else use the one the user has assigned for this opertation
		SetBkColor(hdc, GetRGBColor(nBackColor));	

	if (nBackColor < -26 || nBackColor == 0)
		SetBkMode(hdc, TRANSPARENT);
	else
		SetBkMode(hdc, OPAQUE);
		
	if (nEnabled == 0)
	{
		// draw the enabled text
		TextOut(hdc, sX, sY, sText, sText.GetLength());
		
	}
	else
	{
		szExtent.cy = szExtent.cy + (szExtent.cy / 3);
		LPCTSTR lpszText;
		lpszText =sText;
		::DrawState(hdc, 
			(HBRUSH)NULL,
			NULL,
			(LPARAM)lpszText, 
			(WPARAM)sText.GetLength(),
			ptText.x, ptText.y, 
			szExtent.cx, szExtent.cy,
			DSS_DISABLED|(TRUE ? DST_PREFIXTEXT : DST_TEXT)); 
	}
	
	// restore the old font ** a must
	SelectObject(hdc, pOldFont);
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}


int CPictureBox::DrawWrappedText(int sX, int sY, int eX, int nForeColor, int nBackColor, CString sText, CString sJustification)
{
	HDC hdc = ::GetDC(m_hWnd);
	
	CRect rcThis;
	GetWindowRect(&rcThis);
	
	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = rcThis.Height();
	
	// setup the font			
	HGDIOBJ pOldFont = SelectObject(hdc, GetFont()->m_hObject);

	DWORD dwJustification;
	sJustification.MakeUpper();
	if (sJustification == sL)
	{
		dwJustification = DT_TOP|DT_WORDBREAK|DT_LEFT;
	}
	else if (sJustification == sC)
	{
		dwJustification = DT_TOP|DT_WORDBREAK|DT_CENTER;
	}
	else if (sJustification == sR)
	{
		dwJustification = DT_TOP|DT_WORDBREAK|DT_RIGHT;
	}
	

	// setup the color for the text
	SetTextColor(hdc, GetRGBColor(nForeColor));

	// check if we are to use the preset background color
	if (nBackColor == nUseBackColor)
		SetBkColor(hdc, m_BkColor);	
	else // or else use the one the user has assigned for this opertation
		SetBkColor(hdc, GetRGBColor(nBackColor));	

	if (nBackColor < -26 || nBackColor == 0)
		SetBkMode(hdc, TRANSPARENT);
	else
		SetBkMode(hdc, OPAQUE);
	
	// draw the wrappable text
	LPCTSTR lpszString;
	lpszString = sText;
	int nTextHeight = ::DrawText(hdc, lpszString, sText.GetLength(), &rcCell, dwJustification);
	
	// restore the old font ** a must
	SelectObject(hdc, pOldFont);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	return nTextHeight;
}

void CPictureBox::GetTextExtent(CString sText)
{
	HDC hdc = ::GetDC(m_hWnd);

	CSize szText;
	::GetTextExtentPoint32(hdc, sText, sText.GetLength(), &szText);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, szText.cx,
		RTSHORT, szText.cy,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
	
}



void CPictureBox::DrawFillRect(int sX, int sY, int eX, int eY, COLORREF rgb)
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

void CPictureBox::DrawEdge(int sX, int sY, int eX, int eY, int nEdge)
{
	HDC hdc = ::GetDC(m_hWnd);

	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = eY;

	UINT unEdge = 1;
	switch (nEdge)
	{
	case 1:
		unEdge = BDR_RAISEDOUTER;
		break;
	case 2:
		unEdge = BDR_SUNKENOUTER;
		break;
	case 3:
		unEdge = BDR_RAISEDINNER;
		break;
	case 4:
		unEdge = BDR_SUNKENINNER;
		break;
	case 5:
		unEdge = EDGE_RAISED;
		break;
	case 6:
		unEdge = EDGE_SUNKEN;
		break;
	case 7:
		unEdge = EDGE_ETCHED;
		break;
	case 8:
		unEdge = EDGE_BUMP;
		break;
	}
	// draw the solid rectangle
	::DrawEdge(hdc, &rcCell, unEdge, BF_RECT);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}

void CPictureBox::DrawFocusRect(int sX, int sY, int eX, int eY)
{
	HDC hdc = ::GetDC(m_hWnd);

	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = eY;

	// draw the solid rectangle
	::DrawFocusRect(hdc, &rcCell);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}

void CPictureBox::DrawRect(int sX, int sY, int eX, int eY, COLORREF rgb)
{
	HDC hdc = ::GetDC(m_hWnd);

	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = eY;

	CPoint point;
	
	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(hdc, pen);
		
	::Rectangle(hdc, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom);

	SelectObject(hdc, OldPen);			
	DeleteObject(pen);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}

void CPictureBox::DrawHatchRect(int sX, int sY, int eX, int eY, COLORREF rgb, int nHatchPattern)
{
	HDC hdc = ::GetDC(m_hWnd);

	// setup the CRect for FillRect
	CRect rcCell;
	rcCell.left = sX;
	rcCell.top = sY;
	rcCell.right = eX;
	rcCell.bottom = eY;

	HBRUSH hBrush = ::CreateHatchBrush(nHatchPattern, rgb);

	// draw the Window background for the cell				
	FillRect(hdc, &rcCell, hBrush);
	
	// delete the brush
	DeleteObject(hBrush);
		
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
}


void CPictureBox::PaintPicture(int sX, int sY, int nPictureID, int nEnabled, int nUseMask)
{

	CRect iconRect;

	CPictureObject* pPicture = m_pProject->FindPicture( nPictureID );
	if( !pPicture )
		return;

	CSize IconSize( pPicture->GetWidth(), pPicture->GetHeight() );
	HDC hdc = ::GetDC(m_hWnd);

	iconRect.left = sX; // Center the icon horizontally
	iconRect.top = sY; // Center the icon vertically

	if (PICTYPE_BITMAP == pPicture->GetPicType())
	{
		if (nUseMask)
		{
			HICON hIcon = pPicture->CloneIcon();

			if (nEnabled)
			{
				DrawState(hdc, NULL, NULL,
					(LPARAM)hIcon, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_NORMAL|DST_ICON);
			}
			else
			{					
				DrawState(hdc, NULL, NULL,
					(LPARAM)hIcon, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_DISABLED|DST_ICON);
			}				
		}
		else
		{
			if (nEnabled)
			{
				::DrawState(hdc, NULL,
					NULL, (LPARAM)pPicture->CloneBitmap(), 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_NORMAL|DST_BITMAP);
			}
			else
			{
				
				::DrawState(hdc, NULL,
					NULL, (LPARAM)pPicture->CloneBitmap(), 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_DISABLED|DST_BITMAP);
			}
		}
	}

	// else if picture is an icon
	else if (PICTYPE_ICON == pPicture->GetPicType())
	{
		HICON hIconOut = pPicture->CloneIcon();
		
		if (nEnabled)
		{
			DrawState(hdc, NULL, NULL,
					(LPARAM)hIconOut, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_NORMAL|DST_ICON);
		}
		else
		{				
			DrawState(hdc, NULL, NULL,
					(LPARAM)hIconOut, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_DISABLED|DST_ICON);
		}
	
	}

	else
	{
		// get width and height of picture
		long lWidth = pPicture->GetWidth();
		long lHeight = pPicture->GetHeight();
		
		iconRect.right = iconRect.left + lWidth;
		iconRect.bottom = iconRect.top + lHeight;

		CRect rcThis;
		GetClientRect(&rcThis);
			
		if (pPicture->GetPicType() == PICTYPE_METAFILE ||
			pPicture->GetPicType() == PICTYPE_ENHMETAFILE)
		{
			
			CRect rcCell = CalcFitRect(lWidth, lHeight,rcThis.Width(), rcThis.Height());

			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), rcCell.left, rcCell.top, rcThis);				
		}
		else if (lWidth > rcThis.Width() || lHeight > rcThis.Height())
		{
			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), 0, 0, rcThis);
		}
	
		else
		{
			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), sX, sY, iconRect);
		}		
	}

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

} // End of DrawTheIcon


void CPictureBox::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	m_bHasFocus = false;
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CPictureBox::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	m_bHasFocus = true;
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CPictureBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnLMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	
	CWnd::OnLButtonUp(nFlags, point);
}


void CPictureBox::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnMMouseEvent),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
		
	CWnd::OnMButtonDblClk(nFlags, point);
}

void CPictureBox::OnMButtonDown(UINT nFlags, CPoint point) 
{
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnMMouseEvent),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	CWnd::OnMButtonDown(nFlags, point);
}

void CPictureBox::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnMMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CWnd::OnMButtonUp(nFlags, point);
}

void CPictureBox::OnMouseMove(UINT nFlags, CPoint point) 
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
		SetFocus();
		InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventMouseMovedOff), IsAsyncEvents());
		CWnd::OnMouseMove(nFlags, point);
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
			m_ArxControl->GetStringProperty(Prop::EventMouseEntered),
			IsAsyncEvents());
	}

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CWnd::OnMouseMove(nFlags, point);
}

/*

void CPictureBox::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CWnd::OnMouseMove(nFlags, point);
}

*/
BOOL CPictureBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDblClick),
		nFlags,
		zDelta,
		pt.x,
		pt.y,
		IsAsyncEvents());
	
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CPictureBox::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethod(
		m_ArxControl->GetStringProperty(Prop::EventRDblClick),
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnRMouseEvent),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CWnd::OnRButtonDblClk(nFlags, point);
}

void CPictureBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnRMouseEvent),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	CWnd::OnRButtonDown(nFlags, point);
}

void CPictureBox::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethod(
		m_ArxControl->GetStringProperty(Prop::EventRClick),
		IsAsyncEvents());	
	

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnRMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CPictureBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(m_ArxControl->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
		
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPictureBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(m_ArxControl->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
		
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CPictureBox::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventMouseMovedOff), IsAsyncEvents());
	
	m_bMouseTracking = FALSE;        
	return FALSE;
}


// This function loads a file into an IStream.
void CPictureBox::LoadPictureFile(LPCTSTR szFile, bool bStretch)
{
	m_bStretchLoadedPicture = bStretch;

	// open file
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_ASSERTE(-1 != dwFileSize);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	_ASSERTE(FALSE != bRead);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	_ASSERTE(SUCCEEDED(hr) && pstm);
	
	// Create IPicture from image file
	if (m_pPictureHolder)
	{
		m_pPictureHolder->Release();
		m_pPictureHolder = NULL;
		//delete m_pPictureHolder;
	}
	hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&m_pPictureHolder);
	_ASSERTE(SUCCEEDED(hr) && m_pPictureHolder);	
	pstm->Release();

	m_pPicture = NULL;
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	
	Refresh();
}


void CPictureBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	
	
	InvokeMethod(
		m_ArxControl->GetStringProperty(Prop::EventDblClicked),
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStringProperty(Prop::OnLMouseEvent),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CPictureBox::CopyDC() 
{
	
	// delete the bitmap is valid
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	if (!GetParent()->IsWindowVisible())		
		return;
	
	HDC hDC = ::GetDC(m_hWnd);
	HDC hDCmem = CreateCompatibleDC (hDC) ; 
	int nSourceX = GetDeviceCaps(hDC, HORZRES);
	int nSourceY = GetDeviceCaps(hDC, VERTRES); 

	// Create a compatible bitmap for hdcSource.  
	HBITMAP hbmMem = CreateCompatibleBitmap(hDC, nSourceX, nSourceY); 
	HGDIOBJ hbmOld = SelectObject (hDCmem, hbmMem) ; 

	BitBlt(hDCmem, 0,0, 
			nSourceX,
			nSourceY,
			hDC,0, 0, SRCCOPY);

	SelectObject (hDCmem, hbmOld);
	DeleteObject (hbmOld);
	DeleteDC (hDCmem);
	::ReleaseDC (m_hWnd, hDC);
	
	// delete the bitmap if valid
	if (m_hbmMem != NULL)
		DeleteObject(m_hbmMem);
	
	m_hbmMem = hbmMem;
}

void CPictureBox::OnSize(UINT nType, int cx, int cy) 
{
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	CWnd::OnSize(nType, cx, cy);
}

void CPictureBox::OnClicked() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
}

void CPictureBox::OnPaint() 
{
	
	CPaintDC dc(this); // device context for painting

	//#ifdef USE_MEM_DC
		//CMemDC pdc(&dc);
	//#else
		CDC* pdc = &dc;
	//#endif
	
	if (m_hbmMem == NULL)
	{
		
		if (!GetParent()->IsWindowVisible())		
			return;

		CPictureBox::Refresh(pdc);
			
		CWnd *pFocusWnd = CWnd::GetFocus();
		// if the CWnd that has focus is not this CWnd
		// then send a 0 to indicate this control does not 
		// have focus. Otherwise send a 1 to indicate that
		// it does have focus.

		if (pFocusWnd != this)
			// call methods to invoke the event
			InvokeMethodInt(m_ArxControl->GetStringProperty(Prop::EventPaint), 0, IsAsyncEvents());
		else
			// call methods to invoke the event
			InvokeMethodInt(m_ArxControl->GetStringProperty(Prop::EventPaint), 1, IsAsyncEvents());
		
	}

	if (m_hbmMem != NULL)
	{
		if (!GetParent()->IsWindowVisible())		
			return;

		CDC memdc;
    // Create a compatible memory DC
    memdc.CreateCompatibleDC( pdc );
    // Select the bitmap into the DC
    CBitmap *poldbmp = memdc.SelectObject( CBitmap::FromHandle(m_hbmMem) );

    // Copy (BitBlt) bitmap from memory DC to screen DC
    pdc->BitBlt( 0, 0,
			GetDeviceCaps(pdc->m_hDC, HORZRES), 
			GetDeviceCaps(pdc->m_hDC, VERTRES),                 
			&memdc, 0, 0, SRCCOPY );

    memdc.SelectObject( poldbmp );
	}
}


void CPictureBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
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

BOOL CPictureBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CButton::PreTranslateMessage(pMsg);
}

void CPictureBox::OnDestroy() 
{
	if (m_pPicture != NULL && m_bLoadPicture)
	{
		m_pPicture = NULL;
		delete m_pPicture;
	}

	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CButton::OnDestroy();
}

void CPictureBox::SetDragnDrop(BOOL bRegister)
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

/*
HBRUSH CPictureBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
	{
		m_brBackground.DeleteObject();
		m_brBackground.CreateSolidBrush(GetRGBColor(-16));
	}

	m_BkColor = GetRGBColor(m_ArxControl->GetLongProperty(Prop::BackgroundColor));	
	pDC->SetBkColor(m_BkColor); 
	pDC->SetBkMode(OPAQUE); 
	return m_brBackground; 


}

*/