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


/////////////////////////////////////////////////////////////////////////////
// CPictureBoxCtrl

CPictureBoxCtrl::CPictureBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CPictureBoxCtrl::~CPictureBoxCtrl()
{
}

bool CPictureBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( _T(""), GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CPictureBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
		case Prop::Picture:
			SetPictureID( pProp->GetLongValue() );
			break;
	}
	return !bFailed;
}

DWORD CPictureBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = (__super::GetWndStyle() | WS_CLIPCHILDREN);

	return dwStyle;
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
	mpTemplate->SetLongProperty(Prop::Picture, 0);
	SetPictureBlank();
	
	if (m_pPictureHolder)
	{
		m_pPictureHolder->Release();
		m_pPictureHolder = NULL;
		//delete m_pPictureHolder;
	}

	if (m_hbmMem != NULL)
	{
		DeleteObject(m_hbmMem);
		m_hbmMem = NULL;
	}
	
	//RedrawWindow();
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
	{
		CRect rcCell;	
		GetClientRect(&rcCell);
		HDC hdc = ::GetDC(m_hWnd);
		FillRect(hdc, rcCell, pColorService->GetBackgroundBrush());	
		::ReleaseDC(m_hWnd, hdc);
	}
}

void CPictureBoxCtrl::PaintPicture(int sX, int sY, int nPictureID, int nEnabled, int nUseMask)
{

	CRect iconRect;

	CPictureObject* pPicture = mpTemplate->GetOwnerProject()->FindPicture( nPictureID );
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
			pPicture->Render(CDC::FromHandle(hdc), rcCell.left, rcCell.top, rcThis);				
		}
		else if (lWidth > rcThis.Width() || lHeight > rcThis.Height())
		{
			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), 0, 0, rcThis);
		}
	
		else
		{
			// display picture using IPicture::Render
			pPicture->Render(CDC::FromHandle(hdc), sX, sY, iconRect);
		}		
	}

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

} // End of DrawTheIcon

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
