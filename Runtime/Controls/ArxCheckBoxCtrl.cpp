// ArxCheckBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxCheckBoxCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxCheckBoxCtrl

CArxCheckBoxCtrl::CArxCheckBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CCheckBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxCheckBoxCtrl::~CArxCheckBoxCtrl()
{
}

bool CArxCheckBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxCheckBoxCtrl, CCheckBoxCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxCheckBoxCtrl message handlers

void CArxCheckBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnMouseMove(nFlags, point);
}

void CArxCheckBoxCtrl::OnClicked() 
{
	int nValue = GetCheck();
	mpTemplate->SetLongProperty( Prop::Value, nValue );

	if (mpTemplate->m_bEventsAsAction)
	{
		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    
		CString sVal;
		sVal.Format(_T("%d"), nValue);
    struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)sVal, 0);
    stat = acedPutSym(_T("$value"), VarVal);
    acutRelRb(VarVal);

    CString sText = mpTemplate->GetStrProperty(Prop::EventClicked);

		list = acutBuildList(RTSTR, sText, 0);
		if (list != NULL) 
		{ 
			stat = acedInvoke(list, &result); 
			acutRelRb(list); 
			if(result != NULL) 
			{
				acutRelRb(result); 
			}
		}
		GetParent()->GetParent()->EnableWindow(FALSE);
		GetParent()->EnableWindow(TRUE);
	}
	else
	{
		// call methods to invoke the event
		InvokeMethodInt(
			mpTemplate->GetStrProperty(Prop::EventClicked),
			nValue,
			m_bInvokeWithSendString);
	}
}

void CArxCheckBoxCtrl::OnDoubleclicked() 
{
	InvokeMethod(mpTemplate->GetStrProperty(Prop::EventDblClicked), m_bInvokeWithSendString);
}
