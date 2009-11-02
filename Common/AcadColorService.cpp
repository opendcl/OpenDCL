#include "StdAfx.h"
#include "AcadColorService.h"


static CBrush& CreateTransparentBrush()
{
	static CBrush brTransparent;
	if( !(HBRUSH)brTransparent )
	{
		LOGBRUSH lbr = { BS_HOLLOW, 0, 0 };
		brTransparent.CreateBrushIndirect( &lbr );
	}
	return brTransparent;
}


CAcadColorService::CAcadColorService()
: mclrForeground( GetRGBColor( -19 /*button text*/ ) )
, mclrBackground( CLR_DEFAULT )
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
	COLORREF color = GetRGBColor( nAcadColor );
	if( mclrBackground == color )
		return;
	mclrBackground = color;
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( GetBackgroundColor() );
}

void CAcadColorService::SetBackgroundColor( COLORREF color )
{
	if( mclrBackground == color )
		return;
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
	return mclrBackground;
}

HBRUSH CAcadColorService::GetBackgroundBrush() const
{
	if( IsBackgroundNotSet() )
		return NULL;
	if( IsBackgroundTransparent() )
		return GetTransparentBrush();
	return mbrushBackground;
}

CBrush* CAcadColorService::GetBackgroundCBrush() const
{
	if( IsBackgroundNotSet() )
		return NULL;
	if( IsBackgroundTransparent() )
		return &GetTransparentBrush();
	return CBrush::FromHandle( mbrushBackground );
}

bool CAcadColorService::IsBackgroundNotSet() const
{
	return (mclrBackground == CLR_DEFAULT);
}

bool CAcadColorService::IsBackgroundTransparent() const
{
	return IsTransparentColor( mclrBackground );
}

CBrush& CAcadColorService::GetTransparentBrush()
{
	static CBrush& brTransparent = CreateTransparentBrush();
	return brTransparent;
}
