// EventsCheckTree.cpp : implementation file
//

#include "stdafx.h"
#include "EventsCheckTree.h"
#include "OpenDCLView.h"
#include "UserMessageID.h"

#define nOnEditCopy		3
#define nOnEditPaste	22
#define nOnEditCut		24
#define nOnEditUndo		26


/////////////////////////////////////////////////////////////////////////////
// CEventsCheckTree

CEventsCheckTree::CEventsCheckTree()
{
}

CEventsCheckTree::~CEventsCheckTree()
{
}


BEGIN_MESSAGE_MAP(CEventsCheckTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CEventsCheckTree)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventsCheckTree message handlers

BOOL CEventsCheckTree::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ||  pMsg->message == WM_CHAR )
	{
		
		switch (pMsg->wParam)
		{			
			case VK_DELETE:
				{
					m_pView->DeleteSelectedControls();
				break;
				}
			case nOnEditCopy:
				if (m_pView != NULL)
 					m_pView->OnEditCopy();
				break;
			case nOnEditPaste:
				if (m_pView != NULL)
 					m_pView->OnEditPaste();
				break;
			case nOnEditCut:
				if (m_pView != NULL)
 					m_pView->OnEditCut();
				break;
			case nOnEditUndo:
				if (m_pView != NULL)
 					m_pView->OnEditUndo();
				break;
			default:
				if (m_pView != NULL)
 					m_pView->PreTranslateMessage(pMsg);
				break;
		}
	}	
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CEventsCheckTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem = CTreeCtrl::HitTest(point, NULL);
	CTreeCtrl::SelectItem(hItem);
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CEventsCheckTree::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GetOwner()->SendMessage(WM_USER_ITEMCHECKED);
	CTreeCtrl::OnLButtonUp(nFlags, point);
}
