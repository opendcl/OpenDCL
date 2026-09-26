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

// Process syscolor via the workspace color service (CAD tables at Runtime).
COLORREF OdclSysColor( int nIndex );

// Process-wide solid brushes for WM_CTLCOLOR. Never DeleteObject a returned HBRUSH; another
// control may still hold it during the same paint pass.
inline HBRUSH OdclCachedSolidBrush( COLORREF cr )
{
	struct Entry
	{
		COLORREF cr;
		HBRUSH hbr;
	};
	static Entry entries[32]; // static zero-init (ARX.16 rejects ={})
	static int nEntries = 0;
	for( int i = 0; i < nEntries; ++i )
	{
		if( entries[i].cr == cr && entries[i].hbr )
			return entries[i].hbr;
	}
	HBRUSH hbr = ::CreateSolidBrush( cr );
	if( !hbr )
		return NULL;
	if( nEntries < 32 )
	{
		entries[nEntries].cr = cr;
		entries[nEntries].hbr = hbr;
		++nEntries;
	}
	return hbr;
}