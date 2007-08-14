// PictureBox.cpp : implementation file
//

#include "stdafx.h"
#include "PictureBox.h"
#include "Sharedres.h"
#include "Workspace.h"
#include "Project.h"
#include "PictureObject.h"
#include "AcadColorTable.h"


const int nGrey = 192;
#define HIMETRIC_INCH	2540


/////////////////////////////////////////////////////////////////////////////
// CPictureBox

CPictureBox::CPictureBox()
{
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      NULL,
      NULL,
      NULL);

	m_AutoSize = FALSE;
	m_pPictureHolder = NULL;	
	m_PictureID = 0;
	m_BkColor = RGB(255,255,255);
	m_hIcon = NULL;
}

CPictureBox::~CPictureBox()
{
	if (m_hIcon != NULL)
		::DestroyIcon(m_hIcon);
}


BEGIN_MESSAGE_MAP(CPictureBox, CWnd)
	//{{AFX_MSG_MAP(CPictureBox)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ENABLE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPictureBox message handlers

BOOL CPictureBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle = dwStyle | SS_ICON | SS_CENTERIMAGE;
	
	m_cxIcon = 0;
	m_cyIcon = 0;
	
	// TODO: Add your specialized code here and/or call the base class
	BOOL bReturn = CWnd::Create(
		m_ClassName,
		_T(""), 
		dwStyle,
		rect,
		pParentWnd,
		nID, 
		NULL);

	return bReturn;
}

void CPictureBox::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	Invalidate();
}

void CPictureBox::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	Invalidate();
}

void CPictureBox::OnPaint() 
{
	LONG lClassStyle = GetClassLongPtr( m_hWnd, GCL_STYLE );
	SetClassLongPtr( m_hWnd, GCL_STYLE, (lClassStyle & ~(LONG)CS_PARENTDC) );
	CPaintDC dc(this);
	Refresh(dc.m_hDC);

	if (m_hIcon != NULL)
	{
		CRect rcCell;	
		GetClientRect(&rcCell);

		CPoint pt;

		// Center the icon horizontally
		pt.x = (rcCell.right - m_cxIcon) / 2;
		
		// Center the icon vertically
		pt.y = (rcCell.bottom - m_cyIcon) / 2; 

		CRect rcIconBack( pt.x, pt.y - 2, pt.x + m_cxIcon, pt.y - 2 + m_cyIcon );

		::SetBkColor(dc.m_hDC, RGB(255,255,255));
		::ExtTextOut(dc.m_hDC, 0, 0, ETO_OPAQUE, &rcIconBack, NULL, 0, NULL);
		//pdc->DrawIcon( pt.x, pt.y, m_hIcon );

		//m_ImageList.SetBkColor(RGB(255,255,255));
		m_ImageList.Draw(&dc, 0, pt, ILD_NORMAL);
	}
	SetClassLongPtr( m_hWnd, GCL_STYLE, lClassStyle );

}

void CPictureBox::SetPicture(const CPictureObject* pPict)
{
	if( !pPict || !pPict->IsValid() )
	{
		SetPictureBlank();
		Invalidate();
		return;
	}
	// get the icon
	m_hIcon = pPict->CloneIcon();
	m_cxIcon = pPict->GetWidth();
	m_cyIcon = pPict->GetHeight();
	m_ImageList.DeleteImageList();
	m_ImageList.Create(m_cxIcon, m_cyIcon, ILC_COLOR8 | ILC_MASK, 0, 1);
	m_ImageList.Add(m_hIcon);
	Invalidate(TRUE);
}
//
//void CPictureBox::SetPictureID(long sPictureID)
//{
//	SetPicture (activeProject(sPictureID));
//}

void CPictureBox::SetIcon(HICON hIcon)
{
	m_PictureID = 0;
	m_pPictureHolder = NULL;
	m_hIcon = hIcon;
	if(!hIcon)
		return SetPictureBlank();
	ICONINFO ii;
	GetIconInfo( m_hIcon, &ii );
 	m_cxIcon = ii.xHotspot * 2;
	m_cyIcon = ii.yHotspot * 2;
	m_ImageList.Create( ii.xHotspot * 2, ii.yHotspot * 2, ILC_COLOR8 | ILC_MASK, 0, 1 );
	m_ImageList.Add(m_hIcon);
	::DeleteObject(ii.hbmMask);
	::DeleteObject(ii.hbmColor);
	Invalidate(TRUE);
}

void CPictureBox::SetIcon(UINT nId)
{
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_LABEL), RT_GROUP_ICON);
	SetIcon( LoadIcon(hInstResource, MAKEINTRESOURCE(nId)) );
}

void CPictureBox::Refresh(HDC hdc)
{
	CRect rcCell;	
	GetClientRect(&rcCell);

	try
	{
	
		::SetBkColor(hdc, m_BkColor);
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);

		if (m_pPictureHolder != NULL)
		{
			// get width and height of picture
			long hmWidth;
			long hmHeight;
			m_pPictureHolder->get_Width(&hmWidth);
			m_pPictureHolder->get_Height(&hmHeight);
			
			// convert himetric to pixels
			int nPicWidth	= MulDiv(hmWidth, ::GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_INCH);
			int nPicHeight	= MulDiv(hmHeight, ::GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_INCH);		
			
			m_cxIcon = nPicWidth;
			m_cyIcon = nPicHeight;

			// Center the icon horizontally
			int nPicLeft = ((rcCell.Width() - nPicWidth)/2);
			
			// Center the icon vertically
			int nPicTop = ((rcCell.Height() - nPicHeight)/2);           
			if (m_AutoSize == TRUE)
			{
				nPicTop = 0;
				nPicLeft = 0;
			}

			SHORT nPicType;
			m_pPictureHolder->get_Type(&nPicType);
			if (nPicType == PICTYPE_METAFILE ||
				nPicType == PICTYPE_ENHMETAFILE)
			{
				CRect rcThis;
				GetClientRect(&rcThis);
				// display picture using IPicture::Render
				m_pPictureHolder->Render(hdc, 0, 0, rcThis.Width(), rcThis.Height(), 0, hmHeight, hmWidth, -hmHeight, &rcCell);
			}
			else if (nPicWidth > rcCell.Width() || nPicHeight > rcCell.Height())
			{
				// display picture using IPicture::Render
				m_pPictureHolder->Render(hdc, 0, 0, rcCell.Width(), rcCell.Height(), 0, hmHeight, hmWidth, -hmHeight, &rcCell);
			}
			
			else
			{
				// display picture using IPicture::Render
				m_pPictureHolder->Render(hdc, nPicLeft, nPicTop, nPicWidth, nPicHeight, 0, hmHeight, hmWidth, -hmHeight, &rcCell);
			}
		}
	}
	catch(...)
	{
	}
}


int CPictureBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CPictureBox::SetPictureBlank();
	
	return 0;
}

void CPictureBox::SetPictureBlank() 
{
	m_pPictureHolder = NULL;	
	Invalidate(TRUE);
}

void CPictureBox::SetAcadColor(long nColorArg)
{
	if (nColorArg > 255)
	{
		m_BkColor = nColorArg;
	}
	else if (nColorArg >= 0 && nColorArg <= 255)
	{
		m_BkColor = RGB(acadcol[nColorArg][0], acadcol[nColorArg][1], acadcol[nColorArg][2]);		
	}
	else if (nColorArg < 0 && nColorArg >= -21)
	{
		int nSystemColor = (nColorArg * -1) - 1; 
		m_BkColor = GetSysColor(nSystemColor);
	}
	else if (nColorArg == -22)
	{
		m_BkColor = RGB(0,0,0);
	}
	else 
	{
		m_BkColor = RGB(255,255,255);
	}
	Invalidate(TRUE);
}

void CPictureBox::AutoSize()
{
	if (!m_AutoSize)
		return;

	if (m_AutoSize && 
		m_PictureID == 0
		)
	{
		return;
	}

	if (m_AutoSize)
	{
		CRect rcThis;
		GetWindowRect(&rcThis);
		rcThis.left = 0;
		rcThis.top = 0;
		
		rcThis.right = m_cxIcon;
		rcThis.bottom = m_cyIcon;
		
		MoveWindow(rcThis, TRUE);
	}
}
