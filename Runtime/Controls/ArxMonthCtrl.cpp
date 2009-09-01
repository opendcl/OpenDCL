// ArxMonthCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxMonthCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxMonthCtrl

CArxMonthCtrl::CArxMonthCtrl( TDclControlPtr pTemplate,
															CControlPane* pPane,
															UINT nID,
															bool bCreate /*= true*/ )
: CMonthCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxMonthCtrl::~CArxMonthCtrl()
{
}


BEGIN_MESSAGE_MAP(CArxMonthCtrl, CMonthCtrl)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(MCN_GETDAYSTATE, OnGetdaystate)
	ON_NOTIFY_REFLECT(MCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(MCN_SELECT, OnSelect)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxMonthCtrl message handlers

void CArxMonthCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove( nFlags, point );
}


void CArxMonthCtrl::OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sGetDayStateEvent = mpTemplate->GetStringProperty( Prop::EventGetDayState );
	if( !sGetDayStateEvent.IsEmpty() )
	{
		NMDAYSTATE* pDayState= (NMDAYSTATE *)pNMHDR;
		resbuf rbCount = { NULL, RTSHORT };
		rbCount.resval.rint = pDayState->cDayState;
		resbuf rbStartDate = { &rbCount, RT3DPOINT };
		COleDateTime dt( pDayState->stStart );
		rbStartDate.resval.rpoint[X] = dt.GetYear();
		rbStartDate.resval.rpoint[Y] = dt.GetMonth();
		rbStartDate.resval.rpoint[Z] = dt.GetDay();
		resbuf* prbResult = NULL;
		GetArxServices()->HandleEvent( sGetDayStateEvent, prbResult, &rbStartDate );
		if( prbResult )
		{
			switch( prbResult->restype )
			{
			case RTSHORT:
				if( !prbResult->rbnext )
					pDayState->prgDayState[0] = (MONTHDAYSTATE)prbResult->resval.rint;
				break;
			case RTLONG:
				if( !prbResult->rbnext )
					pDayState->prgDayState[0] = (MONTHDAYSTATE)prbResult->resval.rlong;
				break;
			case RTREAL:
				if( !prbResult->rbnext )
					pDayState->prgDayState[0] = (MONTHDAYSTATE)prbResult->resval.rreal;
				break;
			case RTPOINT:
				if( !prbResult->rbnext )
				{
					pDayState->prgDayState[0] = (MONTHDAYSTATE)prbResult->resval.rpoint[X];
					if( pDayState->cDayState >= 2 )
						pDayState->prgDayState[1] = (MONTHDAYSTATE)prbResult->resval.rpoint[Y];
				}
				break;
			case RT3DPOINT:
				if( !prbResult->rbnext )
				{
					pDayState->prgDayState[0] = (MONTHDAYSTATE)prbResult->resval.rpoint[X];
					if( pDayState->cDayState >= 2 )
						pDayState->prgDayState[1] = (MONTHDAYSTATE)prbResult->resval.rpoint[Y];
					if( pDayState->cDayState >= 3 )
						pDayState->prgDayState[2] = (MONTHDAYSTATE)prbResult->resval.rpoint[Z];
				}
				break;
			case RTLB:
				resbuf* prbNewStates = prbResult->rbnext;
				int ctMonth = pDayState->cDayState;
				for( int idx = 0;
						 prbNewStates && idx < ctMonth;
						 prbNewStates = prbNewStates->rbnext, ++idx )
				{
					short type = prbNewStates->restype;
					if( type == RTLE )
						break;
					if( type == RTSHORT )
						pDayState->prgDayState[idx] = (MONTHDAYSTATE)prbResult->resval.rint;
					else if( type == RTLONG )
						pDayState->prgDayState[idx] = (MONTHDAYSTATE)prbResult->resval.rlong;
					else if( type == RTREAL )
						pDayState->prgDayState[idx] = (MONTHDAYSTATE)prbResult->resval.rreal;
					else
						break;
				}
				break;
			}
			acutRelRb( prbResult );
		}
	}
	*pResult = 0;
}

void CArxMonthCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventSelChanged );
	*pResult = 0;
}

void CArxMonthCtrl::OnSelect(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventSelect );
	*pResult = 0;
}
