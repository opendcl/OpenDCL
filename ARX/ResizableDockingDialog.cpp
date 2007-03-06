// DockingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ResizableDockingDialog.h"
#include "InvokeMethod.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "DclFormObject.h"
#include "AcadDocReactor.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"


const int nDeflateRect = 3;
const int nBottomAdjustment = 5;
const int nHeightOffset = 2;


// CResizableDockingDialogX interface implementation
CResizableDockingDialogX::CResizableDockingDialogX( CResizableDockingDialog& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CResizableDockingDialogX::~CResizableDockingDialogX()
	{
}

DclFormType CResizableDockingDialogX::GetType() const
{
	return VdclDockable;
}

HWND CResizableDockingDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

bool CResizableDockingDialogX::IsFloating() const
{
	return (mpOwner->IsFloating() != FALSE);
}

bool CResizableDockingDialogX::CreateModeless( UINT nID ) const
{
	CDclControlObject* pProps = mpSourceForm->GetControlProperties();
	int nDocHeight = pProps->GetLngProperty(nHeight);
	DWORD dwDockableSides = 0;
	DWORD dwDefaultDockableSide = 0;

	switch (pProps->GetLngProperty(nDockableSides))
	{
	case 1:
		// set the form to only dock on the top side
		dwDockableSides = CBRS_ALIGN_TOP;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		nDocHeight += 8;
		break;
	case 2:
		// set the form to only dock on the bottom side
		dwDockableSides = CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_BOTTOM;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 3:
		// set the form to only dock on the top or bottom sides
		dwDockableSides = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;				
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 4:
		// set the form to only dock on the any side
		dwDockableSides = AFX_IDW_DOCKBAR_TOP | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP;				
		dwDefaultDockableSide = CBRS_ALIGN_LEFT;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 5:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	default:
		// set the form to only dock on the left or right sides
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		break;
	}

	CRect rect = CFrameWnd::rectDefault;
	if (rect.left < 0)
		rect.left = 0;
	if (rect.top < 0)
		rect.top = 0;				
	rect.bottom = rect.top + nDocHeight;
	rect.right = rect.left + pProps->GetLngProperty(nWidth);

	if( !mpOwner->Create( mpSourceForm->GetKeyPath(), rect, nID ) )
		return false;
	if (mpSourceForm->GetUUIDAsString().IsEmpty())
	{
		UUID uuid;
		UuidCreate(&uuid);
		mpOwner->SetToolID (&uuid);
	}
	else
	{
    UUID uuid = mpSourceForm->GetUUID();
		mpOwner->SetToolID (&uuid);
	}			
	// set the form to only dock on the set side(s)
	mpOwner->EnableDocking(dwDockableSides);
	// loads the dockable form but does not display it
	mpOwner->RestoreControlBar(dwDefaultDockableSide);
	return true;
}

void CResizableDockingDialogX::CloseDialog(int nStatus) const
{
	mpOwner->EndModalLoop(nStatus); //set the status
	CWnd* pTopLevel = IsFloating()? mpOwner->GetParent()->GetParent() : mpOwner;
	if( pTopLevel && ::IsWindow(pTopLevel->m_hWnd) )
		pTopLevel->SendMessage(WM_CLOSE);
	if( ::IsWindow(mpOwner->m_hWnd) )
		mpOwner->DestroyWindow();
}

bool CResizableDockingDialogX::GetWindowRect( CRect& rcDlg ) const
{
	if (mpOwner->IsFloating())
		mpOwner->GetFloatingRect(&rcDlg);
	else
		mpOwner->GetClientArea(rcDlg);
	return true;
}

bool CResizableDockingDialogX::GetClientRect( CRect& rcDlg ) const
{
	mpOwner->GetClientArea(rcDlg);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog dialog

CResizableDockingDialog::CResizableDockingDialog( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAdUiDockControlBar( ADUI_DOCK_CS_STDMOUSECLICKS | ADUI_DOCK_CS_DESTROY_ON_CLOSE )
, mpParent( pParent )
, mDialogX( *this, pSourceForm )
, mbClosing( false )
, mbHiding( false )
{
	m_pDocToModReactor = NULL;
}

CResizableDockingDialog::~CResizableDockingDialog()
{
}


BEGIN_MESSAGE_MAP(CResizableDockingDialog, CAdUiDockControlBar)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog message handlers

#include "VdclStatic.h"
bool CResizableDockingDialog::Create( LPCTSTR lpszTitle, CRect rect, UINT nID ) 
{
	CString strWndClass = AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW));	
  if (!CAdUiDockControlBar::Create( strWndClass, lpszTitle,
																		WS_VISIBLE | WS_CHILD /*| WS_CLIPCHILDREN*/,
																		rect, mpParent, nID))
	{
		return false;
	}
	ModifyStyleEx( 0, WS_EX_CONTROLPARENT );

	return true;
}

int CResizableDockingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAdUiDockControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//  setup for assigning the form it's properties
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();

	// set the window text
	SetWindowText(pProps->GetStrProperty(nTitleBarText));
	
	CRect rectPane;
	if (mDialogX.GetSourceForm()->m_bUsesClientRect == TRUE && IsFloating())
		GetClientArea( rectPane );
	else
	{
		GetWindowRect( &rectPane );
		ScreenToClient( &rectPane );
	}

	rectPane.top += (nDeflateRect + 20);
	rectPane.left += nDeflateRect;
	rectPane.bottom += (nBottomAdjustment - nHeightOffset);

	mDialogX.GetControlPane().SetPanePos( rectPane );

	// call method to create the controls
	UINT nID = 1000;
	mDialogX.GetControlPane().CreateControls(nID);

	//if (pProps->GetLngProperty(nMaxDialogWidth) > -1)
	//{
	//	long lMinHeight;
	//	long lMinWidth;
	//	GetFloatingMinSize(&lMinHeight, &lMinWidth);
	//	lMinWidth = pProps->GetLngProperty(nMinDialogWidth);
	//}
	
	// add the doc reactor if required for an event
	CString sEventDefun = pProps->GetStrProperty(nDocEventActivated);
	if (sEventDefun.GetLength() > 0)
	{
		m_pDocToModReactor = new CAcadDocReactor();
		m_pDocToModReactor->m_EventDefun = sEventDefun;
		// activate the reactor
		acDocManager->addReactor(m_pDocToModReactor);
	}	

	// call methods to invoke the event
	InvokeMethod(pProps->GetStrProperty(nFormEventInitialize), true);	
	
	CRect rectWindow;
	GetWindowRect( &rectWindow );
	// call methods to invoke the event
	InvokeMethodIntInt(
		pProps->GetStrProperty(nFormEventSize), 
		rectWindow.Width(),
		rectWindow.Height(),
		false);	

	return 1;
}

void CResizableDockingDialog::GetClientArea(CRect &rect)
{
	GetUsedRect(rect);
}


void CResizableDockingDialog::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	CAdUiDockControlBar::SizeChanged(lpRect, bFloating, flags);
	if (!mbClosing)
	{
		lpRect->top += (nDeflateRect + 2);
		lpRect->left += nDeflateRect;
		lpRect->bottom += (nBottomAdjustment - nHeightOffset);

		// resize the control pane so all offsets are set correctly
		mDialogX.GetControlPane().SetPanePos( *lpRect, true );
		
		// call methods to invoke the event
		CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
		InvokeMethodIntInt(
			pProps->GetStrProperty(nFormEventSize), 
			lpRect->Width(), 
			lpRect->Height(),
			true);	
	}
}

void CResizableDockingDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nFormEventShow), true);	

	mbHiding = !mbClosing && !bShow;
	CAdUiDockControlBar::OnShowWindow(bShow, nStatus);
}

bool CResizableDockingDialog::CanFrameworkTakeFocus ()
{
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

void CResizableDockingDialog::PostNcDestroy() 
{
	CAdUiDockControlBar::PostNcDestroy();
	//the floating docbar container will 'delete this' when floating because 
	//the ADUI_DOCK_CS_DESTROY_ON_CLOSE style is set in the constructor
	if( !IsFloating() )
		delete this;
}

bool CResizableDockingDialog::OnClosing()
{
	mbClosing = true;
	CAdUiDockControlBar::OnClosing();
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nFormEventClose), true);
	if( !mbHiding && !IsFloating() )
		PostMessage(WM_CLOSE); //to make sure the window gets destroyed no matter how we got here
	return true;
}

//*****************************************************************************
// 
// Method: CResizableDockingDialog::AddCustomMenuItems()
// 
// Purpose: [Override for removing right click pop-up menu]
// 
// Parameters:  
//		[hMenu]:  [pointer to the pop-up menu]
// 
// Returns:	BOOL
// 
//*****************************************************************************
BOOL CResizableDockingDialog::AddCustomMenuItems(LPARAM hMenu)
{
	if (IsWindowVisible())
	{
		// Get the menu object.
		CMenu menu;
		menu.Attach((HMENU) hMenu);

		// remove the menu its
		// I am doing this so the user can right click on the ISO view picture and have
		// is scroll thru the different ISO Plane Options
		menu.RemoveMenu(ID_ADUI_ALLOWDOCK, MF_BYCOMMAND);
		menu.RemoveMenu(ID_ADUI_HIDEBAR, MF_BYCOMMAND);

		// Release the menu object.
		menu.Detach();
	}
	
	return TRUE;
}

//*****************************************************************************
// 
// Method: CResizableDockingDialog::OnUserSizing()
// 
// Purpose: [Overrides the OnSizing event and allows us to force the Recomended width 
//			while the user is resizing]
// 
// Parameters:  
//		[nSide]:  [which side is being stretched]
//		[pRect]:  [a pointer to the CRect of the Floating form]
// 
// Returns:	nothing
// 
//*****************************************************************************
void CResizableDockingDialog::OnUserSizing(UINT nSide, LPRECT pRect)
{	
	// pass on OnUserSizing method
	CAdUiDockControlBar::OnUserSizing(nSide, pRect);
		
	CRect FloatingRect;
	CAdUiDockControlBar::GetFloatingRect(&FloatingRect);

	int nThisWidth = pRect->right - pRect->left;
}


//*****************************************************************************
// 
// Method: CResizableDockingDialog::GetFloatingMinSize()
// 
// Purpose: [This method is pressent to allow a smaller than normal minum width and heigth of
//			 the floating form.  Removing this method would make the min width to big]
// 
// Parameters:  
//		[pnMinWidth]:  [Mininum width]
//		[pnMinHeight]:  [Mininum height]
// 
// Returns:	nothing
// 
//*****************************************************************************
void CResizableDockingDialog::GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight)
{
	// by not adjusting the pnMinWidth or pnMinHeight, the min pnMinWidth will become small enough
	// that the OnResizeEvent will take care of the width size itself.
}


BOOL CResizableDockingDialog::PreTranslateMessage(MSG* pMsg) 
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
		
	return CAdUiDockControlBar::PreTranslateMessage(pMsg);
}

void CResizableDockingDialog::OnDestroy() 
{
	if (m_pDocToModReactor != NULL)
	{
		acDocManager->removeReactor(m_pDocToModReactor);
		delete m_pDocToModReactor;
		m_pDocToModReactor = NULL;
	}
	if( !mbClosing )
		OnClosing();
	mDialogX.GetControlPane().CleanUpControls();
	CAdUiDockControlBar::OnDestroy();
}
