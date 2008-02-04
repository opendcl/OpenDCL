// ButtonEditCtrl.h : header file
//

#pragma once

#include "DynamicButtonCtrl.h"
#include "GridCellEditCtrl.h"
#include "GridCtrl.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CButtonEditCtrl class

class CButtonEditCtrl : public CDynamicButtonCtrl, public CGridCellEditCtrl
{
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.right - 24, rcCell.top + 2, rcCell.right - 2, rcCell.bottom - 2 );
		}
public:
	CButtonEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, UINT idIcon, UINT nID = 100 )
		: CDynamicButtonCtrl( pGridCtrl, CalcRect( pGridCtrl->GetCellRect( nRow, nCol ) ), nID )
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			HICON hIcon =
				(HICON)::LoadImage( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(idIcon), IMAGE_ICON, 0, 0, 0 );
			SetIcon( hIcon );
			SetThemeHelper( pGridCtrl->GetControlPane()->GetThemeHelper() );
			SetFlat( FALSE );
		}
	CButtonEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, LPCTSTR pszCaption, UINT nID = 100 )
		: CDynamicButtonCtrl( pGridCtrl, CalcRect( pGridCtrl->GetCellRect( nRow, nCol ) ), nID )
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			SetWindowText( pszCaption );
			SetThemeHelper( pGridCtrl->GetControlPane()->GetThemeHelper() );
			SetFlat( FALSE );
		}
	virtual ~CButtonEditCtrl()
		{
		}
};
