// RegIO.h : Registry access class
//
// Copyright 2007 - 2018 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.

#pragma once

#include <Windows.h>


bool RegRemoveKey( class RegKeyA& rkTarget );


class RegKeyA
{
	HKEY m_hkRoot;
	HKEY m_hkThis;
	REGSAM m_samAccess;
	LPSTR m_pszKey;
public:
	RegKeyA( LPCSTR pszKey, HKEY hkRoot, bool bCreate = false, REGSAM samAccess = KEY_ALL_ACCESS );
	RegKeyA( RegKeyA& Source );
	virtual ~RegKeyA();
	virtual void Close();
	virtual bool IsWritable() const { return (m_hkRoot && (m_samAccess & KEY_WRITE) == KEY_WRITE); }
	virtual bool HasValue( LPCSTR pszName ) const;
	virtual bool SetValue( LPCSTR pszName, LPCSTR pszValue );
	virtual bool SetValue( LPCSTR pszName, DWORD dwValue );
	virtual bool SetValue( LPCSTR pszName, const BYTE* pbData, DWORD cbData );
	virtual LPCSTR GetString( LPCSTR pszName ) const;
	virtual DWORD GetDword( LPCSTR pszName ) const;
	operator HKEY () const { return m_hkThis; }
	LPCSTR Key() const { return m_pszKey; }
	HKEY Root() const { return m_hkRoot; }
protected:
	virtual HKEY InnerOpen( LPCSTR pszKey, HKEY hkRoot, bool bCreate = false, REGSAM samAccess = KEY_ALL_ACCESS );
};

typedef RegKeyA RegKey;
