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


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog dialog

IMPLEMENT_DYNAMIC(CDockingDialog, CAdUiDockControlBar)

CDockingDialog::CDockingDialog( CDclFormObject* pSourceForm )
: CAdUiDockControlBar()
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm )
, mpControl( NULL )
{
	//{{AFX_DATA_INIT(CDockingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDockingSizeAdjusted = false;
	m_bFloatingSizeAdjusted = false;
	m_bClosing = false;
	m_pDocToModReactor = NULL;
}

CDockingDialog::~CDockingDialog()
{
}



BEGIN_MESSAGE_MAP(CDockingDialog, CAdUiDockControlBar)
	//{{AFX_MSG_MAP(CDockingDialog)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog message handlers

BOOL CDockingDialog::Create(CWnd* pParent, LPCTSTR lpszTitle, CRect rect) 
{
  CString title = lpszTitle;
	CString strWndClass;
	strWndClass = AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW));	
    if (!CAdUiDockControlBar::Create (strWndClass,
									   title,
									   WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
									   rect,
                                       pParent, 
									   IDD_DIALOGBAR_UI))
	{
		return FALSE;
	}

	return TRUE;
}

int CDockingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAdUiDockControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//  setup for assigning the form it's properties
	CRect rectThis;
	
	// get the form's properties
	CDclControlObject* pFormProps = mpSourceForm->GetControlProperties();
	// set the window text
	SetWindowText(pFormProps->GetStrProperty(nTitleBarText));
	
	// setup the rect default rect 
	rectThis.top = 0;
	rectThis.left = 0;
	
	// get the width
	int nCtlWidth = pFormProps->GetLngProperty(nWidth);

	// get the height
	int nCtlHeight = pFormProps->GetLngProperty(nHeight);
	
	// set the rect for the control pane to be created
	CRect rcThis;
	GetClientRect(&rcThis);
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	if (mpSourceForm->m_bUsesClientRect == TRUE && IsFloating())
	{
		nCtlHeight += rcWnd.Height() - rcThis.Height();
		nCtlWidth += rcWnd.Width() - rcThis.Width();
	}
	rectThis.right = nCtlWidth;
	rectThis.bottom = nCtlHeight;

	// create the control pane that will display the controls
	mControlPane.m_pControlCol = &m_ControlCol;
	mControlPane.m_pFontCollection = m_pFontCollection;
	mControlPane.m_PanePos = rcThis;
	mControlPane.m_pParentDlg = this;
	mControlPane.m_bInvokeWithSendString = true;
	// call method to create the controls
	mControlPane.CreateControls(mpSourceForm, 1000);
	
	mControlPane.m_sDialogName = mpSourceForm->GetKeyName();
	mControlPane.m_sProjectName = mControlPane.GetProject()->GetKeyName();


	if (pFormProps->GetLngProperty(nMaxDialogWidth) > -1)
	{
		long lMinHeight;
		long lMinWidth;
		GetFloatingMinSize(&lMinHeight, &lMinWidth);
		lMinWidth = pFormProps->GetLngProperty(nMinDialogWidth);
	}
	
	// add the doc reactor if required for an event
	CString sEventDefun = pFormProps->GetStrProperty(nDocEventActivated);
	if (sEventDefun.GetLength() > 0)
	{
		m_pDocToModReactor = new CAcadDocReactor();
		m_pDocToModReactor->m_EventDefun = sEventDefun;
		// activate the reactor
		acDocManager->addReactor(m_pDocToModReactor);
	}	

	// call methods to invoke the event
	InvokeMethod(pFormProps->GetStrProperty(nFormEventInitialize), true);	
	
	GetWindowRect(&rcThis);
	// call methods to invoke the event
	InvokeMethodIntInt(
		pFormProps->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	

	return 1;
}

void CDockingDialog::GetClientArea(CRect &rect)
{
	GetUsedRect(rect);
}

void CDockingDialog::SetDclForm(CDclFormObject *mpSourceFormObject)
{
	// set the internal pointer to equal the object passed
	mpSourceForm = mpSourceFormObject;
}


void CDockingDialog::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	if (!m_bClosing)
	{
		lpRect->top += nDeflateRect + 2;
		lpRect->left += nDeflateRect;
		lpRect->bottom += nBottomAdjustment;

		// resize the control pane so all offsets are set correctly
		mControlPane.m_PanePos.left = lpRect->left;
		mControlPane.m_PanePos.top = lpRect->top;
		mControlPane.SizeChanged(lpRect->Width(), lpRect->Height() - nHeightOffset);
		
		// call methods to invoke the event
		InvokeMethodIntInt(
			mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventSize), 
			lpRect->Width(), 
			lpRect->Height() - nHeightOffset,
			true);	
	}
	
}

void CDockingDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CAdUiDockControlBar::OnShowWindow(bShow, nStatus);
	
	// call methods to invoke the event
	InvokeMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventShow), true);	
		
}

bool CDockingDialog::CanFrameworkTakeFocus ()
{
	// return false to tell AutoCAD not to steal this form's focus on WM_MOUSEMOVE
	return false;
}

void CDockingDialog::CleanupDockable() 
{
	if (m_pDocToModReactor != NULL)
	{
		acDocManager->removeReactor(m_pDocToModReactor);
		delete m_pDocToModReactor;
		m_pDocToModReactor = NULL;
	}
	mControlPane.CleanUpControls();
	mControlPane.m_pParentDlg = NULL;
}

void CDockingDialog::PostNcDestroy() 
{
	CleanupDockable();
	CAdUiDockControlBar::PostNcDestroy();
}

bool CDockingDialog::OnClosing()
{
	m_bClosing = true;
	// call methods to invoke the event
	InvokeMethod(mpSourceForm->GetControlProperties()->GetStrProperty(nFormEventClose), true);	
	return CAdUiDockControlBar::OnClosing();
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

	pRect->right = pRect->left + mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);
	
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

	RetSize.cx = mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);
	
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
		RetSize.cx = mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);;
	}
	else
	{		
		RetSize.cx = mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);;
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
	
	m_DockedWidth = mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);
	
	/*
	int nThisHeight = mpSourceForm->GetControlProperties()->GetLngProperty(nHeight);

	
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
	
	return CAdUiDockControlBar::PreTranslateMessage(pMsg);
}
