// ArxDwgPreviewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDwgPreviewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "ErrorLexicon.h"
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
, mbDrawSelected( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxDwgPreviewCtrl::~CArxDwgPreviewCtrl()
{
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

bool CArxDwgPreviewCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
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

void CArxDwgPreviewCtrl::LoadDwg( LPCTSTR pszFilename )
{
	Clear();
	if( !pszFilename || !*pszFilename )
		return;

	msDwgFilename = theWorkspace.FindFile( pszFilename ); 
	if( msDwgFilename.IsEmpty() )
	{
		theWorkspace.DisplayAlert(CString(ErrorFileNotFound) + "\n" + pszFilename);
		return;
	}
}

void CArxDwgPreviewCtrl::Reset()
{
	msDwgFilename.Empty();
	mbDrawSelected = false;
}

void CArxDwgPreviewCtrl::Clear()
{
	Reset();
	OnNeedRepaint();
}

void CArxDwgPreviewCtrl::SetHighlight( const COLORREF& clrHighlight )
{
	mbDrawSelected = true;
	mclrHighlight = clrHighlight;		
	Invalidate();
}

void CArxDwgPreviewCtrl::RemoveHighlight()
{
	mbDrawSelected = false;
	Invalidate();
}


BEGIN_MESSAGE_MAP(CArxDwgPreviewCtrl, CButton)
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

void CArxDwgPreviewCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	int nDCState = pDC->SaveDC();
	Adesk::UInt32 nBgColor = mColorService.GetBackgroundColor();
	acdbDisplayPreviewFromDwg( msDwgFilename, (void*)m_hWnd, &nBgColor );
	
	CRect rcCell = lpDrawItemStruct->rcItem;

	if( mbDrawSelected )
	{
		rcCell.DeflateRect(1, 1, 1, 1);
		
		CPen Pen(PS_SOLID, 1, mclrHighlight);
		pDC->SelectObject(&Pen);

		pDC->MoveTo(1, 1);
		pDC->LineTo(rcCell.Width(), 1);		
		pDC->LineTo(rcCell.Width(), rcCell.Height());		
		pDC->LineTo(1, rcCell.Height());		
		pDC->LineTo(1, 1);		
	}

	if( lpDrawItemStruct->itemState & ODS_FOCUS )
		pDC->DrawFocusRect( &mrcFocus );

	pDC->RestoreDC( nDCState );
}

void CArxDwgPreviewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	CWnd::OnMouseMove(nFlags, point);
}

void CArxDwgPreviewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize( nType, cx, cy );
	return;
	//if( !msDwgFilename.IsEmpty() )
	//{
	//	SetWindowText( msDwgFilename );
	//	SetWindowText( _T("") );
	//	Adesk::UInt32 Color = RGB(0,0,0);
	//	// static frame works the best for this function
	//	acdbDisplayPreviewFromDwg(msDwgFilename, (void*)m_hWnd, &Color);
	//}
	//else
	//{
	//	CDC *pdc = GetDC();
	//	PaintCtrl( pdc );
	//	ReleaseDC( pdc );
	//}	
}

void CArxDwgPreviewCtrl::OnClicked() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
}

void CArxDwgPreviewCtrl::OnDoubleclicked() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());	
}	

void CArxDwgPreviewCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar != _T('\r') && nChar != _T('\n') )
		CButton::OnChar(nChar, nRepCnt, nFlags);
	else
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
}

void CArxDwgPreviewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CRect rcThis;
	GetClientRect(&rcThis);

	mrcFocus.left = 2;
	mrcFocus.top = 2;
	mrcFocus.right = rcThis.Width() - 2,
	mrcFocus.bottom = rcThis.Height() - 2;

	CDC *pdc = GetDC();
	pdc->DrawFocusRect(mrcFocus);
	ReleaseDC(pdc);
	
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxDwgPreviewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	pdc->DrawFocusRect(mrcFocus);
	ReleaseDC(pdc);

	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxDwgPreviewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == MK_LBUTTON)
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
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
	return mColorService.CtlColor( pDC, nCtlColor );
}
