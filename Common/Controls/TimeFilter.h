// TimeFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CTimeFilter class

class CTimeFilter : public CInputFilter
{

public:
	CTimeFilter() {}
	virtual ~CTimeFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789 :APM");
			return sFilter;
		}
};
