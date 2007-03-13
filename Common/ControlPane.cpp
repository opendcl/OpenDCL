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


// this method ensures all graphic buttons and picture boxes and slide holders are setup 
// correctly to enure the graphic buttons are transparent.	
// It actually does the control sorting.
void CControlPane::SetGrphcBtnsParents(bool bForceRefresh)
{
	POSITION btnPos;
	POSITION parentPos;

	for (int i=1; i<mpSourceForm->mDclControls.GetCount(); i++)
	{
		btnPos = mpSourceForm->mDclControls.FindIndex(i);
		if (btnPos != NULL)
		{
			CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetAt(btnPos);
			if (pCtrl->GetType() == CtlGraphicButton)
			{
				for (int j=1; j<i; j++)
				{
					parentPos = mpSourceForm->mDclControls.FindIndex(j);					
					if (parentPos != NULL)
					{
						CDclControlObject *pOtherCtrl = mpSourceForm->mDclControls.GetAt(parentPos);
						if (pOtherCtrl->GetType() == CtlPictureBox ||
							pOtherCtrl->GetType() == CtlSlideView)
						{
							//pOtherCtrl->GetWindow()->Invalidate();
							SetGrphcBtnsParents(pCtrl, pOtherCtrl, bForceRefresh);
							
						}
					}				
				}				
			}
		}
	}
}

// this method ensures the buttons are on top of the correct controls
void CControlPane::SetGrphcBtnsParents(CDclControlObject *pGrphcBtn, CDclControlObject *pOtherBtn, bool bForceRefresh)
{
	int nBorderOffset = pOtherBtn->GetLngProperty(nBorderStyle);

	if (nBorderOffset == 1)
		nBorderOffset = 2;
	else if (nBorderOffset == 2)
		nBorderOffset = 1;
	
	CRect rcGrphcBtn;
	CRect rcOtherBtn;

	if (pGrphcBtn->GetWindow() == NULL)
	{
		rcGrphcBtn.left = pGrphcBtn->GetLngProperty(nLeft);
		rcGrphcBtn.top = pGrphcBtn->GetLngProperty(nTop);
		rcGrphcBtn.right = pGrphcBtn->GetLngProperty(nLeft) + pGrphcBtn->GetLngProperty(nWidth);
		rcGrphcBtn.bottom = pGrphcBtn->GetLngProperty(nTop) + pGrphcBtn->GetLngProperty(nHeight);
		
	
		rcOtherBtn.left = pOtherBtn->GetLngProperty(nLeft) + nBorderOffset;
		rcOtherBtn.top = pOtherBtn->GetLngProperty(nTop) + nBorderOffset;
		rcOtherBtn.right = pOtherBtn->GetLngProperty(nLeft) + pOtherBtn->GetLngProperty(nWidth) - nBorderOffset;
		rcOtherBtn.bottom = pOtherBtn->GetLngProperty(nTop) + pOtherBtn->GetLngProperty(nHeight) - nBorderOffset;
	}
	else
	{
		pGrphcBtn->GetWindow()->GetWindowRect(&rcGrphcBtn);
		pOtherBtn->GetWindow()->GetWindowRect(&rcOtherBtn);
		pGrphcBtn->GetWindow()->GetParent()->ScreenToClient(rcGrphcBtn);
		pOtherBtn->GetWindow()->GetParent()->ScreenToClient(rcOtherBtn);
	}

	// check if the button is inside the paintable area of the posible parent control
	if (rcOtherBtn.left <= rcGrphcBtn.left &&
		rcOtherBtn.top <= rcGrphcBtn.top &&
		rcOtherBtn.right >= rcGrphcBtn.right &&
		rcOtherBtn.bottom >= rcGrphcBtn.bottom)
	{
		//CXPStyleButtonST *pBtn = (CXPStyleButtonST*)pGrphcBtn->GetWindow();
		//pBtn->m_bDrawTransparent = TRUE;
		//pBtn->m_pParentBackgound = pOtherBtn->GetWindow();
		if (bForceRefresh)
		{
			//pBtn->ClearTransparancy();
			//pBtn->DrawTransparent(TRUE);
			//pBtn->Invalidate();
			
			//pBtn->ShowWindow(FALSE);
			//pBtn->ShowWindow(pGrphcBtn->GetBoolProperty(nVisible));			
		}
	}
	else
	{
		/*if (((CXPStyleButtonST*)pGrphcBtn->GetWindow())->m_pParentBackgound == pOtherBtn->GetWindow())
		{
			((CXPStyleButtonST*)pGrphcBtn->GetWindow())->ClearTransparancy();
			//
		}*/
	}
	if (pGrphcBtn->GetWindow())
		pGrphcBtn->GetWindow()->ShowWindow(pGrphcBtn->m_pVisible->GetBooleanValue());
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

	// call method to ensure all graphic buttons and picture boxes and slide holders are setup 
	// correctly to enure the graphic buttons are transparent.	
	//SetGrphcBtnsParents(true);

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
			CPoint pt( pCtrl->GetLngProperty( nLeft ), pCtrl->GetLngProperty( nTop ) );
			return CRect( pt.x, pt.y, pt.x + pCtrl->GetLngProperty( nWidth ), pt.y + pCtrl->GetLngProperty( nHeight ) );
		}
	}
	rectCurrent.SetRect(0,0,0,0);
	return CRect(0,0,0,0);	
}

void CControlPane::ResetControlsPos(CDclControlObject *pArxObject)
{
	if (pArxObject == NULL)
		return;
	if (pArxObject->m_pUseLeftOffset == NULL)
		return;

	// get the offset flags
	int lLeftFromRight = 0;
	if (pArxObject->m_pUseLeftOffset->GetType() == PropBool)
		lLeftFromRight = pArxObject->m_pUseLeftOffset->GetBooleanValue();
	else
		lLeftFromRight = pArxObject->m_pUseLeftOffset->GetLongValue();

	int lRightFromRight = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		lRightFromRight = pArxObject->m_pUseRightOffset->GetBooleanValue();
	else
		lRightFromRight = pArxObject->m_pUseRightOffset->GetLongValue();

	int lTopFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		lTopFromBottom = pArxObject->m_pUseTopOffset->GetBooleanValue();
	else
		lTopFromBottom = pArxObject->m_pUseTopOffset->GetLongValue();

	int lBottomFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		lBottomFromBottom = pArxObject->m_pUseBottomOffset->GetBooleanValue();
	else
		lBottomFromBottom = pArxObject->m_pUseBottomOffset->GetLongValue();

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
	CWnd *pControl = pArxObject->GetWindow();

	//TODO: move this to ArxControlPane.cpp
	//if (pArxObject->GetType() == CtlGraphicButton)
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
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			rcControl.left = rcThis.right - nOffsetValue;
		}
		else if (lLeftFromRight == 2)
		{
			int nFormCenter = ((rcThis.right + rcThis.left) / 2);
			int nControlOffsetFromCenter = pArxObject->m_pOffsetLeft->GetLongValue();
			rcControl.left = nFormCenter + nControlOffsetFromCenter;
		}
		else if (lLeftFromRight > 2)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lLeftFromRight, rectCurrent);
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			rcControl.left = rectCurrent.left + nOffsetValue;
		}		
		else
			rcControl.left = rcThis.left + pArxObject->m_pLeft->GetLongValue();

		// set the right position if required
		if (lRightFromRight == 1 && pArxObject->GetType() != CtlCheckBox)
		{			
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();
			rcControl.right = rcThis.right - nOffsetValue;
		}
		else if (lRightFromRight > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lRightFromRight, rectCurrent);
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();
			rcControl.right = rectCurrent.left + nOffsetValue;
		}		
		else
		{
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();				
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (lTopFromBottom == 1)
		{
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();				
			rcControl.top = rcThis.bottom - nOffsetValue;
		}
		else if (lTopFromBottom > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lTopFromBottom, rectCurrent);
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();
			rcControl.top = rectCurrent.top + nOffsetValue;
		}
		else
			rcControl.top = rcThis.top + pArxObject->m_pTop->GetLongValue();

		// set the top position if required
		if (lBottomFromBottom  == 1 && pArxObject->GetType() != CtlCheckBox)
		{
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();				
			rcControl.bottom = rcThis.bottom - nOffsetValue;
		}
		else if (lBottomFromBottom > 1)
		{
			CRect rectCurrent;
			CRect rc = GetSplitterRect(lBottomFromBottom, rectCurrent);
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();
			rcControl.bottom = rectCurrent.top - nOffsetValue;
		}
		else
		{
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();				
			rcControl.bottom = rcControl.top + nHeightValue;
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
		
		
		// mpHostDlg control is a tab control
		//TODO: move this to ArxControlPane.cpp
		//if (pArxObject->GetType() == CtlTabStrip)
		//{
		//	// directly set it's internal size/position CRect
		//	((VdclTab*)pControl)->m_rcPos = rcControl;
		//}
		
		//// redraw graphic buttons
		//if (pArxObject->GetType() == CtlGraphicButton)
		//	pControl->ShowWindow(FALSE);

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
				pControl->ShowWindow(FALSE);
		}
		
		pControl->MoveWindow( rcControl, TRUE);
		
		// mpHostDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);

		//if (pArxObject->GetType() == CtlGraphicButton)
		//	pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue()? SW_SHOW : SW_HIDE);

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
		}
		
		// again mpHostDlg control is a tab control
		//TODO: move this to ArxControlPane.cpp
		//if (pArxObject->GetType() == CtlTabStrip)
		//{
		//	((VdclTab*)pControl)->SetPaneVisibility(((VdclTab*)pControl)->m_nCurrentSelectedTab, TRUE);
		//}
	}
	else // if not to be offset at all
	{
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			int nLeftValue = pArxObject->m_pLeft->GetLongValue();	
			int nTopValue = pArxObject->m_pTop->GetLongValue();	
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();	
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();	
			rcControl.SetRect( rcThis.left + nLeftValue, rcThis.top + nTopValue,
												 rcThis.left + nLeftValue + nWidthValue, rcThis.top + nTopValue + nHeightValue);	
		}
		//else
		//{
		//	pArxObject->GetWindow()->GetWindowRect(&rcControl);
		//	mpHostDlg->ScreenToClient(rcControl);
		//}

		// mpHostDlg control is a tab control
		//TODO: move this to ArxControlPane.cpp
		//if (pArxObject->GetType() == CtlTabStrip)
		//{
		//	// directly set it's internal size/position CRect
		//	((VdclTab*)pControl)->m_rcPos = rcControl;
		//}
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
	
		pControl->MoveWindow(rcControl, TRUE);
		
		
		// mpHostDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		// again mpHostDlg control is a tab control
		//TODO: move this to ArxControlPane.cpp
		//if (pArxObject->GetType() == CtlTabStrip)
		//{
		//	((VdclTab*)pControl)->SetPaneVisibility(((VdclTab*)pControl)->m_nCurrentSelectedTab, TRUE);
		//}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
		}
	}
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
						if (pArxControlForm->m_pVisible != NULL)
						{
							BOOL bShowControl = pArxControlForm->m_pVisible->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl == TRUE)
								if (IsWindow(pControl->m_hWnd))		
									pControl->ShowWindow(pArxControlForm->m_pVisible->GetBooleanValue());
						}
						else
						{
							BOOL bShowControl = pArxControlForm->m_pVisible->GetBooleanValue();
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
						if (pArxControlForm->m_pVisible != NULL)
						{
							BOOL bShowControl = pArxControlForm->m_pVisible->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl == TRUE)
								if (IsWindow(pControl->m_hWnd))																		
									pControl->ShowWindow(pArxControlForm->m_pVisible->GetBooleanValue());
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
