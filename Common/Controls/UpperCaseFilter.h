// UpperCaseFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CUpperCaseFilter class

class CUpperCaseFilter : public CInputFilter
{

public:
	CUpperCaseFilter() {}
	virtual ~CUpperCaseFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
			return sFilter;
		}
	bool FilterInput( TCHAR& chInput ) const override //return false to disallow character, else fix it up
		{
			if( _istascii( chInput ) && _istlower( chInput ) )
				chInput = _totupper( chInput );
			return true;
		}
	bool FilterInput( CString& sInput ) const override //return false to discard input, else fix it up
		{
			sInput.MakeUpper();
			return true;
		}
};
