// RadioEditCtrl.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "GridCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CRadioEditCtrl class

class CRadioEditCtrl : public CGridCellEditCtrl
{
public:
	CRadioEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol )
		: CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			if( !pGridCtrl->IsCellChecked( nRow, nCol ) )
				pGridCtrl->ToggleCellState( nRow, nCol );
			pGridCtrl->PostMessage( WM_CANCELMODE, 0, 0 );
		}
	virtual ~CRadioEditCtrl()
		{
		}
};
