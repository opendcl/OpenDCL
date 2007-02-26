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


// CModelessDialogX interface implementation
CModelessDialogX::CModelessDialogX( CModelessDlg& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CModelessDialogX::~CModelessDialogX()
{
}

DclFormType CModelessDialogX::GetType() const
{
	return VdclModeless;
}

bool CModelessDialogX::IsResizable() const
{
	return mpOwner->IsResizable();
}

HWND CModelessDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

bool CModelessDialogX::CreateModeless() const
{
	return (mpOwner->Create( IsResizable()? IDD_RESIZEABLE : IDD_MODALDIALOG ) != FALSE);
}

void CModelessDialogX::CloseDialog(int nStatus) const
{
	mpOwner->EndDialog(nStatus);
	mpOwner->DestroyWindow();
}

bool CModelessDialogX::SetMinMaxSize( const CSize& min, const CSize& max )
{
	CDialogObject::SetMinMaxSize( min, max );
	mpOwner->SetDialogMaxExtents (min.cx, min.cy);
	mpOwner->SetDialogMinExtents (max.cx, max.cy);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

CModelessDlg::CModelessDlg(CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/)
: CSnapDlg(pSourceForm, CModelessDlg::IDD, pParent)
, mDialogX( *this, pSourceForm )
, mnX( (pParams && pParams->lpData)? ((LPPOINT)pParams->lpData)->x : -1 )
, mnY( (pParams && pParams->lpData)? ((LPPOINT)pParams->lpData)->y : -1 )
, mbResizable( pSourceForm->GetControlProperties()->GetBoolProperty(nResizable) )
{
	m_bAsModal = false;
	m_bClosing = false;
	m_bAboutToClose = false;
	m_pDocToModReactor = NULL;
	m_bShowGrip = mbResizable;
}

CModelessDlg::~CModelessDlg()
{
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
	
	// get the form's properties
	CDclControlObject* pControlObject = mDialogX.GetSourceForm()->GetControlProperties();
	// set the window text
	SetWindowText(pControlObject->GetStrProperty(nTitleBarText));
		
	// call the method to set the title bar icon
	SetTitleBarIcon(pControlObject->GetLngProperty(nIcon));

	// set the rect for the control pane to be created
	CRect rcClient;
	GetClientRect(&rcClient);
	mDialogX.GetControlPane().SetPanePos( rcClient, false );

	// call method to create the controls
	UINT nID = 1000;
	mDialogX.GetControlPane().CreateControls(nID);

	// setup the rect default rect 
	rectThis.top = 0;
	rectThis.left = 0;
	
	CRect rcProfile(-1,-1,-1,-1);
	if (mDialogX.GetSourceForm()->GetControlProperties()->GetBoolProperty(nResizable) == TRUE)
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

		if (mDialogX.GetSourceForm()->m_bUsesClientRect == TRUE)
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

	if (mnY > -1)
	{
		// call method to set the start width and position of the form
		SetWindowPos(NULL, mnX, mnY, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	}
	else if (rcProfile.left > -1)
	{
		// call method to set the start width and position of the form
		SetWindowPos(NULL, rcProfile.left, rcProfile.top, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	}	
	else if (mnY == -1)
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
    
	// add the doc reactor if required for an event
	CString sEventDefun = mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nDocEventActivated);
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
		mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	

	SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CModelessDlg::AboutToClose() 
{
	FireCloseEvent();
}

bool CModelessDlg::QueryForClose() 
{
	if (mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose).IsEmpty())
		return true;
	return !(InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false));	
}

void CModelessDlg::OnClose() 
{
	AboutToClose();
	SaveSize();
	CSnapDlg::OnClose();
	DestroyWindow();
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
		mDialogX.GetControlPane().RecalcLayout();
		SaveSize();

		CRect rcThis;
		CWnd::GetWindowRect(&rcThis);
		// call methods to invoke the event
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}
}

void CModelessDlg::SetTitleBarIcon(int nPictureID)
{
	// set start position for navigating Pictures
	POSITION pos = mDialogX.GetSourceForm()->GetProject()->GetPictureList().GetHeadPosition();

	int nCount = 0;

	SetIcon(NULL, FALSE);
	
	if (m_hIconAcad != NULL)
		DestroyIcon(m_hIconAcad);
	
	// do loop to navigate Pictures
	while (nCount < mDialogX.GetSourceForm()->GetProject()->GetPictureList().GetCount())
	{
		// get position
		pos = mDialogX.GetSourceForm()->GetProject()->GetPictureList().FindIndex(nCount);
		// get current Picture in list
		CPictureObject* pPicture = mDialogX.GetSourceForm()->GetProject()->GetPictureList().GetNext(pos);
		
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
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventShow), false);	
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
	mDialogX.GetControlPane().CleanUpControls();
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
	  InvokeMethodIntInt(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventClose), -1, -1, false);	
	else
		// call methods to invoke the event
		InvokeMethodIntInt(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventClose), rc.left, rc.top, false);	

	mDialogX.GetControlPane().CleanUpControls();
	m_bClosing = true;
}


void CModelessDlg::OnOK()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventOnOk), false);
		CSnapDlg::OnOK();
		EndDialog(IDOK);
	}
}

void CModelessDlg::OnCancel()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), true))	
	{
		if (IsWindowEnabled())
		{
			CSnapDlg::OnCancel();
			EndDialog(IDCANCEL);
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
		mDialogX.GetControlPane().RecalcLayout();
		
		if (!m_bAboutToClose)
		{
			CRect rcThis;
			CWnd::GetWindowRect(&rcThis);
			// call methods to invoke the event
			InvokeMethodIntInt(
				mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
				rcThis.Width(),
				rcThis.Height(),
				true );
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
		TDialogControlPtr pControl = GetDialogObject().GetControlPane().FindControl( pMsg->hwnd );
		if( pControl && pControl->GetControlType() )
			return CWnd::PreTranslateMessage(pMsg); //if it's for an ActiveX control, bypass the immediate base class
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
