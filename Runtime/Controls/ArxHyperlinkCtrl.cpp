// ArxHyperlinkCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxHyperlinkCtrl.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxHyperlinkCtrl

CArxHyperlinkCtrl::CArxHyperlinkCtrl( TDclControlPtr pTemplate,
																	CControlPane* pPane,
																	UINT nID,
																	bool bCreate /*= true*/ )
: CHyperlinkCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxHyperlinkCtrl::~CArxHyperlinkCtrl()
{
}

BEGIN_MESSAGE_MAP(CArxHyperlinkCtrl, CHyperlinkCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxHyperlinkCtrl message handlers

void CArxHyperlinkCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}
