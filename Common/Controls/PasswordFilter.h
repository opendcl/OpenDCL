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
	virtual const CString& GetFilter() const { return msEmptyFilter; }
	virtual bool FilterInput( TCHAR& chInput ) const { return true; }
	virtual bool FilterInput( CString& sInput ) const { return true; }
	virtual bool IsMaskedInput() const { return true; }
};
