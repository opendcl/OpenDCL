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
	bool FilterInput( TCHAR& chInput ) const override { return true; }
	bool FilterInput( CString& sInput ) const override { return true; }
	bool ConvertForDisplay( CString& sRaw ) const override //convert raw text to displayable string
		{
			if( sRaw.IsEmpty() )
				return true;
			COleDateTime dt;
			if( !dt.ParseDateTime( sRaw, VAR_DATEVALUEONLY ) )
				return false;
			sRaw = dt.Format( VAR_DATEVALUEONLY );
			return true;
		}
	bool OnValidateInput( CString& sInput ) override //return false to reject final input
		{
			if( !sInput.IsEmpty() )
			{
				COleDateTime dt;
				if( !dt.ParseDateTime( sInput, VAR_DATEVALUEONLY ) )
					return false;
			}
			return __super::OnValidateInput( sInput );
		}
};
