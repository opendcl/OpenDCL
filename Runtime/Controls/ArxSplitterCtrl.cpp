// ArxSplitterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxSplitterCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxSplitterCtrl

CArxSplitterCtrl::CArxSplitterCtrl( TDclControlPtr pTemplate,
																		CControlPane* pPane,
																		UINT nID,
																		bool bCreate /*= true*/ )
: CSplitterCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxSplitterCtrl::~CArxSplitterCtrl()
{
}

bool CArxSplitterCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxSplitterCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::DragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CArxSplitterCtrl, CSplitterCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton message handlers

void CArxSplitterCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}

void CArxSplitterCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventClicked ), IsAsyncEvents() );
}

__UINT_LRESULT CArxSplitterCtrl::OnNcHitTest(CPoint point)
{
	if( !mpTemplate->GetStringProperty(Prop::EventMouseMove).IsEmpty() ||
			!mpTemplate->GetStringProperty(Prop::EventClicked).IsEmpty() ||
			mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) )
		return HTCLIENT;
	return __super::OnNcHitTest( point );
}
