#pragma once

// Process-level color mapping. Studio uses CWin32ColorService. Runtime
// CArxColorService derives from it and remaps through COLORTHEME tables.
// Per-control FG/BG cache remains CAcadColorService.

class CColorService
{
public:
	virtual ~CColorService() {}

	virtual COLORREF GetSysColor( int nIndex ) const = 0;
	virtual COLORREF GetRGBColor( long nColorIndex ) const;
	virtual COLORREF GetSafeRGBColor( long nColorIndex, COLORREF crBackground ) const;
};

class CWin32ColorService : public CColorService
{
public:
	COLORREF GetSysColor( int nIndex ) const override { return ::GetSysColor( nIndex ); }
};
