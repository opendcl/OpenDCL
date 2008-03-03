#include "StdAfx.h"
#include "AcadColorService.h"


static CBrush& CreateTransparentBrush()
{
	static CBrush brTransparent;
	LOGBRUSH lbr = { BS_HOLLOW, 0, 0 };
	brTransparent.CreateBrushIndirect( &lbr );
	return brTransparent;
}


CAcadColorService::CAcadColorService()
: mclrForeground( GetRGBColor( -19 /*button text*/ ) )
, mclrBackground( GetRGBColor( -24 /*transparent*/ ) )
, mbrushBackground( GetRGBColor( -16 /*button face*/ ) )
{
}

CAcadColorService::CAcadColorService( COLORREF colorForeground, COLORREF colorBackground )
: mclrForeground( colorForeground )
, mclrBackground( colorBackground )
, mbrushBackground( colorBackground & 0x00FFFFFF )
{
}

CAcadColorService::CAcadColorService( long nAcadColorForeground, long nAcadColorBackground )
: mclrForeground( GetRGBColor( nAcadColorForeground ) )
, mclrBackground( GetRGBColor( nAcadColorBackground ) )
, mbrushBackground( GetRGBColor( nAcadColorBackground ) & 0x00FFFFFF )
{
}

CAcadColorService::~CAcadColorService()
{
}

void CAcadColorService::SetBackgroundColor( long nAcadColor )
{
	mclrBackground = GetRGBColor( nAcadColor );
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( GetBackgroundColor() );
}

void CAcadColorService::SetBackgroundColor( COLORREF color )
{
	mclrBackground = color;
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( GetBackgroundColor() );
}

void CAcadColorService::SetForegroundColor( long nAcadColor )
{
	mclrForeground = GetRGBColor( nAcadColor );
}

void CAcadColorService::SetForegroundColor( COLORREF color )
{
	mclrForeground = color;
}

COLORREF CAcadColorService::GetForegroundColor() const
{
	return mclrForeground;
}

COLORREF CAcadColorService::GetBackgroundColor() const
{
	return (mclrBackground & 0x0FFFFFF);
}

HBRUSH CAcadColorService::GetBackgroundBrush() const
{
	if( IsBackgroundTransparent() )
		return GetTransparentBrush();
	return mbrushBackground;
}

CBrush* CAcadColorService::GetBackgroundCBrush() const
{
	if( IsBackgroundTransparent() )
		return &GetTransparentBrush();
	return CBrush::FromHandle( mbrushBackground );
}

bool CAcadColorService::IsBackgroundTransparent() const
{
	return ((mclrBackground & 0x80000000) != 0);
}

CBrush& CAcadColorService::GetTransparentBrush()
{
	static CBrush& brTransparent = CreateTransparentBrush();
	return brTransparent;
}

HBRUSH CAcadColorService::CtlColor( CDC* pDC, UINT nCtlColor, CWnd* pWnd /*= NULL*/ )
{
	pDC->SetTextColor( mclrForeground );
	if( IsBackgroundTransparent() )
	{
		if( pWnd )
		{
			CRect rcDlg;
			pWnd->GetWindowRect( &rcDlg );
			CWnd* pParentWnd = pWnd->GetParent();
			pParentWnd->ScreenToClient( &rcDlg );
			CDC* pParentDC = pParentWnd->GetDC();
			pDC->BitBlt( 0, 0, rcDlg.Width(), rcDlg.Height(), pParentDC, rcDlg.left, rcDlg.top, SRCCOPY );
			pParentWnd->ReleaseDC( pParentDC );
		}
		pDC->SetBkMode( TRANSPARENT );
		return GetTransparentBrush();
	}
	pDC->SetBkColor( GetBackgroundColor() );
	if( pWnd )
	{
		pDC->SetBkMode( TRANSPARENT );
		CRect rcClient;
		pWnd->GetClientRect( &rcClient );
		pDC->FillSolidRect( &rcClient, GetBackgroundColor() );
	}
	return mbrushBackground;
}
