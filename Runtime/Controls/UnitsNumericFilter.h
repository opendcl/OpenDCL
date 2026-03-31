// UnitsNumericFilter.h : header file
//

#pragma once

#include "NumericFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CUnitsNumericFilter class

class CUnitsNumericFilter : public CNumericFilter
{

public:
	CUnitsNumericFilter() {}
	virtual ~CUnitsNumericFilter() {}

// CInputFilter overrides
public:
	bool ConvertForDisplay( CString& sRaw ) const override //convert raw text to displayable string
		{
			if( sRaw.IsEmpty() )
				return true;
			ads_real dblValue;
			if( RTNORM != acdbDisToF( sRaw, -1, &dblValue ) )
				return false;
			TCHAR szVal[32];
			int nStat = acdbRToS( dblValue, -1, -1, szVal );
			if( nStat != RTNORM )
				return false;
			sRaw = szVal;
			return true;
		}
	bool OnValidateInput( CString& sInput ) override //return false to reject final input
		{
			if( !sInput.IsEmpty() )
			{
				ads_real dblValue;
				if( RTNORM != acdbDisToF( sInput, -1, &dblValue ) )
					return false;
			}
			return __super::OnValidateInput( sInput );
		}
};
