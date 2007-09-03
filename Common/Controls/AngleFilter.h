// AngleFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CAngleFilter class

class CAngleFilter : public CInputFilter
{

public:
	CAngleFilter() {}
	virtual ~CAngleFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("DdGgRrNnSsEeWw 0123456789.'\"-");
			return sFilter;
		}
};
