// ArxHatchCtrl.h : header file
//

#pragma once

#include "ArxGsViewCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CArxHatchCtrl window

class CArxHatchCtrl : public CArxGsViewCtrl
{
	AcDbDatabase* mpHatchDb;
	CString msHatchPattern;
	AcDbObjectIdArray mridLoop;
	AcDbObjectId midHatch;

public:
	CArxHatchCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxHatchCtrl();

// DialogControl Interface
public:

public:
	bool DisplayHatchPattern( LPCTSTR pszPattern );
	void ResizeHatch();
	void Clear();
};
