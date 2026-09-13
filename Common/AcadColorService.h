// AcadColorService.h : header file
//

#pragma once

#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CAcadColorService class
// Stores logical OpenDCL colors (or an explicit COLORREF). Logical values are
// resolved at get time so theme table switches do not require a cache invalidate.

class CAcadColorService
{
	long mnForeground;
	long mnBackground;
	bool mbForegroundLogical;
	bool mbBackgroundLogical;
	COLORREF mclrForeground;
	COLORREF mclrBackground;
	mutable COLORREF mclrBrush;
	mutable bool mbBrushValid;
	mutable CBrush mbrushBackground;

// Construction
public:
	CAcadColorService();
	CAcadColorService( COLORREF colorForeground, COLORREF colorBackground );
	CAcadColorService( long nAcadColorForeground, long nAcadColorBackground );
	virtual ~CAcadColorService();

// Operations
public:
	virtual void SetBackgroundColor( long nAcadColor );
	virtual void SetBackgroundColor( COLORREF color );
	virtual void SetForegroundColor( long nAcadColor );
	virtual void SetForegroundColor( COLORREF color );

// Attributes
	COLORREF GetForegroundColor() const;
	COLORREF GetBackgroundColor() const;
	HBRUSH GetBackgroundBrush() const;
	CBrush* GetBackgroundCBrush() const;
	bool IsBackgroundNotSet() const;
	bool IsBackgroundTransparent() const;
	static CBrush& GetTransparentBrush();
	static COLORREF GetTransparentColor() { return (COLORREF)-1; }
	static bool IsTransparentColor( COLORREF color ) { return ((color & 0x80000000) != 0); }

private:
	void EnsureBackgroundBrush() const;
};
