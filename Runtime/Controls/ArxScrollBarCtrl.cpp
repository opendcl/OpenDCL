// ArxScrollBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxScrollBarCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxScrollBarCtrl

CArxScrollBarCtrl::CArxScrollBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CScrollBarCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxScrollBarCtrl::~CArxScrollBarCtrl()
{
}

bool CArxScrollBarCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

void CArxScrollBarCtrl::OnScroll(UINT nSBCode, UINT nPos) 
{
	__super::OnScroll( nSBCode, nPos );
	bool bDoneScrolling = false;
	switch( nSBCode )
	{
	case 4:
	case 8:
		bDoneScrolling = true; // user is done scrolling the scrollbar
		break;
	}
	int nNewPos = mpTemplate->GetLongProperty( Prop::Value );
	if( bDoneScrolling )
		InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScrolled ), nNewPos, IsAsyncEvents() );
	else
		InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), nNewPos, IsAsyncEvents() );
}
