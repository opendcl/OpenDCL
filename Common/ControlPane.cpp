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
{
}

CControlPane::CControlPane(CDclFormObject* pSourceForm, CWnd* pHostDlg)
: mpSourceForm( pSourceForm )
, mpProject( pSourceForm? pSourceForm->GetProject() : NULL )
, mpHostDlg( pHostDlg )
{
}

CControlPane::~CControlPane()
{
}

void CControlPane::UpdateGlobalVariables()
{
	for( TDialogControls::iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
		(*iter)->CreateGlobalVariables();
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

bool CControlPane::CreateControls(CDclFormObject *pDclForm, UINT& nId)
{
	bool bFailed = false;
	CRect rcParent;
	mpHostDlg->GetWindowRect(&rcParent);
	mpHostDlg->ScreenToClient(&rcParent);
	mpSourceForm->m_rcPos.left = 0;
	mpSourceForm->m_rcPos.top = 0;
	mpSourceForm->m_rcPos.right = rcParent.Width();
	mpSourceForm->m_rcPos.bottom = rcParent.Height();
	
	POSITION pos = pDclForm->mDclControls.GetTailPosition();
	while (pos != NULL)
	{
		CDclControlObject* pTemplate = pDclForm->mDclControls.GetPrev(pos);
		if( !pos)
			break; //we've reached the properties control at the head of the list, just skip it
		if (pTemplate->GetType() < 0)
			continue;
		UINT idDlg = pTemplate->m_Id;
		if( idDlg <= 0 || pDclForm->GetType() != CtlSplitter )
				idDlg = nId++;
		TDialogControlPtr pControl = CreateNewDialogControl( pTemplate, this, idDlg );
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
	pGrphcBtn->GetWindow()->ShowWindow(pGrphcBtn->m_pVisible->GetBooleanValue());
}

void CControlPane::SizeChanged(int cx, int cy, bool bRefreshOthers)
{
	if (mpSourceForm == NULL)
		return;

	if (mpSourceForm->mDclControls.GetCount() == 0)
		return;

	mPaneWindowPos.right = mPaneWindowPos.left + cx;
	mPaneWindowPos.bottom = mPaneWindowPos.top + cy;

	// first lets calc all the control positions for any splitter controls.
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	mpSourceForm->mDclControls.GetNext(pos);
	while (pos != NULL)
	{
		CDclControlObject *pArxObject = mpSourceForm->mDclControls.GetNext(pos);

		if (pArxObject != NULL && pArxObject->GetType() == CtlSplitter)
			ResetControlsPos(pArxObject, bRefreshOthers);
	}

	//next lets calc all the control positions for all NON-splitter controls.
	pos = mpSourceForm->mDclControls.GetHeadPosition();
	mpSourceForm->mDclControls.GetNext(pos);
	while (pos != NULL)
	{
		CDclControlObject *pObject = mpSourceForm->mDclControls.GetNext(pos);

		if (pObject != NULL && pObject->GetType() != CtlSplitter)
			ResetControlsPos(pObject, bRefreshOthers);
	}

	// call method to ensure all graphic buttons and picture boxes and slide holders are setup 
	// correctly to enure the graphic buttons are transparent.	
	SetGrphcBtnsParents(true);
}

void CControlPane::InvalidateControls()
{
	if (mpSourceForm == NULL)
		return;

	if (mpSourceForm->mDclControls.GetCount() == 0)
		return;

	for (int i=1; i<mpSourceForm->mDclControls.GetCount(); i++)
	{
		// get the arx object
		POSITION pos = mpSourceForm->mDclControls.FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = mpSourceForm->mDclControls.GetAt(pos);

			if (pArxObject != NULL)
				if (pArxObject->GetType() != CtlBlockView && pArxObject->GetType() != CtlHatch)
					pArxObject->GetWindow()->Invalidate();
		}
	}
}

void CControlPane::ResetControlsPos(CDclControlObject *pArxObject, bool bRefreshOthers)
{
	int cx = mPaneWindowPos.Width();
	int cy = mPaneWindowPos.Height();
	ResetControlsPos(pArxObject, cx, cy, bRefreshOthers);
}

void CControlPane::ResetControlsPos2(CDclControlObject *pArxObject)
{
	int cx = mPaneWindowPos.Width();
	int cy = mPaneWindowPos.Height();
	ResetControlsPos2(pArxObject, cx, cy);
}

CRect CControlPane::GetSplitterRect(int nId) 
{
	CRect rc(0,0,0,0);

	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetNext(pos);

		if (pCtrl->GetType() == CtlSplitter && pCtrl->GetControlInstance()->GetControlId() == nId)
		{
			rc.left = pCtrl->GetLngProperty(nLeft);
			rc.top = pCtrl->GetLngProperty(nTop);
			rc.right = rc.left + pCtrl->GetLngProperty(nWidth);
			rc.bottom = rc.top + pCtrl->GetLngProperty(nHeight);
			return rc;	
		}
	}

	return rc;	
}

void CControlPane::ResetControlsPos(CDclControlObject *pArxObject, int cx, int cy, bool bRefreshOthers)
{
	if (pArxObject == NULL)
		return;
	if (pArxObject->m_pUseLeftOffset == NULL)
	{
		if (bRefreshOthers)
			pArxObject->GetWindow()->RedrawWindow();		
		return;
	}

	// get the offset boolean flags
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

	if (mpSourceForm->GetType() == VdclModal ||
		mpSourceForm->GetType() == VdclModeless)
	{
		if (lLeftFromRight  == 0 &&
			lRightFromRight == 0 &&
			lTopFromBottom  == 0 &&
			lBottomFromBottom == 0)
			return;
	}

	
	// get the control being moved			
	CWnd *pControl = pArxObject->GetWindow();

	//TODO: move this to ArxControlPane.cpp
	//if (pArxObject->GetType() == CtlGraphicButton)
	//{
	//	((CButtonST*)pControl)->m_bDrawTransparent = FALSE;
	//}

	
	if (pControl == NULL)
		return;

	// get the current placement of the control
	CRect rcControl;
	rcControl = mPaneWindowPos;

	CRect rcThis;
	mpHostDlg->GetClientRect(&rcThis);

	// if mpHostDlg control is to be moved according to the right or bottom side of the form
	if (lLeftFromRight  > 0 ||
		lRightFromRight > 0 ||
		lTopFromBottom  > 0 ||
		lBottomFromBottom > 0)
	{	
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			rcControl.left = mPaneWindowPos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			rcControl.top = mPaneWindowPos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			rcControl.right = rcControl.left + pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			rcControl.bottom = rcControl.top + pArxObject->m_pHeight->GetLongValue();	
		}
		else
		{
			pArxObject->GetWindow()->GetWindowRect(&rcControl);
			mpHostDlg->ScreenToClient(rcControl);
		}
		
		// set the left position if required
		if (lLeftFromRight == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = cx - nOffsetValue + mPaneWindowPos.left;
		}
		// set the left position if required
		else if (lLeftFromRight == 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = (cx / 2) + nOffsetValue + mPaneWindowPos.left;
		}
		// set the left position if required
		else if (lLeftFromRight > 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lLeftFromRight);

			// set the new offset position
			rcControl.left = rc.left + nOffsetValue;
		}		
		else if (lLeftFromRight == 0)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = mPaneWindowPos.left + pArxObject->GetLngProperty(nLeft);
		}
		
		
		// set the right position if required
		if (lRightFromRight == 1 && pArxObject->GetType() != CtlCheckBox)
		{			
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();
			// set the new offset position
			rcControl.right = cx - nOffsetValue + mPaneWindowPos.left;				
		}
		// set the right position if required
		else if (lRightFromRight > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lRightFromRight);

			// set the new offset position
			rcControl.right = rc.left + nOffsetValue;
		}		
		// if right offset not requested
		else
		{
			// get the offset value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();				
			// set the new offset position
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (lTopFromBottom == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();				
			// set the new offset position
			rcControl.top = cy - nOffsetValue + mPaneWindowPos.top;
		}
		// set the top position if required
		else if (lTopFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lTopFromBottom);

			// set the new offset position
			rcControl.top = rc.top + nOffsetValue;
		}

		// set the top position if required
		if (lBottomFromBottom  == 1 && pArxObject->GetType() != CtlCheckBox)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();				
			// set the new offset position
			rcControl.bottom = cy - nOffsetValue + mPaneWindowPos.top;
		}
		// set the bottom position if required
		else if (lBottomFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lBottomFromBottom);

			// set the new offset position
			rcControl.bottom = rc.top + nOffsetValue;
		}
		// if bottom offset not requested
		else
		{
			// get the offset value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();				
			// set the new offset position
			rcControl.bottom = rcControl.top + nHeightValue;
		}

		// check the width to ensure that it is at least 2 is width
		if (rcControl.right - rcControl.left < 2)
			rcControl.right = rcControl.left + 2;

		// check the height to ensure that it is at least 2 is width
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
		
		// redraw graphic buttons
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(FALSE);
		}
		
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
		
		pControl->MoveWindow(
			rcControl,
			TRUE);
		
		// mpHostDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		
		// mpHostDlg control is a month control
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
		}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
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
		CRect rcControl;
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			int nLeftValue = mPaneWindowPos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			int nTopValue = mPaneWindowPos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();	
			
			rcControl.SetRect(
				nLeftValue,
				nTopValue,
				nLeftValue + nWidthValue,
				nTopValue + nHeightValue);	
		}
		else
		{
			pArxObject->GetWindow()->GetWindowRect(&rcControl);
			mpHostDlg->ScreenToClient(rcControl);
		}

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

void CControlPane::ResetControlsPos2(CDclControlObject *pArxObject, int cx, int cy)
{
	if (pArxObject == NULL)
		return;
	if (pArxObject->m_pUseLeftOffset == NULL)
		return;

	// get the offset boolean flags
	int nLeftFromRight = 0;
	if (pArxObject->m_pUseLeftOffset->GetType() == PropBool)
		nLeftFromRight = pArxObject->m_pUseLeftOffset->GetBooleanValue();
	else
		nLeftFromRight = pArxObject->m_pUseLeftOffset->GetLongValue();

	int nRightFromRight = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		nRightFromRight = pArxObject->m_pUseRightOffset->GetBooleanValue();
	else
		nRightFromRight = pArxObject->m_pUseRightOffset->GetLongValue();

	int nTopFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		nTopFromBottom = pArxObject->m_pUseTopOffset->GetBooleanValue();
	else
		nTopFromBottom = pArxObject->m_pUseTopOffset->GetLongValue();

	int nBottomFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		nBottomFromBottom = pArxObject->m_pUseBottomOffset->GetBooleanValue();
	else
		nBottomFromBottom = pArxObject->m_pUseBottomOffset->GetLongValue();
	
	// get the control being moved			
	CWnd *pControl = pArxObject->GetWindow();

	//TODO: move this to ArxControlPane.cpp
	//if (pArxObject->GetType() == CtlGraphicButton)
	//{
	//	((CButtonST*)pControl)->m_bDrawTransparent = FALSE;
	//}

	
	if (pControl == NULL)
		return;

	// get the current placement of the control
	CRect rcControl;
	rcControl = mPaneWindowPos;

	CRect rcThis;
	mpHostDlg->GetClientRect(&rcThis);

	// if mpHostDlg control is to be moved according to the right or bottom side of the form
	if (nLeftFromRight  > 0 ||
		nRightFromRight > 0 ||
		nTopFromBottom  > 0 ||
		nBottomFromBottom > 0)
	{	
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			rcControl.left = mPaneWindowPos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			rcControl.top = mPaneWindowPos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			rcControl.right = rcControl.left + pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			rcControl.bottom = rcControl.top + pArxObject->m_pHeight->GetLongValue();	
		}
		else
		{
			pArxObject->GetWindow()->GetWindowRect(&rcControl);
			mpHostDlg->ScreenToClient(rcControl);
		}
		
		// set the left position if required
		if (nLeftFromRight == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = cx - nOffsetValue + mPaneWindowPos.left;
		}
		// set the left position if required
		else if (nLeftFromRight == 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = (cx / 2) + nOffsetValue + mPaneWindowPos.left;
		}
		// set the left position if required
		else if (nLeftFromRight > 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nLeftFromRight);

			// set the new offset position
			rcControl.left = rc.left + nOffsetValue;
		}		
		else if (nLeftFromRight == 0)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = mPaneWindowPos.left + pArxObject->GetLngProperty(nLeft);
		}
		
		
		// set the right position if required
		if (nRightFromRight == 1 && pArxObject->GetType() != CtlCheckBox)
		{			
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();
			// set the new offset position
			rcControl.right = cx - nOffsetValue + mPaneWindowPos.left;				
		}
		// set the right position if required
		else if (nRightFromRight > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nRightFromRight);

			// set the new offset position
			rcControl.right = rc.left + nOffsetValue;
		}		
		// if right offset not requested
		else
		{
			// get the offset value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();				
			// set the new offset position
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (nTopFromBottom == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();				
			// set the new offset position
			rcControl.top = cy - nOffsetValue + mPaneWindowPos.top;
		}
		// set the top position if required
		else if (nTopFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nTopFromBottom);

			// set the new offset position
			rcControl.top = rc.top + nOffsetValue;
		}

		// set the top position if required
		if (nBottomFromBottom  == 1 && pArxObject->GetType() != CtlCheckBox)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();				
			// set the new offset position
			rcControl.bottom = cy - nOffsetValue + mPaneWindowPos.top;
		}
		// set the bottom position if required
		else if (nBottomFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nBottomFromBottom);

			// set the new offset position
			rcControl.bottom = rc.top + nOffsetValue;
		}
		// if bottom offset not requested
		else
		{
			// get the offset value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();				
			// set the new offset position
			rcControl.bottom = rcControl.top + nHeightValue;
		}

		// check the width to ensure that it is at least 2 is width
		if (rcControl.right - rcControl.left < 2)
			rcControl.right = rcControl.left + 2;

		// check the height to ensure that it is at least 2 is width
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
		
		// redraw graphic buttons
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(FALSE);
		}
		
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
		
		pControl->MoveWindow(
			rcControl,
			TRUE);
		
		// mpHostDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		
		// mpHostDlg control is a month control
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
		}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
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
		CRect rcControl;
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			int nLeftValue = mPaneWindowPos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			int nTopValue = mPaneWindowPos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();	
			
			rcControl.SetRect(
				nLeftValue,
				nTopValue,
				nLeftValue + nWidthValue,
				nTopValue + nHeightValue);	
		}
		else
		{
			pArxObject->GetWindow()->GetWindowRect(&rcControl);
			mpHostDlg->ScreenToClient(rcControl);
		}

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
	
		pControl->MoveWindow(
			rcControl,
			TRUE);
		
		
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
		mControls[idx]->GetControl()->DestroyWindow();
	mControls.clear();
}

void CControlPane::ShowWindow(BOOL bShow)
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
	CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetTail();
	if (pCtrl != NULL && pCtrl->GetWindow() != NULL)
		pCtrl->GetWindow()->SetFocus();
}
