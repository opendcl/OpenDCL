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

CControlPane::CControlPane(CDclFormObject* pSourceForm, CWnd* pHostDlg)
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
	bool bFailed = false;
	POSITION pos = mpSourceForm->mDclControls.GetTailPosition();
	while (pos != NULL)
	{
		CDclControlObject* pTemplate = mpSourceForm->mDclControls.GetPrev(pos);
		if( !pos)
			break; //we've reached the properties control at the head of the list, just skip it
		if (pTemplate->GetType() < 0)
			continue;
		if (pTemplate->GetType() == CtlFileDlgCtrl)
			continue;
		UINT idDlg = pTemplate->GetID();
		if( idDlg <= 0 || mpSourceForm->GetType() != CtlSplitter )
				idDlg = nId++;
		TDialogControlPtr pControl = CreateNewDialogControl( pTemplate, idDlg );
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

	if( mpSourceForm->mDclControls.GetCount() == 0 )
		return;

	// first lets calc all the control positions for any splitter controls.
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	mpSourceForm->mDclControls.GetNext( pos );
	while( pos )
	{
		CDclControlObject* pControl = mpSourceForm->mDclControls.GetNext( pos );
		if( pControl != NULL && pControl->GetType() == CtlSplitter )
			ResetControlsPos( pControl );
	}

	//next lets calc all the control positions for all NON-splitter controls.
	pos = mpSourceForm->mDclControls.GetHeadPosition();
	mpSourceForm->mDclControls.GetNext( pos );
	while( pos )
	{
		CDclControlObject* pControl = mpSourceForm->mDclControls.GetNext( pos );
		if( pControl->GetType() != CtlSplitter )
			ResetControlsPos( pControl );
	}
	//mpHostDlg->Invalidate(); //can't do this: it paints over the controls in Windows XP with Window Classic theme
}

void CControlPane::InvalidateControls()
{
	if( !mpSourceForm )
		return;

	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	while( pos )
	{
		CDclControlObject *pControl = mpSourceForm->mDclControls.GetNext( pos );
		if( pControl->GetType() != CtlBlockView && pControl->GetType() != CtlHatch )
		{
			CWnd* pWnd = pControl->GetWindow();
			if( pWnd )
				pWnd->Invalidate();
		}
	}
}

CRect CControlPane::GetSplitterRect( int nId, CRect& rectCurrent ) 
{
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	while( pos )
	{
		CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetNext( pos );
		if( pCtrl->GetType() == CtlSplitter && pCtrl->GetID() == nId )
		{
			pCtrl->GetWindow()->GetWindowRect( &rectCurrent );
			mpHostDlg->ScreenToClient( &rectCurrent );
			CPoint pt( pCtrl->GetLongProperty( nLeft ), pCtrl->GetLongProperty( nTop ) );
			return CRect( pt.x, pt.y, pt.x + pCtrl->GetLongProperty( nWidth ), pt.y + pCtrl->GetLongProperty( nHeight ) );
		}
	}
	rectCurrent.SetRect(0,0,0,0);
	return CRect(0,0,0,0);	
}

void CControlPane::ResetControlsPos(CDclControlObject *pDclControl)
{
	if (pDclControl == NULL)
		return;
	if (pDclControl->GetPropertyObject(nUseLeftFromRight) == NULL)
		return;

	// get the offset flags
	int lLeftFromRight = 0;
	if (pDclControl->GetPropertyObject(nUseLeftFromRight)->GetType() == PropBool)
		lLeftFromRight = pDclControl->GetPropertyObject(nUseLeftFromRight)->GetBooleanValue();
	else
		lLeftFromRight = pDclControl->GetPropertyObject(nUseLeftFromRight)->GetLongValue();

	int lRightFromRight = 0;
	if (pDclControl->GetPropertyObject(nUseRightFromRight)->GetType() == PropBool)
		lRightFromRight = pDclControl->GetPropertyObject(nUseRightFromRight)->GetBooleanValue();
	else
		lRightFromRight = pDclControl->GetPropertyObject(nUseRightFromRight)->GetLongValue();

	int lTopFromBottom = 0;
	if (pDclControl->GetPropertyObject(nUseRightFromRight)->GetType() == PropBool)
		lTopFromBottom = pDclControl->GetPropertyObject(nUseTopFromBottom)->GetBooleanValue();
	else
		lTopFromBottom = pDclControl->GetPropertyObject(nUseTopFromBottom)->GetLongValue();

	int lBottomFromBottom = 0;
	if (pDclControl->GetPropertyObject(nUseRightFromRight)->GetType() == PropBool)
		lBottomFromBottom = pDclControl->GetPropertyObject(nUseBottomFromBottom)->GetBooleanValue();
	else
		lBottomFromBottom = pDclControl->GetPropertyObject(nUseBottomFromBottom)->GetLongValue();

	//if (mpSourceForm->GetType() == VdclModal ||
	//		mpSourceForm->GetType() == VdclModeless)
	//{
	//	if (lLeftFromRight  == 0 &&
	//			lRightFromRight == 0 &&
	//			lTopFromBottom  == 0 &&
	//			lBottomFromBottom == 0)
	//		return;
	//}

	
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
	if (lLeftFromRight  > 0 ||
			lRightFromRight > 0 ||
			lTopFromBottom  > 0 ||
			lBottomFromBottom > 0)
	{	
		// set the left position if required
		if (lLeftFromRight == 1)
		{
			int nOffsetValue = pDclControl->GetPropertyObject(nLeftFromRight)->GetLongValue();
			rcControl.left = rcThis.right - nOffsetValue;
		}
		else if (lLeftFromRight == 2)
		{
			int nFormCenter = ((rcThis.right + rcThis.left) / 2);
			int nControlOffsetFromCenter = pDclControl->GetPropertyObject(nLeftFromRight)->GetLongValue();
			rcControl.left = nFormCenter + nControlOffsetFromCenter;
		}
		else if (lLeftFromRight > 2)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lLeftFromRight, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(nLeftFromRight)->GetLongValue();
			rcControl.left = rectCurrent.left + nOffsetValue;
		}		
		else
			rcControl.left = rcThis.left + pDclControl->GetPropertyObject(nLeft)->GetLongValue();

		// set the right position if required
		if (lRightFromRight == 1 && pDclControl->GetType() != CtlCheckBox)
		{			
			int nOffsetValue = pDclControl->GetPropertyObject(nRightFromRight)->GetLongValue();
			rcControl.right = rcThis.right - nOffsetValue;
		}
		else if (lRightFromRight > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lRightFromRight, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(nRightFromRight)->GetLongValue();
			rcControl.right = rectCurrent.left + nOffsetValue;
		}		
		else
		{
			int nWidthValue = pDclControl->GetPropertyObject(nWidth)->GetLongValue();				
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (lTopFromBottom == 1)
		{
			int nOffsetValue = pDclControl->GetPropertyObject(nTopFromBottom)->GetLongValue();				
			rcControl.top = rcThis.bottom - nOffsetValue;
		}
		else if (lTopFromBottom > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lTopFromBottom, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(nTopFromBottom)->GetLongValue();
			rcControl.top = rectCurrent.top + nOffsetValue;
		}
		else
			rcControl.top = rcThis.top + pDclControl->GetPropertyObject(nTop)->GetLongValue();

		// set the top position if required
		if (lBottomFromBottom  == 1 && pDclControl->GetType() != CtlCheckBox)
		{
			int nOffsetValue = pDclControl->GetPropertyObject(nBottomFromBottom)->GetLongValue();				
			rcControl.bottom = rcThis.bottom - nOffsetValue;
		}
		else if (lBottomFromBottom > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lBottomFromBottom, rectCurrent);
			int nOffsetValue = pDclControl->GetPropertyObject(nBottomFromBottom)->GetLongValue();
			rcControl.bottom = rectCurrent.top - nOffsetValue;
		}
		else
		{
			int nHeightValue = pDclControl->GetPropertyObject(nHeight)->GetLongValue();				
			rcControl.bottom = rcControl.top + nHeightValue;
		}
	}
	else // if not to be offset at all
	{
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			int nLeftValue = pDclControl->GetPropertyObject(nLeft)->GetLongValue();	
			int nTopValue = pDclControl->GetPropertyObject(nTop)->GetLongValue();	
			int nWidthValue = pDclControl->GetPropertyObject(nWidth)->GetLongValue();	
			int nHeightValue = pDclControl->GetPropertyObject(nHeight)->GetLongValue();	
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
		rcControl.bottom += pDclControl->GetLongProperty( nDropDownHeight );
	
	pControl->MoveWindow( rcControl, TRUE);
	
	if (pDclControl->GetType() == CtlMonth)
		((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
}

void CControlPane::CleanUpControls() 
{	
	for (int idx = mControls.size() - 1; idx >= 0; --idx)
	{
		CWnd* pControl = mControls[idx]->GetControl();
		if( pControl )
			pControl->DestroyWindow();
	}
	mControls.clear();
}

void CControlPane::ShowControls(BOOL bShow)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	
	if (mpSourceForm == NULL)
		return;

	// set counter for clipboard
	int nCount = 1;
	int nTotal = mpSourceForm->mDclControls.GetCount();
	if (nTotal == 0)
		return;

	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		if (pos != NULL)
		{
			// get current property
			CDclControlObject* pArxControlForm = mpSourceForm->mDclControls.GetAt(pos);
			
			if (pArxControlForm != NULL)
			{
				// return a pointer to the CWnd control
				CWnd *pControl = pArxControlForm->GetWindow();// mpHostDlg->GetDlgItem(pArxControlForm->m_Id);
				if (pControl != NULL)
				{
					if (bShow == TRUE)
					{
						if (pArxControlForm->GetPropertyObject(nVisible) != NULL)
						{
							BOOL bShowControl = pArxControlForm->GetPropertyObject(nVisible)->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl == TRUE)
								if (IsWindow(pControl->m_hWnd))		
									pControl->ShowWindow(pArxControlForm->GetPropertyObject(nVisible)->GetBooleanValue());
						}
						else
						{
							BOOL bShowControl = pArxControlForm->GetPropertyObject(nVisible)->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl > -1)
								if (IsWindow(pControl->m_hWnd))		
									pControl->ShowWindow(bShowControl);
						}
					}
				}
			}
		}
		// increment counter
		nCount++;
	}

}

void CControlPane::ShowPictureBoxes(BOOL bShow)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	
	if (mpSourceForm == NULL)
		return;

	// set counter for clipboard
	int nCount = 1;
	int nTotal = mpSourceForm->mDclControls.GetCount();
	if (nTotal == 0)
		return;

	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		if (pos != NULL)
		{
			// get current property
			CDclControlObject* pArxControlForm = mpSourceForm->mDclControls.GetAt(pos);
			
			if (pArxControlForm && pArxControlForm->GetType() == CtlPictureBox)
			{
				// return a pointer to the CWnd control
				CWnd *pControl = pArxControlForm->GetWindow(); 
				if (pControl != NULL)
				{
					if (bShow == TRUE)
					{
						if (pArxControlForm->GetPropertyObject(nVisible) != NULL)
						{
							BOOL bShowControl = pArxControlForm->GetPropertyObject(nVisible)->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl == TRUE)
								if (IsWindow(pControl->m_hWnd))																		
									pControl->ShowWindow(pArxControlForm->GetPropertyObject(nVisible)->GetBooleanValue());
						}
						else
						{
							BOOL bShowControl = pArxControlForm->GetBoolProperty(nVisible);
							if (!pControl->IsWindowVisible() && bShowControl > -1)
								if (IsWindow(pControl->m_hWnd))									
									pControl->ShowWindow(bShowControl);							
						}
					}
					else
						if (IsWindow(pControl->m_hWnd))									
							pControl->ShowWindow(bShow);			
					
				}
			}
		}
		// increment counter
		nCount++;
	}
}


void CControlPane::SetFirstControlFocus() const
{
	if(!mpSourceForm)
		return;
	POSITION pos = mpSourceForm->mDclControls.GetTailPosition();
	while( pos )
	{
		CDclControlObject* pControl = mpSourceForm->mDclControls.GetPrev( pos );
		if (pControl == NULL )
			continue;
		CWnd* pWnd = pControl->GetWindow();
		if( pWnd == NULL)
			continue;
		if( (pWnd->GetStyle() & (WS_VISIBLE | WS_TABSTOP | WS_DISABLED)) != (WS_VISIBLE | WS_TABSTOP) )
			continue;
		pWnd->SetFocus();
		return;
	}
}
