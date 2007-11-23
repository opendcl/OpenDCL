// SymbolNameFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CSymbolNameFilter class

class CSymbolNameFilter : public CInputFilter
{

public:
	CSymbolNameFilter() {}
	virtual ~CSymbolNameFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&()_-+}{][");
			return sFilter;
		}
};
