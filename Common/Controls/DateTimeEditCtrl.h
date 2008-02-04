// DateTimeEditCtrl.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CDateTimeEditCtrl class

class CDateTimeEditCtrl : public CDateTimeCtrl, public CGridCellEditCtrl
{
	CAcadColorService mColorService;

public:
	CDateTimeEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, bool bDate = true, UINT nID = 100 );
	virtual ~CDateTimeEditCtrl();

public:
	virtual CAcadColorService* GetColorService() { return &mColorService; }

protected:
	static CRect CalcRect( const CRect& rcCell );

protected:
	DECLARE_MESSAGE_MAP();

protected:
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
};
