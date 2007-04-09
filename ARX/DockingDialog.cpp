// DockingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DockingDialog.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "AcadDocReactor.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"


const int nDeflateRect = 3;
const int nBottomAdjustment = 5;
const int nHeightOffset = 2;


// CDockingDialogX interface implementation
CDockingDialogX::CDockingDialogX( CDockingDialog& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CDockingDialogX::~CDockingDialogX()
{
}

DclFormType CDockingDialogX::GetType() const
{
	return VdclDockable;
}

HWND CDockingDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

bool CDockingDialogX::IsFloating() const
{
	return (mpOwner->IsFloating() != FALSE);
}

bool CDockingDialogX::CreateModeless( UINT nID ) const
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
		dwDefaultDockableSide = CBRS_ALIGN_TOP;
		nDocHeight += 8;
		break;
	case 2:
		// set the form to only dock on the bottom side
		dwDockableSides = CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = CBRS_ALIGN_BOTTOM;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 3:
		// set the form to only dock on the top or bottom sides
		dwDockableSides = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;				
		dwDefaultDockableSide = CBRS_ALIGN_TOP;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 4:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP;				
		dwDefaultDockableSide = CBRS_ALIGN_LEFT;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	case 5:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = CBRS_ALIGN_LEFT;
		nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
		break;
	default:
		// set the form to only dock on the left or right sides
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT;
		dwDefaultDockableSide = CBRS_ALIGN_LEFT;
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

void CDockingDialogX::CloseDialog(int nStatus) const
{
	mpOwner->EndModalLoop(nStatus); //set the status
	CWnd* pTopLevel = IsFloating()? mpOwner->GetParent()->GetParent() : mpOwner;
	if( pTopLevel && ::IsWindow(pTopLevel->m_hWnd) )
		pTopLevel->SendMessage(WM_CLOSE);
	if( ::IsWindow(mpOwner->m_hWnd) )
		mpOwner->DestroyWindow();
}

bool CDockingDialogX::GetWindowRect( CRect& rcDlg ) const
{
	if (mpOwner->IsFloating())
		mpOwner->GetFloatingRect(&rcDlg);
	else
		mpOwner->GetClientArea(rcDlg);
	return true;
}

bool CDockingDialogX::GetClientRect( CRect& rcDlg ) const
{
	mpOwner->GetClientArea(rcDlg);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog dialog

CDockingDialog::CDockingDialog( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CAdUiDockControlBar( ADUI_DOCK_CS_STDMOUSECLICKS | ADUI_DOCK_CS_DESTROY_ON_CLOSE )
, mpParent( pParent )
, mDialogX( *this, pSourceForm )
, mbClosing( false )
, mbHiding( false )
, mbTrackingMouse( false )
, mbInMenuLoop( false )
{
	m_bDockingSizeAdjusted = false;
	m_bFloatingSizeAdjusted = false;
	m_pDocToModReactor = NULL;
}


CDockingDialog::~CDockingDialog()
{
	if( mbTrackingMouse )
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, NULL, 0 };
		_TrackMouseEvent( &tm );
	}
}


BEGIN_MESSAGE_MAP(CDockingDialog, CAdUiDockControlBar)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_NCMOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog message handlers

bool CDockingDialog::Create( LPCTSTR lpszTitle, CRect rect, UINT nID ) 
{
	CString strWndClass = AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW));	
  if (!CAdUiDockControlBar::Create( strWndClass, lpszTitle,
																		WS_VISIBLE | WS_CHILD /*| WS_CLIPCHILDREN*/,
																		rect, mpParent, nID))
		return false;
	ModifyStyleEx( 0, WS_EX_CONTROLPARENT );
	__if_exists(idPinBtn)
	{
	CWnd* pPinBtn = GetDlgItem( idPinBtn );
	if( pPinBtn )
		pPinBtn->DestroyWindow();
	}
	return true;
}

int CDockingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAdUiDockControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//  setup for assigning the form it's properties
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();

	// set the window text
	SetWindowText(pProps->GetStrProperty(nTitleBarText));
	
	CRect rectPane;
	if (mDialogX.GetSourceForm()->UsesClientRect() && IsFloating())
		GetClientArea( rectPane );
	else
	{
		GetWindowRect( &rectPane );
		ScreenToClient( &rectPane );
	}

	rectPane.top += (nDeflateRect + 2);
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

	CRect rcThis;
	if( mDialogX.GetSourceForm()->UsesClientRect() )
		GetClientRect( &rcThis );
	else
		GetWindowRect( &rcThis );
	// call methods to invoke the event
	InvokeMethodIntInt(
		pProps->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	

	return 1;
}

void CDockingDialog::GetClientArea(CRect &rect)
{
	GetUsedRect(rect);
}


void CDockingDialog::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	CAdUiDockControlBar::SizeChanged(lpRect, bFloating, flags);
	if (!mbClosing)
	{
		lpRect->top += (nDeflateRect + 2);
		lpRect->left += nDeflateRect;
		lpRect->bottom += (nBottomAdjustment - nHeightOffset);

		// resize the control pane so all offsets are set correctly
		mDialogX.GetControlPane().SetPanePos( *lpRect );

		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		
		// call methods to invoke the event
		CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
		InvokeMethodIntInt(
			pProps->GetStrProperty(nFormEventSize), 
			rcThis.Width(), 
			rcThis.Height(),
			true);	
	}
}

BOOL CDockingDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	InvokeMethod(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nEventOnHelp), true);
	return TRUE;
}

void CDockingDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// call methods to invoke the event
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nFormEventShow), true);	

	mbHiding = !mbClosing && !bShow;
	CAdUiDockControlBar::OnShowWindow(bShow, nStatus);
}

bool CDockingDialog::CanFrameworkTakeFocus ()
{
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

void CDockingDialog::PostNcDestroy() 
{
	CAdUiDockControlBar::PostNcDestroy();
	//the floating docbar container will 'delete this' when floating because 
	//the ADUI_DOCK_CS_DESTROY_ON_CLOSE style is set in the constructor
	if( !IsFloating() )
		delete this;
}

bool CDockingDialog::OnClosing()
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
// Method: CDockingDialog::AddCustomMenuItems()
// 
// Purpose: [Override for removing right click pop-up menu]
// 
// Parameters:  
//		[hMenu]:  [pointer to the pop-up menu]
// 
// Returns:	BOOL
// 
//*****************************************************************************
BOOL CDockingDialog::AddCustomMenuItems(LPARAM hMenu)
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
// Method: CDockingDialog::OnUserSizing()
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
void CDockingDialog::OnUserSizing(UINT nSide, LPRECT pRect)
{	
	// pass on OnUserSizing method
	CAdUiDockControlBar::OnUserSizing(nSide, pRect);

	CRect FloatingRect;
	CAdUiDockControlBar::GetFloatingRect(&FloatingRect);

	pRect->right = pRect->left + mDialogX.GetSourceForm()->GetControlProperties()->GetLngProperty(nWidth);
}


//*****************************************************************************
// 
// Method: CDockingDialog::CalcFloatingSize()
// 
// Purpose: [Calc's the size of the floating form]
// 
// Parameters: none
// 
// Returns:	CSize
// 
//*****************************************************************************
CSize CDockingDialog::CalcFloatingSize()
{
	CSize RetSize;

	// setup for retrieving the current window pos
	CRect FloatingRect;

	// get window size and position
	CAdUiDockControlBar::GetFloatingRect(&FloatingRect);
	
	RetSize.cy = FloatingRect.Height();

	RetSize.cx = mDialogX.GetSourceForm()->GetControlProperties()->GetLngProperty(nWidth);
	
	return RetSize;
}


//*****************************************************************************
// 
// Method: CDockingDialog::CalcDynamicLayout()
// 
// Purpose: [Call what floating to query what the form size should be]
// 
// Returns:	CSize
// 
//*****************************************************************************
CSize CDockingDialog::CalcDynamicLayout( int nLength, DWORD dwMode )
{
	
	CSize RetSize;
	CRect FloatingRect;	
	
	if(CDockingDialog::IsFloating())
	{	
		RetSize = CDockingDialog::CalcFloatingSize();
		RetSize.cx = mDialogX.GetSourceForm()->GetControlProperties()->GetLngProperty(nWidth);;
	}
	else
	{		
		RetSize.cx = mDialogX.GetSourceForm()->GetControlProperties()->GetLngProperty(nWidth);;
		RetSize.cy = m_DockedHeight;
	}
	
	return RetSize;
}


//*****************************************************************************
// 
// Method: CDockingDialog::CalcDockedSize()
// 
// Purpose: [Calcs the size that the dock form should be]
// 
// Parameters: none
// 
// Returns:	CSize
// 
//*****************************************************************************
CSize CDockingDialog::CalcDockedSize()
{
	
	CSize RetSize;
	CRect DockedRect;
	
	// get window size and position		
	CDockingDialog::GetUsedRect(DockedRect);

	CPoint posMouse;
	GetCursorPos(&posMouse);
	
	m_DockedWidth = mDialogX.GetSourceForm()->GetControlProperties()->GetLngProperty(nWidth);
	
	/*
	int nThisHeight = mDialogX.GetSourceForm()->GetControlProperties()->GetLngProperty(nHeight);

	
	int nTitleBarHeight = ::GetSystemMetrics(SM_CYSMCAPTION);
	nThisHeight -= nTitleBarHeight;
	*/
	if (m_DockedHeight > DockedRect.Height())
	{
		RetSize.cy = m_DockedHeight;
	}
	/*else if (nThisHeight < DockedRect.Height())
	{
		RetSize.cy = nThisHeight + DockedRect.top;
		m_DockedHeight = RetSize.cy;
	}*/
	else
	{
		RetSize.cy = DockedRect.Height() + DockedRect.top;
		m_DockedHeight = RetSize.cy;
	}

	    
	RetSize.cx = m_DockedWidth;

	return RetSize;
}
//*****************************************************************************
// 
// Method: CDockingDialog::CalcFixedLayout()
// 
// Purpose: [Called when docked to query what the docked form size should be]
// 
// 
// Returns:	CSize
// 
//*****************************************************************************

CSize CDockingDialog::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{		

	if(!CDockingDialog::IsFloating())
	{
		//CSize szTest = CAdUiDockControlBar::CalcFixedLayout(bStretch, bHorz);
 
		return CDockingDialog::CalcDockedSize();
	}
	else
	{
		CSize RetSize;
				
		RetSize.cx = m_FloatingWidth;
		RetSize.cy = m_FloatingHeight;
		return RetSize;
	}
}
//*****************************************************************************
// 
// Method: CDockingDialog::GetFloatingMinSize()
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
void CDockingDialog::GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight)
{
	// by not adjusting the pnMinWidth or pnMinHeight, the min pnMinWidth will become small enough
	// that the OnResizeEvent will take care of the width size itself.
		
}

BOOL CDockingDialog::PreTranslateMessage(MSG* pMsg) 
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

void CDockingDialog::OnDestroy() 
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

void CDockingDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !mbTrackingMouse )
	{
		SetTimer( WM_MOUSELEAVE, 200, NULL );
		mbTrackingMouse = true;
	}
	__super::OnMouseMove(nFlags, point);
}

void CDockingDialog::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if( !mbTrackingMouse )
	{
		SetTimer( WM_MOUSELEAVE, 200, NULL );
		mbTrackingMouse = true;
	}
	__super::OnNcMouseMove(nHitTest, point);
}

LRESULT CDockingDialog::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	mbTrackingMouse = false;
	if( !mbInMenuLoop && !::GetCapture() )
	{
		CWnd* pFocusWnd = GetFocus();
		if( !pFocusWnd || ((pFocusWnd->SendMessage( WM_GETDLGCODE, 0, 0 ) & (DLGC_WANTCHARS | DLGC_WANTARROWS)) == 0) )
		{
			CWnd* pCmdLine = acedGetAcadDockCmdLine();
			if( pCmdLine && pCmdLine->IsWindowEnabled() && pCmdLine->IsWindowVisible() )
				pCmdLine->SetFocus();
		}
		else
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}
	return 0;
}

void CDockingDialog::OnEnterMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = true;
	__super::OnEnterMenuLoop(bPopupMenu);
}

void CDockingDialog::OnExitMenuLoop(BOOL bPopupMenu)
{
	mbInMenuLoop = false;
	__super::OnExitMenuLoop(bPopupMenu);
}

void CDockingDialog::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == WM_MOUSELEAVE )
	{
		CPoint ptCursor;
		if( GetCursorPos( &ptCursor ) )
		{
			CWnd* pTarget = WindowFromPoint( ptCursor );
			if( pTarget != this && !IsChild( pTarget ) )
			{
				PostMessage( WM_MOUSELEAVE, 0, 0 );
				KillTimer( WM_MOUSELEAVE );
			}
		}
	}
	__super::OnTimer(nIDEvent);
}
