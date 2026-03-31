// MultilineFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CMultilineFilter class

class CMultilineFilter : public CInputFilter
{

public:
	CMultilineFilter() {}
	virtual ~CMultilineFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("\r\n");
			return sFilter;
		}
	bool FilterInput( TCHAR& chInput ) const override //return false to disallow character, else fix it up
		{
			return true;
		}
	bool FilterInput( CString& sInput ) const override //return false to discard input, else fix it up
		{
			return true;
		}
};
