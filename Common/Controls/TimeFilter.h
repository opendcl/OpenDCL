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
	virtual bool FilterInput( TCHAR& chInput ) const { return true; }
	virtual bool FilterInput( CString& sInput ) const { return true; }
	virtual bool ConvertForDisplay( CString& sRaw ) const //convert raw text to displayable string
		{
			if( sRaw.IsEmpty() )
				return true;
			COleDateTime dt;
			if( !dt.ParseDateTime( sRaw, VAR_TIMEVALUEONLY ) )
				return false;
			sRaw = dt.Format( VAR_TIMEVALUEONLY );
			return true;
		}
	virtual bool OnValidateInput( CString& sInput ) //return false to reject final input
		{
			if( !sInput.IsEmpty() )
			{
				COleDateTime dt;
				if( !dt.ParseDateTime( sInput, VAR_TIMEVALUEONLY ) )
					return false;
			}
			return __super::OnValidateInput( sInput );
		}
};
