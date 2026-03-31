// CustomFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CCustomFilter class

class CCustomFilter : public CInputFilter
{
	CString msFilter;

public:
	CCustomFilter() {}
	CCustomFilter( LPCTSTR pszFilter ) : msFilter( pszFilter ) {}
	virtual ~CCustomFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			return msFilter;
		}
	bool FilterInput( TCHAR& chInput ) const override
		{
			if( msFilter.IsEmpty() )
				return true;
			return __super::FilterInput( chInput );
		}
	bool FilterInput( CString& sInput ) const override
		{
			if( msFilter.IsEmpty() )
				return true;
			return __super::FilterInput( sInput );
		}
};
