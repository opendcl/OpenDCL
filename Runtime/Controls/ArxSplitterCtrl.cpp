// ArxSplitterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxSplitterCtrl.h"
#include "DclControlTemplate.h"
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
		SetPosLeft( lPos );
	}
	else
	{
		if( lPos - rcControlArea.top < nMin )
			lPos = nMin;
		else if( rcControlArea.bottom - lPos < nMax )
			lPos = rcControlArea.bottom - nMax;
		SetPosTop( lPos );
	}
	return true;
}

BEGIN_MESSAGE_MAP(CArxSplitterCtrl, CSplitterCtrl)
	ON_WM_MOVE()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxSplitterCtrl message handlers

void CArxSplitterCtrl::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	SavePosition();
	CRect rcWnd = GetEffectiveWindowRect();
	GetArxServices()->HandleEvent( Prop::EventSplitterMoved,
																 args_NNNN( rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height() ) );
}

void CArxSplitterCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if( GetArxServices()->HandleEvent( Prop::EventClicked ) )
		return;
	__super::OnNcLButtonDown( nHitTest, point );
}
