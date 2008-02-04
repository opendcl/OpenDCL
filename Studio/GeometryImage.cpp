// GeometryImage.cpp : implementation file
//

#include "stdafx.h"
#include "GeometryImage.h"
#include "Geometry.h"


const int aUp = 1;
const int aDown = 2;
const int aLeft = 3;
const int aRight = 4;
const int nFormTopY = 55;
const int nFormLeftX = 23;
const int nBottomY = 128;
const int nTopY = 96;
const int nLeftX = 55;
const int nRightX = 103;
const int nExtensionOffset = 2;
const int nExtensionLength = 22;
const int nDimensionOffset = 20;


/////////////////////////////////////////////////////////////////////////////
// CGeometryImage

CGeometryImage::CGeometryImage()
{
	m_pBitmap = NULL;
}

CGeometryImage::~CGeometryImage()
{
	delete m_pBitmap;
}


BEGIN_MESSAGE_MAP(CGeometryImage, CStatic)
	//{{AFX_MSG_MAP(CGeometryImage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeometryImage message handlers

void CGeometryImage::OnPaint() 
{
	PAINTSTRUCT ps; 
	
	CDC* pdc = BeginPaint(&ps);
	DrawDimensions(pdc->m_hDC);
	EndPaint(&ps);
	
}
void CGeometryImage::DrawDimensions(HDC hdc)
{
	CRect rcExampe;
	GetClientRect(&rcExampe);
	CPoint point;
	
	CGeometry* pParent = (CGeometry*)GetParent();
		
	if (m_pBitmap == NULL)
	{
		m_pBitmap = new CBitmap;
		m_pBitmap->LoadBitmap(IDB_GEOMETRY);
	}
	CPoint pt(0,0);

	if (hdc != NULL)
	{
		HBRUSH pBrush = NULL;
		::DrawState(
			hdc,
			NULL,//(HBRUSH)pBrush->GetSafeHandle(),
			NULL,
			(LPARAM)m_pBitmap->GetSafeHandle(), 
			0, 
			pt.x, 
			pt.y, 
			rcExampe.Width(), 
			rcExampe.Height(), 
			DSS_NORMAL|DST_BITMAP); 
	}
	rcExampe.bottom -= 31;
	rcExampe.right -= 24;

	// clear the picture of dimensions and set the fore color to blue
	COLORREF ForeColor = RGB(0, 0, 255);


	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, ForeColor);
	HGDIOBJ OldPen = SelectObject(hdc, pen);

	  // draw the extension line for the top position
	MoveToEx(hdc, nLeftX - nExtensionLength, nTopY, &point);
	LineTo(hdc, nLeftX - nExtensionOffset, nTopY);

	  // draw the extension line for the bottom position
	MoveToEx(hdc, nRightX + nExtensionOffset + 1, nBottomY, &point);
	LineTo(hdc, nRightX + nExtensionLength + 1, nBottomY);

	  // draw the extension line for the left position
	MoveToEx(hdc, nLeftX, nTopY - nExtensionLength, &point);
	LineTo(hdc, nLeftX, nTopY - nExtensionOffset);

	  // draw the extension line for the right position
	MoveToEx(hdc, nRightX, nBottomY + nExtensionOffset + 1, &point);
	LineTo(hdc, nRightX, nBottomY + nExtensionLength + 1);

	  // check if the user has selected the bottom from bottom toggle
	switch (pParent->m_Bottom.GetCurSel())
	{
	case 0:
		// draw the other extension line for the Height dimension
		MoveToEx(hdc, nRightX + nExtensionOffset + 1, nTopY, &point);
		LineTo(hdc, nRightX + nExtensionLength + 1, nTopY);

		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nRightX + nDimensionOffset, nTopY, &point);
		LineTo(hdc, nRightX + nDimensionOffset, nBottomY);

		// draw the top arrow
		DrawArrow(hdc, nRightX + nDimensionOffset, nTopY, aUp);

		// draw the Bottom arrow
		DrawArrow(hdc, nRightX + nDimensionOffset, nBottomY, aDown);
		break;
	case 1:
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nRightX + nDimensionOffset, rcExampe.Height(), &point);
		LineTo(hdc, nRightX + nDimensionOffset, nBottomY);

		// draw the Bottom arrow
		DrawArrow(hdc, nRightX + nDimensionOffset, nBottomY, aUp);

		// draw the top arrow
		DrawArrow(hdc, nRightX + nDimensionOffset, rcExampe.Height() + 1, aDown);
		break;
	//case 2:
	//	DrawSplitter( hdc, CRect( nFormLeftX, nFormTopY + 13 - 4, rcExampe.Width(), nFormTopY + 13 ) );
	//	// draw the dimension line for the Height dimension
	//	MoveToEx(hdc, nRightX + nDimensionOffset, nFormTopY+13, &point);
	//	LineTo(hdc, nRightX + nDimensionOffset, nBottomY);

	//	// draw the top arrow
	//	DrawArrow(hdc, nRightX + nDimensionOffset, nFormTopY+12, aUp);

	//	// draw the Bottom arrow
	//	DrawArrow(hdc, nRightX + nDimensionOffset, nBottomY, aDown);
	//	break;
	default:
		DrawSplitter( hdc, CRect( nFormLeftX, rcExampe.Height() - 11, rcExampe.Width(), rcExampe.Height() - 11 + 4 ) );
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nRightX + nDimensionOffset, rcExampe.Height()-12, &point);
		LineTo(hdc, nRightX + nDimensionOffset, nBottomY);

		// draw the Bottom arrow
		DrawArrow(hdc, nRightX + nDimensionOffset, nBottomY, aUp);

		// draw the top arrow
		DrawArrow(hdc, nRightX + nDimensionOffset, rcExampe.Height() - 11, aDown);
		break;
	}

	  // check if the user has selected the top from bottom toggle
	switch (pParent->m_Top.GetCurSel())
	{      
	case 0:
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nLeftX - nDimensionOffset, nTopY, &point);
		LineTo(hdc, nLeftX - nDimensionOffset, nFormTopY);

		// draw the top arrow
		DrawArrow(hdc, nLeftX - nDimensionOffset, nFormTopY, aUp);

		// draw the Bottom arrow
		DrawArrow(hdc, nLeftX - nDimensionOffset, nTopY, aDown);
		break;
	case 1:
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nLeftX - nDimensionOffset, rcExampe.Height(), &point);
		LineTo(hdc, nLeftX - nDimensionOffset, nTopY);

		// draw the Bottom arrow
		DrawArrow(hdc, nLeftX - nDimensionOffset, nTopY, aUp);

		// draw the top arrow
		DrawArrow(hdc, nLeftX - nDimensionOffset, rcExampe.Height() + 1, aDown);
		break;
	default:
		DrawSplitter( hdc, CRect( nFormLeftX, nFormTopY+12 - 4, rcExampe.Width(), nFormTopY+12 ) );
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nLeftX - nDimensionOffset, nTopY, &point);
		LineTo(hdc, nLeftX - nDimensionOffset, nFormTopY+12);

		// draw the top arrow
		DrawArrow(hdc, nLeftX - nDimensionOffset, nFormTopY+12, aUp);

		// draw the Bottom arrow
		DrawArrow(hdc, nLeftX - nDimensionOffset, nTopY, aDown);			
		break;
	//default:
	//	// draw the dimension line for the Height dimension
	//	MoveToEx(hdc, nLeftX - nDimensionOffset, rcExampe.Height()-12, &point);
	//	LineTo(hdc, nLeftX - nDimensionOffset, nTopY);

	//	// draw the Bottom arrow
	//	DrawArrow(hdc, nLeftX - nDimensionOffset, nTopY, aUp);

	//	// draw the top arrow
	//	DrawArrow(hdc, nLeftX - nDimensionOffset, rcExampe.Height() -11, aDown);			
	//	break;	
	}

	  // check if the user has selected the right from right toggle
	switch (pParent->m_Right.GetCurSel())
	{
	case 0:
		// draw the other extension line for the Height dimension
		MoveToEx(hdc, nLeftX, nBottomY + nExtensionOffset + 1, &point);
		LineTo(hdc, nLeftX, nBottomY + nExtensionLength + 1);

		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nLeftX, nBottomY + nDimensionOffset, &point);
		LineTo(hdc, nRightX, nBottomY + nDimensionOffset);

		// draw the left arrow
		DrawArrow(hdc, nLeftX, nBottomY + nDimensionOffset, aLeft);

		// draw the right arrow
		DrawArrow(hdc, nRightX, nBottomY + nDimensionOffset, aRight);
		break;
	case 1:
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nRightX, nBottomY + nDimensionOffset, &point);
		LineTo(hdc, rcExampe.Width(), nBottomY + nDimensionOffset);

		// draw the left arrow
		DrawArrow(hdc, nRightX, nBottomY + nDimensionOffset, aLeft);

		// draw the right arrow
		DrawArrow(hdc, rcExampe.Width(), nBottomY + nDimensionOffset, aRight);
		break;
	//case 2:
	//	// draw the dimension line for the Height dimension
	//	MoveToEx(hdc, nLeftX-10, nBottomY + nDimensionOffset, &point);
	//	LineTo(hdc, nRightX, nBottomY + nDimensionOffset);

	//	// draw the left arrow
	//	DrawArrow(hdc, nLeftX-10, nBottomY + nDimensionOffset, aLeft);

	//	// draw the right arrow
	//	DrawArrow(hdc, nRightX, nBottomY + nDimensionOffset, aRight);
	//	break;
	default:
		DrawSplitter( hdc, CRect( rcExampe.Width()-24, nFormTopY + 1, rcExampe.Width()-24 + 4, nFormTopY + 112 ) );
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nRightX, nBottomY + nDimensionOffset, &point);
		LineTo(hdc, rcExampe.Width()-24, nBottomY + nDimensionOffset);

		// draw the left arrow
		DrawArrow(hdc, nRightX, nBottomY + nDimensionOffset, aLeft);

		// draw the right arrow
		DrawArrow(hdc, rcExampe.Width()-24, nBottomY + nDimensionOffset, aRight);
		break;
	}

	// check if the user has selected the left from right toggle
	switch (pParent->m_Left.GetCurSel())
	{
	case 0:
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nFormLeftX, nTopY - nDimensionOffset, &point);
		LineTo(hdc, nLeftX, nTopY - nDimensionOffset);

		// draw the left arrow
		DrawArrow(hdc, nFormLeftX - 1, nTopY - nDimensionOffset, aLeft);

		// draw the right arrow
		DrawArrow(hdc, nLeftX, nTopY - nDimensionOffset, aRight);
		break;
	case 1:
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nLeftX, nTopY - nDimensionOffset, &point);
		LineTo(hdc, rcExampe.Width(), nTopY - nDimensionOffset);

		// draw the left arrow
		DrawArrow(hdc, nLeftX, nTopY - nDimensionOffset, aLeft);

		// draw the right arrow
		DrawArrow(hdc, rcExampe.Width(), nTopY - nDimensionOffset, aRight);
		break;
	case 2:
		{
			int nX = ((rcExampe.Width() - nFormLeftX) / 2) + nFormLeftX;
			// draw the dimension line for the Height dimension
			MoveToEx(hdc, nLeftX, nTopY - nDimensionOffset, &point);
			LineTo(hdc, nX, nTopY - nDimensionOffset);

			MoveToEx(hdc, nX, nTopY-nDimensionOffset+3, &point);
			LineTo(hdc, nX, nFormTopY);

			// draw the left arrow
			DrawArrow(hdc, nLeftX - 1, nTopY - nDimensionOffset, aLeft);

			// draw the right arrow
			DrawArrow(hdc, nX, nTopY - nDimensionOffset, aRight);
		}
		break;
	default:
		DrawSplitter( hdc, CRect( rcExampe.Width()-24, nFormTopY + 1, rcExampe.Width()-24 + 4, nFormTopY + 112 ) );
		// draw the dimension line for the Height dimension
		MoveToEx(hdc, nLeftX, nTopY - nDimensionOffset, &point);
		LineTo(hdc, rcExampe.Width()-24, nTopY - nDimensionOffset);

		// draw the left arrow
		DrawArrow(hdc, nLeftX, nTopY - nDimensionOffset, aLeft);

		// draw the right arrow
		DrawArrow(hdc, rcExampe.Width()-24, nTopY - nDimensionOffset, aRight);
		break;
	}
	SelectObject(hdc, OldPen);			
	DeleteObject(pen);
}


void CGeometryImage::DrawSplitter(HDC hdc, const CRect& rcSplitter)
{
	CRect rc( rcSplitter );
	DrawEdge( hdc, &rc, EDGE_SUNKEN, BF_RECT );
}


void CGeometryImage::DrawArrow(HDC hdc, int x,int Y, int Direction)
{
        COLORREF Color = RGB(0, 0, 255);

        switch (Direction)
		{
            case aUp:
              // draw the right side of the arrow
            SetPixel(hdc, x + 1, Y + 2, Color);
            SetPixel(hdc, x + 1, Y + 3, Color);
            SetPixel(hdc, x + 2, Y + 4, Color);
              // draw the left side of the arrow
            SetPixel(hdc, x - 1, Y + 2, Color);
            SetPixel(hdc, x - 1, Y + 3, Color);
            SetPixel(hdc, x - 2, Y + 4, Color);
			break;
      
			case aDown:
              // draw the right side of the arrow
           SetPixel(hdc, x + 1, Y - 2, Color);
           SetPixel(hdc, x + 1, Y - 3, Color);
           SetPixel(hdc, x + 2, Y - 4, Color);
              // draw the left side of the arrow
           SetPixel(hdc, x - 1, Y - 2, Color);
           SetPixel(hdc, x - 1, Y - 3, Color);
           SetPixel(hdc, x - 2, Y - 4, Color);
		   break;
      
			case aLeft:
              // draw the top side of the arrow
           SetPixel(hdc, x + 2, Y - 1, Color);
           SetPixel(hdc, x + 3, Y - 1, Color);
           SetPixel(hdc, x + 4, Y - 2, Color);
              // draw the bottom side of the arrow
           SetPixel(hdc, x + 2, Y + 1, Color);
           SetPixel(hdc, x + 3, Y + 1, Color);
           SetPixel(hdc, x + 4, Y + 2, Color);
		   break;
      
			case aRight:
              // draw the top side of the arrow
           SetPixel(hdc, x - 2, Y - 1, Color);
           SetPixel(hdc, x - 3, Y - 1, Color);
           SetPixel(hdc, x - 4, Y - 2, Color);
              // draw the bottom side of the arrow
           SetPixel(hdc, x - 2, Y + 1, Color);
           SetPixel(hdc, x - 3, Y + 1, Color);
           SetPixel(hdc, x - 4, Y + 2, Color);
		   break;
      
		}
}
