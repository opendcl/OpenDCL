// PictureBox.cpp : implementation file
//

#include "stdafx.h"
#include "PictureBox.h"
#include "Workspace.h"
#include "PictureObject.h"


#define HIMETRIC_INCH	2540

const TCHAR sTL[] = _T("TL");
const TCHAR sTC[] = _T("TC");
const TCHAR sTR[] = _T("TR");
const TCHAR sBL[] = _T("BL");
const TCHAR sBC[] = _T("BC");
const TCHAR sBR[] = _T("BR");

const TCHAR sL[] = _T("L");
const TCHAR sR[] = _T("R");
const TCHAR sC[] = _T("C");


static CRect CalcFitRect(int nPicWidth, int nPicHeight, int nCtrlWidth, int nCtrlHeight)
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
: mpPicture( NULL )
{
	m_bMouseTracking = FALSE;
	m_pPictureHolder = NULL;	
	m_hbmMem = NULL;
	m_bStretchLoadedPicture = false;
	m_cxIcon = 16;
	m_cyIcon = 16;
}

CPictureBox::CPictureBox( CWnd* pParentWnd, UINT nID, const CRect& rcWnd, UINT nIconResId /*= -1*/ )
: mpPicture( NULL )
{
	m_bMouseTracking = FALSE;
	m_pPictureHolder = NULL;	
	m_hbmMem = NULL;
	m_bStretchLoadedPicture = false;
	m_cxIcon = 16;
	m_cyIcon = 16;

	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetBackgroundColor( RGB(255,255,255) );
	Create( _T(""), (WS_CHILD | WS_VISIBLE), rcWnd, pParentWnd, nID );
	CPictureObject* pPic = new CPictureObject( -1 );
	pPic->LoadResourceIcon( nIconResId );
	SetPicture( pPic );
}

CPictureBox::~CPictureBox()
{
	if (m_pPictureHolder != NULL)
	{
		m_pPictureHolder->Release();
		m_pPictureHolder = NULL;
	}
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
}

void CPictureBox::SetPictureBlank() 
{
	mpPicture = NULL;
}

void CPictureBox::SetPicture( CPictureObject* pPicture )
{
	if (mpPicture != NULL)
	{
		mpPicture = NULL;
		delete mpPicture;
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

	mpPicture = pPicture;
	if( mpPicture )
	{
		m_cxIcon = mpPicture->GetWidth();
		m_cyIcon = mpPicture->GetHeight();
		AutoSize();
	}
	else
		SetPictureBlank();

	CDC *pdc = GetDC();
	Refresh(pdc);
	ReleaseDC(pdc);
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
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pdc->FillRect(rcCell, pColorService->GetBackgroundCBrush());
		
	if (mpPicture)
	{
		CSize IconSize(mpPicture->GetWidth(), mpPicture->GetHeight());

		int nPicWidth = m_cxIcon;
		int nPicHeight = m_cyIcon;
		int nPicLeft = 0;
		int nPicTop = 0;           
		if (!IsAutoSized())
		{
			nPicTop = ((rcCell.Height() - nPicHeight)/2); // Center the icon vertically
			nPicLeft = ((rcCell.Width() - nPicWidth)/2); // Center the icon horizontally
		}

		CRect rcPic(
			nPicLeft,
			nPicTop,
			nPicLeft + nPicWidth,
			nPicTop + nPicHeight
			);
		if (PICTYPE_BITMAP == mpPicture->GetPicType())
		{			
			if (Prop::Enabled)
			{
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								mpPicture->GetBitmap(), 
								DSS_NORMAL, 
								NULL);				
			}
			else
			{			
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								mpPicture->GetBitmap(), 
								DSS_DISABLED, 
								NULL);				
			}
		}
		// else if picture is an icon
		else if (PICTYPE_ICON == mpPicture->GetPicType())
		{
			if (Prop::Enabled)
			{
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								mpPicture->CloneIcon(), 
								DSS_NORMAL, 
								(CBrush*)NULL);
			}
			else
			{				
				// and finish!
				pdc->DrawState(	rcPic.TopLeft(),
								IconSize, 
								mpPicture->CloneIcon(), 
								DSS_DISABLED, 
								(CBrush*)NULL);
			}
		}
		else
		{
			// get width and height of picture
			long lWidth = mpPicture->GetWidth();
			long lHeight = mpPicture->GetHeight();
			
			m_cxIcon = lWidth;
			m_cyIcon = lHeight;
			int nPicLeft = 0;
			int nPicTop = 0;           
			if (!IsAutoSized())
			{
				nPicTop = ((rcCell.Height() - nPicHeight)/2); // Center the icon vertically
				nPicLeft = ((rcCell.Width() - nPicWidth)/2); // Center the icon horizontally
			}

			if (mpPicture->GetPicType() == PICTYPE_METAFILE ||
				mpPicture->GetPicType() == PICTYPE_ENHMETAFILE)
			{
				rcCell = CalcFitRect(lWidth, lHeight,rcCell.Width(), rcCell.Height());

				// display picture using IPicture::Render
				mpPicture->Render(pdc, rcCell.left, rcCell.top, rcCell);				
			}
			else if (lWidth > rcCell.Width() || lHeight > rcCell.Height() || m_bStretchLoadedPicture)
			{
				// display picture using IPicture::Render
				mpPicture->Render(pdc, 0, 0, rcCell);
			}		
			else
			{
				// display picture using IPicture::Render
				mpPicture->Render(pdc, nPicLeft, nPicTop, rcCell);
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
		int nPicLeft = 0;
		int nPicTop = 0;
		if (!IsAutoSized())
		{
			int nPicLeft = ((rc.Width() - nPicWidth)/2); // Center the picture horizontally
			int nPicTop = ((rc.Height() - nPicHeight)/2); // Center the picture vertically
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

			int nDrawWidth = int(dW * rc.Width());
			int nDrawHeight = int(dH * nDrawWidth);
			
			// if the calc height is too large
			if (nDrawHeight > rc.Height())
			{
				dFactor = (double)nPicWidth / (double)nPicHeight;
				dH = 1.0;
				dW = dFactor;

				nDrawHeight = int(dH * rc.Height());
				nDrawWidth = int(dW * nDrawHeight);

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

				nDrawWidth = int(dW * rc.Width());
				nDrawHeight = int(nDrawWidth * dH);
				
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



void CPictureBox::DrawText(int sX, int sY, COLORREF crFore, COLORREF crBack, CString sText, CString sJustification, int nEnabled)
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
	SetTextColor(hdc, crFore);

	// check if we are to use the preset background color
	if (crBack == COLOR_USEBACKGROUND)
	{
		CAcadColorService* pColorService = GetColorService();
		if( pColorService )
			SetBkColor(hdc, pColorService->GetBackgroundColor());
		else
			SetBkColor(hdc, crBack);	
	}
	else // or else use the one the user has assigned for this opertation
		SetBkColor(hdc, crBack);	

	if (crBack == COLOR_TRANSPARENT)
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


int CPictureBox::DrawWrappedText(int sX, int sY, int eX, COLORREF crFore, COLORREF crBack, CString sText, CString sJustification)
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
	SetTextColor(hdc, crFore);

	// check if we are to use the preset background color
	if (crBack == COLOR_USEBACKGROUND)
	{
		CAcadColorService* pColorService = GetColorService();
		if( pColorService )
			SetBkColor(hdc, pColorService->GetBackgroundColor());
		else
			SetBkColor(hdc, crBack);	
	}
	else // or else use the one the user has assigned for this opertation
		SetBkColor(hdc, crBack);	

	if (crBack == COLOR_TRANSPARENT)
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

void CPictureBox::GetTextExtent(LPCTSTR pszText, CSize& ext)
{
	HDC hdc = ::GetDC(m_hWnd);
	::GetTextExtentPoint32(hdc, pszText, lstrlen( pszText ), &ext);
	::ReleaseDC(m_hWnd, hdc);
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

	mpPicture = NULL;
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	
	Refresh();
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


BEGIN_MESSAGE_MAP(CPictureBox, CButton)
	ON_WM_ENABLE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPictureBox message handlers

void CPictureBox::OnEnable(BOOL bEnable) 
{
	__super::OnEnable(bEnable);
	
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	Invalidate();
}

void CPictureBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	__super::OnLButtonDown(nFlags, point);
}

void CPictureBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// setup the mouse tracking event reactor
	if (!m_bMouseTracking)       
	{
		TRACKMOUSEEVENT tme;        
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		if (::_TrackMouseEvent(&tme))                
			m_bMouseTracking = TRUE;
	}
	__super::OnMouseMove(nFlags, point);
}

LRESULT CPictureBox::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	m_bMouseTracking = FALSE;        
	//return __super::OnMouseMove(wParam, lParam);
	return FALSE;
}

void CPictureBox::OnSize(UINT nType, int cx, int cy) 
{
	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	__super::OnSize(nType, cx, cy);
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
	if (nChar != 13 && nChar != 10 )
		__super::OnChar(nChar, nRepCnt, nFlags);
}

void CPictureBox::OnDestroy() 
{
	if (mpPicture != NULL)
	{
		mpPicture = NULL;
		delete mpPicture;
	}
	__super::OnDestroy();
}

/*
HBRUSH CPictureBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
	{
		m_brBackground.DeleteObject();
		m_brBackground.CreateSolidBrush(GetRGBColor(-16));
	}

	m_BkColor = GetRGBColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));	
	pDC->SetBkColor(m_BkColor); 
	pDC->SetBkMode(OPAQUE); 
	return m_brBackground; 


}

*/