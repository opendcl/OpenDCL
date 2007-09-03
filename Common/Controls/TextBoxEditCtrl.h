// TextBoxEditCtrl.h : header file
//

#pragma once

#include "FilteredEditEx.h"
#include "GridCellEditCtrl.h"
#include "GridCtrl.h"

class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CTextBoxEditCtrl class

class CTextBoxEditCtrl : public CFilteredEditEx, public CGridCellEditCtrl
{
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 4, rcCell.top + 4, rcCell.right - 4, rcCell.bottom - 4 );
		}
public:
	CTextBoxEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CFilteredEditEx( pFilter )
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			Create( pGridCtrl,
							CalcRect( pGridCtrl->GetCellRect( nRow, nCol ) ),
							100 );
			SetFont( pGridCtrl->GetFont() );
			SetWindowText( pGridCtrl->GetCellText( nRow, nCol ) );
			SetSel( 0, -1, TRUE );
			SetFocus();
		}
	virtual ~CTextBoxEditCtrl()
		{
			CString sText;
			GetWindowText( sText );
			mpGridCtrl->SetCellText( mnRow, mnCol, sText );
			DestroyWindow();
		}
};
