// UnsignedIntegerFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CUnsignedIntegerFilter class

class CUnsignedIntegerFilter : public CInputFilter
{

public:
	CUnsignedIntegerFilter() {}
	virtual ~CUnsignedIntegerFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789");
			return sFilter;
		}
};
