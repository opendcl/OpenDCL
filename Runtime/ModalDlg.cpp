// ModalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModalDlg.h"
#include "Resource.h"
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
		return IDD_BASERESIZABLEDLG;
	return IDD_BASEDLG;
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
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CModalDlg message handlers

BOOL CModalDlg::OnInitDialog() 
{
	__super::OnInitDialog();

	return TRUE; //return TRUE so the dialog manager does its own control focus work
}

int CModalDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableToolTips (TRUE);
	return 0;
}

void CModalDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if( IsIgnoreSizing() || !IsWindowVisible() )
		return;
	GetArxServices()->HandleEvent( Prop::FormEventMove, false, args_NN( x, y ) );
}

void CModalDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if( !IsIgnoreSizing() && IsWindowVisible() )
	{	
		GetArxServices()->HandleEvent( Prop::FormEventSize, false,
																	 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																						mpTemplate->GetLongProperty( Prop::Height ) ) );
	}	
}

void CModalDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	__super::OnShowWindow(bShow, nStatus);
	GetArxServices()->HandleEvent( Prop::FormEventShow, false );
	SetForegroundWindow();
	SetActiveWindow();
}

void CModalDlg::OnOK()
{
	if( IsClosing() || IsCloseAllowed( false ) )
	{
    if( GetArxServices()->HandleEvent( Prop::FormEventOnOk, false ) )
			return;
		__super::OnOK();
		CloseDialog(IDOK);
	}
}

void CModalDlg::OnCancel()
{
	if( IsClosing() || IsCloseAllowed( true ) )
	{
    if( GetArxServices()->HandleEvent( Prop::FormEventOnCancel, false ) )
			return;
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
