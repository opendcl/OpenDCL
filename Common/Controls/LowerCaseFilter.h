// LowerCaseFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CLowerCaseFilter class

class CLowerCaseFilter : public CInputFilter
{

public:
	CLowerCaseFilter() {}
	virtual ~CLowerCaseFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("abcdefghijklmnopqrstuvwxyz");
			return sFilter;
		}
	bool FilterInput( TCHAR& chInput ) const override //return false to disallow character, else fix it up
		{
			if( _istascii( chInput ) && _istupper( chInput ) )
				chInput = _totlower( chInput );
			return true;
		}
	bool FilterInput( CString& sInput ) const override //return false to discard input, else fix it up
		{
			sInput.MakeLower();
			return true;
		}
};
