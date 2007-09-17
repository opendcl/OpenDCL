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
	virtual bool ConvertForDisplay( CString& sRaw ) const //convert raw text to displayable string
		{
			if( sRaw.IsEmpty() )
				return true;
			ads_real dblValue;
			if( RTNORM != acdbDisToF( sRaw, -1, &dblValue ) )
				return false;
			int nStat = acdbRToS( dblValue, -1, -1, sRaw.GetBuffer( 256 ) );
			sRaw.ReleaseBuffer();
			if( nStat != RTNORM )
				return false;
			return true;
		}
	virtual bool OnValidateInput( CString& sInput ) //return false to reject final input
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
