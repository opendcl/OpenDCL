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
: mnForeground( -19 )
, mnBackground( 0 )
, mbForegroundLogical( true )
, mbBackgroundLogical( false )
, mclrForeground( 0 )
, mclrBackground( CLR_DEFAULT )
, mclrBrush( 0 )
, mbBrushValid( false )
{
}

CAcadColorService::CAcadColorService( COLORREF colorForeground, COLORREF colorBackground )
: mnForeground( 0 )
, mnBackground( 0 )
, mbForegroundLogical( false )
, mbBackgroundLogical( false )
, mclrForeground( colorForeground )
, mclrBackground( colorBackground )
, mclrBrush( colorBackground & 0x00FFFFFF )
, mbBrushValid( true )
, mbrushBackground( colorBackground & 0x00FFFFFF )
{
}

CAcadColorService::CAcadColorService( long nAcadColorForeground, long nAcadColorBackground )
: mnForeground( nAcadColorForeground )
, mnBackground( nAcadColorBackground )
, mbForegroundLogical( true )
, mbBackgroundLogical( true )
, mclrForeground( 0 )
, mclrBackground( 0 )
, mclrBrush( 0 )
, mbBrushValid( false )
{
}

CAcadColorService::~CAcadColorService()
{
}

void CAcadColorService::SetBackgroundColor( long nAcadColor )
{
	mnBackground = nAcadColor;
	mbBackgroundLogical = true;
	mbBrushValid = false;
	mbrushBackground.DeleteObject();
}

void CAcadColorService::SetBackgroundColor( COLORREF color )
{
	mbBackgroundLogical = false;
	if( mclrBackground == color && (HBRUSH)mbrushBackground )
		return;
	mclrBackground = color;
	mclrBrush = color & 0x00FFFFFF;
	mbBrushValid = true;
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( mclrBrush );
}

void CAcadColorService::SetForegroundColor( long nAcadColor )
{
	mnForeground = nAcadColor;
	mbForegroundLogical = true;
}

void CAcadColorService::SetForegroundColor( COLORREF color )
{
	mbForegroundLogical = false;
	mclrForeground = color;
}

COLORREF CAcadColorService::GetForegroundColor() const
{
	if( mbForegroundLogical )
		return GetRGBColor( mnForeground );
	return mclrForeground;
}

COLORREF CAcadColorService::GetBackgroundColor() const
{
	if( mbBackgroundLogical )
		return GetRGBColor( mnBackground );
	return mclrBackground;
}

void CAcadColorService::EnsureBackgroundBrush() const
{
	COLORREF cr = GetBackgroundColor() & 0x00FFFFFF;
	if( mbBrushValid && (HBRUSH)mbrushBackground && mclrBrush == cr )
		return;
	mclrBrush = cr;
	mbBrushValid = true;
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush( cr );
}

HBRUSH CAcadColorService::GetBackgroundBrush() const
{
	if( IsBackgroundNotSet() )
		return NULL;
	if( IsBackgroundTransparent() )
		return GetTransparentBrush();
	EnsureBackgroundBrush();
	return mbrushBackground;
}

CBrush* CAcadColorService::GetBackgroundCBrush() const
{
	if( IsBackgroundNotSet() )
		return NULL;
	if( IsBackgroundTransparent() )
		return &GetTransparentBrush();
	EnsureBackgroundBrush();
	return CBrush::FromHandle( mbrushBackground );
}

bool CAcadColorService::IsBackgroundNotSet() const
{
	return !mbBackgroundLogical && (mclrBackground == CLR_DEFAULT);
}

bool CAcadColorService::IsBackgroundTransparent() const
{
	return IsTransparentColor( GetBackgroundColor() );
}

CBrush& CAcadColorService::GetTransparentBrush()
{
	static CBrush& brTransparent = CreateTransparentBrush();
	return brTransparent;
}
