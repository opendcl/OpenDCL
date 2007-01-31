// GraphicButton.cpp : implementation file
//

#include "stdafx.h"
#include "GraphicButton.h"
#include "PictureObject.h"
#include "Resource.h"
#include "Project.h"

#define nLightGrey 192


HICON ExtractIconFromPicture(CPictureHolder *NewPicture, CDC * cdc)
{
	BOOL bRetVal = TRUE;
	long lPicHeight = 0;
	long lPicWidth = 0;
	CSize sizePic;
	int nRetVal;

	if (NULL == NewPicture->m_pPict)
	{
		return NULL;
	}

	// if picture is a bitmap
	if (PICTYPE_BITMAP == NewPicture->GetType())
	{
		CImageList ImageList;

		HBITMAP hBitmap = NULL;

		// get handle of the bitmap
		NewPicture->m_pPict->get_Handle((OLE_HANDLE FAR *) &hBitmap);

		// get dimensions of bitmap
		NewPicture->m_pPict->get_Width(&lPicWidth);
		NewPicture->m_pPict->get_Height(&lPicHeight);

		sizePic.cx = (int)lPicWidth;
		sizePic.cy = (int)lPicHeight;

		// convert coordinates from units to logical units
		cdc->HIMETRICtoLP(&sizePic);

		// if image list has not been created
		if (NULL == ImageList.m_hImageList)
		{			
			// create the image list
			bRetVal = ImageList.Create(sizePic.cx, sizePic.cy, ILC_COLOR8 | ILC_MASK, 1, 1);

			// set the background color of the image list
			//ImageList.SetBkColor(RGB(nWhite,nWhite,nWhite));		
			
		}

		// create a temp bitmap
		CBitmap * TempBmp = CBitmap::FromHandle(hBitmap);

		if (bRetVal)
		{
			// add bitmap to imagelist; mask is ignored in this sample
			nRetVal = ImageList.Add(TempBmp, RGB(nLightGrey, nLightGrey, nLightGrey) ) ;			
			DeleteObject(TempBmp);	
			return ImageList.ExtractIcon(0);
		}
		
		
		return NULL;
	}
	// else if picture is an icon
	else if (PICTYPE_ICON == NewPicture->GetType())
	{
		HICON hIcon;

		// get handle of the icon
		NewPicture->m_pPict->get_Handle((OLE_HANDLE FAR *) &hIcon);

		return hIcon;
	}
	else
	{
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphicButton

CGraphicButton::CGraphicButton()
{
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      NULL,
      NULL,
      NULL);

	m_PictureID = 0;
	m_pPicture = NULL;
	m_hIconOut = NULL;
	m_hBmpPic = NULL;
	m_hIconPic = NULL;
	
}

CGraphicButton::~CGraphicButton()
{
	m_pPicture = NULL;
	// Note: the following two lines MUST be here! even if
	// BoundChecker says they are unnecessary!
	if (m_hIconOut != NULL) 
	{
		::DestroyIcon(m_hIconOut);
		m_hIconOut = NULL;
	}

}


BEGIN_MESSAGE_MAP(CGraphicButton, CColorButton)
	//{{AFX_MSG_MAP(CGraphicButton)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphicButton message handlers

BOOL CGraphicButton::Create(int ButtonStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle = dwStyle | SS_ICON | SS_CENTERIMAGE;
	m_ButtonStyle = ButtonStyle;
	if (m_ButtonStyle > 1 && m_ButtonStyle < 5)
	{
		m_cxIcon = 16;
		m_cyIcon = 16;
	}
	/*
	BOOL bReturn = CWnd::Create(
		m_ClassName,
		_T(""), 
		dwStyle,
		rect,
		pParentWnd,
		nID, 
		NULL);
	*/

	BOOL bReturn = CColorButton::Create(
		_T(""), 
		dwStyle,
		rect,
		pParentWnd,
		nID);

	return bReturn;
}

int CGraphicButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CColorButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetDefaultPicture(m_ButtonStyle);
	
	return 0;
}

void CGraphicButton::SetDefaultPicture(int nNewButtonStyle)
{
	m_ButtonStyle = nNewButtonStyle;

	
	CBitmap bitmap;	
	BITMAP bmp;

	switch (m_ButtonStyle)
	{
	case 2:
		bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_PICK));
		break;
	case 3:
		bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_SELECT));
		break;
	case 4:
		bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_FILTER));
		break;
	default:
		return;
		break;
	}
	bitmap.GetObject(sizeof(BITMAP), &bmp);
	m_cxIcon = bmp.bmWidth;
	m_cyIcon = bmp.bmHeight;

	CImageList tImageList;
	tImageList.Create(bmp.bmWidth, bmp.bmHeight, ILC_COLOR8 | ILC_MASK, 1, 1);
	tImageList.Add(&bitmap, RGB(192,192,192));
	// Note: the following two lines MUST be here! even if
	// BoundChecker says they are unnecessary!
	if (m_hIconOut != NULL) 
	{
		::DestroyIcon(m_hIconOut);
		m_hIconOut = NULL;
	}

	m_hIconOut = tImageList.ExtractIcon(0);

	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	

}

void CGraphicButton::OnDestroy() 
{
	if (m_hIconOut != NULL) 
	{
		::DestroyIcon(m_hIconOut);
		m_hIconOut = NULL;
	}
	CColorButton::OnDestroy();
	
}


void CGraphicButton::DrawTheIcon(CDC* pdc, RECT* rcItem, CRect *captionRect)
{
	CPoint pt;

	// Center the icon horizontally
	pt.x = rcItem->right - m_cxIcon;
	pt.x = pt.x / 2;
	
	// Center the icon vertically
	pt.y = rcItem->bottom - m_cyIcon; 
	pt.y = pt.y / 2;
    
	CSize szIcon(m_cxIcon, m_cyIcon);

	// Read the button's title
	CString sTitle;
	GetWindowText(sTitle);

	if (!sTitle.IsEmpty())
	{
		// L'icona deve vedersi subito dentro il focus rect
		pt.x = rcItem->left + 3;  
		captionRect->left += m_cxIcon + 3;
	}
		
	
	if (m_hIconOut != NULL)
	{
		// and finish!
		pdc->DrawState(	pt,
						szIcon, 
						m_hIconOut, 
						(CGraphicButton::IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED), 					
						(CBrush*)NULL);	
		return;
	}
	if (m_pPicture == NULL)
	{
		return;
	}
	
	// if the icon has been set
	if (m_hIconPic != NULL)
	{
		// and finish!
		pdc->DrawState(	pt,
						szIcon, 
						m_hIconPic, 
						(CGraphicButton::IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED), 					
						(CBrush*)NULL);	
		return;
	}

	// if the bitmap has been set
	if (m_hBmpPic != NULL)
	{
		// and finish!
		pdc->DrawState(	pt,
						szIcon, 
						m_hBmpPic, 
						(CGraphicButton::IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED),
						NULL);
		return;
	}


	// ensure the picture holder is valid
	try
	{
		short nPicType = m_pPicture->GetPicType();
		if (nPicType == -1)	
			return;

		if (m_pPicture->GetPicture().GetPictureDispatch() == NULL)
			return;
	}
	catch(...)
	{
		return;
	}
	
	if (PICTYPE_BITMAP == m_pPicture->GetPicType())
	{
		if (m_hBmpPic == NULL)
			// get the bitmap
			m_pPicture->GetPicture().m_pPict->get_Handle((OLE_HANDLE FAR *) &m_hBmpPic);
		
		// and finish!
		pdc->DrawState(	pt,
						szIcon, 
						m_hBmpPic, 
						(CGraphicButton::IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED),
						NULL);
		
	}
	// else if picture is an icon
	else if (PICTYPE_ICON == m_pPicture->GetPicType())
	{
		if (m_hIconPic == NULL)
			// get handle of the icon
			m_pPicture->GetPicture().m_pPict->get_Handle((OLE_HANDLE FAR *) &m_hIconPic);
	
		// and finish!
		pdc->DrawState(	pt,
						szIcon, 
						m_hIconPic, 
						(CGraphicButton::IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED), 					(CBrush*)NULL);
		//m_pPicture->m_hPicture.CreateFromIcon(hIcon);
		
	}
	 
} // End of DrawTheIcon


void CGraphicButton::OnPaint() 
{
	PAINTSTRUCT ps; 
	
	CDC* pdc = BeginPaint(&ps);

	Refresh(pdc);
	
	EndPaint(&ps);
}

void CGraphicButton::SetPictureID(int sPictureID)
{
	if (m_PictureID == sPictureID)
		return;
		
	// Note: the following two lines MUST be here! even if
	// BoundChecker says they are unnecessary!
	if (m_hIconOut != NULL) 
	{
		::DestroyIcon(m_hIconOut);
		m_hIconOut = NULL;
	}
	m_hIconOut = NULL;
	
	m_PictureID = sPictureID;

	if (sPictureID == 0)
	{
		SetPictureBlank();

		Invalidate();
		return;
	}	
	
	// do loop to navigate Pictures
  int nCount;
	for (nCount = 0; nCount < activeProject->GetPictureList().GetCount(); nCount++)
	{		
		POSITION pos = activeProject->GetPictureList().FindIndex(nCount);
		if (pos != NULL)
		{
			CPictureObject *pPic = activeProject->GetPictureList().GetAt(pos);
			if (pPic != NULL)
			{
				if (pPic->GetID() == sPictureID)
				{
					m_hBmpPic = NULL;
					m_hIconPic = NULL;
					m_cxIcon = pPic->GetWidth();
					m_cyIcon = pPic->GetHeight();
					m_pPicture = pPic;
				}
			}
		}
	}
	nCount = 0;

	Invalidate();

}


void CGraphicButton::Refresh(CDC *pdc)
{
	CRect rcThis;	
	CColorButton::GetClientRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());

	
	
	pdc->SetBkMode(TRANSPARENT);

	// draw the Window background for the cell				
	pdc->FillRect(rcCell, m_pStaticBrush);
	
	if (m_ButtonStyle != 1 && m_ButtonStyle != 5)
	{		
		pdc->DrawEdge(rcCell, EDGE_RAISED, BF_RECT);
	}

	CRect captionRect = rcCell;
	
	DrawTheIcon(pdc, rcCell, &captionRect);
	
	captionRect = rcCell;

	// Read the button's title
	CString sTitle;
	GetWindowText(sTitle);

	
	// Write the button title (if any)
	if (sTitle.IsEmpty() == FALSE)
	{
		
		CFont *pCurrentFont = GetFont(); 
		CFont *pOldFont = pdc->SelectObject(pCurrentFont);
		
		
		pdc->SetTextColor(m_ForeColor);
		
		// Center text
		CRect centerRect = captionRect;
		pdc->DrawText(sTitle, -1, captionRect, DT_SINGLELINE|DT_CALCRECT);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())/2, (centerRect.Height() - captionRect.Height())/2);
		/* RFU
		captionRect.OffsetRect(0, (centerRect.Height() - captionRect.Height())/2);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())-4, (centerRect.Height() - captionRect.Height())/2);
		*/

		pdc->SetBkMode(TRANSPARENT);
		pdc->DrawState(captionRect.TopLeft(), captionRect.Size(), (LPCTSTR)sTitle, (IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED), 
					   TRUE, 0, (CBrush*)NULL);
		
		pdc->SelectObject(pOldFont);		
	}
	
}

void CGraphicButton::OnEnable(BOOL bEnable) 
{
	CColorButton::OnEnable(bEnable);
	
	Invalidate();
}

void CGraphicButton::OnSysColorChange() 
{
	CColorButton::OnSysColorChange();
	
	Invalidate();
}
void CGraphicButton::SetPictureBlank() 
{
	
	// Note: the following two lines MUST be here! even if
	// BoundChecker says they are unnecessary!
	if (m_hIconOut != NULL) 
	{
		::DestroyIcon(m_hIconOut);
		m_hIconOut = NULL;
	}
	m_pPicture = NULL;
	m_hIconOut = NULL;
	m_PictureID = 0;
	m_hBmpPic = NULL;
	m_hIconPic = NULL;
		
}

void CGraphicButton::OnSize(UINT nType, int cx, int cy) 
{
	int nThisHeight = cy;
	int nThisWidth = cx;

	switch (m_ButtonStyle)
	{
		case 2:
		case 3:
		case 4:
			{
			nThisHeight = 25;
			nThisWidth = 26;
			SetWindowPos(0, 0, 0, nThisWidth, nThisHeight, SWP_FRAMECHANGED|SWP_NOMOVE);
			break;
			}
	}
	CColorButton::OnSize(nType, nThisWidth, nThisHeight);

}
