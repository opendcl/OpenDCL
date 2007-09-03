// AcadColorService.h : header file
//

#pragma once

#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CAcadColorService class

class CAcadColorService
{
	COLORREF mclrForeground;
	COLORREF mclrBackground;
	CBrush mbrushBackground;

// Construction
public:
	CAcadColorService()
		: mclrForeground( GetRGBColor( -19 /*button text*/ ) )
		, mclrBackground( GetRGBColor( -16 /*button face*/ ) )
		, mbrushBackground( GetRGBColor( -16 /*button face*/ ) )
		{}
	CAcadColorService( COLORREF colorForeground, COLORREF colorBackground )
		: mclrForeground( colorForeground )
		, mclrBackground( colorBackground )
		, mbrushBackground( colorBackground )
		{}
	CAcadColorService( long nAcadColorForeground, long nAcadColorBackground )
		: mclrForeground( GetRGBColor( nAcadColorForeground ) )
		, mclrBackground( GetRGBColor( nAcadColorBackground ) )
		, mbrushBackground( mclrBackground )
		{}
	virtual ~CAcadColorService()
		{
		}

// Operations
public:
	virtual void SetBackgroundColor( long nAcadColor )
	{
		mclrBackground = GetRGBColor( nAcadColor );
		mbrushBackground.DeleteObject();
		mbrushBackground.CreateSolidBrush( mclrBackground );
	}
	virtual void SetBackgroundColor( COLORREF color )
	{
		mclrBackground = color;
		mbrushBackground.DeleteObject();
		mbrushBackground.CreateSolidBrush( color );
	}
	virtual void SetForegroundColor( long nAcadColor )
	{
		mclrForeground = GetRGBColor( nAcadColor );
	}
	virtual void SetForegroundColor( COLORREF color )
	{
		mclrForeground = color;
	}

// Attributes
	COLORREF GetForegroundColor() const { return mclrForeground; }
	COLORREF GetBackgroundColor() const { return mclrBackground; }
	HBRUSH GetBackgroundBrush() const { return mbrushBackground; }
};
