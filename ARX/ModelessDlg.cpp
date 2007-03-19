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

bool CModelessDialogX::CreateModeless( UINT nID ) const
{
	return mpOwner->Create( IsResizable()? IDD_RESIZEABLE : IDD_MODALDIALOG );
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

CModelessDlg::CModelessDlg( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CBaseDlg( pSourceForm, CModelessDlg::IDD, pParent, pParams )
, mpParent( pParent )
, mDialogX( *this, pSourceForm )
, mbResizable( pSourceForm->GetControlProperties()->GetBoolProperty( nResizable ) )
{
	m_bAsModal = false;
	m_bClosing = false;
	m_bAboutToClose = false;
	m_pDocToModReactor = NULL;
}

CModelessDlg::~CModelessDlg()
{
}

bool CModelessDlg::Create( UINT nTemplateID )
{
	return (__super::Create( nTemplateID, mpParent ) != FALSE);
}

BEGIN_MESSAGE_MAP(CModelessDlg, CBaseDlg)
	//{{AFX_MSG_MAP(CModelessDlg)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus)	
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

BOOL CModelessDlg::OnInitDialog() 
{
	CBaseDlg::OnInitDialog();

	if( m_bAsModal )
	{
		GetParent()->EnableWindow( FALSE );
		EnableWindow( TRUE );
	}

	// Modify the style
	//GetLayeredDialog()->AddLayeredStyle(m_hWnd);

	// Set the trasparency effect to 70%
	//GetLayeredDialog()->SetTransparentPercentage(m_hWnd, 80);

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
	__super::OnClose();
	DestroyWindow();
}

LONG CModelessDlg::onAcadKeepFocus(UINT, LONG)
{
	return TRUE;
}

void CModelessDlg::OnSize(UINT nType, int cx, int cy) 
{
	CBaseDlg::OnSize(nType, cx, cy);
	mDialogX.GetControlPane().RecalcLayout();
	if (IsWindowVisible() && m_bClosing == false  && m_bAboutToClose == false)
	{
		CRect rcThis;
		CWnd::GetWindowRect(&rcThis);
		// call methods to invoke the event
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void CModelessDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBaseDlg::OnShowWindow(bShow, nStatus);
	
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
	__super::OnDestroy();
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
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventOnOk), true);
		__super::OnOK();
		mDialogX.CloseDialog(IDOK);
	}
}

void CModelessDlg::OnCancel()
{
	if (!InvokeCancelMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventCancelClose), true))	
	{
    InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventOnCancel), true);
		__super::OnCancel();
		mDialogX.CloseDialog(IDCANCEL);
	}
}

void CModelessDlg::OnMove(int x, int y) 
{
	CBaseDlg::OnMove(x, y);

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
		if( pControl && pControl->GetControlType() == CtlActiveX )
			return CWnd::PreTranslateMessage(pMsg); //if it's for an ActiveX control, bypass the immediate base class
	}

	return CBaseDlg::PreTranslateMessage(pMsg);
}
