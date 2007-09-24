// ArxGraphicButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGraphicButtonCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGraphicButtonCtrl

CArxGraphicButtonCtrl::CArxGraphicButtonCtrl( TDclControlPtr pTemplate,
																							CControlPane* pPane,
																							UINT nID,
																							bool bCreate /*= true*/ )
: CGraphicButtonCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxGraphicButtonCtrl::~CArxGraphicButtonCtrl()
{
}

bool CArxGraphicButtonCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxGraphicButtonCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::DragnDropAllowDrop:
		{
			SetDragnDrop( pProp->GetBooleanValue() );
			break;
		}
	}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CArxGraphicButtonCtrl, CGraphicButtonCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CArxGraphicButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetTemplate())
	{
	InvokeMethodIntIntInt(
		GetTemplate()->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	}
	__super::OnMouseMove(nFlags, point);
}

void CArxGraphicButtonCtrl::OnDoubleclicked() 
{
	if (GetTemplate())
	{
		// call methods to invoke the event
		InvokeMethod(GetTemplate()->GetStringProperty(Prop::EventDblClicked), false);
	}
}

void CArxGraphicButtonCtrl::OnClicked() 
{	
	if (GetTemplate())
	{	
		if (GetTemplate()->m_bEventsAsAction)
		{
			GetParent()->GetParent()->EnableWindow(TRUE);
			int stat;
			struct resbuf *result = NULL, *list;    

			CString sText = GetTemplate()->GetStringProperty(Prop::EventClicked);
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
			InvokeMethod(GetTemplate()->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
		}
	}
}

void CArxGraphicButtonCtrl::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = mDropTarget.Register(this);
		mDropTarget.m_pThisArxControl = GetTemplate();
		mDropTarget.m_pParent = this;
	}
	else
		mDropTarget.Revoke();
}


void CArxGraphicButtonCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown(nFlags, point);

	if (GetTemplate() && GetTemplate()->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == 1)
		BeginDragnDrop(GetTemplate(), point, IsAsyncEvents());
}
