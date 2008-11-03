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


static UINT GetDialogTemplateIdFromForm( TDclFormPtr pSourceForm )
{
	TDclControlPtr pDclProperties = pSourceForm->GetControlProperties();
	if (pDclProperties->GetBooleanProperty(Prop::AllowResizing))
		return IDD_RESIZEABLE;
	return IDD_MODALDIALOG;
}

/////////////////////////////////////////////////////////////////////////////
// CModalDlg dialog


CModalDlg::CModalDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CBaseDlg( pSourceForm, GetDialogTemplateIdFromForm( pSourceForm ), pParent, pParams )
{
}


CModalDlg::~CModalDlg()	
{
}

void CModalDlg::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return; //already in the process of closing
	SetClosing();
	EndDialog( nStatus );
	if( m_hWnd )
		SendMessage(WM_CLOSE, 0, 0); //just to make sure
	//DestroyWindow();
}

INT_PTR CModalDlg::DoModal()
{
	INT_PTR nResult = CDialog::DoModal();
	delete this;
	return nResult;
}


void CModalDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModalDlg, CBaseDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
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

void CModalDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if (IsWindowVisible())
	{	
		InvokeMethodIntInt(
			mpTemplate->GetStringProperty(Prop::FormEventSize), 
			mpTemplate->GetLongProperty(Prop::Width),
			mpTemplate->GetLongProperty(Prop::Height),
			false);	
	}	
}

void CModalDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	__super::OnShowWindow(bShow, nStatus);
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::FormEventShow), false);	
}

void CModalDlg::OnOK()
{
	if (IsClosing() ||
			!InvokeCancelMethod(mpTemplate->GetStringProperty(Prop::FormEventCancelClose), false))	
	{
    InvokeMethod(mpTemplate->GetStringProperty(Prop::FormEventOnOk), false);
		__super::OnOK();
		CloseDialog(IDOK);
	}
}

void CModalDlg::OnCancel()
{
	if (IsClosing() ||
			!InvokeCancelMethod(mpTemplate->GetStringProperty(Prop::FormEventCancelClose), true))	
	{
    InvokeMethod(mpTemplate->GetStringProperty(Prop::FormEventOnCancel), false);
		__super::OnCancel();
		CloseDialog(IDCANCEL);
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
		TDialogControlPtr pControl = GetControlPane()->FindControl( pMsg->hwnd );
		if( pControl && pControl->GetControlType() == CtlActiveX )
			return CWnd::PreTranslateMessage(pMsg); //if it's for an ActiveX control, bypass the immediate base class
  }	
	
	return __super::PreTranslateMessage(pMsg);
}
