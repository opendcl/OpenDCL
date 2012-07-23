// FontToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "FontToolBar.h"
#include "StudioFrame.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "StudioDialogControl.h"
#include "OleFont.h"
#include "AxContainerCtrl.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ToolboxPane.h"
#include "StudioWorkspace.h"
#include "Project.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CFontToolbar dialog

CFontToolbar::CFontToolbar( CStudioFrame* pStudioFrame )
	: CDialogBar()
	, mpStudioFrame( pStudioFrame )
	, mpTBButtons( NULL )
{
	CString sDefaultFont = theWorkspace.GetDefaultFontName();
	if( !mFont.CreatePointFont( 80, theStudioWorkspace.GetDefaultFontName() ) )
		mFont.CreateStockObject( DEFAULT_GUI_FONT );
}


void CFontToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFontToolbar, CDialogBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_CONTROL(CBN_SELCHANGE, ID_FONTFACE, OnFontName)
	ON_CONTROL(CBN_SELCHANGE, IDC_FONTSIZE, OnFontSize)
	ON_COMMAND(ID_FONTBOLDBTN, OnFontBold)
	ON_COMMAND(ID_FONTITALICBTN, OnFontItalic)
	ON_COMMAND(ID_FONTUNDERLINEBTN, OnFontUnderline)
	ON_COMMAND(ID_FONTSCALED, OnFontScaled)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_FONTBOLDBTN, ID_FONTSCALED, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_FONTBOLDBTN, ID_FONTSCALED, OnNeedTextW)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontToolbar message handlers

int CFontToolbar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
		| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!m_Buttons.Create( dwStyle, CRect (0,2,317,26), this, IDR_FONTBTNS) )
		return -1;

	if( !m_FontNames.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL |
													CBS_OWNERDRAWFIXED | CBS_SORT | CBS_HASSTRINGS | 
													WS_VSCROLL | WS_TABSTOP,
													CRect(2,1,183,175), &m_Buttons, ID_FONTFACE) )
		return -1;
	m_FontNames.SetFont( &mFont );

	if( !m_FontSizes.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_AUTOHSCROLL | 
													WS_TABSTOP | WS_VSCROLL,
													CRect(184,1,225,155),&m_Buttons, IDC_FONTSIZE) )
		return -1;
	m_FontSizes.SetFont( &mFont );

	// Fill Size combobox with "common" sizes
	TCHAR* rszDefaults[] =
		{ _T("8"), _T("9"), _T("10"), _T("11"), _T("12"), _T("14"),
			_T("16"), _T("18"), _T("20"), _T("22"), _T("24"), _T("26"), 
			_T("28"), _T("36"), _T("48") };
	for( int idx = 0; idx < (sizeof(rszDefaults)/sizeof(rszDefaults[0])); ++idx )
		m_FontSizes.AddString( rszDefaults[idx] );
	
	// add the buttons to the toolbar
	AddTheButtons();
	m_Buttons.AutoSize();

	ClearFont();
	return 0;
}

void CFontToolbar::AddTheButtons()
{
	static const int ctButton = 14;

	m_Buttons.AddBitmap(ctButton,IDR_FONTBTNS);
	m_Buttons.SetBitmapSize( CSize(16, 15) );
	mpTBButtons = new TBBUTTON[ctButton];

	for( int i = 0; i < 10; i++)
	{
		mpTBButtons[i].fsState = NULL;
		mpTBButtons[i].fsStyle = BYTE(TBSTYLE_BUTTON);
		mpTBButtons[i].dwData = 0;
		mpTBButtons[i].iBitmap = 4;
		mpTBButtons[i].idCommand = 0;
		mpTBButtons[i].iString = 0;		
		VERIFY(m_Buttons.AddButtons( 1, &mpTBButtons[i] ));		
	}
	
	mpTBButtons[10].fsState = TBSTATE_ENABLED;
	mpTBButtons[10].fsStyle = BYTE(TBSTYLE_CHECK);
	mpTBButtons[10].dwData = 0;
	mpTBButtons[10].iBitmap = 0;
	mpTBButtons[10].idCommand = ID_FONTBOLDBTN;
	mpTBButtons[10].iString = ID_FONTBOLDBTN;		
	VERIFY(m_Buttons.AddButtons( 1, &mpTBButtons[10] ));		


	mpTBButtons[11].fsState = TBSTATE_ENABLED;
	mpTBButtons[11].fsStyle = BYTE(TBSTYLE_CHECK);
	mpTBButtons[11].dwData = 0;
	mpTBButtons[11].iBitmap = 1;
	mpTBButtons[11].idCommand = ID_FONTITALICBTN;
	mpTBButtons[11].iString = ID_FONTITALICBTN;		
	VERIFY(m_Buttons.AddButtons( 1, &mpTBButtons[11] ));		

	mpTBButtons[12].fsState = TBSTATE_ENABLED;
	mpTBButtons[12].fsStyle = BYTE(TBSTYLE_CHECK);
	mpTBButtons[12].dwData = 0;
	mpTBButtons[12].iBitmap = 2;
	mpTBButtons[12].idCommand = ID_FONTUNDERLINEBTN;
	mpTBButtons[12].iString = ID_FONTUNDERLINEBTN;			
	VERIFY(m_Buttons.AddButtons( 1, &mpTBButtons[12] ));		
	
	mpTBButtons[13].fsState = TBSTATE_ENABLED;
	mpTBButtons[13].fsStyle = BYTE(TBSTYLE_CHECK);
	mpTBButtons[13].dwData = 0;
	mpTBButtons[13].iBitmap = 3;
	mpTBButtons[13].idCommand = ID_FONTSCALED;
	mpTBButtons[13].iString = ID_FONTSCALED;		
	VERIFY(m_Buttons.AddButtons( 1, &mpTBButtons[13] ));
}

void CFontToolbar::OnActivateDclControl( TDclControlPtr pDclControl )
{
	if( !pDclControl )
	{
		if( !m_hWnd )
			return; //shutting down
		mControls.clear();
		ClearFont();
		return;
	}
	bool bAlreadyActive = false;
	for( std::vector< TDclControlPtr >::const_iterator iterControl = mControls.begin();
		 iterControl != mControls.end();
		 ++iterControl )
	{
		if( (*iterControl) == pDclControl && iterControl != mControls.end() )
		{ //the control is already in the list
			bAlreadyActive = true;
			break;
		}
	}
	if( !bAlreadyActive )
		mControls.push_back( pDclControl );
	if( mControls.size() == 1 )
		SetFont( FontSettings( pDclControl ) );
	else
		AddFont( FontSettings( pDclControl ) );
}

void CFontToolbar::SetFont( const FontSettings& FS )
{
	if( !FS )
	{
		ClearFont();
		return;
	}
	m_Buttons.EnableWindow( TRUE );
	mSettings = FS;
	m_Buttons.SetState( ID_FONTBOLDBTN, (FS.isBold()? TBSTATE_CHECKED | TBSTATE_ENABLED : TBSTATE_ENABLED) );
	m_Buttons.SetState( ID_FONTITALICBTN, (FS.isItalic()? TBSTATE_CHECKED | TBSTATE_ENABLED : TBSTATE_ENABLED) );
	m_Buttons.SetState( ID_FONTUNDERLINEBTN, (FS.isUnderlined()? TBSTATE_CHECKED | TBSTATE_ENABLED : TBSTATE_ENABLED) );
	m_Buttons.SetState( ID_FONTSCALED, (FS.size() >= 0? TBSTATE_CHECKED | TBSTATE_ENABLED : TBSTATE_ENABLED) );

	m_FontNames.SelectString( -1, FS.name() );
	m_FontSizes.SelectString( -1, FS.absoluteSize() );
	m_FontSizes.SetWindowText( FS.absoluteSize() );
}

void CFontToolbar::AddFont( const FontSettings& FS )
{
	if( !FS )
		return;
	if( !mSettings )
		SetFont( FS );

	m_Buttons.EnableWindow( TRUE );
	if( mSettings.isBold() != FS.isBold() )
		m_Buttons.SetState( ID_FONTBOLDBTN, TBSTATE_INDETERMINATE | TBSTATE_ENABLED );
	if( mSettings.isItalic() != FS.isItalic() )
		m_Buttons.SetState( ID_FONTITALICBTN, TBSTATE_INDETERMINATE | TBSTATE_ENABLED );
	if( mSettings.isUnderlined() != FS.isUnderlined() )
		m_Buttons.SetState( ID_FONTUNDERLINEBTN, TBSTATE_INDETERMINATE | TBSTATE_ENABLED );
	if( mSettings.isScaled() != FS.isScaled() )
		m_Buttons.SetState( ID_FONTSCALED, TBSTATE_INDETERMINATE | TBSTATE_ENABLED );

	if( lstrcmp( mSettings.name(), FS.name() ) != 0 )
		m_FontNames.SetCurSel( -1 );

	if( mSettings.size() != FS.size() )
	{
		m_FontSizes.SetCurSel( -1 );
		m_FontSizes.SetWindowText( _T("") );
	}
}

void CFontToolbar::ClearFont() 
{
	m_Buttons.EnableWindow( FALSE );
	mSettings.clear();
	m_FontNames.SetCurSel( -1 );
	m_FontNames.SetWindowText( _T("") );
	m_FontSizes.SetCurSel( -1 );
	m_FontSizes.SetWindowText( _T("") );
	m_Buttons.SetState( ID_FONTBOLDBTN, TBSTATE_INDETERMINATE );
	m_Buttons.SetState( ID_FONTITALICBTN, TBSTATE_INDETERMINATE );
	m_Buttons.SetState( ID_FONTUNDERLINEBTN, TBSTATE_INDETERMINATE );
	m_Buttons.SetState( ID_FONTSCALED, TBSTATE_INDETERMINATE );
}

void CFontToolbar::OnDestroy() 
{
	CDialogBar::OnDestroy();
	// delete the button info holders
	delete[] mpTBButtons;
}

void CFontToolbar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	m_Buttons.MoveWindow(0,2,cx,cy,TRUE);
}

CString CFontToolbar::GetTooltipText( UINT nID )
{
	CString toolTipText;
	switch (nID)
	{
	case ID_FONTBOLDBTN:
		toolTipText = theWorkspace.LoadResourceString(IDS_BOLD);
		break;
	case ID_FONTITALICBTN:
		toolTipText = theWorkspace.LoadResourceString(IDS_ITALIC);
		break;
	case ID_FONTUNDERLINEBTN:
		toolTipText = theWorkspace.LoadResourceString(IDS_UNDERLINE);
		break;
	case ID_FONTSCALED:
		toolTipText = theWorkspace.LoadResourceString(IDS_FONTSCALED);
		break;		
	}
	return toolTipText;
}


/////////////////////////////////////////////////////////////////////////////
// CToolBar message handlers

void CFontToolbar::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CStringW toolTipText = CStringW( GetTooltipText(nID) );
	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;
	lstrcpynW(lpTTT->szText, toolTipText, _elements(lpTTT->szText));
}

void CFontToolbar::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CStringA toolTipText = CStringA( GetTooltipText(nID) );
	LPTOOLTIPTEXTA lpTTT = (LPTOOLTIPTEXTA)pNotifyStruct;
	lstrcpynA(lpTTT->szText, toolTipText, _elements(lpTTT->szText));
}

BOOL CFontToolbar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;
	int nCode = pNMHDR->code;
	if (nCode == TTN_NEEDTEXTA || nCode == TTN_NEEDTEXTW)
	{
		UINT nID = (UINT)wParam;

		ASSERT((UINT)pNMHDR->idFrom == (UINT)wParam);
		ASSERT(hWndCtrl != NULL);
		ASSERT(::IsWindow(hWndCtrl));

		if (AfxGetThreadState()->m_hLockoutNotifyWindow == m_hWnd)
			return TRUE;        // locked out - ignore control notification

		// reflect notification to child window control
		if (ReflectLastMsg(hWndCtrl, pResult))
			return TRUE;        // eaten by child

		AFX_NOTIFY notify;
		notify.pResult = pResult;
		notify.pNMHDR = pNMHDR;
		return OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL); 
	}
	
	return __super::OnNotify(wParam, lParam, pResult);
}

void CFontToolbar::OnFontName()
{		
	CString sName;
	m_FontNames.GetLBText( m_FontNames.GetCurSel(), sName );
	mSettings.setName( sName );
	mpStudioFrame->OnFontChange( mSettings, fontName );
}

void CFontToolbar::OnFontSize()
{		
	CString sSize;
	m_FontSizes.GetLBText( m_FontSizes.GetCurSel(), sSize );
	long lSize = _tstol( sSize );
	if( !mSettings.isScaled() )
		lSize = -lSize;
	mSettings.setSize( lSize );
	mpStudioFrame->OnFontChange( mSettings, fontSize );
}

void CFontToolbar::OnFontBold()
{		
	mSettings.setBold( (m_Buttons.IsButtonChecked(ID_FONTBOLDBTN) != FALSE) );
	mpStudioFrame->OnFontChange( mSettings, fontBold );
	m_Buttons.SetState( ID_FONTBOLDBTN, m_Buttons.GetState( ID_FONTBOLDBTN ) & ~TBSTATE_INDETERMINATE );
}

void CFontToolbar::OnFontItalic()
{
	mSettings.setItalic( (m_Buttons.IsButtonChecked(ID_FONTITALICBTN) != FALSE) );
	mpStudioFrame->OnFontChange( mSettings, fontItalic );
	m_Buttons.SetState( ID_FONTITALICBTN, m_Buttons.GetState( ID_FONTITALICBTN ) & ~TBSTATE_INDETERMINATE );
}

void CFontToolbar::OnFontUnderline()
{
	mSettings.setUnderlined( (m_Buttons.IsButtonChecked(ID_FONTUNDERLINEBTN) != FALSE) );
	mpStudioFrame->OnFontChange( mSettings, fontUnderlined );
	m_Buttons.SetState( ID_FONTUNDERLINEBTN, m_Buttons.GetState( ID_FONTUNDERLINEBTN ) & ~TBSTATE_INDETERMINATE );
}

void CFontToolbar::OnFontScaled()
{
	if( (m_Buttons.IsButtonChecked(ID_FONTSCALED) != FALSE) ^ (mSettings.size() > 0) )
		mSettings.setSize( -mSettings.size() );
	mpStudioFrame->OnFontChange( mSettings, fontScaled );
	m_Buttons.SetState( ID_FONTSCALED, m_Buttons.GetState( ID_FONTSCALED ) & ~TBSTATE_INDETERMINATE );
}
