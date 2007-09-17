// PercentageFilter.h : header file
//

#pragma once

#include "NumericFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CPercentageFilter class

class CPercentageFilter : public CNumericFilter
{

public:
	CPercentageFilter() {}
	virtual ~CPercentageFilter() {}

// CInputFilter overrides
public:
	virtual bool ConvertForDisplay( CString& sRaw ) const //convert raw text to displayable string
		{
			if( !__super::ConvertForDisplay( sRaw ) )
				return false;
			if( !sRaw.IsEmpty() && sRaw.Right( 1 ) != _T("%") )
				sRaw += _T('%');
			return true;
		}
	virtual bool OnValidateInput( CString& sInput ) //return false to reject final input
		{
			sInput.TrimRight( _T("%") );
			return __super::OnValidateInput( sInput );
		}
};
