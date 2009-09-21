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
	bool IsBackgroundTransparent() const;
	static CBrush& GetTransparentBrush();
	static COLORREF GetTransparentColor() { return (COLORREF)-1; }
	static bool IsTransparentColor( COLORREF color ) { return ((color & 0x80000000) != 0); }

// Control painting helpers
	HBRUSH CtlColor( CDC* pDC, UINT nCtlColor, CWnd* pWnd = NULL );
};
