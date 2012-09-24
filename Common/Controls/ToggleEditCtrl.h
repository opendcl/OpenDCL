// ToggleEditCtrl.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "GridCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CToggleEditCtrl class

class CToggleEditCtrl : public CGridCellEditCtrl
{
public:
	CToggleEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol )
		: CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			pGridCtrl->ToggleCellState( nRow, nCol );
			pGridCtrl->PostMessage( WM_CANCELMODE, 0, 0 );
		}
	virtual ~CToggleEditCtrl()
		{
		}
};
