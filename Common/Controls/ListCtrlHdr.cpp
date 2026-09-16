// ListCtrlHdr.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlHdr.h"
#include "HostThemeHelper.h"
#include "ColorService.h"

#define HDM_GETBITMAPMARGIN (HDM_FIRST + 21)


/////////////////////////////////////////////////////////////////////////////
// CListCtrlHdr

CListCtrlHdr::CListCtrlHdr() : mTipWnd(this), mbTrackingMouse(false)
{
}

CListCtrlHdr::~CListCtrlHdr()
{
}

void CListCtrlHdr::CHdrTipWnd::Track(const CPoint& point)
{
	mnCol = -1;
	if (IsWindowVisible())
	{
		if (!mpHdrCtrl->TooltipHitTest(point, mnCol))
			Hide();
	}
}

void CListCtrlHdr::CHdrTipWnd::Show(int nCol)
{
	if (IsWindowVisible() && nCol == mnCol)
		return;
	mnCol = nCol;
	CRect rcCell;
	if (!mpHdrCtrl->GetItemRect(nCol, &rcCell))
	{
		Hide();
		return;
	}
	CString sLabel;
	HDITEM hdi = { HDI_TEXT | HDI_FORMAT, 0, sLabel.GetBuffer(1024), NULL, 1024 };
	mpHdrCtrl->GetItem(nCol, &hdi);
	sLabel.ReleaseBuffer();
	if (sLabel.IsEmpty())
	{
		Hide();
		return;
	}
	int nNonLabelWidth = 12;
	if ((hdi.fmt & (HDF_BITMAP | HDF_IMAGE)) != 0)
	{
		nNonLabelWidth += mpHdrCtrl->SendMessage(HDM_GETBITMAPMARGIN, 0, 0) * 2;
		CImageList* pImageList = mpHdrCtrl->GetImageList();
		if (pImageList)
		{
			IMAGEINFO imgInfo = { 0 };
			if (pImageList->GetImageInfo(0, &imgInfo))
				nNonLabelWidth += CRect(&imgInfo.rcImage).Width();
		}
	}
	CFont* pFont = mpHdrCtrl->GetFont();
	CDC* pDC = mpHdrCtrl->GetDC();
	CFont* pOldFont = pDC->SelectObject(pFont);
	CRect rcText;
	pDC->DrawText(sLabel, -1, &rcText, DT_CALCRECT);
	pDC->SelectObject(pOldFont);
	mpHdrCtrl->ReleaseDC(pDC);
	CSize sizCell = rcText.Size();
	if (rcCell.Width() < (sizCell.cx + nNonLabelWidth) || rcCell.Height() < sizCell.cy)
	{
		if (!m_hWnd)
			Create();
		SetFont(pFont);
		SetWindowText(sLabel);
		mpHdrCtrl->ClientToScreen(&rcCell);
		if (rcCell.Width() < sizCell.cx + 12)
			rcCell.right = rcCell.left + sizCell.cx + 12;
		if (rcCell.Height() < sizCell.cy + 6)
			rcCell.bottom = rcCell.top + sizCell.cy + 6;
		__super::Show(rcCell.TopLeft(), rcCell.Size());
	}
	else
		Hide();
}

bool CListCtrlHdr::TooltipHitTest(const CPoint& point, int& nCol) const
{
__if_not_exists(CHeaderCtrl::HitTest)
{
	return false;
}
__if_exists(CHeaderCtrl::HitTest)
{
	nCol = -1;
	HDHITTESTINFO htiHeader = { point, 0, -1 };
	nCol = const_cast< CListCtrlHdr* >(this)->HitTest(&htiHeader);
	return (htiHeader.flags == HHT_ONHEADER);
}
}

void CListCtrlHdr::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	EnableToolTips(FALSE);
	mTipWnd.Create();
}

BOOL CListCtrlHdr::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL bSuccess = __super::Create(dwStyle, rect, pParentWnd, nID);
    if (bSuccess)
    {
        EnableToolTips(FALSE);
        mTipWnd.Create();
    }
	return bSuccess;
}


BEGIN_MESSAGE_MAP(CListCtrlHdr, CHeaderCtrl)
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, &CListCtrlHdr::OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, &CListCtrlHdr::OnMouseHover)
	ON_WM_PAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListCtrlHdr message handlers

void CListCtrlHdr::OnPaint()
{
	if( !CHostThemeHelper::HostMaps() )
	{
		__super::OnPaint();
		return;
	}
	CPaintDC dc( this );
	CRect rc;
	GetClientRect( &rc );
	dc.FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? dc.SelectObject( pFont ) : NULL;
	dc.SetTextColor( OdclSysColor( COLOR_BTNTEXT ) );
	dc.SetBkMode( TRANSPARENT );
	const int nCount = GetItemCount();
	for( int i = 0; i < nCount; ++i )
	{
		CRect rcItem;
		if( !GetItemRect( i, &rcItem ) )
			continue;
		TCHAR sz[256] = {};
		HDITEM item = {};
		item.mask = HDI_TEXT;
		item.pszText = sz;
		item.cchTextMax = 255;
		GetItem( i, &item );
		rcItem.DeflateRect( 6, 0 );
		dc.DrawText( sz, &rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
	}
	if( pOldFont )
		dc.SelectObject( pOldFont );
}

int CListCtrlHdr::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	mTipWnd.Hide();
	if (mbTrackingMouse)
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_HOVER | TME_LEAVE, m_hWnd, 0 };
		mbTrackingMouse = (_TrackMouseEvent(&tm) == FALSE);
	}
	return __super::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CListCtrlHdr::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);

	if (!mbTrackingMouse)
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_HOVER | TME_LEAVE, m_hWnd, 0 };
		mbTrackingMouse = (_TrackMouseEvent(&tm) != FALSE);
	}
	mTipWnd.Track(point);
}

LRESULT CListCtrlHdr::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	mbTrackingMouse = false;
	mTipWnd.Hide();
	return FALSE;
}

LRESULT CListCtrlHdr::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_HOVER | TME_LEAVE, m_hWnd, 0 };
	mbTrackingMouse = (_TrackMouseEvent(&tm) != FALSE);
	if (wParam != 0)
		mTipWnd.Hide();
	else
	{
		CPoint ptHover(lParam);
		int nCol = -1;
		if (TooltipHitTest(ptHover, nCol))
			mTipWnd.Show(nCol);
		else
			mTipWnd.Hide();
	}
	return FALSE;
}
