// RegIO.cpp : Registry access class implementation
//
// Copyright 2007 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.

#include "StdAfx.h"
#include "RegIO.h"


RegKeyA::RegKeyA( LPCSTR pszKey, HKEY hkRoot, bool bCreate /*= false*/, bool bForWrite /*= true*/ )
:	m_hkRoot( hkRoot ),
	m_hkThis( NULL ),
	m_bForWrite( bForWrite ),
	m_pszKey( NULL )
{
	InnerOpen( pszKey, hkRoot, bCreate, bForWrite );
}

RegKeyA::RegKeyA( RegKeyA& Source )
:	m_hkRoot( Source.m_hkRoot ),
	m_hkThis( Source.m_hkThis ),
	m_bForWrite( Source.m_bForWrite ),
	m_pszKey( Source.m_pszKey )
{
	Source.m_hkRoot = NULL;
	Source.m_hkThis = NULL;
	Source.m_pszKey = NULL;
}

HKEY RegKeyA::InnerOpen( LPCSTR pszKey, HKEY hkRoot, bool bCreate /*= false*/, bool bForWrite /*= true*/ )
{
	if( !pszKey )
	{
		m_hkThis = hkRoot;
		return hkRoot;
	}
	m_pszKey = new CHAR[lstrlenA( pszKey ) + 1];
	lstrcpyA( m_pszKey, pszKey );
	if( ERROR_SUCCESS !=
				bCreate?
					::RegCreateKeyExA( hkRoot,
														 pszKey,
														 0,
														 NULL,
														 REG_OPTION_NON_VOLATILE,
														 (m_bForWrite? (KEY_READ | KEY_WRITE) : KEY_READ),
														 NULL,
														 &m_hkThis,
														 NULL ) :
					::RegOpenKeyExA( hkRoot,
													 pszKey,
													 0,
													 (m_bForWrite? (KEY_READ | KEY_WRITE) : KEY_READ),
													 &m_hkThis ) )
		m_hkThis = NULL;
	return m_hkThis;
}

RegKeyA::~RegKeyA()
{
	Close();
}

void RegKeyA::Close()
{
	if( m_hkThis )
	{
		::RegCloseKey( m_hkThis );
		m_hkThis = NULL;
	}
	delete[] m_pszKey;
	m_pszKey = NULL;
	m_hkRoot = NULL;
}

bool RegKeyA::HasValue( LPCSTR pszName ) const
{
	if( !m_hkThis )
		return false;
	DWORD dwType;
	if( ::RegQueryValueExA( m_hkThis, pszName, 0, &dwType, NULL, NULL ) != ERROR_SUCCESS )
		return false;
	return true;
}

bool RegKeyA::SetValue( LPCSTR pszName, LPCSTR pszValue )
{
	if( !m_bForWrite || !m_hkThis )
		return false;
	return (::RegSetValueExA( m_hkThis, pszName, 0, REG_SZ, (BYTE*)pszValue, lstrlenA( pszValue ) + 1 ) == ERROR_SUCCESS);
}

bool RegKeyA::SetValue( LPCSTR pszName, DWORD dwValue )
{
	if( !m_bForWrite || !m_hkThis )
		return false;
	return (::RegSetValueExA( m_hkThis, pszName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS);
}

bool RegKeyA::SetValue( LPCSTR pszName, const BYTE* pbData, DWORD cbData )
{
	if( !m_bForWrite || !m_hkThis )
		return false;
	return (::RegSetValueExA( m_hkThis, pszName, 0, REG_BINARY, pbData, cbData) == ERROR_SUCCESS);
}


bool RegRemoveKey( RegKeyA& rkTarget )
{
	if( !rkTarget.IsWritable() )
		return false;
	DWORD cchKeyNameSize = MAX_PATH;
	DWORD dwIndex = 0;
	::RegQueryInfoKeyA( rkTarget, NULL, NULL, NULL, &dwIndex, &cchKeyNameSize, NULL, NULL, NULL, NULL, NULL, NULL );
	LPSTR pszSubkey = new CHAR[++cchKeyNameSize];
	while( dwIndex-- > 0 &&
				 ERROR_SUCCESS == ::RegEnumKeyA( rkTarget, dwIndex, pszSubkey, cchKeyNameSize ) )
		RegRemoveKey( RegKeyA( pszSubkey, rkTarget ) );
	delete[] pszSubkey;

	HKEY hkRoot = rkTarget.Root();
	LPSTR pszKey = new CHAR[lstrlenA( rkTarget.Key() ) + 1];
	lstrcpyA( pszKey, rkTarget.Key() );
	rkTarget.Close();
	bool bSuccess = (ERROR_SUCCESS == ::RegDeleteKeyA( hkRoot, pszKey ));
	delete[] pszKey;
	return bSuccess;
}
