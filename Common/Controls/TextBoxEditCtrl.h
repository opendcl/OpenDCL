// TextBoxEditCtrl.h : header file
//

#pragma once

#include "FilteredEditCtrl.h"
#include "GridCellEditCtrl.h"
#include "GridCtrl.h"

class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CTextBoxEditCtrl class

class CTextBoxEditCtrl : public CFilteredEditCtrl, public CGridCellEditCtrl
{
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 4, rcCell.top + 4, rcCell.right - 4, rcCell.bottom - 4 );
		}
public:
	CTextBoxEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CInputFilter* pFilter = NULL, DWORD dwStyle = GetDefaultWndStyle(), UINT nID = 100 )
		: CFilteredEditCtrl( pGridCtrl, CalcRect( pGridCtrl->GetCellRect( nRow, nCol ) ), nID, pFilter, dwStyle )
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			SetFont( pGridCtrl->GetFont() );
			SetWindowText( pGridCtrl->GetCellText( nRow, nCol ) );
			SetSel( 0, -1, TRUE );
			SetFocus();
		}
	virtual ~CTextBoxEditCtrl()
		{
			CString sText;
			GetWindowText( sText );
			CInputFilter* pFilter = GetInputFilter();
			if( pFilter )
			{
				if( !pFilter->OnValidateInput( sText ) )
				{
					sText = pFilter->OnBadInput();
					pFilter->ConvertForDisplay( sText );
				}
			}
			mpGridCtrl->SetCellText( mnRow, mnCol, sText );
			DestroyWindow();
		}
};
