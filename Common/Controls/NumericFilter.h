// NumericFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CNumericFilter class

class CNumericFilter : public CInputFilter
{

public:
	CNumericFilter() {}
	virtual ~CNumericFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("Ee +0123456789./'\"-");
			return sFilter;
		}
};
