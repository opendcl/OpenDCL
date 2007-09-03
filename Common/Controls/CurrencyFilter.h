// CurrencyFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CCurrencyFilter class

class CCurrencyFilter : public CInputFilter
{

public:
	CCurrencyFilter() {}
	virtual ~CCurrencyFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789.");
			return sFilter;
		}
};
