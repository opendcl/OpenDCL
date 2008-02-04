
#pragma once

#include "PtrTypes.h"

class CAxContainerCtrl;


class FontSettings
{
protected:
	CString msName;
	long mlSize; //positive means point size, negative means normal character size in pixels
	bool mbBold;
	bool mbUnderlined;
	bool mbItalic;

public:
	FontSettings();
	FontSettings( LPCTSTR pszName, long lSize, bool bBold, bool bUnderlined, bool bItalic );
	FontSettings( LPCTSTR pszName, bool bScaled, UINT nSize, bool bBold, bool bUnderlined, bool bItalic );
	FontSettings( TDclControlPtr pDclControl );
	FontSettings( CAxContainerCtrl* pAxControl, DISPID dispid );

	void set( LPCTSTR pszName, long lSize, bool bBold, bool bUnderlined, bool bItalic );
	void set( LPCTSTR pszName, bool bScaled, UINT nSize, bool bBold, bool bUnderlined, bool bItalic );
	void setFrom( TDclControlPtr pDclControl );
	void setFrom( CAxContainerCtrl* pAxControl, DISPID dispid );
	void clear() { msName.Empty(); mlSize = 0; mbBold = false; mbUnderlined = false; mbItalic = false; }

	void setName( LPCTSTR pszName ) { msName = pszName; }
	void setSize( long lSize ) { mlSize = lSize; }
	void setBold( bool bBold = true ) { mbBold = bBold; }
	void setUnderlined( bool bUnderlined = true ) { mbUnderlined = bUnderlined; }
	void setItalic( bool bItalic = true ) { mbItalic = bItalic; }

	operator LPCTSTR() const { return name(); }
	LPCTSTR name() const { if( msName.IsEmpty() ) return NULL; return msName; }
	long size() const { return mlSize; }
	bool isBold() const { return mbBold; }
	bool isUnderlined() const { return mbUnderlined; }
	bool isItalic() const { return mbItalic; }
	bool isScaled() const { return (mlSize > 0); }
	CString absoluteSize() const;
};
