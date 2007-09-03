// IntegerFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CIntegerFilter class

class CIntegerFilter : public CInputFilter
{

public:
	CIntegerFilter() {}
	virtual ~CIntegerFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789-");
			return sFilter;
		}
};
