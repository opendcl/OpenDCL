// ArxAnimationCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxAnimationCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAnimationCtrl

CArxAnimationCtrl::CArxAnimationCtrl( TDclControlPtr pTemplate,
																			CControlPane* pPane,
																			UINT nID,
																			bool bCreate /*= true*/ )
: CAnimationCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxAnimationCtrl::~CArxAnimationCtrl()
{
}
