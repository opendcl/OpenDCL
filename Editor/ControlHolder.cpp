// ControlHolder.cpp : implementation file
//

#include "stdafx.h"
#include "ControlHolder.h"
#include "DclControlObject.h"
#include "ControlTypes.h"
#include "AxContainer.h"
#include "Resource.h"
#include "Workspace.h"


IMPLEMENT_DYNCREATE(CControlHolder, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CControlHolder

CControlHolder::CControlHolder()
{
	m_ControlId = nChildCtrlIdStart;	
	m_bSelected = false;
	m_pArxObject = NULL;
	m_bActiveXCtrl = false;
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		  NULL,
		  ::GetSysColorBrush(COLOR_BTNFACE),
		  NULL);
	
}

CControlHolder::~CControlHolder()
{
}

int CControlHolder::GetId()
{
	m_ControlId++;
	return m_ControlId;
}


BEGIN_MESSAGE_MAP(CControlHolder, CWnd)
	//{{AFX_MSG_MAP(CControlHolder)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlHolder message handlers

BOOL CControlHolder::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CStatic::Create(CString(), dwStyle, rect, pParentWnd, nID);
}

int CControlHolder::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == nNotSet)
		return nNotSet;

	CreateGrips();

	return 0;
}
void CControlHolder::CreateGrips()
{
	int nGridIDCounter = nDeGridIDCounter;
	CRect rcGrip(0,0,nGripSizeConst,nGripSizeConst);
	
	// create all the grip rect and hide them for later selection use
	m_bGripsCreate = m_GripRect1.Create(rcGrip, this, nGridIDCounter);
	m_GripRect1.ShowWindow(FALSE);		
	m_GripRect1.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect2.Create(rcGrip, this, nGridIDCounter);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect2.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect3.Create(rcGrip, this, nGridIDCounter);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect3.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect4.Create(rcGrip, this, nGridIDCounter);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect4.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect5.Create(rcGrip, this, nGridIDCounter);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect5.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect6.Create(rcGrip, this, nGridIDCounter);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect6.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect7.Create(rcGrip, this, nGridIDCounter);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect7.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect8.Create(rcGrip, this, nGridIDCounter);
	m_GripRect8.ShowWindow(FALSE);
	m_GripRect8.m_bShowAsSelected = true;
	

}

void CControlHolder::ReleaseSelection() 
{
	m_bSelected = false;
	// now hide all the grip rect's
	m_GripRect1.ShowWindow(FALSE);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect8.ShowWindow(FALSE);

}
void CControlHolder::ForceGripsForward() 
{
	m_GripRect1.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect2.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect3.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect4.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect5.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect6.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect7.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect8.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	
}

void CControlHolder::HideGrips() 
{
	m_GripRect1.ShowWindow(FALSE);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect8.ShowWindow(FALSE);
	
}

void CControlHolder::SetSelected(BOOL bSelected) 
{
	if (bSelected == TRUE)
		m_bSelected = true;
	else
		m_bSelected = false;

	CRect rcThis;
	GetWindowRect(&rcThis);


	// set the predetermined pos's
	int MidX = (rcThis.Width() - nGripSizeConst) / 2;
	int MidY = (rcThis.Height() - nGripSizeConst) / 2;
	int RightX = rcThis.Width() - nGripSizeConst;
	int BottomY = rcThis.Height() - nGripSizeConst;

	// set the positions of all the grip rect's
	m_GripRect1.MoveWindow( 
		0, 0, nGripSizeConst, nGripSizeConst, 
		TRUE);
	
	m_GripRect2.MoveWindow( 
		MidX, 0, nGripSizeConst, nGripSizeConst,
		TRUE);

	m_GripRect3.MoveWindow( 
		RightX, 0, nGripSizeConst, nGripSizeConst,
		TRUE);

	m_GripRect4.MoveWindow( 
		0, MidY, nGripSizeConst, nGripSizeConst, 
		TRUE);
		
	m_GripRect5.MoveWindow( 
		RightX, MidY, nGripSizeConst, nGripSizeConst,
		TRUE);

	// set the positions of all the grip rect's
	m_GripRect6.MoveWindow( 
		0, BottomY, nGripSizeConst, nGripSizeConst, 
		TRUE);
	
	m_GripRect7.MoveWindow( 
		MidX, BottomY, nGripSizeConst, nGripSizeConst,
		TRUE);

	m_GripRect8.MoveWindow( 
		RightX, BottomY, nGripSizeConst, nGripSizeConst,
		TRUE);
	
	ForceGripsForward();

	// now show all the grip rect's
	m_GripRect1.ShowWindow(TRUE);
	m_GripRect2.ShowWindow(TRUE);
	m_GripRect3.ShowWindow(TRUE);
	m_GripRect4.ShowWindow(TRUE);
	m_GripRect5.ShowWindow(TRUE);
	m_GripRect6.ShowWindow(TRUE);
	m_GripRect7.ShowWindow(TRUE);
	m_GripRect8.ShowWindow(TRUE);
	
	m_GripRect1.Invalidate();
	m_GripRect2.Invalidate();
	m_GripRect3.Invalidate();
	m_GripRect4.Invalidate();
	m_GripRect5.Invalidate();
	m_GripRect6.Invalidate();
	m_GripRect7.Invalidate();
	m_GripRect8.Invalidate();
	

}

void CControlHolder::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if (m_pArxObject == NULL)
		return;

	if (m_pArxObject->GetType() == CtlActiveX)
	{
		GetActiveXCtrl()->MoveWindow(
				0,
				0,
				cx,
				cy,
				TRUE);

		CRect rc;
		GetActiveXCtrl()->GetWindowRect(rc);
		if (rc.Width() != cx || rc.Height() != cy)
		{
			CRect rcThis;
			GetWindowRect(&rcThis);
			GetParent()->ScreenToClient(rcThis);
			rcThis.right = rcThis.left + rc.Width();
			rcThis.bottom = rcThis.top + rc.Height();
			MoveWindow(rcThis, TRUE);
		}
	}
	else
	{
		CWnd *pControl = GetDlgItem(m_ControlId);
	
		if (pControl != NULL)
		{
			pControl->MoveWindow(
				0,
				0,
				cx,
				cy,
				TRUE);

			CRect rc;
			pControl->GetWindowRect(rc);
			if (rc.Width() != cx || rc.Height() != cy)
			{
				CRect rcThis;
				GetWindowRect(&rcThis);
				GetParent()->ScreenToClient(rcThis);
				rcThis.right = rcThis.left + rc.Width();
				rcThis.bottom = rcThis.top + rc.Height();
				MoveWindow(rcThis, TRUE);
			}
			// force a proper repaint.
			pControl->RedrawWindow(NULL, NULL);
		}
	}
}

void CControlHolder::OnPaint() 
{

	CPaintDC dc(this); // device context for painting
	
	if (m_pArxObject == NULL)
		return;

	if (m_pArxObject->GetType() != CtlActiveX)
		return;
	
	CString sName = m_pArxObject->GetActiveXTypeName();

	CString sText;	
	sText = theWorkspace.LoadResourceString(IDS_ImageListCtrl);

	if (sName != sText)
		return;
						

	
	CRect rcControl;
	CAxContainer *pCtrl = GetActiveXCtrl();

	CRect rcClient;
	GetClientRect(&rcClient);
	
	IViewObject2 *m_lpViewObject = NULL;
	IDispatch *pDispatch = NULL;
	pDispatch = pCtrl->GetOleIDispatch();
	if (pDispatch == NULL)
	{
		return;
	}
	HRESULT hr = pDispatch->QueryInterface(IID_IViewObject2, (void **) &m_lpViewObject);
	if (!SUCCEEDED(hr))
		return; // not supported

	if (m_lpViewObject == NULL)
	{
		return;
	}

	RECTL rc;
	rc.left = rcClient.left;
	rc.right = rcClient.right;
	rc.top = rcClient.top;
	rc.bottom = rcClient.bottom;
	
	m_lpViewObject->Draw( DVASPECT_CONTENT, nNotSet, NULL, NULL, NULL, dc.m_hDC, &rc, NULL, NULL, 0 );
	
}


void CControlHolder::OnDestroy() 
{
	m_GripRect1.DestroyWindow();
	m_GripRect2.DestroyWindow();
	m_GripRect3.DestroyWindow();
	m_GripRect4.DestroyWindow();
	m_GripRect5.DestroyWindow();
	m_GripRect6.DestroyWindow();
	m_GripRect7.DestroyWindow();
	m_GripRect8.DestroyWindow();
	

	CStatic::OnDestroy();
		
	
}

CWnd *CControlHolder::GetChildControl() 
{
	return GetDlgItem(m_ControlId);
}



void CControlHolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnMouseMove(nFlags, point);
}

void CControlHolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnLButtonDblClk(nFlags, point);
}

void CControlHolder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnLButtonDown(nFlags, point);
}

void CControlHolder::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CControlHolder::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYUP:
	case WM_MOUSEMOVE:
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
			return TRUE;
			break;
		}	
	}
		
	return CStatic::PreTranslateMessage(pMsg);
}


CAxContainer* CControlHolder::GetActiveXCtrl()
{
	return (CAxContainer*)GetDlgItem(m_ControlId);
}

void CControlHolder::SetColor(DISPID dispid, unsigned long ulColor)
{
	GetActiveXCtrl()->SetColor(dispid, ulColor);
}
unsigned long CControlHolder::GetColor(DISPID dispid)
{
	return GetActiveXCtrl()->GetColor(dispid);
}

HRESULT CControlHolder::GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue)
{
	return GetActiveXCtrl()->GetProperty(axProp, strReturnValue);
}
void CControlHolder::SetProperty(AxPropertyDescriptor *axProp, CString sNewValue)
{
	GetActiveXCtrl()->SetProperty(axProp, sNewValue);
}
	
void CControlHolder::ShowPropertyPages()
{
	GetActiveXCtrl()->ShowPropertyPages();
}

void CControlHolder::LoadPictureFile(DISPID dispid, CString sFile, WORD flag)
{
	GetActiveXCtrl()->LoadPictureFile(dispid, sFile, flag);
}

void CControlHolder::SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag)
{
	GetActiveXCtrl()->SetPicture(dispid, newValue, flag);
}

BOOL CControlHolder::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
{

	if (dispid == DISPID_AMBIENT_USERMODE)
	{		
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = 0;
		return TRUE;
	}
	if (dispid == DISPID_AMBIENT_UIDEAD)
	{		
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = 1;
		return TRUE;
	}	
	return CStatic::OnAmbientProperty(pSite, dispid, pvar);
}

void CControlHolder::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
}


