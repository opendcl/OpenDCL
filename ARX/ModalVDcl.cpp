// CModalVDcl.cpp : implementation file
//

#include "stdafx.h"
#include "ModalVDcl.h"
#include "DialogControl.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "Project.h"
#include "PictureObject.h"
#include "ControlTypes.h"
#include "ArxWorkspace.h"

static int ChildFrameID = 10101;


// CModalDialogX interface implementation
CModalDialogX::CModalDialogX( CModalVDcl& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CModalDialogX::~CModalDialogX()
{
}

DclFormType CModalDialogX::GetType() const
{
	return VdclModal;
}

HWND CModalDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

void CModalDialogX::CloseDialog(int nStatus) const
{
	mpOwner->EndDialog( nStatus );
	mpOwner->SendMessage(WM_CLOSE, 0, 0); //just to make sure
}

INT_PTR CModalDialogX::DoModal()
{
	return mpOwner->DoModal();
}

bool CModalDialogX::SetMinMaxSize( const CSize& min, const CSize& max )
{
	CDialogObject::SetMinMaxSize( min, max );
	mpOwner->SetDialogMaxExtents (min.cx, max.cy);
	mpOwner->SetDialogMinExtents (min.cx, max.cy);
	return true;
}


static UINT GetDialogTemplateIdFromForm( CDclFormObject* pSourceForm )
{
	CDclControlObject *pDclProperties = pSourceForm->GetControlProperties();
	if (pDclProperties->GetBoolProperty(nResizable))
		return IDD_RESIZEABLE;
	return IDD_MODALDIALOG;
}


/////////////////////////////////////////////////////////////////////////////
// CModalVDcl dialog


CModalVDcl::CModalVDcl(CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/)
: CSnapDlg(pSourceForm, GetDialogTemplateIdFromForm( pSourceForm ), pParent)
, mDialogX( *this, pSourceForm )
, mnX( (pParams && pParams->lpData)? ((LPPOINT)pParams->lpData)->x : -1 )
, mnY( (pParams && pParams->lpData)? ((LPPOINT)pParams->lpData)->y : -1 )
{
}


CModalVDcl::~CModalVDcl()	
{
}


void CModalVDcl::DoDataExchange(CDataExchange* pDX)
{
	CSnapDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModalVDcl)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModalVDcl, CSnapDlg)
	//{{AFX_MSG_MAP(CModalVDcl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CModalVDcl message handlers

BOOL CModalVDcl::OnInitDialog() 
{
	CSnapDlg::OnInitDialog();
//GetParent()->EnableWindow(TRUE);
	//  setup for assigning the form it's properties
	CPoint pt;
	CRect rectThis;
	int nCtlWidth;
	int nCtlHeight;

	// get the form's properties
	CDclControlObject* pFormProps = mDialogX.GetSourceForm()->GetControlProperties();
	// set the window text
	SetWindowText(pFormProps->GetStrProperty(nTitleBarText));
	
	// call the method to set the title bar icon
	SetTitleBarIcon(pFormProps->GetLngProperty(nIcon));

	// create the control pane that will display the controls
	CRect rcClient;
	GetClientRect(&rcClient);
	mDialogX.GetControlPane().SetPanePos( rcClient, false );	
	
	// call method to create the controls
	UINT nID = 1000;
	mDialogX.GetControlPane().CreateControls(mDialogX.GetSourceForm(), nID);

	// setup the rect default rect 
	rectThis.top = 0;
	rectThis.left = 0;
	
	CSize szProfile(-1,-1);
	if (mDialogX.GetSourceForm()->GetControlProperties()->GetBoolProperty(nResizable) == TRUE)
		szProfile = ReadSize();

	CRect rcLastRect(-1,-1,-1,-1);
	rcLastRect = ReadRect();

	CRect rcThis;
	GetClientRect(&rcThis);
		
	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	if (szProfile.cx > 0)
	{
		// get the width
		nCtlWidth = szProfile.cx;

		// get the height
		nCtlHeight = szProfile.cy;

		rectThis.right = nCtlWidth;
		rectThis.bottom = nCtlHeight;

	}
	else
	{
		// get the width
		nCtlWidth = pFormProps->GetLngProperty(nWidth);

		// get the height
		nCtlHeight = pFormProps->GetLngProperty(nHeight);
	
		// set the rect for the control pane to be created
		CRect rcWnd;
		GetWindowRect(&rcWnd);

		if (mDialogX.GetSourceForm()->m_bUsesClientRect == TRUE)
		{
			nCtlHeight += rcWnd.Height() - rcThis.Height();
			nCtlWidth += rcWnd.Width() - rcThis.Width();
		}
		
		if (nCtlWidth > nScreenWidth)
			nCtlWidth = nScreenWidth;

		if (nCtlHeight > nScreenHeight)
			nCtlHeight = nScreenHeight;

		rectThis.right = nCtlWidth;
		rectThis.bottom = nCtlHeight;

		
	}
	if (mnX > -1)
	{
		// call method to set the start width and position of the form
		SetWindowPos(NULL, mnX, mnY, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);

	}	
	else if (rcLastRect.left < 0)
	{
		// get the left and top values to center the form on the screen	
		pt.y =  (nScreenHeight - rectThis.Height()) / 2;
		pt.x =  (nScreenWidth - rectThis.Width()) / 2;

		// call method to set the start width and position of the form
		SetWindowPos(NULL, pt.x, pt.y, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);

	}
	else 
	{
		// call method to set the start width and position of the form
		SetWindowPos(NULL, rcLastRect.left, rcLastRect.top, nCtlWidth, nCtlHeight, SWP_NOACTIVATE);
	}

	if (pFormProps->GetLngProperty(nMaxDialogWidth) > -1)
	{
		int nMaxWidth = pFormProps->GetLngProperty(nMaxDialogWidth);
		int nMaxHeight = pFormProps->GetLngProperty(nMaxDialogHeight);
		int nMinWidth = pFormProps->GetLngProperty(nMinDialogWidth);
		int nMinHeight = pFormProps->GetLngProperty(nMinDialogHeight);

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
	
	// call methods to invoke the OnInitDialog event
	InvokeMethod(pFormProps->GetStrProperty(nFormEventInitialize), false);

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

int CModalVDcl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSnapDlg::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableToolTips (TRUE);
	
	
	return 0;
}

void CModalVDcl::OnDestroy() 
{
	SaveSize();

	CRect rcThis;
	GetWindowRect(&rcThis);
	CPoint pt;

	CDialog::SetIcon(NULL, FALSE);

	if (m_hIconAcad != NULL)
		DestroyIcon(m_hIconAcad);
	
	// get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - rcThis.Height()) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - rcThis.Width()) / 2;
	
	if (pt.y == rcThis.top &&
		pt.x == rcThis.left)
		// call methods to invoke the event
		InvokeMethodIntInt(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventClose), -1, -1, false);	
	else
		// call methods to invoke the event
		InvokeMethodIntInt(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventClose), rcThis.left, rcThis.top, false);	
	
	mDialogX.GetControlPane().CleanUpControls();	
	
	CSnapDlg::OnDestroy();
}


BOOL CModalVDcl::OnNotify_ToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	return FALSE;
}

void CModalVDcl::CloseDialog()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false))	
	{
		CSnapDlg::OnOK();
	}
}
bool CModalVDcl::QueryForClose() 
{
	if (mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose) == CString())
		return true;

	return !(InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false));	
}

void CModalVDcl::OnClose() 
{
	SaveSize();
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false))	
	{
		CSnapDlg::OnClose();				
	}
}

void CModalVDcl::OnSize(UINT nType, int cx, int cy) 
{
	CSnapDlg::OnSize(nType, cx, cy);
	
	mDialogX.GetControlPane().RecalcLayout();

	if (CWnd::IsWindowVisible())
	{	
		CRect rcThis;
		CWnd::GetWindowRect(&rcThis);
		SaveSize();
		// call methods to invoke the event
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}	
	
}

void CModalVDcl::SetTitleBarIcon(int nPictureID)
{
	//
	// set start position for navigating Pictures
	POSITION pos = mDialogX.GetSourceForm()->GetProject()->GetPictureList().GetHeadPosition();

	int nCount = 0;
	bool bFoundIcon = false;
	
	CDialog::SetIcon(NULL, FALSE);
	
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
			CDialog::SetIcon(m_hIconAcad, FALSE);
			return;
		}
		// increment counter
		nCount++;
	}

	// if no icon was found to be specified, display nothing.
  // Used to display acad icon, but no more
	// load and display the Acad Small icon
	HMODULE hRes = _hdllInstance;		
	m_hIconAcad = LoadIcon(hRes, MAKEINTRESOURCE(IDI_DMICON));
	CDialog::SetIcon(m_hIconAcad, FALSE);	
}

void CModalVDcl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSnapDlg::OnShowWindow(bShow, nStatus);
	
	// call methods to invoke the event
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventShow), false);	
		
}

void CModalVDcl::OnOK()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), false))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventOnOk), false);
		CSnapDlg::OnOK();
		//CDialog::EndDialog(MB_OK);
	}
}

void CModalVDcl::OnCancel()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), true))	
	{
		CSnapDlg::OnCancel();
		//CDialog::EndDialog(2);
	}
}

BOOL CModalVDcl::PreTranslateMessage(MSG* pMsg) 
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

    //This currently does not work terribly well. But I would like it to, so I am leaving
    //it to maybe be updated.
    /*
    //If tab was pressed, manually force the focus to the next or previous control.
    //Our grid doesn't work with tabs, which is why I am doing this.
    //Tab stop value of -1 means it can't be a tab stop.
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) {
      if (pMsg->lParam != VK_SHIFT) {
        //Shift not pressed, go forward        
        try {
          POSITION pos = posActiveControl;
          m_ControlCol.GetNext(pos);
          //Find the next enabled, tab-stop control
          while (pos != NULL) {
            TDialogControlPtr ctrlAtPos = m_ControlCol.GetNext(pos);
            if (ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != -1
                && ctrlAtPos->m_pControl->m_pWnd->IsWindowEnabled()) 
            {
              ctrlAtPos->m_pControl->m_pWnd->SetFocus();
              return true;
            }
          }
          //Did not find one. Loop back to the beginning and try from there.
          pos = m_ControlCol.GetHeadPosition();
          while (pos != NULL) {
            TDialogControlPtr ctrlAtPos = m_ControlCol.GetNext(pos);
            if (ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != -1
                && ctrlAtPos->m_pControl->m_pWnd->IsWindowEnabled()) 
            {
              ctrlAtPos->m_pControl->m_pWnd->SetFocus();
              return true;
            }
          }
          //Never found a next control to activate. Fall through to pre-translate
          //call at the end.
        }
        catch(...) {}
      } else {
        //Shift pressed, go backward
        //Code is similar to above, but uses getPrevious rather than get next
        //and starts at tail rather than head.
        try {
          POSITION pos = posActiveControl;
          m_ControlCol.GetPrev(pos);
          //Find the previous enabled, tab-stop control
          while (pos != NULL) {
            TDialogControlPtr ctrlAtPos = m_ControlCol.GetPrev(pos);
            if (ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != -1
                && ctrlAtPos->m_pControl->m_pWnd->IsWindowEnabled()) 
            {
              ctrlAtPos->m_pControl->m_pWnd->SetFocus();
              return true;
            }
          }
          //Did not find one. Loop back to the beginning and try from there.
          pos = m_ControlCol.GetTailPosition();
          while (pos != NULL) {
            TDialogControlPtr ctrlAtPos = m_ControlCol.GetPrev(pos);
            if (ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBoolProperty(nIsTabStop) != -1
                && ctrlAtPos->m_pControl->m_pWnd->IsWindowEnabled()) 
            {
              ctrlAtPos->m_pControl->m_pWnd->SetFocus();
              return true;
            }
          }
          //Never found a previous control to activate. Fall through to pre-translate
          //call at the end.
        }
        catch(...) {}
      }
    }*/
  }	
	
	return CSnapDlg::PreTranslateMessage(pMsg);
}
