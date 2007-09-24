// ControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ControlPane.h"
#include "DialogControl.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Project.h"


/////////////////////////////////////////////////////////////////////////////
// CControlPane

CControlPane::CControlPane()
: mpSourceForm( NULL )
, mpProject( NULL )
, mpHostDlg( NULL )
, mlLeftOffset( 0 )
, mlRightOffset( 0 )
, mlTopOffset( 0 )
, mlBottomOffset( 0 )
{
}

CControlPane::CControlPane(TDclFormPtr pSourceForm, CWnd* pHostDlg)
: mpSourceForm( pSourceForm )
, mpProject( pSourceForm? pSourceForm->GetProject() : NULL )
, mpHostDlg( pHostDlg )
, mlLeftOffset( 0 )
, mlRightOffset( 0 )
, mlTopOffset( 0 )
, mlBottomOffset( 0 )
{
}

CControlPane::~CControlPane()
{
}

void CControlPane::SetPanePos( CRect rectNew, bool bRecalc /*= true*/ )
{
	assert( mpHostDlg->m_hWnd != NULL );
	CRect rectHostClient;
	mpHostDlg->GetClientRect( &rectHostClient );
	mlLeftOffset = rectNew.left - rectHostClient.left;
	mlTopOffset = rectNew.top - rectHostClient.top;
	mlRightOffset = rectNew.right - rectHostClient.right;
	mlBottomOffset = rectNew.bottom - rectHostClient.bottom;
	if( bRecalc )
		RecalcLayout();
}

void CControlPane::ZOrderFront(CWnd *pControl)
{
	if (pControl != NULL)
		pControl->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
}

void CControlPane::ZOrderBack(CWnd *pControl)
{
	if (pControl != NULL)
		pControl->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
}

void CControlPane::AddControl( TDialogControlPtr pControl )
{
	mControls.push_back( pControl );
}

bool CControlPane::CreateControls(UINT& nId)
{
	TDclControlList& Controls = mpSourceForm->mDclControls;
	if( Controls.empty() )
		return true;
	bool bFailed = false;
	TDclControlList::iterator iter = Controls.end();
	while( iter != Controls.begin() )
	{
		--iter;
		if ((*iter)->GetType() <= CtlForm)
			continue;
		if ((*iter)->GetType() == CtlFileDlgCtrl)
			continue;
		UINT idDlg = (*iter)->GetID();
		if( idDlg <= 0 || mpSourceForm->GetType() != CtlSplitter )
			idDlg = nId++;
		TDialogControlPtr pControl = CreateNewDialogControl( (*iter), idDlg );
		assert( pControl != NULL );
		if( pControl )
			mControls.push_back( pControl );
		else
			bFailed = true;
	}
	return !bFailed;
}

void CControlPane::RecalcLayout()
{
	if( !mpSourceForm )
		return;

	if( mpSourceForm->GetControlCount() == 0 )
		return;

	// first lets calc all the control positions for any splitter controls.
	TDclControlList& Controls = mpSourceForm->mDclControls;
	for( TDclControlList::iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() == CtlSplitter )
			ResetControlsPos( (*iter) );
	}

	//next lets calc all the control positions for all NON-splitter controls.
	for( TDclControlList::iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() != CtlSplitter )
			ResetControlsPos( (*iter) );
	}
	//mpHostDlg->Invalidate(); //can't do this: it paints over the controls in Windows XP with Window Classic theme
}

void CControlPane::InvalidateControls()
{
	if( !mpSourceForm )
		return;

	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() != CtlBlockView && (*iter)->GetType() != CtlHatch )
		{
			CWnd* pWnd = (*iter)->GetWindow();
			if( pWnd )
				pWnd->Invalidate();
		}
	}
}

CRect CControlPane::GetSplitterRect( int nId, CRect& rectCurrent ) 
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() == CtlSplitter && (*iter)->GetID() == nId )
		{
			(*iter)->GetWindow()->GetWindowRect( &rectCurrent );
			mpHostDlg->ScreenToClient( &rectCurrent );
			CPoint pt( (*iter)->GetLongProperty( Prop::Left ), (*iter)->GetLongProperty( Prop::Top ) );
			return CRect( pt.x, pt.y, pt.x + (*iter)->GetLongProperty( Prop::Width ), pt.y + (*iter)->GetLongProperty( Prop::Height ) );
		}
	}
	rectCurrent.SetRect(0,0,0,0);
	return CRect(0,0,0,0);	
}

void CControlPane::ResetControlsPos(TDclControlPtr pDclControl)
{
	if (pDclControl == NULL)
		return;
	if (pDclControl->GetPropertyObject(Prop::UseLeftFromRight) == NULL)
		return;

	// get the offset flags
	int lLeftFromRight = 0;
	if (pDclControl->GetPropertyObject(Prop::UseLeftFromRight)->GetType() == PropBool)
		lLeftFromRight = pDclControl->GetPropertyObject(Prop::UseLeftFromRight)->GetBooleanValue();
	else
		lLeftFromRight = pDclControl->GetPropertyObject(Prop::UseLeftFromRight)->GetLongValue();

	int lRightFromRight = 0;
	if (pDclControl->GetPropertyObject(Prop::UseRightFromRight)->GetType() == PropBool)
		lRightFromRight = pDclControl->GetPropertyObject(Prop::UseRightFromRight)->GetBooleanValue();
	else
		lRightFromRight = pDclControl->GetPropertyObject(Prop::UseRightFromRight)->GetLongValue();

	int lTopFromBottom = 0;
	if (pDclControl->GetPropertyObject(Prop::UseRightFromRight)->GetType() == PropBool)
		lTopFromBottom = pDclControl->GetPropertyObject(Prop::UseTopFromBottom)->GetBooleanValue();
	else
		lTopFromBottom = pDclControl->GetPropertyObject(Prop::UseTopFromBottom)->GetLongValue();

	int lBottomFromBottom = 0;
	if (pDclControl->GetPropertyObject(Prop::UseRightFromRight)->GetType() == PropBool)
		lBottomFromBottom = pDclControl->GetPropertyObject(Prop::UseBottomFromBottom)->GetBooleanValue();
	else
		lBottomFromBottom = pDclControl->GetPropertyObject(Prop::UseBottomFromBottom)->GetLongValue();
	
	// get the control being moved			
	CWnd *pControl = pDclControl->GetWindow();

	//TODO: move this to ArxControlPane.cpp
	//if (pDclControl->GetType() == CtlGraphicButton)
	//{
	//	((CButtonST*)pControl)->m_bDrawTransparent = FALSE;
	//}

	if (pControl == NULL)
		return;

	CRect rcControl;
	pControl->GetWindowRect( &rcControl );
	mpHostDlg->ScreenToClient( &rcControl );

	CRect rcThis;
	mpHostDlg->GetClientRect(&rcThis);
	rcThis.top += mlTopOffset;
	rcThis.bottom += mlBottomOffset;
	rcThis.left += mlLeftOffset;
	rcThis.right += mlRightOffset;

	// if mpHostDlg control is to be moved according to the right or bottom side of the form
	if (lLeftFromRight  > 0 || lRightFromRight > 0 || lTopFromBottom  > 0 || lBottomFromBottom > 0)
	{	
		// set the left position if required
		if (lLeftFromRight == 1)
		{
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::LeftFromRight)->GetLongValue();
			rcControl.left = rcThis.right - nOffsetValue;
		}
		else if (lLeftFromRight == 2)
		{
			int nFormCenter = ((rcThis.right + rcThis.left) / 2);
			int nControlOffsetFromCenter = pDclControl->GetPropertyObject(Prop::LeftFromRight)->GetLongValue();
			rcControl.left = nFormCenter + nControlOffsetFromCenter;
		}
		else if (lLeftFromRight > 2)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lLeftFromRight, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::LeftFromRight)->GetLongValue();
			rcControl.left = rectCurrent.left + nOffsetValue;
		}		
		else
			rcControl.left = rcThis.left + pDclControl->GetPropertyObject(Prop::Left)->GetLongValue();

		// set the right position if required
		if (lRightFromRight == 1 && pDclControl->GetType() != CtlCheckBox)
		{			
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::RightFromRight)->GetLongValue();
			rcControl.right = rcThis.right - nOffsetValue;
		}
		else if (lRightFromRight > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lRightFromRight, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::RightFromRight)->GetLongValue();
			rcControl.right = rectCurrent.left + nOffsetValue;
		}		
		else
		{
			int nWidthValue = pDclControl->GetPropertyObject(Prop::Width)->GetLongValue();				
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (lTopFromBottom == 1)
		{
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::TopFromBottom)->GetLongValue();				
			rcControl.top = rcThis.bottom - nOffsetValue;
		}
		else if (lTopFromBottom > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lTopFromBottom, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::TopFromBottom)->GetLongValue();
			rcControl.top = rectCurrent.top + nOffsetValue;
		}
		else
			rcControl.top = rcThis.top + pDclControl->GetPropertyObject(Prop::Top)->GetLongValue();

		// set the top position if required
		if (lBottomFromBottom  == 1 && pDclControl->GetType() != CtlCheckBox)
		{
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::BottomFromBottom)->GetLongValue();				
			rcControl.bottom = rcThis.bottom - nOffsetValue;
		}
		else if (lBottomFromBottom > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lBottomFromBottom, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(Prop::BottomFromBottom)->GetLongValue();
			rcControl.bottom = rectCurrent.top - nOffsetValue;
		}
		else
		{
			int nHeightValue = pDclControl->GetPropertyObject(Prop::Height)->GetLongValue();				
			rcControl.bottom = rcControl.top + nHeightValue;
		}
	}
	else // if not to be offset at all
	{
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			int nLeftValue = pDclControl->GetPropertyObject(Prop::Left)->GetLongValue();	
			int nTopValue = pDclControl->GetPropertyObject(Prop::Top)->GetLongValue();	
			int nWidthValue = pDclControl->GetPropertyObject(Prop::Width)->GetLongValue();	
			int nHeightValue = pDclControl->GetPropertyObject(Prop::Height)->GetLongValue();	
			rcControl.SetRect( rcThis.left + nLeftValue, rcThis.top + nTopValue,
												 rcThis.left + nLeftValue + nWidthValue, rcThis.top + nTopValue + nHeightValue);	
		}
	}

	// check the width to ensure that it is at least 2
	if (rcControl.right - rcControl.left < 2)
		rcControl.right = rcControl.left + 2;

	// check the height to ensure that it is at least 2
	if (rcControl.bottom - rcControl.top < 2)
		rcControl.bottom = rcControl.top + 2;

	// check the top is less then 1 then keep it at 0
	if (rcControl.top < 1)
		rcControl.top = 1;
	
	// check the left is less then 1 then keep it at 0
	if (rcControl.left < 1)
		rcControl.left = 1;
	
	if( (pDclControl->GetType() == CtlComboBox || pDclControl->GetType() == CtlImageComboBox) &&
			((pControl->GetStyle() & CBS_DROPDOWN) != 0) )
		rcControl.bottom += pDclControl->GetLongProperty( Prop::DropDownHeight );
	
	pControl->MoveWindow( rcControl, TRUE);
	
	if (pDclControl->GetType() == CtlMonth)
		((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
}

void CControlPane::CleanUpControls() 
{	
	for (int idx = mControls.size() - 1; idx >= 0; --idx)
	{
		CWnd* pControl = mControls[idx]->GetControlWnd();
		if( pControl )
			pControl->DestroyWindow();
	}
	mControls.clear();
}


void CControlPane::SetFirstControlFocus() const
{
	if(!mpSourceForm)
		return;
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	TDclControlList::const_iterator iter = Controls.end();
	while( iter != Controls.begin() )
	{
		--iter;
		CWnd* pWnd = (*iter)->GetWindow();
		if( !pWnd )
			continue;
		if( (pWnd->GetStyle() & (WS_VISIBLE | WS_TABSTOP | WS_DISABLED)) != (WS_VISIBLE | WS_TABSTOP) )
			continue;
		pWnd->SetFocus();
		return;
	}
}
