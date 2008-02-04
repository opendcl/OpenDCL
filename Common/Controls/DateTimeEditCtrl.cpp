#include "stdafx.h"
#include "DateTimeEditCtrl.h"
#include "GridCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CDateTimeEditCtrl

CDateTimeEditCtrl::CDateTimeEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol,
																			bool bDate /*= true*/, UINT nID /*= 100*/ )
: CDateTimeCtrl()
, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
{
	DWORD dwStyle = (WS_CHILD | WS_VISIBLE);
	if( bDate )
		dwStyle |= DTS_SHORTDATEFORMAT;
	else
		dwStyle |= (DTS_UPDOWN | DTS_TIMEFORMAT);
	if( !Create( dwStyle, CalcRect( pGridCtrl->GetCellRect( nRow, nCol ) ), pGridCtrl, nID ) )
		return;
	SetFont( pGridCtrl->GetFont() );
	CString sText = pGridCtrl->GetCellText( nRow, nCol );
	COleDateTime dt;
	if( dt.ParseDateTime( sText, (bDate? VAR_DATEVALUEONLY : VAR_TIMEVALUEONLY) ) )
		SetTime( dt );
	else
		SetWindowText( sText );
	SetFocus();
}

CDateTimeEditCtrl::~CDateTimeEditCtrl()
{
	CString sText;
	GetWindowText( sText );
	mpGridCtrl->SetCellText( mnRow, mnCol, sText );
	DestroyWindow();
}

//static
CRect CDateTimeEditCtrl::CalcRect( const CRect& rcCell )
{
	return CRect( rcCell.left + 4, rcCell.top + 4, rcCell.right - 4, rcCell.bottom - 4 );
}


BEGIN_MESSAGE_MAP(CDateTimeEditCtrl, CDateTimeCtrl)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDateTimeEditCtrl message handlers

HBRUSH CDateTimeEditCtrl::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	if( !IsWindowEnabled() )
		return NULL;

	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return NULL;
	return pColorService->CtlColor( pDC, nCtlColor );
}
