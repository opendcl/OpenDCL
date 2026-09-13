// ArxColorService.cpp — COLORTHEME syscolor tables + refcounted sysVarChanged

#include "StdAfx.h"
#include "ArxColorService.h"
#include "ArxWorkspace.h"

// Sampled AutoCAD/BricsCAD dark dialog chrome (COLORTHEME 0), mapped onto
// Win32 GetSysColor indices. Light table stays live GetSysColor.
static const COLORREF kCadDarkSysColors[] =
{
	RGB( 59,  68,  83 ), //  0 COLOR_SCROLLBAR
	RGB( 33,  40,  48 ), //  1 COLOR_BACKGROUND / COLOR_DESKTOP
	RGB(  0,  82, 117 ), //  2 COLOR_ACTIVECAPTION
	RGB( 59,  68,  83 ), //  3 COLOR_INACTIVECAPTION
	RGB( 59,  68,  83 ), //  4 COLOR_MENU
	RGB( 33,  40,  48 ), //  5 COLOR_WINDOW
	RGB(  0,   0,   0 ), //  6 COLOR_WINDOWFRAME
	RGB(255, 255, 255 ), //  7 COLOR_MENUTEXT
	RGB(255, 255, 255 ), //  8 COLOR_WINDOWTEXT
	RGB(255, 255, 255 ), //  9 COLOR_CAPTIONTEXT
	RGB( 59,  68,  83 ), // 10 COLOR_ACTIVEBORDER
	RGB( 59,  68,  83 ), // 11 COLOR_INACTIVEBORDER
	RGB( 33,  40,  48 ), // 12 COLOR_APPWORKSPACE
	RGB(  0, 120, 215 ), // 13 COLOR_HIGHLIGHT
	RGB(255, 255, 255 ), // 14 COLOR_HIGHLIGHTTEXT
	RGB( 59,  68,  83 ), // 15 COLOR_BTNFACE / COLOR_3DFACE
	RGB( 37,  45,  56 ), // 16 COLOR_BTNSHADOW / COLOR_3DSHADOW
	RGB(128, 128, 128 ), // 17 COLOR_GRAYTEXT
	RGB(255, 255, 255 ), // 18 COLOR_BTNTEXT
	RGB(160, 160, 160 ), // 19 COLOR_INACTIVECAPTIONTEXT
	RGB( 90, 100, 115 ), // 20 COLOR_BTNHIGHLIGHT / COLOR_3DHIGHLIGHT
	RGB( 20,  25,  30 ), // 21 COLOR_3DDKSHADOW
	RGB( 80,  90, 105 ), // 22 COLOR_3DLIGHT
	RGB(255, 255, 255 ), // 23 COLOR_INFOTEXT
	RGB( 45,  52,  64 ), // 24 COLOR_INFOBK
	RGB( 59,  68,  83 ), // 25 (unused)
	RGB(  0, 162, 232 ), // 26 COLOR_HOTLIGHT
	RGB(  0,  82, 117 ), // 27 COLOR_GRADIENTACTIVECAPTION
	RGB( 59,  68,  83 ), // 28 COLOR_GRADIENTINACTIVECAPTION
	RGB(  0, 120, 215 ), // 29 COLOR_MENUHILIGHT
	RGB( 59,  68,  83 ), // 30 COLOR_MENUBAR
	RGB( 59,  68,  83 ), // 31
};
C_ASSERT( sizeof( kCadDarkSysColors ) == 32 * sizeof( COLORREF ) );

CArxColorService::CArxColorService( CArxWorkspace* pWorkspace )
: mpWorkspace( pWorkspace )
, m_pTable( NULL )
, m_nClients( 0 )
, m_bReactorAdded( false )
, m_bRefreshing( false )
, mEdReactor( this )
{
	memset( m_light, 0, sizeof( m_light ) );
	memcpy( m_dark, kCadDarkSysColors, sizeof( m_dark ) );
}

CArxColorService::~CArxColorService()
{
	Shutdown();
}

void CArxColorService::FillFromSysColors( COLORREF* pTable )
{
	for( int i = 0; i < kSysColorCount; ++i )
		pTable[i] = ::GetSysColor( i );
}

bool CArxColorService::ColorThemeExistsAndIsZero() const
{
#if (defined(_ARXTARGET) && (_ARXTARGET < 20)) || (defined(_BRXTARGET) && (_BRXTARGET < 19))
	return false;
#else
	struct resbuf rb;
	memset( &rb, 0, sizeof( rb ) );
	if( acedGetVar( _T("COLORTHEME"), &rb ) != RTNORM )
		return false;

	int n = 0;
	if( rb.restype == RTSHORT )
		n = rb.resval.rint;
	else if( rb.restype == RTLONG )
		n = static_cast< int >( rb.resval.rlong );
	else
	{
		TraceFmt( _T("COLORTHEME GETVAR restype=%d (not short/long) -> light table\r\n"), rb.restype );
		return false;
	}

	TraceFmt( _T("COLORTHEME GETVAR restype=%d value=%d -> %s table\r\n"),
		rb.restype, n, n == 0 ? _T("dark") : _T("light") );
	return n == 0;
#endif
}

bool CArxColorService::SelectTable()
{
	FillFromSysColors( m_light );

	const COLORREF* pNew = m_light;
	if( ColorThemeExistsAndIsZero() )
		pNew = m_dark;
	if( pNew == m_pTable )
		return false;
	m_pTable = pNew;
	return true;
}

void CArxColorService::AttachReactor()
{
	if( m_bReactorAdded || !acedEditor )
		return;
	acedEditor->addReactor( &mEdReactor );
	m_bReactorAdded = true;
}

void CArxColorService::DetachReactor()
{
	if( !m_bReactorAdded )
		return;
	if( acedEditor )
		acedEditor->removeReactor( &mEdReactor );
	m_bReactorAdded = false;
}

void CArxColorService::OnColorThemeSysVar( LPCTSTR pszVarName )
{
	if( !pszVarName || _tcsicmp( pszVarName, _T("COLORTHEME") ) != 0 )
		return;
	if( m_bRefreshing )
		return;
	m_bRefreshing = true;
	const bool changed = SelectTable();
	m_bRefreshing = false;
	if( !changed || !mpWorkspace )
		return;
	TraceFmt( _T("sysVarChanged(COLORTHEME) table switched\r\n") );
	mpWorkspace->OnHostThemeChanged();
}

void CArxColorService::AddClient()
{
	if( ++m_nClients == 1 )
	{
		AttachReactor();
		SelectTable();
	}
}

void CArxColorService::ReleaseClient()
{
	if( m_nClients <= 0 )
		return;
	if( --m_nClients == 0 )
		DetachReactor();
}

void CArxColorService::Shutdown()
{
	m_nClients = 0;
	DetachReactor();
	m_pTable = NULL;
}

COLORREF CArxColorService::GetSysColor( int nIndex ) const
{
	if( m_pTable && nIndex >= 0 && nIndex < kSysColorCount )
		return m_pTable[nIndex];
	return CWin32ColorService::GetSysColor( nIndex );
}
