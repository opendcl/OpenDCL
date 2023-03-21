// PaletteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PaletteDlg.h"
#include "DclFormTemplate.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ArxProject.h"
#include "ArxDialogControl.h"
#include "DclControlTemplate.h"
#include "DclPicture.h"
#include "Resource.h"

#if defined(_BRXTARGET) || defined(_ZRXTARGET) || defined(_GRXTARGET)
static bool AcadIsQuitting() { return false; }
#else
extern bool AcadIsQuitting(void);
#endif


/////////////////////////////////////////////////////////////////////////////
// CPaletteDlg dialog

CPaletteDlg::CPaletteDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CDialog()
, CArxDialogObject( pSourceForm, this )
, mpParent( pParent )
, mHostPaletteSet( *new CAcadPaletteHost( this, pParent ) )
, mptInitPos( -1, -1 )
, mbKeepFocus( true )
, mbResizable( true )
{
	TDclControlPtr pProps = pSourceForm->GetControlProperties();
	TPropertyPtr pResizableProp = pProps->GetPropertyObject( Prop::AllowResizing );
	mbResizable = (!pResizableProp || pResizableProp->GetBooleanValue());
	if( pParams )
		mptInitPos = pParams->position;
}


CPaletteDlg::~CPaletteDlg()
{
}

bool CPaletteDlg::IsFloating() const
{
	return (const_cast< CAcadPaletteHost* >( &mHostPaletteSet )->IsFloating() != FALSE);
}

CWnd* CPaletteDlg::GetTopLevelWnd()
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

bool CPaletteDlg::CreateModeless( UINT nID )
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
	OnApplyMinMaxSize( NULL );
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
#if defined(_GRXTARGET) && (_GRXTARGET <= 2022)
	if (mptInitPos.x >= 0 && mptInitPos.y >= 0)
	{
		if (!IsFloating())
			mHostPaletteSet.UndockPane();
		MoveDialog( mptInitPos.x, mptInitPos.y );
	}
	mHostPaletteSet.ShowPane(TRUE, TRUE, FALSE);
#else
	if( mptInitPos.x >= 0 && mptInitPos.y >= 0 )
	{
		if( !IsFloating() )
		{
			if( mHostPaletteSet.m_pDockContext )
				mHostPaletteSet.m_pDockContext->ToggleDocking();
		}
		MoveDialog( mptInitPos.x, mptInitPos.y );
	}
	AfxGetMainWnd()->GetTopLevelFrame()->ShowControlBar( &mHostPaletteSet, TRUE, TRUE );
#endif

	if( !CDialog::Create( IDD_DOCKINGDLGHOST, &mHostPaletteSet ) )
		return false;

	return true;
}

void CPaletteDlg::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return;
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	HWND hwndHost = mHostPaletteSet.m_hWnd;
#endif
	CWnd* pTopLevel = GetTopLevelWnd();
	HWND hwndTopLevel = pTopLevel? pTopLevel->m_hWnd : NULL;
	mHostPaletteSet.EndModalLoop( nStatus ); //set the status
	if( hwndTopLevel && ::IsWindow( hwndTopLevel ) )
		::SendMessage( hwndTopLevel, WM_CLOSE, 0, 0 );
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	if( hwndTopLevel != hwndHost )
		::SendMessage( hwndHost, WM_CLOSE, 0, 0 ); //Bricscad only: the floating palette set window must be closed explicitly
#endif
}

bool CPaletteDlg::CenterDialog()
{
	if( !IsFloating() )
		return false;
	return __super::CenterDialog();
}

bool CPaletteDlg::MoveDialog( long nNewLeft, long nNewTop )
{
	if( !IsFloating() )
		return false;
	return __super::MoveDialog( nNewLeft, nNewTop );
}

bool CPaletteDlg::ResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

bool CPaletteDlg::CenterAndResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsFloating() )
		return false;
	return __super::ResizeDialog( nNewWidth, nNewHeight );
}

CRect CPaletteDlg::GetEffectiveWindowRect() const
{
	CRect rectWindow;
	CWnd* pTopLevelWnd = const_cast< CPaletteDlg* >( this )->GetTopLevelWnd();
	if( pTopLevelWnd == &mHostPaletteSet && mHostPaletteSet.RolledUp() )
		mHostPaletteSet.GetFullRect( rectWindow );
	else
		pTopLevelWnd->GetWindowRect( &rectWindow );
	return rectWindow;
}

CRect CPaletteDlg::GetEffectiveClientRect() const
{
	CRect rcDlg;
	const_cast< CPaletteDlg* >( this )->mHostPaletteSet.GetClientArea( rcDlg );
	return rcDlg;
}

void CPaletteDlg::OnFrameChanged()
{
	CRect rcClient = GetEffectiveClientRect();
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	long lNCWidth = IsFloating()? 0 : 9;
	long lNCHeight = IsFloating()? 0 : 16;
#else
	CRect rectWindow = GetEffectiveWindowRect();
	long lNCWidth = rectWindow.Width() - rcClient.Width();
	long lNCHeight = rectWindow.Height() - rcClient.Height();
#endif
	SetNCWidth( lNCWidth );
	SetNCHeight( lNCHeight );
	assert( GetNCWidth() >= 0 && GetNCHeight() >= 0 );
	OnApplyMinMaxSize( NULL );
	SetWindowPos( NULL, rcClient.left, rcClient.top, 0, 0,
								SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
}

void CPaletteDlg::ApplyPosition()
{
	if( IsEnumeratingProperties() )
		return; //defer
	if( IsIgnoreSizing() )
		return;
	//if( !IsFloating() )
	//	return;
	long lWidth = FromDIP( mpTemplate->GetLongProperty( Prop::Width ) );
	long lHeight = FromDIP( mpTemplate->GetLongProperty( Prop::Height ) );
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

bool CPaletteDlg::ApplyProperty( TPropertyPtr pProp )
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

bool CPaletteDlg::OnApplyCaption( TPropertyPtr pProp )
{
	CString sCaption = pProp->GetStringValue();
	mHostPaletteSet.SetWindowText( sCaption );
	mHostPaletteSet.SetName( sCaption );
	OnNeedRepaint();
	return true;
}

bool CPaletteDlg::OnApplyResizable( TPropertyPtr pProp )
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

bool CPaletteDlg::OnApplyIcon( TPropertyPtr pProp )
{
	DestroyIcon( mHostPaletteSet.SetIcon( NULL, FALSE ) );
	TPicturePtr pPicture = mpSourceForm->GetProject()->FindPicture( pProp->GetLongValue() );
	if( pPicture )
		mHostPaletteSet.SetIcon( pPicture->CloneIcon(), FALSE );
	return true;
}

BOOL CPaletteDlg::HandleEraseBkgnd( CDC* pDC )
{
	BOOL bResult = __super::HandleEraseBkgnd( pDC );
	if( !bResult )
	{
		CAcadColorService* pColorService = GetColorService();
		if( pColorService && pColorService->IsBackgroundTransparent() )
		{
			CWnd* pParent = mpControlWnd->GetParent();
			if( pParent )
			{
				CBrush* pbrBackground = pDC->GetCurrentBrush();
			#if defined(_BRXTARGET)
				int nDCInfo = pDC->SaveDC();
				HBRUSH hbrBackground = (HBRUSH)pParent->SendMessage( WM_CTLCOLORDLG, (WPARAM)pDC, (LPARAM)pParent->m_hWnd );
				pDC->RestoreDC(nDCInfo);
				if( hbrBackground )
					pbrBackground = CBrush::FromHandle( hbrBackground );
			#endif //_BRXTARGET
				CRect rcClip;
				pDC->GetClipBox( &rcClip );
				pDC->FillRect( &rcClip, pbrBackground );
			}
			return TRUE;
		}
	}
	return bResult;
}

void CPaletteDlg::OnMouseEnter()
{
	GetArxServices()->HandleEvent( Prop::EventMouseEntered );
};

void CPaletteDlg::OnMouseLeave()
{
	GetArxServices()->HandleEvent( Prop::EventMouseMovedOff );
};


BEGIN_MESSAGE_MAP(CPaletteDlg, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CPaletteDlg::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPaletteDlg message handlers

LRESULT CPaletteDlg::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

int CPaletteDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
		mpTemplate->SetLongProperty( Prop::Width, ToDIP( rcClient.Width() ) );
		mpTemplate->SetLongProperty( Prop::Height, ToDIP( rcClient.Height() ) );
	}
	else
	{
		IgnoreSizing( false );
		SetWindowPos( NULL, rcClient.left, rcClient.top,
									mpTemplate->GetLongProperty( Prop::Width ), mpTemplate->GetLongProperty( Prop::Height ),
									SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	}

	ApplyPropertiesEnum();
	IgnoreSizing( false );
	UINT nID = 1000;
	GetControlPane()->CreateControls(nID);
	GetControlPane()->RecalcLayout();

	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );
	GetArxServices()->HandleEvent( Prop::FormEventMove,
																 args_NN( ToDIP( rcWindow.left ), ToDIP( rcWindow.top ) ) );
	GetArxServices()->HandleEvent( Prop::FormEventSize,
																 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																					mpTemplate->GetLongProperty( Prop::Height ) ) );

	return 1;
}

void CPaletteDlg::PostNcDestroy()
{
	__super::PostNcDestroy();
	CAcadPaletteHost* pHostToDelete = NULL;
	if( !IsFloating() && !AcadIsQuitting() )
		pHostToDelete = &mHostPaletteSet;
	delete this;
	delete pHostToDelete;
}

void CPaletteDlg::OnTimer( UINT_PTR nID )
{
	switch( nID )
	{
	case CDialogObject::idUserTimer:
		StartTimer( (UINT)-1 );
		GetArxServices()->HandleEvent( Prop::FormEventTimer );
		break;
	default:
		__super::OnTimer( nID );
		break;
	};
}

void CPaletteDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if( IsIgnoreSizing() )
		return;
	GetArxServices()->HandleEvent( Prop::FormEventMove, args_NN( ToDIP( x ), ToDIP( y ) ) );
}

void CPaletteDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() || !IsResizable() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, ToDIP( cx ) );
	mpTemplate->SetLongProperty( Prop::Height, ToDIP( cy ) );
	mpControlPane->RecalcLayout();
	GetArxServices()->HandleEvent( Prop::FormEventSize,
																 args_NN( mpTemplate->GetLongProperty( Prop::Width ),
																					mpTemplate->GetLongProperty( Prop::Height ) ) );
}

BOOL CPaletteDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	GetArxServices()->HandleEvent( Prop::EventHelp );
	return TRUE;
}

void CPaletteDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if( GetArxServices()->HandleEvent( Prop::FormEventShow ) )
		return;

	//mbHiding = !IsClosing() && !bShow;
#if !defined(_GRXTARGET) || (_GRXTARGET >= 2023)
	mHostPaletteSet.DelayShow(bShow);
#endif
	__super::OnShowWindow(bShow, nStatus);
#if !defined(_GRXTARGET) || (_GRXTARGET >= 2023)
	mHostPaletteSet.m_pDockSite->DelayRecalcLayout();
#endif
}

BOOL CPaletteDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg);
}

bool CPaletteDlg::OnClosing()
{
	if( IsClosing() )
		return true;
	SetClosing();
	CRect rcThis = GetEffectiveWindowRect();
	ToDIP( rcThis.TopLeft() );
	GetArxServices()->HandleEvent( Prop::FormEventClose, args_NN( rcThis.left, rcThis.top ) );
	if( /*!mbHiding && */!IsFloating() )
		mHostPaletteSet.PostMessage(WM_CLOSE); //to make sure the window gets destroyed no matter how we got here
	return true;
}

void CPaletteDlg::OnDestroy()
{
	OnClosing();
	GetControlPane()->CleanUpControls();
	__super::OnDestroy();
}

BOOL CPaletteDlg::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

LRESULT CPaletteDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}
