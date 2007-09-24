// ArxRadioButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxRadioButtonCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxRadioButtonCtrl

CArxRadioButtonCtrl::CArxRadioButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CRadioButtonCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxRadioButtonCtrl::~CArxRadioButtonCtrl()
{
}

bool CArxRadioButtonCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxRadioButtonCtrl, CRadioButtonCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclRadioButton message handlers

void CArxRadioButtonCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMouseMove(nFlags, point);
}


void CArxRadioButtonCtrl::OnClicked() 
{
	int nValue = __super::GetCheck();
	
	if (mpTemplate->m_bEventsAsAction)
	{
		CString sText = mpTemplate->GetStringProperty(Prop::EventClicked);

		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    
		CString sVal;
		sVal.Format(_T("%d"),nValue);
		struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)sVal, 0);
		stat = acedPutSym(_T("$value"), VarVal);
    acutRelRb(VarVal);
	
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
		if (mpTemplate->GetBooleanProperty( Prop::Value ) != (nValue != BST_UNCHECKED))
		{
			mpTemplate->SetBooleanProperty( Prop::Value, (nValue != BST_UNCHECKED) );
			InvokeMethodInt(
				mpTemplate->GetStringProperty(Prop::EventClicked),
				nValue,
				IsAsyncEvents());
		}
	}
}

void CArxRadioButtonCtrl::OnDoubleclicked() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());
}
