#include "StdAfx.h"
#include "StdioUnicodeFile.h"

CStdioUnicodeFile::CStdioUnicodeFile(void)
: mfReadType( Unknown )
{
}

CStdioUnicodeFile::CStdioUnicodeFile(FILE* pOpenStream)
: CStdioFile( pOpenStream )
, mfReadType( Unknown )
{
}

CStdioUnicodeFile::CStdioUnicodeFile(LPCTSTR lpszFileName, UINT nOpenFlags)
: CStdioFile( lpszFileName, nOpenFlags )
, mfReadType( Unknown )
{
}

CStdioUnicodeFile::~CStdioUnicodeFile(void)
{
}

BOOL CStdioUnicodeFile::ReadString(CString& rString)
{
	switch( mfReadType )
	{
	case Ansi:
		return __super::ReadString( rString );
	case Unicode:
	#ifdef _UNICODE
		return ReadUnicodeString( rString );
	#else
		CStringW sLine;
		BOOL bSuccess = ReadUnicodeString( sLine );
		if( bSuccess )
			rString = sLine;
		return bSuccess;
	#endif
	case Unknown:
		mfReadType = IsUnicode()? Unicode : Ansi;
		return ReadString( rString );
	}
	return FALSE;
}

LPCTSTR FindEol( LPCTSTR pszSearch, ULONG cchLimit )
{
	LPCTSTR pchCursor = pszSearch;
	LPCTSTR pchEnd = pchCursor + cchLimit;
	while( pchCursor < pchEnd )
	{
		switch( *pchCursor )
		{
		case _T('\0'):
		case _T('\r'):
		case _T('\n'):
			return pchCursor;
		}
		++pchCursor;
	}
	return NULL;
}

BOOL CStdioUnicodeFile::ReadUnicodeString( CStringW& rString )
{
	static wchar_t szBuffer[4096];
	BOOL bReadData = FALSE;
	rString.Empty();
	ULONGLONG nPos = GetPosition();
	ULONGLONG cchTotalRead = 0;

	UINT cchRead = 4096;
	while( cchRead == 4096 )
	{
		cchRead = Read( szBuffer, 4096 );
		wchar_t* pszEol = (LPTSTR)FindEol( szBuffer, cchRead );
		if( pszEol )
		{
			UINT cchLine = (pszEol - szBuffer);
			cchTotalRead += cchLine;
			if( *pszEol == L'\r' )
			{
				*pszEol = L'\0';
				++cchTotalRead; //skip the \r
				if( cchLine < cchRead && *++pszEol == L'\n' )
					++cchTotalRead; //skip the \n
			}
			else if( *pszEol == L'\n' )
			{
				*pszEol = L'\0';
				++cchTotalRead; //skip the \n
				if( cchLine < cchRead && *++pszEol == L'\r' )
					++cchTotalRead; //skip the \r
			}
			cchRead = 1;
		}
		if( cchRead > 0 )
		{
			bReadData = TRUE;
			rString += szBuffer;
		}
	}
	Seek( nPos + (cchTotalRead * sizeof(TCHAR)), FILE_BEGIN );

	return bReadData;
}

BOOL CStdioUnicodeFile::IsUnicode()
{
	ULONGLONG nPos = GetPosition();
	if( nPos != 0 )
		return FALSE; //if we're starting anywhere but the beginning, assume Ansi
	WORD wchTest;
	if( sizeof(wchTest) == Read( &wchTest, sizeof(wchTest) ) && wchTest == 0xFEFF )
		return TRUE;
	Seek( 0, FILE_BEGIN ); //reset the file pointer
	return FALSE;
}
