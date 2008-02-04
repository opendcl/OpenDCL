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
, mbrushBackground( colorBackground )
{
}

CAcadColorService::CAcadColorService( long nAcadColorForeground, long nAcadColorBackground )
: mclrForeground( GetRGBColor( nAcadColorForeground ) )
, mclrBackground( GetRGBColor( nAcadColorBackground ) )
, mbrushBackground( mclrBackground )
{
}

CAcadColorService::~CAcadColorService()
{
}

void CAcadColorService::SetBackgroundColor( long nAcadColor )
{
	mclrBackground = GetRGBColor( nAcadColor );
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( mclrBackground );
}

void CAcadColorService::SetBackgroundColor( COLORREF color )
{
	mclrBackground = color;
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( color );
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
	return mclrBackground;
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
	return (mclrBackground == ~COLORREF(0));
}

CBrush& CAcadColorService::GetTransparentBrush()
{
	static CBrush& brTransparent = CreateTransparentBrush();
	return brTransparent;
}

HBRUSH CAcadColorService::CtlColor( CDC* pDC, UINT nCtlColor )
{
	pDC->SetTextColor( mclrForeground );
	if( IsBackgroundTransparent() )
	{
		pDC->SetBkMode( TRANSPARENT );
		return GetTransparentBrush();
	}
	pDC->SetBkColor( mclrBackground );
	return mbrushBackground;
}
