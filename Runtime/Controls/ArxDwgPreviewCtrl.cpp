// ArxDwgPreviewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDwgPreviewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "ErrorLexicon.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDwgPreviewCtrl

CArxDwgPreviewCtrl::CArxDwgPreviewCtrl( TDclControlPtr pTemplate, CControlPane* pPane,
																				UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mArxServices( this )
, mDragDropService( this )
{
	m_bSelectedRect = false;

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
	CString sPath = theWorkspace.FindFile( pszFilename ); 
	if( sPath.IsEmpty() )
	{
		theWorkspace.DisplayAlert(ErrorFileNotFound);
		return;
	}	
	m_filename = sPath;
	RedrawWindow();
}

void CArxDwgPreviewCtrl::PaintCtrl( CDC *pdc ) 
{
	CRect rcCell;
	CWnd::GetClientRect( &rcCell );
	
	// draw the Window background for the cell				
	pdc->FillSolidRect( rcCell, mColorService.GetBackgroundColor() );
	
	if( !m_filename.IsEmpty() )
	{
		Adesk::UInt32 nBgColor = mColorService.GetBackgroundColor();
		acdbDisplayPreviewFromDwg( m_filename, (void*)m_hWnd, &nBgColor );
	}

	if (m_bSelectedRect)
	{
		CRect rcCell;
		GetClientRect (&rcCell);
		CSize szValue(1,1);
		rcCell.DeflateRect(szValue);
		
		CPen Pen(PS_SOLID, 1, m_HighlightColor);
		CPen* pOldPen = pdc->SelectObject(&Pen);

		pdc->MoveTo(1, 1);
		pdc->LineTo(rcCell.Width(), 1);		
		pdc->LineTo(rcCell.Width(), rcCell.Height());		
		pdc->LineTo(1, rcCell.Height());		
		pdc->LineTo(1, 1);		
		pdc->SelectObject(pOldPen);
		Pen.DeleteObject();
	}

	if (GetFocus() == this)
		// draw the solid rectangle
		pdc->DrawFocusRect(m_rcFocus);
}

void CArxDwgPreviewCtrl::SetHighLight( int nColorIndex )
{
	m_bSelectedRect = true;
	m_HighlightColor = GetRGBColor(nColorIndex);		
	Invalidate();
}

void CArxDwgPreviewCtrl::RemoveHighLight()
{
	m_bSelectedRect = false;
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
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetBkColor( mColorService.GetBackgroundColor() );
	PaintCtrl( pDC );
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
	if( !m_filename.IsEmpty() )
	{
		SetWindowText( m_filename );
		SetWindowText( _T("") );
		Adesk::UInt32 Color = RGB(0,0,0);
		// static frame works the best for this function
		acdbDisplayPreviewFromDwg(m_filename, (void*)m_hWnd, &Color);
	}
	else
	{
		CDC *pdc = GetDC();
		PaintCtrl( pdc );
		ReleaseDC( pdc );
	}	
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

	m_rcFocus.left = 2;
	m_rcFocus.top = 2;
	m_rcFocus.right = rcThis.Width() - 2,
	m_rcFocus.bottom = rcThis.Height() - 2;

	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);
	
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxDwgPreviewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
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
