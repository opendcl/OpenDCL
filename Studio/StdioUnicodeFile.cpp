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
: CStdioFile( lpszFileName, nOpenFlags | CFile::typeBinary )
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
	#ifdef _UNICODE
		{
			CStringA rS;
			if( !ReadAnsiString( rS ) )
				return FALSE;
			rString = rS;
		}
		return TRUE;
	#else
		return __super::ReadString( rString );
	#endif
	case Unicode:
	#ifdef _UNICODE
		return ReadUnicodeString( rString );
	#else
		{
			CStringW sLine;
			if( !ReadUnicodeString( sLine ) )
				return FALSE;
			rString = sLine;
		}
		return TRUE;
	#endif
	case Unknown:
		mfReadType = IsUnicode()? Unicode : Ansi;
		return ReadString( rString );
	}
	return FALSE;
}

LPCSTR FindEol( LPCSTR pszSearch, ULONG cchLimit )
{
	LPCSTR pchCursor = pszSearch;
	LPCSTR pchEnd = pchCursor + cchLimit;
	while( pchCursor < pchEnd )
	{
		switch( *pchCursor )
		{
		case '\0':
		case '\r':
		case '\n':
			return pchCursor;
		}
		++pchCursor;
	}
	return NULL;
}

LPCWSTR FindEol( LPCWSTR pszSearch, ULONG cchLimit )
{
	LPCWSTR pchCursor = pszSearch;
	LPCWSTR pchEnd = pchCursor + cchLimit;
	while( pchCursor < pchEnd )
	{
		switch( *pchCursor )
		{
		case L'\0':
		case L'\r':
		case L'\n':
			return pchCursor;
		}
		++pchCursor;
	}
	return NULL;
}

BOOL CStdioUnicodeFile::ReadAnsiString( CStringA& rString )
{
	static char szBuffer[4097];
	BOOL bReadData = FALSE;
	rString.Empty();
	ULONGLONG nPos = GetPosition();
	ULONGLONG nLen = GetLength();
	if( nPos >= nLen )
		return FALSE;
	ULONGLONG cchTotalRead = 0;

	UINT cchRead = 4096;
	while( cchRead == 4096 )
	{
		cchRead = Read( szBuffer, sizeof(szBuffer) );
		if( cchRead > 4096 )
			cchRead = 4096;
		szBuffer[cchRead] = '\0';
		char* pszEol = (LPSTR)FindEol( szBuffer, cchRead );
		if( pszEol )
		{
			UINT cchLine = (pszEol - szBuffer);
			cchTotalRead += cchLine;
			if( *pszEol == '\r' )
			{
				*pszEol = '\0';
				++cchTotalRead; //skip the \r
				if( *++pszEol == '\n' )
					++cchTotalRead; //skip the \n
			}
			else if( *pszEol == '\n' )
			{
				*pszEol = '\0';
				++cchTotalRead; //skip the \n
				if( *++pszEol == '\r' )
					++cchTotalRead; //skip the \r
			}
			cchRead = 1;
		}
		else
			cchTotalRead = cchRead;
		if( cchRead > 0 )
		{
			bReadData = TRUE;
			rString += szBuffer;
		}
		nPos += cchTotalRead;
		if( nPos >= nLen )
			break;
		Seek( nPos, FILE_BEGIN );
	}

	return bReadData;
}

BOOL CStdioUnicodeFile::ReadUnicodeString( CStringW& rString )
{
	static wchar_t szBuffer[4097];
	BOOL bReadData = FALSE;
	rString.Empty();
	ULONGLONG nPos = GetPosition();
	ULONGLONG nLen = GetLength();
	if( nPos >= nLen )
		return FALSE;
	ULONGLONG cchTotalRead = 0;

	UINT cchRead = 4096;
	while( cchRead == 4096 )
	{
		cchRead = Read( szBuffer, sizeof(szBuffer) ) >> 1;
		if( cchRead > 4096 )
			cchRead = 4096;
		szBuffer[cchRead] = L'\0';
		wchar_t* pszEol = (LPWSTR)FindEol( szBuffer, cchRead );
		if( pszEol )
		{
			UINT cchLine = (pszEol - szBuffer);
			cchTotalRead += cchLine;
			if( *pszEol == L'\r' )
			{
				*pszEol = L'\0';
				++cchTotalRead; //skip the \r
				if( *++pszEol == L'\n' )
					++cchTotalRead; //skip the \n
			}
			else if( *pszEol == L'\n' )
			{
				*pszEol = L'\0';
				++cchTotalRead; //skip the \n
				if( *++pszEol == L'\r' )
					++cchTotalRead; //skip the \r
			}
			cchRead = 1;
		}
		else
			cchTotalRead = cchRead;
		if( cchRead > 0 )
		{
			bReadData = TRUE;
			rString += szBuffer;
		}
		nPos += (cchTotalRead << 1);
		if( nPos >= nLen )
			break;
		Seek( nPos, FILE_BEGIN );
	}

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
