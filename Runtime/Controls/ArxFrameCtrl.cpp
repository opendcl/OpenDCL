// ArxFrameCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxFrameCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxFrameCtrl

CArxFrameCtrl::CArxFrameCtrl( TDclControlPtr pTemplate,
															CControlPane* pPane,
															UINT nID,
															bool bCreate /*= true*/ )
: CFrameCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxFrameCtrl::~CArxFrameCtrl()
{
}
