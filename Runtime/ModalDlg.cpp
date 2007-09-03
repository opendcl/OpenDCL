// ModalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModalDlg.h"
#include "DialogControl.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "Project.h"
#include "ControlTypes.h"
#include "ArxWorkspace.h"

static int ChildFrameID = 10101;


// CModalDialogX interface implementation
CModalDialogX::CModalDialogX( CModalDlg& Owner, CDclFormObject* pDclForm )
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

bool CModalDialogX::IsResizable() const {
	return mpOwner->IsResizable();
}

HWND CModalDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

void CModalDialogX::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return; //already in the process of closing
	SetClosing();
	mpOwner->EndDialog( nStatus );
	//mpOwner->DestroyWindow();
	mpOwner->SendMessage(WM_CLOSE, 0, 0); //just to make sure
}

INT_PTR CModalDialogX::DoModal()
{
	INT_PTR nResult = mpOwner->DoModal();
	delete mpOwner;
	return nResult;
}

bool CModalDialogX::SetMinMaxSize( const CSize& min, const CSize& max )
{
	CDialogObject::SetMinMaxSize( min, max );
	mpOwner->SetMinMaxSize (min, max);
	return true;
}


static UINT GetDialogTemplateIdFromForm( CDclFormObject* pSourceForm )
{
	CDclControlObject *pDclProperties = pSourceForm->GetControlProperties();
	if (pDclProperties->GetBooleanProperty(Prop::Resizable))
		return IDD_RESIZEABLE;
	return IDD_MODALDIALOG;
}

/////////////////////////////////////////////////////////////////////////////
// CModalDlg dialog


CModalDlg::CModalDlg( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CBaseDlg( pSourceForm, GetDialogTemplateIdFromForm( pSourceForm ), pParent, pParams )
, mDialogX( *this, pSourceForm )
, mbResizable( pSourceForm->GetControlProperties()->GetBooleanProperty( Prop::Resizable ) )
{
}


CModalDlg::~CModalDlg()	
{
}


void CModalDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModalDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModalDlg, CBaseDlg)
	//{{AFX_MSG_MAP(CModalDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CModalDlg message handlers

BOOL CModalDlg::OnInitDialog() 
{
	__super::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

int CModalDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableToolTips (TRUE);
	return 0;
}

BOOL CModalDlg::OnNotify_ToolTipTitle(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	return FALSE;
}

void CModalDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	mDialogX.GetControlPane().RecalcLayout();
	if (IsWindowVisible())
	{	
		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		// call methods to invoke the event
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(Prop::FormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}	
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void CModalDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	__super::OnShowWindow(bShow, nStatus);
	// call methods to invoke the event
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(Prop::FormEventShow), false);	
}

void CModalDlg::OnOK()
{
	if (mDialogX.IsClosing() ||
			!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(Prop::FormEventCancelClose), false))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(Prop::FormEventOnOk), false);
		__super::OnOK();
		mDialogX.CloseDialog(IDOK);
	}
}

void CModalDlg::OnCancel()
{
	if (mDialogX.IsClosing() ||
			!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(Prop::FormEventCancelClose), true))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(Prop::FormEventOnCancel), false);
		__super::OnCancel();
		mDialogX.CloseDialog(IDCANCEL);
	}
}

BOOL CModalDlg::PreTranslateMessage(MSG* pMsg) 
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
		if( pControl && pControl->GetControlType() == CtlActiveX )
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
            if (ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != -1
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
            if (ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != -1
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
            if (ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != -1
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
            if (ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE 
                && ctrlAtPos->m_pControl->GetBooleanProperty(Prop::IsTabStop) != -1
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
	
	return __super::PreTranslateMessage(pMsg);
}
