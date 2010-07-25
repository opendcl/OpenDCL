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
	double dblScale = (double)nCtrlHeight / (double)nPicHeight;
	double dblScaleH = (double)nCtrlWidth / (double)nPicWidth;
	if( dblScaleH < dblScale )
		dblScale = dblScaleH;

	int nDrawWidth = int(dblScale * nPicWidth);
	int nDrawHeight = int(dblScale * nPicHeight);
	
	if( nDrawHeight < nCtrlHeight )
	{
		rcCell.top = (nCtrlHeight - nDrawHeight) / 2;
		rcCell.bottom = rcCell.top + nDrawHeight;
	}
	if( nDrawWidth < nCtrlWidth )
	{
		rcCell.left = (nCtrlWidth - nDrawWidth) / 2;
		rcCell.right = rcCell.left + nDrawWidth;
	}

	return rcCell;
}

static void DrawTransparentBitmap( CBitmap* pBitmap, CDC* pDC, int x, int y, int nWidth, int nHeight, COLORREF crTransparent )
{
	CDC dcImage, dcTrans;

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(pBitmap);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crTransparent);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	// Do the work - True Mask method - cool if not actual display
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);

	// Restore settings
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
}

static void DrawDisabledTransparentBitmap( CBitmap* pBitmap, CDC* pDC, int x, int y, int nWidth, int nHeight, COLORREF crTransparent )
{
	pDC->SaveDC();
	CDC dcImage, dcTrans;

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(pBitmap);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crTransparent);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	// Create monochrome DC
	CDC dcBW;
	dcBW.CreateCompatibleDC(pDC);

	// Create a monochrome DIB section with a black and white palette
	struct
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[2];
	} RGBBWBITMAPINFO =
	{
		{	// a BITMAPINFOHEADER
			sizeof(BITMAPINFOHEADER),	// biSize 
			nWidth, 					// biWidth; 
			nHeight,					// biHeight; 
			1,							// biPlanes; 
			1,							// biBitCount 
			BI_RGB, 					// biCompression; 
			0,							// biSizeImage; 
			0,							// biXPelsPerMeter; 
			0,							// biYPelsPerMeter; 
			0,							// biClrUsed; 
			0							// biClrImportant; 
		},
		{
			{ 0xA0, 0xA0, 0xA0, 0x00 }, //gray color for anything that's not background color
			{ LOBYTE(crTransparent), LOBYTE(crTransparent>>8), LOBYTE(crTransparent>>16), LOBYTE(crTransparent>>24) }
		}
	};

	VOID *pbitsBW;
	HBITMAP hbmBW = CreateDIBSection(dcBW.m_hDC, (LPBITMAPINFO) &RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);

	ASSERT(hbmBW);

	if (hbmBW)
	{
		// Attach the monochrome DIB section and the bitmap to the DCs
		SelectObject(dcBW.m_hDC, hbmBW);

		// BitBlt the bitmap into the monochrome DIB section
		dcBW.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

		// BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
		// The magic ROP comes from Charles Petzold's book
		CBrush brush;
		brush.CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
		CBrush* pOldBrush = dcBW.SelectObject(&brush);
		dcBW.BitBlt(1, 1, nWidth, nHeight, &dcImage, 0, 0, 0xB8074A);

		// BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
		brush.DeleteObject();
		brush.CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
		dcBW.SelectObject(&brush);
		dcBW.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, 0xB8074A);

		dcBW.SelectObject(pOldBrush);
		brush.DeleteObject();
		DeleteObject(hbmBW);
	}

	// Do the work - True Mask method - cool if not actual display
	pDC->BitBlt(x, y, nWidth, nHeight, &dcBW, 0, 0, SRCINVERT);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcBW, 0, 0, SRCINVERT);

	// Restore settings
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->RestoreDC(-1);
}


/////////////////////////////////////////////////////////////////////////////
// CPictureBox

CPictureBox::CPictureBox()
: mpPicture( NULL )
, mbFitPictureToCtrl( false )
, mhbmMem( NULL )
{
}

CPictureBox::CPictureBox( CWnd* pParentWnd, UINT nID, const CRect& rcWnd, UINT nIconResId /*= -1*/ )
: mpPicture( NULL )
, mbFitPictureToCtrl( false )
, mhbmMem( NULL )
{
	Create( _T(""), (WS_CHILD | WS_VISIBLE), rcWnd, pParentWnd, nID );
	SetPicture( nIconResId );
}

CPictureBox::~CPictureBox()
{
	if( mhbmMem )
		DeleteObject( mhbmMem );
}

CSize CPictureBox::GetPictureSize() const
{
	if( mpPicture )
		return CSize( mpPicture->GetWidth(), mpPicture->GetHeight() );
	return CSize( 0, 0 );
}

void CPictureBox::Clear() 
{
	if( mhbmMem )
	{
		DeleteObject( mhbmMem );
		mhbmMem = NULL;
	}
	if( m_hWnd )
		Invalidate();
}

void CPictureBox::ClearPicture()
{
	mpPicture = NULL;
	Refresh();
}

void CPictureBox::SetPicture( TPicturePtr pPicture )
{
	mpPicture = pPicture;
	if( mpPicture )
		AutoSize();
	Refresh();
}

void CPictureBox::SetPicture( UINT nIconResId )
{
	mpPicture = new CPictureObject( -1 );
	mpPicture->LoadResourceIcon( nIconResId );
	AutoSize();
	Refresh();
}

void CPictureBox::Refresh()
{
	Clear();
	if( !IsWindowVisible() )
		return;
	UpdateWindow();
}

void CPictureBox::DrawPicture( TPicturePtr pPicture, bool bFitToCtrl /*= false*/, CDC* pDestDC /*= NULL*/ )
{	
	if( !pPicture )
		return;
	CDC* pdc = pDestDC? pDestDC : GetDC();
	CRect rcCell;	
	GetClientRect(&rcCell);
	pdc->IntersectClipRect( &rcCell );

	// get width and height of picture
	long nPicWidth = pPicture->GetWidth();
	long nPicHeight = pPicture->GetHeight();
	long nPicLeft = 0;
	long nPicTop = 0;
	CRect rcPic = rcCell;
	if( bFitToCtrl )
		rcPic = CalcFitRect( nPicWidth, nPicHeight, rcCell.Width(), rcCell.Height() );
	else
	{
		nPicTop = ( (rcCell.Height() - nPicHeight) / 2 ); // Center the icon vertically
		nPicLeft = ( (rcCell.Width() - nPicWidth) / 2 ); // Center the icon horizontally
		rcPic.SetRect( nPicLeft, nPicTop, nPicLeft + nPicWidth, nPicTop + nPicHeight );
	}
	if( PICTYPE_BITMAP == mpPicture->GetPicType() && !bFitToCtrl )
	{			
		if( IsWindowEnabled() )
			DrawTransparentBitmap( CBitmap::FromHandle( mpPicture->GetBitmap() ), pdc,
														 rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), RGB(192,192,192) );
		else
			DrawDisabledTransparentBitmap( CBitmap::FromHandle( mpPicture->GetBitmap() ), pdc,
																		 rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), RGB(192,192,192) );
	}
	else if( PICTYPE_ICON == mpPicture->GetPicType() )
	{
		if( IsWindowEnabled() )
			pdc->DrawState(	rcPic.TopLeft(), rcPic.Size(),
											mpPicture->GetIcon(), DSS_NORMAL, (HBRUSH)NULL );
		else
			pdc->DrawState(	rcPic.TopLeft(), rcPic.Size(),
											mpPicture->GetIcon(), DSS_DISABLED, (HBRUSH)NULL );
	}
	else
		mpPicture->Render( pdc, rcPic );
	if( !pDestDC )
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
		SetBkColor(hdc, RGB(255,255,255) );
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
		SetBkColor( hdc, RGB(255,255,255) );
	else // or else use the one the user has assigned for this opertation
		SetBkColor( hdc, crBack );	

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
bool CPictureBox::LoadPictureFile(LPCTSTR pszFile, bool bStretch)
{
	mbFitPictureToCtrl = bStretch;
	if( !pszFile || !*pszFile )
	{
		Clear();
		return true;
	}
	CString sPicFile = theWorkspace.FindFile( pszFile );
	if( sPicFile.IsEmpty() )
		sPicFile = pszFile;
	mpPicture = CPictureObject::CreatePictureObject( -1, sPicFile );
	Refresh();
	return (mpPicture != NULL);
}

void CPictureBox::CopyDC() 
{
	if( mhbmMem )
	{
		DeleteObject( mhbmMem );
		mhbmMem = NULL;
	}
	if( !GetParent()->IsWindowVisible() )		
		return;

	HDC hDC = ::GetDC(m_hWnd);
	HDC hDCmem = CreateCompatibleDC (hDC) ; 
	CRect rcClient;
	GetClientRect( &rcClient );
	int nSourceX = rcClient.Width();
	int nSourceY = rcClient.Height(); 

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
	mhbmMem = hbmMem;
}


BEGIN_MESSAGE_MAP(CPictureBox, CButton)
	ON_WM_ENABLE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPictureBox message handlers

void CPictureBox::OnEnable(BOOL bEnable) 
{
	__super::OnEnable(bEnable);
	Refresh();
}

void CPictureBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	__super::OnLButtonDown(nFlags, point);
}

void CPictureBox::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	Refresh();
}

void CPictureBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if( !mhbmMem )
	{
		if( !mpPicture )
			return;
		DrawPicture( mpPicture, mbFitPictureToCtrl, &dc );
		CRect rcClient;
		GetClientRect( &rcClient );
		CRect rcClip;
		dc.GetClipBox( &rcClip );
		rcClip.IntersectRect( &rcClip, &rcClient );
		if( rcClip == rcClient )
			CopyDC();
		return;
	}
	CDC memdc;
  memdc.CreateCompatibleDC( &dc );
  CBitmap* pBmpSave = memdc.SelectObject( CBitmap::FromHandle( mhbmMem ) );

  // Copy (BitBlt) bitmap from memory DC to screen DC
	CRect rcClient;
	GetClientRect( &rcClient );
  dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &memdc, 0, 0, SRCCOPY );
  memdc.SelectObject( pBmpSave );
}

void CPictureBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 13 || nChar == 10 )
		return;
	__super::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CPictureBox::OnEraseBkgnd(CDC* pDC)
{
	if( mhbmMem )
		return TRUE;
	if( !pDC ) //derived classes can pass NULL to check whether the buffered bitmap exists
		return FALSE;
	CRect rcClip;
	pDC->GetClipBox( &rcClip );
	CRect rcClient;
	GetClientRect( &rcClient );
	rcClip.IntersectRect( &rcClip, &rcClient );
	pDC->FillSolidRect( &rcClip, RGB(255,255,255) );
	return TRUE;
}
