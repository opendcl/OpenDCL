// ArxColorService.cpp — COLORTHEME syscolor tables + refcounted sysVarChanged

#include "StdAfx.h"
#include "ArxColorService.h"
#include "ArxWorkspace.h"

CArxColorService::CArxColorService( CArxWorkspace* pWorkspace )
: mpWorkspace( pWorkspace )
, m_pTable( NULL )
, m_nClients( 0 )
, m_bReactorAdded( false )
, m_bRefreshing( false )
, mEdReactor( this )
{
	memset( m_light, 0, sizeof( m_light ) );
	memset( m_dark, 0, sizeof( m_dark ) );
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
	// Placeholder: dark table matches light until sampled CAD dark colors exist.
	FillFromSysColors( m_dark );

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
