// ArxRectangleCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxRectangleCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxRectangleCtrl

CArxRectangleCtrl::CArxRectangleCtrl( TDclControlPtr pTemplate,
																			CControlPane* pPane,
																			UINT nID,
																			bool bCreate /*= true*/ )
: CRectangleCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxRectangleCtrl::~CArxRectangleCtrl()
{
}
