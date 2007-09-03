// DateFilter.h : header file
//

#pragma once

#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CDateFilter class

class CDateFilter : public CInputFilter
{

public:
	CDateFilter() {}
	virtual ~CDateFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789-/ ");
			return sFilter;
		}
};
