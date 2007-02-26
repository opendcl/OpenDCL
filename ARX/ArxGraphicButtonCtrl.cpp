// ArxGraphicButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGraphicButtonCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "SpreadSheet.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGraphicButtonCtrl

CArxGraphicButtonCtrl::CArxGraphicButtonCtrl( CDclControlObject* pTemplate,
																							CControlPane* pPane,
																							UINT nID,
																							bool bCreate /*= true*/ )
: CGraphicButtonCtrl( pTemplate, pPane, nID, false )
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
		CGraphicButtonCtrl::Create( pParentWnd, nID );

	if( GetTemplate()->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

bool CArxGraphicButtonCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case nDragnDropAllowDrop:
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
	//ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CArxGraphicButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetTemplate())
	{
	InvokeMethodIntIntInt(
		GetTemplate()->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	}
	__super::OnMouseMove(nFlags, point);
/*
	ClientToScreen(&point);
	wndUnderMouse = WindowFromPoint(point);

	// If the mouse enter the button with the left button pressed then do nothing
	if (nFlags & MK_LBUTTON && m_bMouseOnButton == FALSE) return;

	// If our button is not flat then do nothing
	if (m_bIsFlat == FALSE) return;

	if (m_bAlwaysTrack == FALSE)	wndActive = GetActiveWindow();

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd && wndActive)
	{
		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = TRUE;

			if (GetTemplate())
			{
	
			if (GetTemplate()->GetLngProperty(nButtonStyle) != 5)
			{
				Invalidate();

#ifdef	BTNST_USE_SOUND
				// Play sound ?
				if (m_csSounds[0].lpszSound)
					::PlaySound(m_csSounds[0].lpszSound, m_csSounds[0].hMod, m_csSounds[0].dwFlags);
#endif

				csTME.cbSize = sizeof(csTME);
				csTME.dwFlags = TME_LEAVE;
				csTME.hwndTrack = m_hWnd;
				::_TrackMouseEvent(&csTME);
			}
			}
		} // if
	} else CancelHover();
	*/
}

void CArxGraphicButtonCtrl::OnDoubleclicked() 
{
	if (GetTemplate())
	{
		// call methods to invoke the event
		InvokeMethod(GetTemplate()->GetStrProperty(nEventDblClicked), false);
	}
}

void CArxGraphicButtonCtrl::OnClicked() 
{	
	if (m_nDirectory == 2)
	{
		CSpreadSheet *pParent = (CSpreadSheet*)GetParent();		
		pParent->DoFileDlg(m_nDirectory);
	}
	else if (m_nDirectory == 3)
	{
		CSpreadSheet *pParent = (CSpreadSheet*)GetParent();		
		pParent->DoFileDlg(m_nDirectory);
	}
	
	if (msOnClickedEvent.GetLength() > 0)
	{
		CSpreadSheet *pParent = (CSpreadSheet*)GetParent();
		// call methods to invoke the event
		InvokeMethodIntInt(msOnClickedEvent, pParent->m_nRowSelected, pParent->m_nColSelected, m_bInvokeWithSendString);
	}
	
	if (GetTemplate())
	{	
		if (GetTemplate()->m_bEventsAsAction)
		{
			GetParent()->GetParent()->EnableWindow(TRUE);
			int stat;
			struct resbuf *result = NULL, *list;    

			CString sText = GetTemplate()->GetStrProperty(nEventClicked);
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
			InvokeMethod(GetTemplate()->GetStrProperty(nEventClicked), m_bInvokeWithSendString);
		}
	}
	__super::OnClicked();
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

	if (GetTemplate() && GetTemplate()->GetBoolProperty(nDragnDropAllowBegin) == TRUE && nFlags == 1)
		BeginDragnDrop(GetTemplate(), point, m_bInvokeWithSendString);
}
