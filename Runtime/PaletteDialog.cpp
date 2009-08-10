// PaletteDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PaletteDialog.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PictureObject.h"

extern bool AcadIsQuitting(void);


/////////////////////////////////////////////////////////////////////////////
// CPaletteDialog dialog

CPaletteDialog::CPaletteDialog( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CDialog()
, CArxDialogObject( pSourceForm, this )
, mpParent( pParent )
, mHostPaletteSet( *new CAcadPaletteHost( this, pParent ) )
, mbKeepFocus( true )
, mbResizable( true )
{
	TDclControlPtr pProps = pSourceForm->GetControlProperties();
	TPropertyPtr pResizableProp = pProps->GetPropertyObject( Prop::AllowResizing );
	mbResizable = (!pResizableProp || pResizableProp->GetBooleanValue());
}


CPaletteDialog::~CPaletteDialog()
{
}

bool CPaletteDialog::IsFloating() const
{
	return (const_cast< CAcadPaletteHost* >( &mHostPaletteSet )->IsFloating() != FALSE);
}

CWnd* CPaletteDialog::GetTopLevelWnd()
{
	HWND hwndPaletteSet = mHostPaletteSet.m_hWnd;
	if( !hwndPaletteSet )
		return NULL;
	if( IsFloating() )
	{
		HWND hwndTopLevel = ::GetParent( ::GetParent( hwndPaletteSet ) );
		if( hwndTopLevel && hwndTopLevel != AfxGetMainWnd()->m_hWnd )
			return CWnd::FromHandle( hwndTopLevel );
	}
	return &mHostPaletteSet;
}

bool CPaletteDialog::CreateModeless( UINT nID )
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
		// set the form to only dock on the left, right, or top sides
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

	IgnoreSizing();
	if( !mHostPaletteSet.Create( GetWndCaption(), GetWndRect() ) )
		return false;

	if( mpSourceForm->GetUUIDAsString().IsEmpty() )
	{
		UUID uuid;
		UuidCreate( &uuid );
		mHostPaletteSet.SetToolID( &uuid );
	}
	else
	{
		UUID uuid = mpSourceForm->GetUUID();
		mHostPaletteSet.SetToolID( &uuid );
	}			
	mHostPaletteSet.EnableDocking( dwDockableSides );

	//SetIcon must be called before RestoreControlBar!
	OnApplyIcon( mpTemplate->GetPropertyObject( Prop::TitleBarIcon ) );

	mHostPaletteSet.RestoreControlBar( dwDefaultDockableSide ); // loads the dockable form but does not display it
	AfxGetMainWnd()->GetTopLevelFrame()->ShowControlBar( &mHostPaletteSet, TRUE, TRUE );

	if( !CDialog::Create( IDD_DOCKINGDLGHOST, &mHostPaletteSet ) )
		return false;

	return true;
}

void CPaletteDialog::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
	CWnd* pTopLevel = GetTopLevelWnd();
	HWND hwndTopLevel = pTopLevel? pTopLevel->m_hWnd : NULL;
	mHostPaletteSet.EndModalLoop( nStatus ); //set the status
	if( hwndTopLevel && ::IsWindow( hwndTopLevel ) )
		::SendMessage( hwndTopLevel, WM_CLOSE, 0, 0 );
}

bool CPaletteDialog::CenterDialog()
{
	if( !IsFloating() )
		return false;
	return __super::CenterDialog();
}

bool CPaletteDialog::MoveDialog( long nNewLeft, long nNewTop )
{
	if( !IsFloating() )
		return false;
	return __super::MoveDialog( nNewLeft, nNewTop );
}

bool CPaletteDialog::ResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

bool CPaletteDialog::CenterAndResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

CRect CPaletteDialog::GetEffectiveWindowRect() const
{
	CRect rectWindow;
	CWnd* pTopLevelWnd = const_cast< CPaletteDialog* >( this )->GetTopLevelWnd();
	if( pTopLevelWnd == &mHostPaletteSet && mHostPaletteSet.RolledUp() )
		mHostPaletteSet.GetFullRect( rectWindow );
	else
		pTopLevelWnd->GetWindowRect( &rectWindow );
	return rectWindow;
}

CRect CPaletteDialog::GetEffectiveClientRect() const
{
	CRect rcDlg;
	const_cast< CPaletteDialog* >( this )->mHostPaletteSet.GetClientArea( rcDlg );
	return rcDlg;
}

void CPaletteDialog::OnFrameChanged()
{
	CRect rectWindow = GetEffectiveWindowRect();
	CRect rcClient = GetEffectiveClientRect();
	long lNCWidth = rectWindow.Width() - rcClient.Width();
	long lNCHeight = rectWindow.Height() - rcClient.Height();
	SetNCWidth( lNCWidth );
	SetNCHeight( lNCHeight );
	assert( GetNCWidth() >= 0 && GetNCHeight() >= 0 );
	OnApplyMinMaxSize( NULL );
	//ApplyPosition();
}

void CPaletteDialog::ApplyPosition()
{
	if( IsIgnoreSizing() )
		return;
	long lWidth = mpTemplate->GetLongProperty(Prop::Width);
	long lHeight = mpTemplate->GetLongProperty(Prop::Height);
	CWnd* pTopLevelWnd = GetTopLevelWnd();
	pTopLevelWnd->SetWindowPos( NULL, 0, 0,
															lWidth + GetNCWidth(),
															lHeight + GetNCHeight(),
															SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	CRect rcClient = GetEffectiveClientRect();
	SetWindowPos( NULL, rcClient.left, rcClient.top, lWidth, lHeight,
								SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	mpControlPane->RecalcLayout();
}

bool CPaletteDialog::OnApplyProperty( TPropertyPtr pProp )
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

bool CPaletteDialog::OnApplyCaption( TPropertyPtr pProp )
{
	mHostPaletteSet.SetWindowText( pProp->GetStringValue() );
	OnNeedRepaint();
	return true;
}

bool CPaletteDialog::OnApplyResizable( TPropertyPtr pProp )
{
	mbResizable = pProp->GetBooleanValue();
	bool bIgnoreSizing = IgnoreSizing();
	if( mbResizable && IsFloating() )
		GetTopLevelWnd()->ModifyStyle( 0, WS_THICKFRAME, SWP_FRAMECHANGED );
	else
		GetTopLevelWnd()->ModifyStyle( WS_THICKFRAME, 0, SWP_FRAMECHANGED );
	IgnoreSizing( bIgnoreSizing );
	OnFrameChanged();
	return true;
}

bool CPaletteDialog::OnApplyIcon( TPropertyPtr pProp )
{
	DestroyIcon( mHostPaletteSet.SetIcon( NULL, FALSE ) );
	TPicturePtr pPicture = mpSourceForm->GetProject()->FindPicture( pProp->GetLongValue() );
	if( pPicture )
		mHostPaletteSet.SetIcon( pPicture->CloneIcon(), FALSE );
	return true;
}

void CPaletteDialog::OnMouseEnter()
{
  InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseEntered ), IsAsyncEvents() );
};

void CPaletteDialog::OnMouseLeave()
{
  InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseMovedOff ), IsAsyncEvents() );
};


BEGIN_MESSAGE_MAP(CPaletteDialog, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPaletteDialog message handlers

int CPaletteDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rcWindow = GetEffectiveWindowRect();
	CRect rcClient = GetEffectiveClientRect();
	SetNCWidth( rcWindow.Width() - rcClient.Width() );
	SetNCHeight( rcWindow.Height() - rcClient.Height() );
	if( IsResizable() )
	{
		SetWindowPos( NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
									SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
		mpTemplate->SetLongProperty( Prop::Width, rcClient.Width() );
		mpTemplate->SetLongProperty( Prop::Height, rcClient.Height() );
	}
	else
		IgnoreSizing( false );

	ApplyPropertiesEnum();
	IgnoreSizing( false );
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

void CPaletteDialog::PostNcDestroy() 
{
	__super::PostNcDestroy();
	CAcadPaletteHost* pHostToDelete = NULL;
	if( !IsFloating() && !AcadIsQuitting() )
		pHostToDelete = &mHostPaletteSet;
	delete this;
	delete pHostToDelete;
}

void CPaletteDialog::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() || !IsResizable() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
	InvokeMethodIntInt( mpTemplate->GetStringProperty( Prop::FormEventSize ), 
											mpTemplate->GetLongProperty( Prop::Width ), 
											mpTemplate->GetLongProperty( Prop::Height ),
											IsAsyncEvents() );	
}

BOOL CPaletteDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventHelp), IsAsyncEvents());
	return TRUE;
}

void CPaletteDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::FormEventShow), IsAsyncEvents());	

	//mbHiding = !IsClosing() && !bShow;
	__super::OnShowWindow(bShow, nStatus);
}

BOOL CPaletteDialog::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

bool CPaletteDialog::OnClosing()
{
	if( IsClosing() )
		return true;
	SetClosing();
	CRect rcThis = GetEffectiveWindowRect();
	InvokeMethodIntInt( mpTemplate->GetStringProperty( Prop::FormEventClose ), rcThis.left, rcThis.top, IsAsyncEvents() );	
	if( /*!mbHiding && */!IsFloating() )
		mHostPaletteSet.PostMessage(WM_CLOSE); //to make sure the window gets destroyed no matter how we got here
	return true;
}

void CPaletteDialog::OnDestroy() 
{
	OnClosing();
	GetControlPane()->CleanUpControls();
	__super::OnDestroy();
}

BOOL CPaletteDialog::OnEraseBkgnd(CDC* pDC)
{
	if( !mColorService.IsBackgroundTransparent() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		pDC->FillSolidRect( &rcClient, mColorService.GetBackgroundColor() );
		return TRUE;
	}
	return __super::OnEraseBkgnd(pDC);
}
