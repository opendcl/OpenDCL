// ControlBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ControlBarDlg.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "Resource.h"

#if (defined(_BRXTARGET) || defined(_ZRXTARGET))
static bool AcadIsQuitting() { return false; }
#else
extern bool AcadIsQuitting(void);
#endif


/////////////////////////////////////////////////////////////////////////////
// CControlBarDlg dialog

CControlBarDlg::CControlBarDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CDialog()
, CArxDialogObject( pSourceForm, this )
, mpParent( pParent )
, mHostControlBar( *new CAcadDockBarHost( this, pParent ) )
, mptInitPos( -1, -1 )
, mbKeepFocus( true )
, mbResizable( true )
, mbHiding( false )
{
	TDclControlPtr pProps = pSourceForm->GetControlProperties();
	TPropertyPtr pResizableProp = pProps->GetPropertyObject( Prop::AllowResizing );
	mbResizable = (!pResizableProp || pResizableProp->GetBooleanValue());
	if( pParams )
		mptInitPos = pParams->position;
}


CControlBarDlg::~CControlBarDlg()
{
}

bool CControlBarDlg::IsFloating() const
{
	return (const_cast< CAcadDockBarHost* >( &mHostControlBar )->IsFloating() != FALSE);
}

CWnd* CControlBarDlg::GetTopLevelWnd()
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

bool CControlBarDlg::CreateModeless( UINT nID )
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

	IgnoreSizing();
	OnApplyMinMaxSize( NULL );
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
	if( mptInitPos.x >= 0 && mptInitPos.y >= 0 )
	{
		if( !IsFloating() )
		{
			if( mHostControlBar.m_pDockContext )
				mHostControlBar.m_pDockContext->ToggleDocking();
		}
		MoveDialog( mptInitPos.x, mptInitPos.y );
	}
	AfxGetMainWnd()->GetTopLevelFrame()->ShowControlBar( &mHostControlBar, TRUE, TRUE );

	if( !CDialog::Create( IDD_DOCKINGDLGHOST, &mHostControlBar ) )
		return false;

	return true;
}

void CControlBarDlg::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
	CWnd* pTopLevel = GetTopLevelWnd();
	HWND hwndTopLevel = pTopLevel? pTopLevel->m_hWnd : NULL;
	HWND hwndOwner = mHostControlBar.m_hWnd;
	mHostControlBar.EndModalLoop( nStatus ); //set the status
	if( hwndTopLevel && ::IsWindow( hwndTopLevel ) )
		::SendMessage( hwndTopLevel, WM_CLOSE, 0, 0 );
}

bool CControlBarDlg::CenterDialog()
{
	if( !IsFloating() )
		return false;
	return __super::CenterDialog();
}

bool CControlBarDlg::MoveDialog( long nNewLeft, long nNewTop )
{
	if( !IsFloating() )
		return false;
	return __super::MoveDialog( nNewLeft, nNewTop );
}

bool CControlBarDlg::ResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

bool CControlBarDlg::CenterAndResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

CRect CControlBarDlg::GetEffectiveWindowRect() const
{
	CWnd* pTop = const_cast< CControlBarDlg* >( this )->GetTopLevelWnd();
	CRect rcWnd;
	pTop->GetWindowRect( &rcWnd );
	return rcWnd;
	//CRect rcDlg;
	//const_cast< CControlBarDlg* >( this )->mHostControlBar.GetFloatingRect( &rcDlg );
	//return rcDlg;
}

CRect CControlBarDlg::GetEffectiveClientRect() const
{
	CRect rcDlg;
	const_cast< CControlBarDlg* >( this )->mHostControlBar.GetClientArea( rcDlg );
	return rcDlg;
}

void CControlBarDlg::GetClientArea( CRect& rect )
{
	rect = GetEffectiveClientRect();
}

void CControlBarDlg::OnFrameChanged()
{
	CRect rectWindow = GetEffectiveWindowRect();
	CRect rcClient = GetEffectiveClientRect();
	long lNCWidth = rectWindow.Width() - rcClient.Width();
	long lNCHeight = rectWindow.Height() - rcClient.Height();
	SetNCWidth( lNCWidth );
	SetNCHeight( lNCHeight );
	assert( GetNCWidth() >= 0 && GetNCHeight() >= 0 );
	OnApplyMinMaxSize( NULL );
	SetWindowPos( NULL, rcClient.left, rcClient.top, 0, 0,
								SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
}

void CControlBarDlg::ApplyPosition()
{
	if( IsEnumeratingProperties() )
		return; //defer
	if( IsIgnoreSizing() )
		return;
	//if( !IsFloating() )
	//	return;
	long lWidth = mpTemplate->GetLongProperty(Prop::Width);
	long lHeight = mpTemplate->GetLongProperty(Prop::Height);
	CWnd* pTopLevelWnd = GetTopLevelWnd();
	//TODO: in Bricscad, the top level window refuses to resize because CalcDynamicLayout
	//gets called when it shouldn't, and overrides the desired size
	pTopLevelWnd->SetWindowPos( NULL, 0, 0,
															lWidth + GetNCWidth(),
															lHeight + GetNCHeight(),
															SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	SetWindowPos( NULL, 0, 0,
								lWidth,
								lHeight,
								SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	mpControlPane->RecalcLayout();
}

bool CControlBarDlg::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
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

bool CControlBarDlg::OnApplyResizable( TPropertyPtr pProp )
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

void CControlBarDlg::OnMouseEnter()
{
	GetArxServices()->HandleEvent( Prop::EventMouseEntered );
};

void CControlBarDlg::OnMouseLeave()
{
	GetArxServices()->HandleEvent( Prop::EventMouseMovedOff );
};


BEGIN_MESSAGE_MAP(CControlBarDlg, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CControlBarDlg message handlers

int CControlBarDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the window text
	mHostControlBar.SetWindowText(mpTemplate->GetStringProperty(Prop::TitleBarText));
	
	CRect rcWindow = GetEffectiveWindowRect();
	CRect rcClient = GetEffectiveClientRect();
	SetNCWidth( rcWindow.Width() - rcClient.Width() );
	SetNCHeight( rcWindow.Height() - rcClient.Height() );
//#if (defined(_BRXTARGET) && _BRXTARGET <= 10)
//	CRect rcWnd = GetWndRect();
//	LONG lWidth = rcWnd.Width() + GetNCWidth();
//	LONG lHeight = rcWnd.Height() + GetNCHeight();
//	mHostControlBar.SetWindowPos( NULL, 0, 0, lWidth, lHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
//	rcWindow = GetEffectiveWindowRect();
//	rcClient = GetEffectiveClientRect();
//#endif
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

	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );	
	GetArxServices()->HandleEvent( Prop::FormEventMove,
																 args_NN( rcWindow.left, rcWindow.top ) );
	GetArxServices()->HandleEvent( Prop::FormEventSize,
																 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																					mpTemplate->GetLongProperty( Prop::Height ) ) );

	return 1;
}

void CControlBarDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if( IsIgnoreSizing() )
		return;
	GetArxServices()->HandleEvent( Prop::FormEventMove, args_NN( x, y ) );
}

void CControlBarDlg::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() || !IsResizable() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
	GetArxServices()->HandleEvent( Prop::FormEventSize,
																 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																					mpTemplate->GetLongProperty( Prop::Height ) ) );
}

BOOL CControlBarDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	GetArxServices()->HandleEvent( Prop::EventHelp );
	return TRUE;
}

void CControlBarDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	GetArxServices()->HandleEvent( Prop::FormEventShow );	

	mbHiding = !IsClosing() && !bShow;
	__super::OnShowWindow(bShow, nStatus);
}

void CControlBarDlg::PostNcDestroy() 
{
	__super::PostNcDestroy();
	CAcadDockBarHost* pHostToDelete = NULL;
	if( !IsFloating() && !AcadIsQuitting() )
		pHostToDelete = &mHostControlBar;
	delete this;
	delete pHostToDelete;
}

BOOL CControlBarDlg::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

bool CControlBarDlg::OnClosing()
{
	if( IsClosing() )
		return true;
	SetClosing();
	CRect rcThis = GetEffectiveWindowRect();
	GetArxServices()->HandleEvent( Prop::FormEventClose, args_NN( rcThis.left, rcThis.top ) );
	if( !mbHiding && !IsFloating() )
		mHostControlBar.PostMessage(WM_CLOSE); //to make sure the window gets destroyed no matter how we got here
	return true;
}

void CControlBarDlg::OnDestroy() 
{
	OnClosing();
	GetControlPane()->CleanUpControls();
	__super::OnDestroy();
}

BOOL CControlBarDlg::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

LRESULT CControlBarDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}
