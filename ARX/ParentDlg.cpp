// ParentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ParentDlg.h"
#include "InvokeMethod.h"
#include "DclControlObject.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CParentDlg dialog

CParentDlg::CParentDlg(CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/)
: CCommonDialog(pParent)
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm )
, mpControl( NULL )
{
	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);
}


void CParentDlg::DoDataExchange(CDataExchange* pDX)
{
	CCommonDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParentDlg, CCommonDialog)
	//{{AFX_MSG_MAP(CParentDlg)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParentDlg message handlers

void CParentDlg::OnCancel() 
{
	CCommonDialog::OnCancel();
}

void CParentDlg::OnOK() 
{	
	CCommonDialog::OnOK();	
}


void CParentDlg::OnDestroy() 
{
	InvokeMethodIntInt(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventClose), -1, -1, false);	
	CCommonDialog::OnDestroy();
	
}

void CParentDlg::OnSize(UINT nType, int cx, int cy) 
{
	CCommonDialog::OnSize(nType, cx, cy);
	
	CRect rcThis;
	CWnd::GetClientRect(&rcThis);

	
	if (CWnd::IsWindowVisible())	
	{
		// call methods to invoke the event
		InvokeMethodIntInt(
			mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}

	UpdateGripPos();

	mControlPane.SizeChanged(rcThis.Width(), rcThis.Height(), false);
	
	
	
}

void CParentDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_bShowGrip && !IsZoomed())
	{
		// draw size-grip
		dc.DrawFrameControl(&m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}
	
}


UINT CParentDlg::OnNcHitTest(CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);

	// if in size grip and in client area
	if (m_bShowGrip && m_rcGripRect.PtInRect(pt) &&
		pt.x >= 0 && pt.y >= 0)
		return HTBOTTOMRIGHT;
	
	return CDialog::OnNcHitTest(point);
}


void CParentDlg::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	
	if (!m_bShowGrip)
		return;
	
	//InvalidateRect(&m_rcGripRect);
	HDC hdc = ::GetDC(m_hWnd);
	
	// draw the solid rectangle
	::SetBkColor(hdc, ::GetSysColor(COLOR_BTNFACE));
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &m_rcGripRect, NULL, 0, NULL);
	
	
	GetClientRect(&m_rcGripRect);

	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;

	// draw size-grip
	DrawFrameControl(hdc, &m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
	
}

// protected members

void CParentDlg::ShowSizeGrip(BOOL bShow)
{
	if (m_bShowGrip != bShow)
	{
		m_bShowGrip = bShow;
		InvalidateRect(&m_rcGripRect);
	}
}