// ArxUrlLinkCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxUrlLinkCtrl.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxUrlLinkCtrl

CArxUrlLinkCtrl::CArxUrlLinkCtrl( TDclControlPtr pTemplate,
																	CControlPane* pPane,
																	UINT nID,
																	bool bCreate /*= true*/ )
: CUrlLinkCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxUrlLinkCtrl::~CArxUrlLinkCtrl()
{
}

BEGIN_MESSAGE_MAP(CArxUrlLinkCtrl, CUrlLinkCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxUrlLinkCtrl message handlers

void CArxUrlLinkCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}
