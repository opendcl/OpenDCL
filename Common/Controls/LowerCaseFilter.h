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
	virtual bool FilterInput( TCHAR& chInput ) const //return false to disallow character, else fix it up
		{
			if( _istascii( chInput ) && _istupper( chInput ) )
				chInput = _totlower( chInput );
			return true;
		}
	virtual bool FilterInput( CString& sInput ) const //return false to discard input, else fix it up
		{
			sInput.MakeLower();
			return true;
		}
};
