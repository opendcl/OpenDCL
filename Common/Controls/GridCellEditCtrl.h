// GridCellEditCtrl.h : header file
//

#pragma once

class CGridCtrl;


/////////////////////////////////////////////////////////////////////////////
// CGridCellEditCtrl class

class CGridCellEditCtrl
{
protected:
	CGridCtrl* mpGridCtrl;
	int mnRow;
	int mnCol;
public:
	CGridCellEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol )
		: mpGridCtrl( pGridCtrl ), mnRow( nRow ), mnCol( nCol )
		{}
	virtual ~CGridCellEditCtrl() {}
};
