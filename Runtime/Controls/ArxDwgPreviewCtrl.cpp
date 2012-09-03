// ArxDwgPreviewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDwgPreviewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Workspace.h"


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


/////////////////////////////////////////////////////////////////////////////
// CArxDwgPreviewCtrl

CArxDwgPreviewCtrl::CArxDwgPreviewCtrl( TDclControlPtr pTemplate, CControlPane* pPane,
																				UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mArxServices( this )
, mDragDropService( this )
, mBlockInsertDropTarget( this )
, mhbmLast( NULL )
, mhbmSaved( NULL )
, mclrHighlight( CAcadColorService::GetTransparentColor() )
//, mbDrawSelected( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxDwgPreviewCtrl::~CArxDwgPreviewCtrl()
{
	if( mhbmLast )
		::DeleteObject( mhbmLast );
	if( mhbmSaved )
		::DeleteObject( mhbmSaved );
}

bool CArxDwgPreviewCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CArxDwgPreviewCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (BS_OWNERDRAW | BS_NOTIFY);
	return dwStyle;
}

bool CArxDwgPreviewCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::RenderMode:
	//	SetRenderMode( pProp->GetLongValue() );
	//	break;
	//case Prop::BlockName:
	//	DisplayBlock( pProp->GetStringValue() );
	//	break;
	//}
	return !bFailed;
}

DROPEFFECT CArxDwgPreviewCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwDropEffect = __super::OnBeginDrag( point, SourceData );
	if( msDwgFilename.IsEmpty() )
		return dwDropEffect;
	CStringA sTextA( msDwgFilename );
	SIZE_T cchText = sTextA.GetLength() + 1;
	HGLOBAL hData = GlobalAlloc( GHND, cchText );
	if( !hData )
		return dwDropEffect;
	lstrcpynA( (char*)GlobalLock( hData ), sTextA, cchText );
	GlobalUnlock( hData );
	SourceData.CacheGlobalData( CF_TEXT, hData );
	return DROPEFFECT_COPY;
}

void CArxDwgPreviewCtrl::SetHighlight(const COLORREF& clrHighlight)
{
	mclrHighlight = clrHighlight;		
	OnNeedRepaint( true );
}

void CArxDwgPreviewCtrl::RemoveHighlight()
{
	mclrHighlight = CAcadColorService::GetTransparentColor();
	OnNeedRepaint( true );
}

bool CArxDwgPreviewCtrl::LoadDwg( LPCTSTR pszFilename )
{
	Clear();
	if( !pszFilename || !*pszFilename )
		return true;

	msDwgFilename = theWorkspace.FindFile( pszFilename ); 
	if( msDwgFilename.IsEmpty() )
		return false;
	OnNeedRepaint( true );
	return true;
}

void CArxDwgPreviewCtrl::Reset()
{
	msDwgFilename.Empty();
	RemoveHighlight();
	OnNeedRepaint( true );
}

void CArxDwgPreviewCtrl::Clear()
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
	Reset();
}

void CArxDwgPreviewCtrl::SaveDC()
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

void CArxDwgPreviewCtrl::Snapshot() 
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


BEGIN_MESSAGE_MAP(CArxDwgPreviewCtrl, CButton)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArxDwgPreviewCtrl message handlers

void CArxDwgPreviewCtrl::OnPaint() 
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
	GetArxServices()->HandleEvent( Prop::EventPaint, args_B( (GetFocus() == this) ) );

	if( GetFocus() == this )
	{
		CRect rcFocus;
		GetClientRect( &rcFocus );
		rcFocus.DeflateRect( 2, 2 );
		dc.DrawFocusRect( rcFocus );
	}
}

void CArxDwgPreviewCtrl::PaintControl(CDC* pDC) 
{
	int nDCState = pDC->SaveDC();
	CRect rcCell;
	GetClientRect(&rcCell);
	if( msDwgFilename.IsEmpty() )
		pDC->FillSolidRect( &rcCell, mColorService.GetBackgroundColor() );
	else
	{
		Adesk::UInt32 nBgColor = mColorService.GetBackgroundColor();
		acdbDisplayPreviewFromDwg( msDwgFilename, (void*)m_hWnd, &nBgColor );
	}
	
	if( !CAcadColorService::IsTransparentColor( mclrHighlight ) )
	{
		CRect rcHighlight = rcCell;
		rcHighlight.DeflateRect( 1, 1 );
		
		CPen pen( PS_SOLID, 1, mclrHighlight );
		CPen* pOldPen = pDC->SelectObject( &pen );
		pDC->MoveTo( rcHighlight.TopLeft() );
		pDC->LineTo( rcHighlight.right, rcHighlight.top );		
		pDC->LineTo( rcHighlight.right, rcHighlight.bottom );		
		pDC->LineTo( rcHighlight.left, rcHighlight.bottom );		
		pDC->LineTo( rcHighlight.left, rcHighlight.top );		
		pDC->SelectObject( pOldPen );
	}

	pDC->RestoreDC( nDCState );
	SaveDC();
}

void CArxDwgPreviewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize( nType, cx, cy );
	if( mhbmLast )
	{
		DeleteObject( mhbmLast );
		mhbmLast = NULL;
	}
}

void CArxDwgPreviewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	CWnd::OnMouseMove(nFlags, point);
}

void CArxDwgPreviewCtrl::OnClicked() 
{
	GetArxServices()->HandleEvent( Prop::EventClicked );
}

void CArxDwgPreviewCtrl::OnDoubleclicked() 
{
	GetArxServices()->HandleEvent( Prop::EventDblClicked );	
}	

void CArxDwgPreviewCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar != _T('\r') && nChar != _T('\n') )
		CButton::OnChar(nChar, nRepCnt, nFlags);
	else
		GetArxServices()->HandleEvent( Prop::EventClicked );
}

void CArxDwgPreviewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus( pOldWnd );
	OnNeedRepaint( false );
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxDwgPreviewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	OnNeedRepaint( true );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxDwgPreviewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) && nFlags == MK_LBUTTON)
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
		PostMessage( WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y) );	
		if( dwDropEffect != DROPEFFECT_NONE )
			return;
	}
	__super::OnLButtonDown(nFlags, point);
}

void CArxDwgPreviewCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CArxDwgPreviewCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CArxDwgPreviewCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return HandleCtlColor( pDC, nCtlColor );
}
