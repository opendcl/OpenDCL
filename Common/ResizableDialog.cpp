// ResizableDialog.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000 by Paolo Messina
// (ppescher@yahoo.com)
//
// Free for non-commercial use.
// You may change the code to your needs,
// provided that credits to the original
// author is given in the modified files.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResizableDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CResizableDialog

inline void CResizableDialog::Construct()
{
	m_bInitDone = FALSE;

	m_bUseMinTrack = TRUE;
	m_bUseMaxTrack = FALSE;
	m_bUseMaxRect = FALSE;

	m_bShowGrip = TRUE;

	m_bEnableSaveRestore = FALSE;

	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);
}

CResizableDialog::CResizableDialog()
{
	Construct();
}

CResizableDialog::CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd)
{
	Construct();
}

CResizableDialog::CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: CDialog(lpszTemplateName, pParentWnd)
{
	Construct();
}

CResizableDialog::~CResizableDialog()
{
	// free memory used by the layout
	Layout *pl;

	POSITION pos = m_plLayoutList.GetHeadPosition();

	while (pos != NULL)
	{
		pl = (Layout*)m_plLayoutList.GetNext(pos);
		delete pl;
	}
}


BEGIN_MESSAGE_MAP(CResizableDialog, CDialog)
	//{{AFX_MSG_MAP(CResizableDialog)
	ON_WM_NCHITTEST()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResizableDialog message handlers


BOOL CResizableDialog::OnInitDialog()
{
	__super::OnInitDialog();

	UpdateGripPos();

	// gets the template size as the min track size
	CRect rc;
	GetWindowRect(&rc);
	m_ptMinTrackSize.x = rc.Width();
	m_ptMinTrackSize.y = rc.Height();

	m_bInitDone = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CResizableDialog::OnDestroy()
{
	__super::OnDestroy();

	if (m_bEnableSaveRestore)
		SaveWindowRect();
}

BOOL CResizableDialog::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = __super::OnEraseBkgnd(pDC);
	if( m_bShowGrip && !IsZoomed() )
	{
		int nBlock = m_rcGripRect.Width() / 8;
		for( int nRow = 2; nRow >= 0; --nRow )
		{
			for( int nCol = (2 - nRow); nCol >= 0; --nCol )
			{
				CPoint ptLR( m_rcGripRect.BottomRight() - CSize( (nCol * 2 + 1) * nBlock, (nRow * 2 + 1) * nBlock ) );
				CRect rcBlock( ptLR - CSize( nBlock, nBlock ), ptLR );
				pDC->FillSolidRect( &rcBlock, GetSysColor( COLOR_BTNSHADOW ) );
			}
		}
	}
	return bResult;
}

void CResizableDialog::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;		// arrangement not needed

	if (m_bInitDone)
	{
		ArrangeLayout();
	}
}

__UINT_LRESULT CResizableDialog::OnNcHitTest(CPoint point)
{
	CPoint pt = point;
	ScreenToClient( &pt );
	if( pt.x >= 0 && pt.y >= 0 &&
			pt.x <= m_rcGripRect.right && pt.y <= m_rcGripRect.bottom )
	{ // if in client area
		if( (m_rcGripRect.right - pt.x) + (m_rcGripRect.bottom - pt.y) <= m_rcGripRect.Width() )
			return HTBOTTOMRIGHT; // and in size grip
	}

	return __super::OnNcHitTest(point);
}

void CResizableDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	if (!m_bInitDone)
		return;

	if (m_bUseMinTrack)
		lpMMI->ptMinTrackSize = m_ptMinTrackSize;

	if (m_bUseMaxTrack)
		lpMMI->ptMaxTrackSize = m_ptMaxTrackSize;

	if (m_bUseMaxRect)
	{
		lpMMI->ptMaxPosition = m_ptMaxPos;
		lpMMI->ptMaxSize = m_ptMaxSize;
	}
}

// layout functions

void CResizableDialog::AddAnchor(HWND wnd, CSize tl_type, CSize br_type)
{
	ASSERT(wnd != NULL && ::IsWindow(wnd));
	ASSERT(::IsChild(*this, wnd));
	ASSERT(tl_type != NOANCHOR);

	// get control's window class

	CString st;
	GetClassName(wnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();
	st.MakeUpper();


	// wnd classes that don't redraw client area correctly
	// when the hor scroll pos changes due to a resizing
	BOOL hscroll = FALSE;

	// wnd classes that need refresh when resized
	BOOL refresh = FALSE;

	// get dialog's and control's rect
	CRect wndrc, objrc;

	GetClientRect(&wndrc);
	::GetWindowRect(wnd, &objrc);
	ScreenToClient(&objrc);

	CSize tl_margin, br_margin;

	if (br_type == NOANCHOR)
		br_type = tl_type;

	// calculate margin for the top-left corner

	tl_margin.cx = objrc.left - wndrc.Width() * tl_type.cx / 100;
	tl_margin.cy = objrc.top - wndrc.Height() * tl_type.cy / 100;

	// calculate margin for the bottom-right corner

	br_margin.cx = objrc.right - wndrc.Width() * br_type.cx / 100;
	br_margin.cy = objrc.bottom - wndrc.Height() * br_type.cy / 100;

	// add to the list
	m_plLayoutList.AddTail(new Layout(wnd, tl_type, tl_margin,
		br_type, br_margin, hscroll, refresh));
}

void CResizableDialog::ArrangeLayout()
{
	// init some vars
	CRect wndrc;
	GetClientRect(&wndrc);

	Layout *pl;
	POSITION pos = m_plLayoutList.GetHeadPosition();

	HDWP hdwp = BeginDeferWindowPos((int)m_plLayoutList.GetCount());

	while (pos != NULL)
	{
		pl = (Layout*)m_plLayoutList.GetNext(pos);

		CRect objrc, newrc;
		CWnd* wnd = CWnd::FromHandle(pl->hwnd); // temporary solution

		wnd->GetWindowRect(&objrc);
		ScreenToClient(&objrc);

		// calculate new top-left corner

		newrc.left = pl->tl_margin.cx + wndrc.Width() * pl->tl_type.cx / 100;
		newrc.top = pl->tl_margin.cy + wndrc.Height() * pl->tl_type.cy / 100;

		// calculate new bottom-right corner

		newrc.right = pl->br_margin.cx + wndrc.Width() * pl->br_type.cx / 100;
		newrc.bottom = pl->br_margin.cy + wndrc.Height() * pl->br_type.cy / 100;

		if (!newrc.EqualRect(&objrc))
		{
			BOOL add = TRUE;

			if (pl->adj_hscroll)
			{
				// needs repainting, due to horiz scrolling
				int diff = newrc.Width() - objrc.Width();
				int max = wnd->GetScrollLimit(SB_HORZ);

				if (max > 0 && wnd->GetScrollPos(SB_HORZ) > max - diff)
				{

					wnd->MoveWindow(&newrc);
					wnd->Invalidate();
					//wnd->UpdateWindow();

					//add = FALSE;
				}
			}

			if (pl->need_refresh)
			{
				wnd->MoveWindow(&newrc);
				wnd->Invalidate();
				//wnd->UpdateWindow();

				//add = FALSE;
			}

			if (add)
				DeferWindowPos(hdwp, pl->hwnd, NULL, newrc.left, newrc.top,
					newrc.Width(), newrc.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	// update size-grip
	InvalidateRect(&m_rcGripRect);
	UpdateGripPos();
	InvalidateRect(&m_rcGripRect);

	// go re-arrange child windows
	EndDeferWindowPos(hdwp);
}

void CResizableDialog::UpdateGripPos()
{
	// size-grip goes bottom right in the client area

	GetClientRect(&m_rcGripRect);

	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;
}

// protected members

void CResizableDialog::ShowSizeGrip(BOOL bShow)
{
	if (m_bShowGrip != bShow)
	{
		m_bShowGrip = bShow;
		InvalidateRect(&m_rcGripRect);
	}
}

void CResizableDialog::SetMaximizedRect(const CRect& rc)
{
	m_bUseMaxRect = TRUE;

	m_ptMaxPos = rc.TopLeft();
	m_ptMaxSize.x = rc.Width();
	m_ptMaxSize.y = rc.Height();
}

void CResizableDialog::ResetMaximizedRect()
{
	m_bUseMaxRect = FALSE;
}

void CResizableDialog::SetMinTrackSize(const CSize& size)
{
	m_bUseMinTrack = TRUE;

	m_ptMinTrackSize.x = size.cx;
	m_ptMinTrackSize.y = size.cy;
}

void CResizableDialog::ResetMinTrackSize()
{
	m_bUseMinTrack = FALSE;
}

void CResizableDialog::SetMaxTrackSize(const CSize& size)
{
	m_bUseMaxTrack = TRUE;

	m_ptMaxTrackSize.x = size.cx;
	m_ptMaxTrackSize.y = size.cy;
}

void CResizableDialog::ResetMaxTrackSize()
{
	m_bUseMaxTrack = FALSE;
}

// NOTE: this must be called after all the other settings
//       to have the dialog and its controls displayed properly
void CResizableDialog::EnableSaveRestore(LPCTSTR pszSection, LPCTSTR pszEntry)
{
	m_sSection = pszSection;
	m_sEntry = pszEntry;

	m_bEnableSaveRestore = TRUE;

	LoadWindowRect();
}


// used to save/restore window's size and position
// either in the registry or a private .INI file
// depending on your application settings

#define PROFILE_FMT 	_T("%ld,%ld,%ld,%ld,%ud,%ud")

void CResizableDialog::SaveWindowRect()
{
	CString data;
	WINDOWPLACEMENT wp;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);

	RECT& rc = wp.rcNormalPosition;	// alias

	data.Format(PROFILE_FMT, rc.left, rc.top,
		rc.right, rc.bottom, wp.showCmd, wp.flags);

	AfxGetApp()->WriteProfileString(m_sSection, m_sEntry, data);
}

void CResizableDialog::LoadWindowRect()
{
	CString data;
	WINDOWPLACEMENT wp;

	data = AfxGetApp()->GetProfileString(m_sSection, m_sEntry);

	if (data.IsEmpty())	// never saved before
		return;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);

	RECT& rc = wp.rcNormalPosition;	// alias

	if (_stscanf(data, PROFILE_FMT, &rc.left, &rc.top,
		&rc.right, &rc.bottom, &wp.showCmd, &wp.flags) == 6)
	{
		SetWindowPlacement(&wp);
	}
}
