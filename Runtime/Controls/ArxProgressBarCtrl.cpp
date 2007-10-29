// ArxProgressBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxProgressBarCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxProgressBarCtrl

CArxProgressBarCtrl::CArxProgressBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CProgressBarCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxProgressBarCtrl::~CArxProgressBarCtrl()
{
}

bool CArxProgressBarCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxProgressBarCtrl, CProgressBarCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxProgressBarCtrl message handlers
