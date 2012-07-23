// TipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TipWnd.h"


#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW       0x00020000
#endif


/////////////////////////////////////////////////////////////////////////////
// CTipWnd

CTipWnd::CTipWnd()
: mclrForeground( GetSysColor( COLOR_INFOTEXT ) )
, mclrBackground( GetSysColor( COLOR_INFOBK ) )
{
}

CTipWnd::CTipWnd(COLORREF clrForeground, COLORREF clrBackground)
: mclrForeground( clrForeground )
, mclrBackground( clrBackground )
{
}

CTipWnd::~CTipWnd()
{
}

BOOL CTipWnd::Create( CWnd* pParentWnd ) 
{
	HWND hwndParent = pParentWnd? pParentWnd->m_hWnd : ::GetDesktopWindow();
	DWORD dwStyle = (WS_BORDER | WS_POPUP);
	DWORD dwExStyle = (WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
	BOOL bSuccess = CreateEx( dwExStyle, AfxRegisterWndClass( CS_DROPSHADOW ), NULL, dwStyle, 0, 0, 0, 0, hwndParent, NULL );
	if( !bSuccess )
		return FALSE;

	LOGFONT lfDefault;
	::GetObject( (HFONT)GetStockObject( DEFAULT_GUI_FONT ), sizeof(lfDefault), &lfDefault );
	mFont.CreateFontIndirect( &lfDefault );

	return TRUE;
}

void CTipWnd::Show(const CPoint& ptTip, const CSize& szTip)
{
	CRect rcWindow;
	GetWindowRect( &rcWindow );
	CRect rcClient;
	GetClientRect( &rcClient );
	int nDeltaX = rcWindow.Width() - rcClient.Width();
	int nDeltaY = rcWindow.Height() - rcClient.Height();
	CPoint ptWindowOrg( ptTip );
	ScreenToClient( &rcWindow );
	ptWindowOrg.Offset( rcWindow.left, rcWindow.top );
	SetWindowPos( NULL, ptWindowOrg.x, ptWindowOrg.y, szTip.cx + nDeltaX, szTip.cy + nDeltaY, SWP_SHOWWINDOW | SWP_NOACTIVATE );
}

void CTipWnd::Show()
{
	ShowWindow( SW_SHOWNA );
}

void CTipWnd::Hide()
{
	ShowWindow( SW_HIDE );
}

void CTipWnd::Paint(CDC* pDC)
{
	//paint background
	CRect rcClip;
	pDC->GetClipBox( &rcClip );
	CBrush brBackground( mclrBackground );
	CBrush* pOldBrush = pDC->SelectObject( &brBackground );
	pDC->PatBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), PATCOPY );
	pDC->SelectObject( pOldBrush );

	//paint foreeground
	CString sText;
	GetWindowText( sText );
	CFont* pOldFont = pDC->SelectObject( &mFont );
	CRect rcTip;
	GetClientRect( &rcTip );
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextColor( mclrForeground );
	pDC->DrawText( sText, &rcTip, DT_SINGLELINE | DT_VCENTER | DT_CENTER );
	pDC->SelectObject( pOldFont );
}


BEGIN_MESSAGE_MAP(CTipWnd, CWnd)
	ON_MESSAGE(WM_GETFONT, &CTipWnd::OnGetFont)
	ON_MESSAGE(WM_SETFONT, &CTipWnd::OnSetFont)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTipWnd message handlers

LRESULT CTipWnd::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)mFont.m_hObject;
}

LRESULT CTipWnd::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	LOGFONT lf = { NULL };
	CFont::FromHandle( (HFONT)wParam )->GetLogFont( &lf );
	mFont.DeleteObject();
	mFont.CreateFontIndirect(&lf);
	if( LOWORD(lParam) )
		RedrawWindow();
	return 0;
}

BOOL CTipWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CTipWnd::OnPaint() 
{
	CPaintDC dc(this);
	Paint( &dc );
}

__UINT_LRESULT CTipWnd::OnNcHitTest(CPoint point)
{
	return HTTRANSPARENT;
}
