// PasswordFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CPasswordFilter class

class CPasswordFilter : public CInputFilter
{
	CString msEmptyFilter;

public:
	CPasswordFilter() {}
	virtual ~CPasswordFilter() {}
public:
	const CString& GetFilter() const override { return msEmptyFilter; }
	bool FilterInput( TCHAR& chInput ) const override { return true; }
	bool FilterInput( CString& sInput ) const override { return true; }
	bool IsMaskedInput() const override { return true; }
};
