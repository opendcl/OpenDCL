// PictureBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PictureBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "AcadColorService.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "Workspace.h"
#include "PictureObject.h"


const int nClientBorderSize = 4;
const int nStaticBorderSize = 2;

#ifndef WM_UPDATEUISTATE
#define WM_UPDATEUISTATE                0x0128 //from WinUser.h
#endif



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

static bool IsButtonLikeRequired( TDclControlPtr pDclControl )
{
	if( !pDclControl->GetStringProperty( Prop::EventClicked ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventDblClicked ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::DragnDropBegin ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventKeyDown ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventKeyUp ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventKillFocus ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventSetFocus ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventMouseDown ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventMouseEntered ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventMouseMove ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventMouseMovedOff ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventMouseUp ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventMouseWheel ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventRightClick ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::EventRightDblClick ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::ToolTipTitle ).IsEmpty() ||
			!pDclControl->GetStringProperty( Prop::ToolTipBody ).IsEmpty() )
		return true;
	return false;
};


/////////////////////////////////////////////////////////////////////////////
// CPictureBoxCtrl

CPictureBoxCtrl::CPictureBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbButtonLike( IsButtonLikeRequired( pTemplate ) )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CPictureBoxCtrl::~CPictureBoxCtrl()
{
}

DWORD CPictureBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = (__super::GetWndStyle() | WS_CLIPCHILDREN);

	return dwStyle;
}

bool CPictureBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( _T(""), GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CPictureBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
		case Prop::Picture:
			SetPictureID( pProp->GetLongValue() );
			break;
		case Prop::EventClicked:
		case Prop::EventDblClicked:
		case Prop::DragnDropBegin:
		case Prop::EventKeyDown:
		case Prop::EventKeyUp:
		case Prop::EventKillFocus:
		case Prop::EventSetFocus:
		case Prop::EventMouseDown:
		case Prop::EventMouseEntered:
		case Prop::EventMouseMove:
		case Prop::EventMouseMovedOff:
		case Prop::EventMouseUp:
		case Prop::EventMouseWheel:
		case Prop::EventRightClick:
		case Prop::EventRightDblClick:
		case Prop::ToolTipTitle:
		case Prop::ToolTipBody:
			if( pProp->GetStringValue().IsEmpty() )
				mbButtonLike = IsButtonLikeRequired( mpTemplate );
			else
				mbButtonLike = true;
			break;
	}
	return !bFailed;
}

bool CPictureBoxCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	if( !__super::OnApplyBackgroundColor( pProp ) )
		return false;
	Refresh();
	return true;
}

void CPictureBoxCtrl::SetPictureID(int nPictureID)
{
	SetPicture( mpTemplate->GetOwnerProject()->FindPicture( nPictureID ) );
}

bool CPictureBoxCtrl::IsAutoSized()
{
	return mpTemplate->GetBooleanProperty( Prop::AutoSize );
}

void CPictureBoxCtrl::AutoSize()
{
	if( !IsAutoSized() )
		return;

	int nBorderSize = 0;
	switch( mpTemplate->GetLongProperty( Prop::BorderStyle ) )
	{		
	case 0:
		break;
	case 1:
		nBorderSize = nClientBorderSize;
		break;
	case 2:
		nBorderSize = nStaticBorderSize;
		break;
	}

	mpTemplate->SetLongProperty( Prop::Width, m_cxIcon + nBorderSize );
	mpTemplate->SetLongProperty( Prop::Height, m_cyIcon + nBorderSize );
	ApplyPosition();
}

void CPictureBoxCtrl::Clear()
{
	// set the picture ID to a value that indicates it's blank
	mpTemplate->SetLongProperty(Prop::Picture, -1);
	ClearPicture();
	OnNeedRepaint( true, true );
}

void CPictureBoxCtrl::PaintPicture(int sX, int sY, int nPictureID, int nEnabled, int nUseMask)
{

	CRect iconRect;

	TPicturePtr pPicture = mpTemplate->GetOwnerProject()->FindPicture( nPictureID );
	if( !pPicture )
		return;

	CSize IconSize( pPicture->GetWidth(), pPicture->GetHeight() );
	HDC hdc = ::GetDC(m_hWnd);

	iconRect.left = sX; // Center the icon horizontally
	iconRect.top = sY; // Center the icon vertically

	if (PICTYPE_BITMAP == pPicture->GetPicType())
	{
		if (nUseMask)
		{
			HICON hIcon = pPicture->GetIcon();
			if (nEnabled)
			{
				DrawState(hdc, NULL, NULL,
					(LPARAM)hIcon, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_NORMAL|DST_ICON);
			}
			else
			{					
				DrawState(hdc, NULL, NULL,
					(LPARAM)hIcon, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_DISABLED|DST_ICON);
			}
		}
		else
		{
			HBITMAP hBmp = pPicture->GetBitmap();
			if( nEnabled )
				DrawState( hdc, NULL, NULL, (LPARAM)hBmp, 0,
									 iconRect.left, iconRect.top, IconSize.cx, IconSize.cy,
									 DSS_NORMAL | DST_BITMAP );
			else
				DrawState( hdc, NULL, NULL, (LPARAM)hBmp, 0,
									 iconRect.left, iconRect.top, IconSize.cx, IconSize.cy,
									 DSS_DISABLED | DST_BITMAP );
		}
	}

	// else if picture is an icon
	else if (PICTYPE_ICON == pPicture->GetPicType())
	{
		HICON hIconOut = pPicture->GetIcon();
		
		if (nEnabled)
		{
			DrawState(hdc, NULL, NULL,
					(LPARAM)hIconOut, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_NORMAL|DST_ICON);
		}
		else
		{				
			DrawState(hdc, NULL, NULL,
					(LPARAM)hIconOut, 0, iconRect.left, iconRect.top, IconSize.cx, IconSize.cy, DSS_DISABLED|DST_ICON);
		}
	
	}

	else
	{
		// get width and height of picture
		long lWidth = pPicture->GetWidth();
		long lHeight = pPicture->GetHeight();
		
		iconRect.right = iconRect.left + lWidth;
		iconRect.bottom = iconRect.top + lHeight;

		CRect rcThis;
		GetClientRect(&rcThis);
			
		if (pPicture->GetPicType() == PICTYPE_METAFILE ||
			pPicture->GetPicType() == PICTYPE_ENHMETAFILE)
		{
			
			CRect rcCell = CalcFitRect(lWidth, lHeight,rcThis.Width(), rcThis.Height());

			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), rcCell);				
		}
		else if (lWidth > rcThis.Width() || lHeight > rcThis.Height())
		{
			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), rcThis);
		}
	
		else
		{
			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), iconRect);
		}		
	}

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

} // End of DrawTheIcon

BEGIN_MESSAGE_MAP(CPictureBoxCtrl, CPictureBox)
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPictureBoxCtrl message handlers

void CPictureBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CPictureBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent( pMsg );
	return __super::PreTranslateMessage( pMsg );
}

LRESULT CPictureBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == BM_SETSTYLE && (wParam & BS_DEFPUSHBUTTON) )
		return 0;
	if( message == WM_UPDATEUISTATE )
		return 0;

	return __super::WindowProc(message, wParam, lParam);
}

__UINT_LRESULT CPictureBoxCtrl::OnNcHitTest(CPoint point)
{
	if( !mbButtonLike )
		return HTTRANSPARENT;

	return CButton::OnNcHitTest(point);
}

HBRUSH CPictureBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CPictureBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( __super::OnEraseBkgnd( NULL ) )
		return TRUE;
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}
