// DockingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DockingDialog.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"

extern bool AcadIsQuitting(void);


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog dialog

CDockingDialog::CDockingDialog( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CDialog( IDD_DOCKINGDLGHOST, &mHostControlBar )
, CArxDialogObject( pSourceForm, this )
, mpParent( pParent )
, mHostControlBar( *new CAcadDockBarHost( this, pParent ) )
, mbKeepFocus( true )
, mbResizable( true )
, mbHiding( false )
{
	TDclControlPtr pProps = pSourceForm->GetControlProperties();
	TPropertyPtr pResizableProp = pProps->GetPropertyObject( Prop::AllowResizing );
	mbResizable = (!pResizableProp || pResizableProp->GetBooleanValue());
}


CDockingDialog::~CDockingDialog()
{
}

bool CDockingDialog::IsFloating() const
{
	return (const_cast< CAcadDockBarHost* >( &mHostControlBar )->IsFloating() != FALSE);
}

CWnd* CDockingDialog::GetTopLevelWnd()
{
	HWND hwndControlBar = mHostControlBar.m_hWnd;
	if( !hwndControlBar )
		return NULL;
	if( IsFloating() )
	{
		HWND hwndTopLevel = ::GetParent( ::GetParent( hwndControlBar ) );
		if( hwndTopLevel && hwndTopLevel != AfxGetMainWnd()->m_hWnd )
			return CWnd::FromHandle( hwndTopLevel );
	}
	return &mHostControlBar;
}

bool CDockingDialog::CreateModeless( UINT nID )
{
	DWORD dwDockableSides = 0;
	DWORD dwDefaultDockableSide = 0;

	switch (mpTemplate->GetLongProperty(Prop::DockableSides))
	{
	case 1:
		// set the form to only dock on the top side
		dwDockableSides = CBRS_ALIGN_TOP;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		break;
	case 2:
		// set the form to only dock on the bottom side
		dwDockableSides = CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_BOTTOM;
		break;
	case 3:
		// set the form to only dock on the top or bottom sides
		dwDockableSides = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;				
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
		break;
	case 4:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP;				
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		break;
	case 5:
		// set the form to only dock on the any side
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		break;
	default:
		// set the form to only dock on the left or right sides
		dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT;
		dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
		break;
	}

	mbIgnoreSizing = true;
  if( !mHostControlBar.Create( GetWndCaption(), GetWndRect(), nID ) )
		return false;

	if( mpSourceForm->GetUUIDAsString().IsEmpty() )
	{
		UUID uuid;
		UuidCreate( &uuid );
		mHostControlBar.SetToolID( &uuid );
	}
	else
	{
    UUID uuid = mpSourceForm->GetUUID();
		mHostControlBar.SetToolID( &uuid );
	}			
	mHostControlBar.EnableDocking( dwDockableSides );
	mHostControlBar.RestoreControlBar( dwDefaultDockableSide ); // loads the dockable form but does not display it
	AfxGetMainWnd()->GetTopLevelFrame()->ShowControlBar( &mHostControlBar, TRUE, TRUE );

	if( !CDialog::Create( IDD_DOCKINGDLGHOST, &mHostControlBar ) )
		return false;

	return true;
}

void CDockingDialog::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
	CWnd* pTopLevel = GetTopLevelWnd();
	HWND hwndTopLevel = pTopLevel? pTopLevel->m_hWnd : NULL;
	HWND hwndOwner = mHostControlBar.m_hWnd;
	mHostControlBar.EndModalLoop( nStatus ); //set the status
	if( hwndTopLevel && ::IsWindow( hwndTopLevel ) )
		::SendMessage( hwndTopLevel, WM_CLOSE, 0, 0 );
	//if( hwndOwner && ::IsWindow( hwndOwner ) )
	//	::DestroyWindow( hwndOwner );
}

bool CDockingDialog::CenterDialog()
{
	if( !IsFloating() )
		return false;
	return __super::CenterDialog();
}

bool CDockingDialog::ResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

bool CDockingDialog::CenterAndResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

bool CDockingDialog::GetEffectiveWindowRect( CRect& rcDlg ) const
{
	if (!IsFloating())
		return GetEffectiveClientRect( rcDlg );
	const_cast< CDockingDialog* >( this )->mHostControlBar.GetFloatingRect( &rcDlg );
	return true;
}

bool CDockingDialog::GetEffectiveClientRect( CRect& rcDlg ) const
{
	const_cast< CDockingDialog* >( this )->mHostControlBar.GetClientArea( rcDlg );
	return true;
}

void CDockingDialog::GetClientArea( CRect& rect )
{
	if( !IsFloating() )
	{
		mHostControlBar.GetClientArea( rect );
		rect.top += 5;
	}
	else
		mHostControlBar.GetClientRect( &rect );
}

void CDockingDialog::OnFrameChanged()
{
	CWnd* pTopLevelWnd = GetTopLevelWnd();
	CRect rectWindow;
	pTopLevelWnd->GetWindowRect( &rectWindow );
	CRect rcClient;
	GetClientArea( rcClient );
	long lNCWidth = rectWindow.Width() - rcClient.Width();
	long lNCHeight = rectWindow.Height() - rcClient.Height();
	SetNCWidth( lNCWidth );
	SetNCHeight( lNCHeight );
	assert( GetNCWidth() >= 0 && GetNCHeight() >= 0 );
	OnApplyMinMaxSize( NULL );
	ApplyPosition();
}

void CDockingDialog::ApplyPosition()
{
	if( mbIgnoreSizing )
		return;
	if( !IsFloating() )
		return; //size cannot be changed while docked
	mbIgnoreSizing = true;
	long lWidth = mpTemplate->GetLongProperty(Prop::Width);
	long lHeight = mpTemplate->GetLongProperty(Prop::Height);
	CWnd* pTopLevelWnd = GetTopLevelWnd();
	pTopLevelWnd->SetWindowPos( NULL, 0, 0,
															lWidth + GetNCWidth(),
															lHeight + GetNCHeight(),
															SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	SetWindowPos( NULL, 0, 0,
								lWidth,
								lHeight,
								SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	mpControlPane->RecalcLayout();
	mbIgnoreSizing = false;
}

bool CDockingDialog::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::KeepFocus:
		mbKeepFocus = pProp->GetBooleanValue();
		break;
	}
	return !bFailed;
}

bool CDockingDialog::OnApplyResizable( TPropertyPtr pProp )
{
	mbResizable = pProp->GetBooleanValue();
	return true; //skip base class
}

void CDockingDialog::OnMouseEnter()
{
  InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseEntered ), IsAsyncEvents() );
};

void CDockingDialog::OnMouseLeave()
{
  InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseMovedOff ), IsAsyncEvents() );
};


BEGIN_MESSAGE_MAP(CDockingDialog, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog message handlers

int CDockingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the window text
	mHostControlBar.SetWindowText(mpTemplate->GetStringProperty(Prop::TitleBarText));
	
	CWnd* pTopLevelWnd = GetTopLevelWnd();
	CRect rectWindow;
	pTopLevelWnd->GetWindowRect( &rectWindow );
	CRect rcClient;
	GetClientArea( rcClient );
	SetNCWidth( rectWindow.Width() - rcClient.Width() );
	SetNCHeight( rectWindow.Height() - rcClient.Height() );
	SetWindowPos( NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
								SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	mpTemplate->SetLongProperty( Prop::Width, rcClient.Width() );
	mpTemplate->SetLongProperty( Prop::Height, rcClient.Height() );

	ApplyPropertiesEnum();
	mbIgnoreSizing = false;
	UINT nID = 1000;
	GetControlPane()->CreateControls(nID);
	GetControlPane()->RecalcLayout();

	InvokeMethod( mpTemplate->GetStringProperty( Prop::FormEventInitialize ), false );	
	InvokeMethodIntInt( mpTemplate->GetStringProperty( Prop::FormEventSize ), 
											mpTemplate->GetLongProperty( Prop::Width ),
											mpTemplate->GetLongProperty( Prop::Height ),
											IsAsyncEvents() );	

	return 1;
}

void CDockingDialog::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if( mbIgnoreSizing )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
	InvokeMethodIntInt( mpTemplate->GetStringProperty( Prop::FormEventSize ), 
											mpTemplate->GetLongProperty( Prop::Width ), 
											mpTemplate->GetLongProperty( Prop::Height ),
											IsAsyncEvents() );	
}

BOOL CDockingDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventHelp), IsAsyncEvents());
	return TRUE;
}

void CDockingDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::FormEventShow), IsAsyncEvents());	

	mbHiding = !IsClosing() && !bShow;
	__super::OnShowWindow(bShow, nStatus);
}

void CDockingDialog::PostNcDestroy() 
{
	__super::PostNcDestroy();
	CAcadDockBarHost* pHostToDelete = NULL;
	if( !IsFloating() && !AcadIsQuitting() )
		pHostToDelete = &mHostControlBar;
	delete this;
	delete pHostToDelete;
}

BOOL CDockingDialog::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

bool CDockingDialog::OnClosing()
{
	if( IsClosing() )
		return true;
	SetClosing();
	CRect rcThis;
	GetEffectiveWindowRect( rcThis );
	InvokeMethodIntInt( mpTemplate->GetStringProperty( Prop::FormEventClose ), rcThis.left, rcThis.top, IsAsyncEvents() );	
	if( !mbHiding && !IsFloating() )
		mHostControlBar.PostMessage(WM_CLOSE); //to make sure the window gets destroyed no matter how we got here
	return true;
}

void CDockingDialog::OnDestroy() 
{
	OnClosing();
	GetControlPane()->CleanUpControls();
	__super::OnDestroy();
}

BOOL CDockingDialog::OnEraseBkgnd(CDC* pDC)
{
	if( !mColorService.IsBackgroundTransparent() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		pDC->FillSolidRect( &rcClient, mColorService.GetBackgroundColor() );
	}
	return TRUE;
	return __super::OnEraseBkgnd(pDC);
}
