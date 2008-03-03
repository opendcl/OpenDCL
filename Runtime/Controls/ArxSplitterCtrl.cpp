// ArxSplitterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxSplitterCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "Workspace.h"


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
	ReadPosition();
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

void CArxSplitterCtrl::SavePosition() const
{
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpTemplate->GetKeyPath(); 
	CRect rectCurrent;
	GetWindowRect( &rectCurrent );
	GetParent()->ScreenToClient( &rectCurrent );
	AfxGetApp()->WriteProfileInt( sProfileName, _T("Position"), IsVertical()? rectCurrent.left : rectCurrent.top );
}

bool CArxSplitterCtrl::ReadPosition()
{	
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpTemplate->GetKeyPath(); 
	long lPos = AfxGetApp()->GetProfileInt( sProfileName, _T("Position"), -1 );
	if( lPos < 0 )
		return false;
	CRect rcControlArea = mpControlPane->GetControlArea();
	long nMin = mpTemplate->GetLongProperty( Prop::SplitterMin );
	long nMax = mpTemplate->GetLongProperty( Prop::SplitterMax );
	if( IsVertical() )
	{
		if( lPos - rcControlArea.left < nMin )
			lPos = nMin;
		else if( rcControlArea.right - lPos < nMax )
			lPos = rcControlArea.right - nMax;
		mpTemplate->SetLongProperty( Prop::Left, lPos );
	}
	else
	{
		if( lPos - rcControlArea.top < nMin )
			lPos = nMin;
		else if( rcControlArea.bottom - lPos < nMax )
			lPos = rcControlArea.bottom - nMax;
		mpTemplate->SetLongProperty( Prop::Top, lPos );
	}
	return true;
}

BEGIN_MESSAGE_MAP(CArxSplitterCtrl, CSplitterCtrl)
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxSplitterCtrl message handlers

void CArxSplitterCtrl::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	SavePosition();
}

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
