// ArxAcadSlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxAcadSlideCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "MemDC.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAcadSlideCtrl

CArxAcadSlideCtrl::CArxAcadSlideCtrl( CControlPane& Pane, TDclControlPtr pTemplate,
																			UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, &Pane, this )
, mArxServices( this )
, mDragDropService( this )
, mArxSlide( *this )
, mbTrackingMouse( false )
, mhbmLast( NULL )
, mhbmSaved( NULL )
, mclrHighlight( CAcadColorService::GetTransparentColor() )
{
	if( bCreate )
		Create( Pane.GetHostDialog(), nID );
}

CArxAcadSlideCtrl::~CArxAcadSlideCtrl()
{
	if( mhbmLast )
		::DeleteObject( mhbmLast );
	if( mhbmSaved )
		::DeleteObject( mhbmSaved );
}

COLORREF CArxAcadSlideCtrl::CArxSlide::getBackgroundColor() const
{
	CAcadColorService* pColorService = mCtrl.GetColorService();
	if( !pColorService )
		return RGB(0,0,0);
	return pColorService->GetBackgroundColor();
}

bool CArxAcadSlideCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CArxAcadSlideCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	return (dwStyle | BS_OWNERDRAW | BS_NOTIFY);
}

void CArxAcadSlideCtrl::SetHighlight(const COLORREF& clrHighlight)
{
	mclrHighlight = clrHighlight;		
	OnNeedRepaint( false );
}

void CArxAcadSlideCtrl::RemoveHighlight()
{
	mclrHighlight = CAcadColorService::GetTransparentColor();
	OnNeedRepaint( false );
}

void CArxAcadSlideCtrl::DrawLine(int sX, int sY, int eX, int eY, const COLORREF& rgb)
{
	CDC* pDC = GetDC();
	if( !pDC )
		return;
	int nDC = pDC->SaveDC();
	CRect rcClient;
	GetClientRect( &rcClient );
	pDC->IntersectClipRect( &rcClient );
	CMemDCx dc( pDC, rcClient );
	if( !mhbmLast )
	{
		if( mhbmSaved )
		{
			CMemDCx dcSaved( pDC, rcClient );
			CBitmap* pOldBmp = dcSaved.SelectObject( CBitmap::FromHandle( mhbmSaved ) );
			mhbmLast = CreateCompatibleBitmap( dc.GetSafeHdc(), rcClient.Width(), rcClient.Height() ); 
			CBitmap* pOldBmp2 = dc.SelectObject( CBitmap::FromHandle( mhbmLast ) );
			dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &dcSaved, 0, 0, SRCCOPY );
			dcSaved.SelectObject( pOldBmp );
			dc.SelectObject( pOldBmp2 );
		}
		else
			RedrawWindow();
	}
	CRect rcCell( sX, sY, eX, eY );
	CBitmap* pBmp = CBitmap::FromHandle( mhbmLast );
	CBitmap* pOldBmp = dc.SelectObject( pBmp );
	CPen pen( PS_SOLID, 1, rgb );
	dc.SelectObject( &pen );
	dc.MoveTo( sX, sY );
	dc.LineTo( eX, eY );		
	dc.SetPixel( sX, sY, rgb );
	dc.SetPixel( eX, eY, rgb );
	dc.SelectObject( pOldBmp );
	pDC->RestoreDC( nDC );
	ReleaseDC( pDC );
	OnNeedRepaint( false, true );
}

void CArxAcadSlideCtrl::DrawFillRect(int sX, int sY, int eX, int eY, const COLORREF& rgb)
{
	CDC* pDC = GetDC();
	if( !pDC )
		return;
	int nDC = pDC->SaveDC();
	CRect rcClient;
	GetClientRect( &rcClient );
	pDC->IntersectClipRect( &rcClient );
	CMemDCx dc( pDC, rcClient );
	if( !mhbmLast )
	{
		if( mhbmSaved )
		{
			CMemDCx dcSaved( pDC, rcClient );
			CBitmap* pOldBmp = dcSaved.SelectObject( CBitmap::FromHandle( mhbmSaved ) );
			mhbmLast = CreateCompatibleBitmap( dc.GetSafeHdc(), rcClient.Width(), rcClient.Height() ); 
			CBitmap* pOldBmp2 = dc.SelectObject( CBitmap::FromHandle( mhbmLast ) );
			dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &dcSaved, 0, 0, SRCCOPY );
			dcSaved.SelectObject( pOldBmp );
			dc.SelectObject( pOldBmp2 );
		}
		else
			RedrawWindow();
	}
	CBitmap* pBmp = CBitmap::FromHandle( mhbmLast );
	CBitmap* pOldBmp = dc.SelectObject( pBmp );
	dc.SetBkColor( rgb );
	CRect rcCell( sX, sY, eX, eY );
	dc.ExtTextOut( 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL );
	dc.SelectObject( pOldBmp );
	pDC->RestoreDC( nDC );
	ReleaseDC( pDC );
	OnNeedRepaint( false, true );
}

void CArxAcadSlideCtrl::DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, LPCTSTR pszFilename, LPCTSTR pszSlideName)
{
	CDC* pDC = GetDC();
	if( !pDC )
		return;
	CRect rcCell(nX, nY, nX + nSlideWidth, nY + nSlideHeight);
	CArxSlide Slide( *this );
	Slide.Load( theWorkspace.FindFile( pszFilename ), pszSlideName);
	CRect rcClient;
	GetClientRect( &rcClient );
	CMemDCx dc( pDC, rcClient );
	if( !mhbmLast )
	{
		if( mhbmSaved )
		{
			CMemDCx dcSaved( pDC, rcClient );
			CBitmap* pOldBmp = dcSaved.SelectObject( CBitmap::FromHandle( mhbmSaved ) );
			mhbmLast = CreateCompatibleBitmap( dc.GetSafeHdc(), rcClient.Width(), rcClient.Height() ); 
			CBitmap* pOldBmp2 = dc.SelectObject( CBitmap::FromHandle( mhbmLast ) );
			dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &dcSaved, 0, 0, SRCCOPY );
			dcSaved.SelectObject( pOldBmp );
			dc.SelectObject( pOldBmp2 );
		}
		else
			RedrawWindow();
	}
	CBitmap* pBmp = CBitmap::FromHandle( mhbmLast );
	CBitmap* pOldBmp = dc.SelectObject( pBmp );
	Slide.Draw( &dc, rcCell );
	dc.SelectObject( pOldBmp );
	ReleaseDC( pDC );
	OnNeedRepaint( false, true );
}

void CArxAcadSlideCtrl::SaveDC()
{
	HDC hDC = ::GetDC( m_hWnd );
	HDC hDCmem = CreateCompatibleDC (hDC) ; 
	CRect rcClient;
	GetClientRect( &rcClient );
	int nSourceX = rcClient.Width();
	int nSourceY = rcClient.Height(); 

	// Create a compatible bitmap
	HBITMAP hbmMem = CreateCompatibleBitmap( hDC, nSourceX, nSourceY ); 
	HGDIOBJ hbmOld = SelectObject( hDCmem, hbmMem ) ; 
	BitBlt( hDCmem, 0, 0, nSourceX, nSourceY, hDC, 0, 0, SRCCOPY );

	SelectObject( hDCmem, hbmOld );
	DeleteObject( hbmOld );
	DeleteDC( hDCmem );
	::ReleaseDC( m_hWnd, hDC );
	if( mhbmLast )
		DeleteObject( mhbmLast );
	mhbmLast = hbmMem;
}

void CArxAcadSlideCtrl::Snapshot() 
{
	if( mhbmSaved )
	{
		DeleteObject( mhbmSaved );
		mhbmSaved = NULL;
	}
	if( !mhbmLast )
		RedrawWindow();
	if( mhbmSaved )
	{
		DeleteObject( mhbmSaved );
		mhbmSaved = NULL;
	}
	mhbmSaved = mhbmLast;
	mhbmLast = NULL;
}

bool CArxAcadSlideCtrl::SetFileName( LPCTSTR pszFilename, LPCTSTR pszSlide )
{
	if( mhbmSaved )
	{
		DeleteObject( mhbmSaved );
		mhbmSaved = NULL;
	}
	if( mhbmLast )
	{
		DeleteObject( mhbmLast );
		mhbmLast = NULL;
	}
	if( !mArxSlide.Load( theWorkspace.FindFile( pszFilename ), pszSlide ) )
		return false;
	OnNeedRepaint( false );
	return true;
}

void CArxAcadSlideCtrl::Clear()
{
	if( mhbmSaved )
	{
		DeleteObject( mhbmSaved );
		mhbmSaved = NULL;
	}
	if( mhbmLast )
	{
		DeleteObject( mhbmLast );
		mhbmLast = NULL;
	}
	RemoveHighlight();
	mArxSlide.Load( NULL );
	RedrawWindow();
}

void CArxAcadSlideCtrl::PaintControl(CDC *pdc)
{
	CRect rcCell;
	GetClientRect(&rcCell);

	int nThisWidth = rcCell.Width();
	int nThisHeight = rcCell.Height();
		
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, mColorService.GetBackgroundCBrush());
	
	if( !mArxSlide.GetSlideName().IsEmpty() )
	{
		try
		{
			CSize sizeSlide( mArxSlide.GetSlideSize() );
			double dFactor;
			double dH;
			double dW;

			dFactor = (double)sizeSlide.cy / (double)sizeSlide.cx;
			dH = dFactor;
			dW = 1.0;

			int nDrawWidth = dW * nThisWidth;
			int nDrawHeight = dH * nDrawWidth;
			
			// if the calc height is too large
			if (nDrawHeight > nThisHeight)
			{
				dFactor = (double)sizeSlide.cx / (double)sizeSlide.cy;
				dH = 1.0;
				dW = dFactor;

				nDrawHeight = dH * nThisHeight;
				nDrawWidth = dW * nDrawHeight;

				rcCell.left = (nThisWidth - nDrawWidth) / 2;
				rcCell.right = nThisWidth - rcCell.left;
			}
			else if (nDrawHeight < nThisHeight)
			{
				rcCell.top = (nThisHeight - nDrawHeight) / 2;
				rcCell.bottom = nThisHeight - rcCell.top;
			}
			
			// if the calc width is too large
			if (nDrawWidth > nThisWidth)
			{
				dFactor = (double)sizeSlide.cy / (double)sizeSlide.cx;
				dH = dFactor;
				dW = 1.0;

				nDrawWidth = dW * nThisWidth;
				nDrawHeight = (double)nDrawWidth * dH;
				
				rcCell.left = 1;
				rcCell.right = nThisWidth;
				rcCell.top = (nThisHeight - nDrawHeight) / 2;
				rcCell.bottom = nThisHeight - rcCell.top;
			}
			// draw the slide
			mArxSlide.Draw(pdc, rcCell);
		}
		catch(...)
		{
		}
	}
	SaveDC();
}


BEGIN_MESSAGE_MAP(CArxAcadSlideCtrl, CButton)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   	
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxAcadSlideCtrl message handlers

void CArxAcadSlideCtrl::OnPaint() 
{
	CPaintDC dc( this ); // Device context for painting
	CRect rcClient;
	GetClientRect( &rcClient );
	if( mhbmSaved || mhbmLast )
	{
		CDC memdc;
		memdc.CreateCompatibleDC( &dc );
		CBitmap* pSavedBmp = memdc.SelectObject( CBitmap::FromHandle( mhbmLast? mhbmLast : mhbmSaved ) );
		// Copy (BitBlt) bitmap from memory DC to screen DC
		dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &memdc, 0, 0, SRCCOPY );
    memdc.SelectObject( pSavedBmp );
	}
	else
		PaintControl( &dc );

	GetArxServices()->HandleEvent( Prop::EventPaint, args_B( (GetFocus() == this) ) );

	if( !CAcadColorService::IsTransparentColor( mclrHighlight ) )
	{
		CRect rcCell;
		GetClientRect( &rcCell );
		rcCell.DeflateRect( 1, 1 );
		
		CPen pen( PS_SOLID, 1, mclrHighlight );
		CPen* pOldPen = dc.SelectObject( &pen );
		dc.MoveTo( rcCell.TopLeft() );
		dc.LineTo( rcCell.right, rcCell.top );		
		dc.LineTo( rcCell.right, rcCell.bottom );		
		dc.LineTo( rcCell.left, rcCell.bottom );		
		dc.LineTo( rcCell.left, rcCell.top );		
		dc.SelectObject( pOldPen );
	}

	if( GetFocus() == this )
	{
		CRect rcFocus;
		GetClientRect( &rcFocus );
		rcFocus.DeflateRect( 2, 2 );
		dc.DrawFocusRect( rcFocus );
	}
}

void CArxAcadSlideCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 2, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventRightDblClick );
	CWnd::OnRButtonDblClk(nFlags, point);
}

void CArxAcadSlideCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 2, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventRightClick );	
	GetArxServices()->HandleEvent( Prop::EventRMouse, args_NNNN( 2, nFlags, point.x, point.y ) );
	CWnd::OnRButtonUp(nFlags, point);
}

void CArxAcadSlideCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar != _T('\n') && nChar != _T('\r') )
		__super::OnChar(nChar, nRepCnt, nFlags);
	else
		GetArxServices()->HandleEvent( Prop::EventClicked );
}

void CArxAcadSlideCtrl::OnClicked() 
{
	CString sEvent = mpTemplate->GetStringProperty(Prop::EventClicked);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		resbuf rbEvent = { NULL, RTSTR };
		rbEvent.resval.rstring = sEvent.LockBuffer();
		resbuf* prbResult = NULL;
		acedInvoke( &rbEvent, &prbResult ); 
		if( prbResult ) 
			acutRelRb( prbResult ); 
		GetParent()->GetParent()->EnableWindow( FALSE );
		GetParent()->EnableWindow( TRUE );
	}
	else
		GetArxServices()->HandleEvent( sEvent );	
}

void CArxAcadSlideCtrl::OnDoubleclicked() 
{
	GetArxServices()->HandleEvent( Prop::EventDblClicked );	
}

void CArxAcadSlideCtrl::OnDestroy() 
{
	__super::OnDestroy();
}

void CArxAcadSlideCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus( pOldWnd );
	OnNeedRepaint( false );
	GetArxServices()->HandleEvent( Prop::EventSetFocus );	
}

void CArxAcadSlideCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus( pNewWnd );
	OnNeedRepaint( true );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

BOOL CArxAcadSlideCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxAcadSlideCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if( mhbmSaved )
	{
		DeleteObject( mhbmSaved );
		mhbmSaved = NULL;
	}
	if( mhbmLast )
	{
		DeleteObject( mhbmLast );
		mhbmLast = NULL;
	}
}

void CArxAcadSlideCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) && nFlags == MK_LBUTTON)
	{
		BeginDragDrop( point );
		GetArxServices()->HandleEvent( Prop::EventClicked );	
		return;
	}
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventLMouse, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonDown(nFlags, point);
}

void CArxAcadSlideCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 1, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventLMouse, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonUp(nFlags, point);
}

void CArxAcadSlideCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 4, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventMMouse, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxAcadSlideCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventMMouse, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDown(nFlags, point);
}

void CArxAcadSlideCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 4, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventMMouse, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonUp(nFlags, point);
}

void CArxAcadSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);

	if( !mbTrackingMouse )
	{
		GetArxServices()->HandleEvent( Prop::EventMouseEntered );
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		if( _TrackMouseEvent( &tm ) )
			mbTrackingMouse = true;
	}
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
}

BOOL CArxAcadSlideCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseWheel, args_NNNN( nFlags, zDelta, pt.x, pt.y ) );
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CArxAcadSlideCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventRMouse, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonDown(nFlags, point);
}

void CArxAcadSlideCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxAcadSlideCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CArxAcadSlideCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMovedOff );
	mbTrackingMouse = false;        
	return FALSE;
}

void CArxAcadSlideCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 1, nFlags, point.x, point.y ) );
	GetArxServices()->HandleEvent( Prop::EventLMouse, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonDblClk(nFlags, point);
}

void CArxAcadSlideCtrl::OnSysColorChange() 
{
	__super::OnSysColorChange();
	OnApplyBackgroundColor( mpTemplate->GetPropertyObject( Prop::BackgroundColor ) );
}

void CArxAcadSlideCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
