// ModelessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelessDlg.h"
#include "AcadDocReactor.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "ArxProject.h"
#include "PictureObject.h"
#include "DialogControl.h"
#include "ControlTypes.h"

const int nFormEventOnOk = 251;


/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

CModelessDlg::CModelessDlg(CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, int nX, int nY)
: CSnapDlg(pSourceForm, CModelessDlg::IDD, pParent)
{
	m_bAsModal = false;
	m_nX = nX;
	m_nY = nY;
	m_bClosing = false;
	m_bAboutToClose = false;
	m_pDocToModReactor = NULL;
}



BEGIN_MESSAGE_MAP(CModelessDlg, CSnapDlg)
	//{{AFX_MSG_MAP(CModelessDlg)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus)	
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

void CModelessDlg::CenterDialog() 
{
	CPoint pt;
	CRect rcThis;

	GetWindowRect(&rcThis);

	// get the width
	int nCtlWidth = rcThis.Width();
	
	// get the height
	int nCtlHeight = rcThis.Height();
	
	// get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - nCtlHeight) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - nCtlWidth) / 2;
	
	// call method to set the start width and position of the form
	SetWindowPos(NULL, pt.x, pt.y, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	
}	
void CModelessDlg::CenterDialog(int nNewWidth, int nNewHeight) 
{
	CPoint pt;
	
	// set the width
	mpSourceForm->GetControlProperties()->SetLngProperty(nWidth, nNewWidth);
	
	// set the height
	mpSourceForm->GetControlProperties()->SetLngProperty(nHeight, nNewHeight);
	
	// get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - nNewHeight) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - nNewWidth) / 2;
	
	// call method to set the start width and position of the form
	SetWindowPos(NULL, pt.x, pt.y, nNewWidth, nNewHeight, SWP_NOACTIVATE);

	// call the method to ensure all the controls are offset correctly
	mControlPane.SizeChanged(nNewWidth, nNewHeight);

}	
void CModelessDlg::ResizeDialog(int nNewWidth, int nNewHeight) 
{
	CPoint pt;
	CRect rcThis;
	CWnd *pThisWnd = this;
	pThisWnd->GetWindowRect(&rcThis);

	// set the width
	mpSourceForm->GetControlProperties()->SetLngProperty(nWidth, nNewWidth);
	
	// set the height
	mpSourceForm->GetControlProperties()->SetLngProperty(nHeight, nNewHeight);
	
	// call method to set the start width and position of the form
	pThisWnd->SetWindowPos(NULL, rcThis.left, rcThis.top, nNewWidth, nNewHeight, SWP_NOACTIVATE);

	// call the method to ensure all the controls are offset correctly
	//mControlPane.SizeChanged(nNewWidth, nNewHeight);
}	

BOOL CModelessDlg::OnInitDialog() 
{
	CSnapDlg::OnInitDialog();

	if (m_bAsModal)
	{
		GetParent()->EnableWindow(FALSE);
		EnableWindow(TRUE);
	}
	// Modify the style
	//GetLayeredDialog()->AddLayeredStyle(m_hWnd);

	// Set the trasparency effect to 70%
	//GetLayeredDialog()->SetTransparentPercentage(m_hWnd, 80);

	//  setup for assigning the form it's properties
	CPoint pt;
	CRect rectThis;
	int nCtlWidth;
	int nCtlHeight;
	m_bClosing = false;
	
	// get the form's properties
	CDclControlObject* pControlObject = mpSourceForm->GetControlProperties();
	// set the window text
	SetWindowText(pControlObject->GetStrProperty(nTitleBarText));
		
	// call the method to set the title bar icon
	SetTitleBarIcon(pControlObject->GetLngProperty(nIcon));

	// setup the rect default rect 
	rectThis.top = 0;
	rectThis.left = 0;
	
	CRect rcProfile(-1,-1,-1,-1);
	if (mpSourceForm->GetControlProperties()->GetBoolProperty(nResizable) == TRUE)
		rcProfile = ReadRect();

	if (rcProfile.Width() > 0)
	{
		// get the width
		nCtlWidth = rcProfile.Width();

		// get the height
		nCtlHeight = rcProfile.Height();
	}
	else
	{
		// get the width
		nCtlWidth = pControlObject->GetLngProperty(nWidth);

		// get the height
		nCtlHeight = pControlObject->GetLngProperty(nHeight);
	}
	
	CRect rcThis;
	CRect rcWnd;
		
	if (rcProfile.top == -1 && rcProfile.left == -1)
	{
		GetClientRect(&rcThis);
		GetWindowRect(&rcWnd);

		if (mpSourceForm->m_bUsesClientRect == TRUE)
		{
			nCtlHeight += rcWnd.Height() - rcThis.Height();
			nCtlWidth += rcWnd.Width() - rcThis.Width();
		}
	}

	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	if (nCtlWidth > nScreenWidth)
		nCtlWidth = nScreenWidth;

	if (nCtlHeight > nScreenHeight)
		nCtlHeight = nScreenHeight;

	rectThis.right = nCtlWidth;
	rectThis.bottom = nCtlHeight;

	if (m_nY > -1)
	{
		// call method to set the start width and position of the form
		SetWindowPos(NULL, m_nX, m_nY, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	}
	else if (rcProfile.left > -1)
	{
		// call method to set the start width and position of the form
		SetWindowPos(NULL, rcProfile.left, rcProfile.top, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	}	
	else if (m_nY == -1)
	{
		// get the left and top values to center the form on the screen	
		pt.y =  (nScreenHeight - rectThis.Height()) / 2;
		pt.x =  (nScreenWidth - rectThis.Width()) / 2;

		// call method to set the start width and position of the form
		SetWindowPos(NULL, pt.x, pt.y, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	}
	
	if (pControlObject->GetLngProperty(nMaxDialogWidth) > -1)
	{
		int nMaxWidth = pControlObject->GetLngProperty(nMaxDialogWidth);
		int nMaxHeight = pControlObject->GetLngProperty(nMaxDialogHeight);
		int nMinWidth = pControlObject->GetLngProperty(nMinDialogWidth);
		int nMinHeight = pControlObject->GetLngProperty(nMinDialogHeight);

		// ensure the max extents is not larger than the screen.
		if (nMaxWidth > nScreenWidth)
			nMaxWidth = nScreenWidth;
		if (nMaxHeight > nScreenHeight)
			nMaxHeight = nScreenHeight;

		// ensure the min extents is not larger than the screen.
		if (nMinWidth > nScreenWidth)
			nMinWidth = nScreenWidth;
		if (nMinHeight > nScreenHeight)
			nMinHeight = nScreenHeight;

		// set the min and max allowable settings
		SetDialogMaxExtents (
			nMaxWidth,
			nMaxHeight);		
		SetDialogMinExtents (
			nMinWidth,
			nMinHeight);
	}
    

	// set the rect for the control pane to be created
	rcThis.SetRect(0,0, nCtlWidth, nCtlHeight);
	
	// create the control pane that will display the controls
	mControlPane.m_pControlCol = &m_ControlCol;
	mControlPane.m_pFontCollection = m_pFontCollection;
	mControlPane.m_PanePos = rcThis;
	mControlPane.m_pParentDlg = this;

	// set the parent dialog position
	mControlPane.m_sDialogName = m_sDialogName;
	mControlPane.m_sProjectName = m_sProjectName;
	mControlPane.m_bInvokeWithSendString = true;
	// call method to create the controls
	mControlPane.CreateControls(mpSourceForm, 1000);

	CRect rcClient;
	GetClientRect(&rcClient);
	// resize the control pane so all offsets are set correctly
	mControlPane.SizeChanged(rcClient.Width(),rcClient.Height());	

	// add the doc reactor if required for an event
	CString sEventDefun = mpSourceForm->GetControlProperties()->GetStrProperty(nDocEventActivated);
	if (sEventDefun.GetLength() > 0)
	{
		m_pDocToModReactor = new CAcadDocReactor();
		m_pDocToModReactor->m_EventDefun = sEventDefun;
		// activate the reactor
		acDocManager->addReactor(m_pDocToModReactor);
	}	


	// call methods to invoke the OnInitDialog event
	InvokeMethod(pControlObject->GetStrProperty(nFormEventInitialize), false);

	GetWindowRect(&rcThis);
	// call methods to invoke the event
	InvokeMethodIntInt(
		mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	

	SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CModelessDlg::SetDclForm(CDclFormObject *mpSourceFormObject)
{
	// set the internal pointer to equal the object passed
	mpSourceForm = mpSourceFormObject;
}

void CModelessDlg::AboutToClose() 
{
	FireCloseEvent();
}

bool CModelessDlg::QueryForClose() 
{
	if (mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose) == "")
		return true;

	return !(InvokeCancelMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false));	
}

void CModelessDlg::OnClose() 
{	
	SaveSize();

	if (IsWindowEnabled())
		CSnapDlg::OnClose();
	return;
	
}

LONG CModelessDlg::onAcadKeepFocus(UINT, LONG)
{
	// Set the trasparency effect to 70%
	//GetLayeredDialog()->SetTransparentPercentage(m_hWnd, 100);

    return TRUE;
}

void CModelessDlg::OnSize(UINT nType, int cx, int cy) 
{
	CSnapDlg::OnSize(nType, cx, cy);
	
	
	if (CWnd::IsWindowVisible() && m_bClosing == false  && m_bAboutToClose == false)
	{
		CRect rcThis;
		CWnd::GetClientRect(&rcThis);
		
		mControlPane.SizeChanged(rcThis.Width(), rcThis.Height());
		CWnd::GetWindowRect(&rcThis);
		
		if (!m_bAboutToClose)
		{
			SaveSize();
			//m_nX = rcThis.left; //Removed 10-19-06
			//m_nY = rcThis.top;
			// call methods to invoke the event
			InvokeMethodIntInt(
				mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventSize), 
				rcThis.Width(),
				rcThis.Height(),
				false);	
		}

	}
	
	
}

void CModelessDlg::SetTitleBarIcon(int nPictureID)
{
	// set start position for navigating Pictures
	POSITION pos = mpSourceForm->GetProject()->GetPictureList().GetHeadPosition();

	int nCount = 0;

	SetIcon(NULL, FALSE);
	
	if (m_hIconAcad != NULL)
		DestroyIcon(m_hIconAcad);
	
	// do loop to navigate Pictures
	while (nCount < mpSourceForm->GetProject()->GetPictureList().GetCount())
	{
		// get position
		pos = mpSourceForm->GetProject()->GetPictureList().FindIndex(nCount);
		// get current Picture in list
		CPictureObject* pPicture = mpSourceForm->GetProject()->GetPictureList().GetNext(pos);
		
		if (pPicture->GetID() == nPictureID)
		{
			if (pPicture->GetPicType() == PICTYPE_BITMAP)
			{
				// if the icon is already extracted, return that Icon
				if (pPicture->GetIcon() != NULL)
				{
					// get the icon
					SetIcon(pPicture->GetIcon(), FALSE);
					return;
				}
			}
			// get the icon
			m_hIconAcad = pPicture->GetIcon();
			// set the icon
			SetIcon(m_hIconAcad, FALSE);
			// set the found flag to true
			return;
		}
		// increment counter
		nCount++;
	}
	
	// load and display the Acad Small icon
	HMODULE hRes = _hdllInstance;		
	m_hIconAcad = LoadIcon(hRes, MAKEINTRESOURCE(IDI_DMICON));
	SetIcon(m_hIconAcad, FALSE);
}



void CModelessDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSnapDlg::OnShowWindow(bShow, nStatus);
	
	// call methods to invoke the event
	InvokeMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventShow), false);	
	
	
}

void CModelessDlg::OnDestroy() 
{
	if (m_pDocToModReactor != NULL)
	{
		acDocManager->removeReactor(m_pDocToModReactor);
		delete m_pDocToModReactor;
		m_pDocToModReactor = NULL;
	}

	//HICON hIcon = CDialog::GetIcon(FALSE);
	SetIcon(NULL, FALSE);
	if (m_hIconAcad != NULL)
		DestroyIcon(m_hIconAcad);
	
	CSnapDlg::OnDestroy();
	
}
void CModelessDlg::FireCloseEvent()
{
	if (m_bClosing)
		return;

	CPoint pt;
	CRect rcThis;
	GetWindowRect(&rcThis);

	// get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - rcThis.Height()) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - rcThis.Width()) / 2;

  //Get actual location of the dialog
  CRect rc;
  CWnd::GetWindowRect(&rc);

  if (pt.y == rc.left &&
		  pt.x == rc.top)
	  // call methods to invoke the event
	  InvokeMethodIntInt(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventClose), -1, -1, false);	
	else
		// call methods to invoke the event
		InvokeMethodIntInt(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventClose), rc.left, rc.top, false);	

	
	for (int i=0; i<m_ControlCol.GetCount(); i++)
	{
		POSITION pos = m_ControlCol.FindIndex(i);
		if (pos != NULL)
		{
			CArxDialogControl *pCtrl = m_ControlCol.GetAt(pos);
			m_ControlCol.RemoveAt(pos);
			delete pCtrl;
		}
	}
	mControlPane.CleanUpControls();
	mControlPane.m_pParentDlg = NULL;	
	
	m_bClosing = true;
}


void CModelessDlg::OnOK()
{
	if (!InvokeCancelMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false))	
	{
    InvokeMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventOnOk), false);
		CSnapDlg::OnOK();
		theArxWorkspace.RemoveDialog(mpSourceForm);
	}
}
void CModelessDlg::OnCancel()
{
	if (!InvokeCancelMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventCancelClose), true))	
	{
		if (IsWindowEnabled())
		{
			CSnapDlg::OnCancel();
			theArxWorkspace.RemoveDialog(mpSourceForm);
		}
		
	}
}

void CModelessDlg::OnMove(int x, int y) 
{
	CSnapDlg::OnMove(x, y);

  //Removed 10-19-06	
  /*BOOL bVisible = CDialog::IsWindowVisible();
  if (!m_bAboutToClose && bVisible)
  {
    CRect rc;
    CWnd::GetWindowRect(&rc);
    m_nX = rc.left;
    m_nY = rc.top;
  }*/
}


void CModelessDlg::SizeDialog ()
{
	if (CWnd::IsWindowVisible() && m_bClosing == false)
	{
		CRect rcThis;
		CWnd::GetClientRect(&rcThis);
		
		mControlPane.SizeChanged(rcThis.Width(), rcThis.Height());
		CWnd::GetWindowRect(&rcThis);
		
		if (!m_bAboutToClose)
		{
      //Removed 10-19-06
			//m_nX = rcThis.left;
			//m_nY = rcThis.top;
			// call methods to invoke the event
			InvokeMethodIntInt(
				mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventSize), 
				rcThis.Width(),
				rcThis.Height(),
				mControlPane.m_bInvokeWithSendString);	
		}

	}
}


	

BOOL CModelessDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
    {
		if	(pMsg->message == WM_KEYDOWN &&
			(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL))
		{
			HWND hItem = ::GetDlgItem(m_hWnd, IDCANCEL);
			if (hItem == NULL || ::IsWindowEnabled(hItem))
			{
				SendMessage(WM_COMMAND, IDCANCEL, 0);
				return TRUE;
			}
		}
		try
		{
			CWnd *pCtrl = CWnd::FromHandle(pMsg->hwnd);
			POSITION pos = m_ControlCol.GetHeadPosition();
			while (pos != NULL)
			{
				CArxDialogControl *pCtrlObj = m_ControlCol.GetNext(pos);
				// if this is the control and it is an ActiveX control
				if (pCtrlObj->GetWindow() == pCtrl && pCtrlObj->GetDclControlType() == CtlActiveX)
					return CWnd::PreTranslateMessage(pMsg);
			}
		}
		catch(...)
		{
		}
    }	
	
	return CSnapDlg::PreTranslateMessage(pMsg);
}

void CModelessDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CSnapDlg::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}

void CModelessDlg::OnSetFocus(CWnd* pNewWnd) 
{
	CSnapDlg::OnSetFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}

void CModelessDlg::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CSnapDlg::OnCaptureChanged(pWnd);
}



BOOL CModelessDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CSnapDlg::OnSetCursor(pWnd, nHitTest, message);
}
