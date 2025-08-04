//
//	CStgFile.cpp
//+
//	implementation file
//-
//	rev 11/04/98 gls continue
//	rev 11/03/98 gls
//

#include "stdafx.h"
#include "StgFile.h"


// ////////////////////////////////////////////////////////////
// CStgFile

CStgFile::CStgFile()
{

	m_pStgRoot = NULL;
	m_pStgStreamOpen = NULL;

	m_pStreamOpen = NULL;

}

CStgFile::~CStgFile()
{
	CloseStg();						// just in case
}

// ////////////////////////////////////////////////////////////
//
//	Recursize enumeration
//
	static	BOOL	__EnumStreams( void *pContext, LPCTSTR pszStgPrefix, STATSTG *pStat )
	{

	if( pStat->type == STGTY_STREAM )
		{
		CStringArray *pSA = (CStringArray *)pContext;
		ASSERT( pSA != NULL );
		CString sStream;
		sStream.Format( _T("%s%s"), pszStgPrefix, (LPCTSTR)CString( pStat->pwcsName ) );
		pSA->Add( sStream );
		}
	return( TRUE );
	}

BOOL	CStgFile::EnumStreams( CStringArray &rgStreams, LPCSTR pszPattern )
{

//	Enum( NULL, __EnumStreams, (void *)&rgStreams );
	Enum( NULL, _T("\\"), __EnumStreams, &rgStreams );

	return( TRUE );
}
// ////////////////////////////////////////////////////////////
//
//	Recursize enumeration
//
BOOL	CStgFile::Enum( IStorage *pStg, LPCTSTR pszStgStart,
		BOOL (*pFuncCallback)( void *pContext, LPCTSTR pszStgStart, STATSTG *pStat ),
		void *pContextCallback )
{
    USES_CONVERSION;                // needed for OLE2CT

	ASSERT( m_pStgRoot != NULL );



	if( pStg == NULL )
		{
		pStg = m_pStgRoot;
		pszStgStart = _T("\\");
		}

	//
	//	Start the enumeration
	//
	ASSERT( pStg != NULL );
	IEnumSTATSTG *pEnum = NULL;

	HRESULT hRes = pStg->EnumElements( 0, NULL, 0, &pEnum );
	ASSERT( pEnum != NULL );


	while( hRes == S_OK )
		{
	    STATSTG StatStg;

		hRes = pEnum->Next( 1, &StatStg, NULL);

		if( hRes != S_OK )
			{
			continue;
			}

		//
		//	Call the callback if defined
		//
		if( pFuncCallback != NULL )
			{
			if( !pFuncCallback( pContextCallback, pszStgStart, &StatStg ) )
				{
				hRes = S_FALSE;
				}
			}

		//
		//	Now, the virtual function
		//
		if( StatStg.type == STGTY_STORAGE )
			{
			if( !OnEnumStg( pszStgStart, &StatStg ) )
				{
				break;
				}


			//
			//	If Storage, must recurse to find others
			//
			IStorage *pStgSub = NULL;
			hRes = pStg->OpenStorage( StatStg.pwcsName,
					NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE , NULL, 0, &pStgSub );

			if( hRes == S_OK )
				{
				ASSERT( pStgSub != NULL );

				CString sStg( StatStg.pwcsName );
				CString sStgStart;
				sStgStart.Format( _T("%s%s\\"), pszStgStart, (LPCTSTR)sStg );

				Enum( pStgSub, sStgStart, pFuncCallback, pContextCallback );

				pStgSub->Release();
				}
			else
				{
				ASSERT( 0 );
				}
			}
		else if( StatStg.type == STGTY_STREAM )
			{
			if( !OnEnumStream( pszStgStart, &StatStg ) )
				{
				break;
				}

			}


		}


	pEnum->Release();


	return( TRUE );
}

// ////////////////////////////////////////////////////////////


ULONGLONG CStgFile::Seek( LONG lOff, UINT nFrom )
{
	ULONGLONG nSeek = 0;

	if( m_pStgRoot != NULL )
		{
		ASSERT( m_pStreamOpen != NULL );
		if( m_pStreamOpen != NULL )
			{
			nSeek = m_pStreamOpen->Seek( lOff, nFrom );		// allow the stream to close
			}
		}
	else
		{
		nSeek = CFile::Seek( lOff, nFrom );
		}


	return( nSeek );
}

// ////////////////////////////////////////////////////////////
void CStgFile::Flush( void )
{
	if( m_pStgRoot != NULL )
		{
		ASSERT( m_pStreamOpen != NULL );
		if( m_pStreamOpen != NULL )
			{
			m_pStreamOpen->Flush();		// allow the stream to close
			}
		}
	else
		{
		CFile::Flush();
		}
}

// ////////////////////////////////////////////////////////////
//
//	Read from stream
//
UINT CStgFile::Read( void* lpBuf, UINT nCount )
{
	UINT cRead = 0;

	if( m_pStgRoot != NULL )
		{
		ASSERT( m_pStreamOpen != NULL );
		if( m_pStreamOpen != NULL )
			{
			cRead = m_pStreamOpen->Read( lpBuf, nCount );
			}
		}
	else
		{
		cRead = CFile::Read( lpBuf, nCount );	// delegate to base class
		}

	return( cRead );
}

// ////////////////////////////////////////////////////////////
//
//	Get length from stream
//
ULONGLONG CStgFile::GetLength()
{
	ULONGLONG length = 0;

	if( m_pStgRoot != NULL )
	{
		ASSERT( m_pStreamOpen != NULL );
		if( m_pStreamOpen != NULL )
		{
			length = m_pStreamOpen->GetLength();
		}
	}
	else
	{
		length = CFile::GetLength();	// delegate to base class
	}

	return( length );
}

// ////////////////////////////////////////////////////////////
//
//	SeekToBegin from the stream
//
void CStgFile::SeekToBegin()
{
	if( m_pStgRoot != NULL )
	{
		ASSERT( m_pStreamOpen != NULL );
		if( m_pStreamOpen != NULL )
		{
			m_pStreamOpen->SeekToBegin();
		}
	}
	else
	{
		CFile::SeekToBegin();	// delegate to base class
	}

}
// ////////////////////////////////////////////////////////////
//
//	Write to stream
//
void CStgFile::Write( const void* lpBuf, UINT nCount )
{
	BOOL	fSuccess = FALSE;

	if( m_pStgRoot != NULL )
		{
		ASSERT( m_pStreamOpen != NULL );
		if( m_pStreamOpen != NULL )
			{
			fSuccess = TRUE;
			m_pStreamOpen->Write( lpBuf, nCount );
			}
		}
	else
		{
		CFile::Write( lpBuf, nCount );	// delegate to base class
		}
}
// ////////////////////////////////////////////////////////////
//
//	Close the currently open stream
//
void CStgFile::Close( void )
{

	if( m_pStgRoot != NULL )
		{
		ASSERT( m_pStreamOpen != NULL );

		if( m_pStreamOpen != NULL )
			{
			m_pStreamOpen->Flush();
			m_pStreamOpen->Close();		// allow the stream to close

			delete m_pStreamOpen;		// delete the object

			m_pStreamOpen = NULL;
			}

		if( m_pStgStreamOpen != NULL )	// associated sub-storage?
			{
			VERIFY( m_pStgStreamOpen->Commit( STGC_OVERWRITE ) == S_OK );

			m_pStgStreamOpen->Release();

			m_pStgStreamOpen = NULL;
			}

		VERIFY( m_pStgRoot->Commit( STGC_OVERWRITE ) == S_OK );
		}
	else if( m_hFile != CFile::hFileNull  )
		{
		CFile::Close();
		}
}
// ////////////////////////////////////////////////////////////
//
//	Open either an external file or a stream.
//
//	Stream supports either a first level stream (root)
//	or in a single sub-storage specification.
//
BOOL CStgFile::Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException *pError )
{
    USES_CONVERSION;                // needed for OLE2CT

//	ASSERT( m_pStreamOpen == NULL );		// nice cleanup?


	BOOL	fSuccess = FALSE;

	if( m_pStgRoot != NULL )				// is it open?
		{
		ASSERT( m_pStreamOpen == NULL );		// nice cleanup?

		ASSERT( m_pStreamOpen == NULL );	// this should be normal
		ASSERT( m_pStgStreamOpen == NULL );	// this should be normal

		if( m_pStreamOpen != NULL )
			{
			m_pStreamOpen->Flush();
			m_pStreamOpen->Close();

			delete m_pStreamOpen;				// close any previous
			}



		if( m_pStgStreamOpen != NULL )
			{
			m_pStgStreamOpen->Commit( STGC_OVERWRITE );
			m_pStgStreamOpen->Release();
			}




		//
		//	Create temporary variables for possible changes based
		//	on the filename passed in by the caller.
		//
		IStorage *pStgRoot = m_pStgRoot;		// duplicate for redirection
		CString sFileName = lpszFileName;
		CString sStgDir, sStream;

		int iSlash = sFileName.Find( '\\' );
		if( iSlash != -1 )						// must be a directory
			{
			int iSlash2 = sFileName.ReverseFind( '\\' );
			if( iSlash == 0 )					// absolute specification
				{
				ASSERT( iSlash != iSlash2 );

				sStgDir = sFileName.Mid( 1, iSlash2-1 );
				}
			else								// relative specification
				{
				ASSERT( iSlash == iSlash2 );
				sStgDir = sFileName.Mid( 0, iSlash );
				}

			sStream = sFileName.Mid( iSlash2+1 );
			}


		//
		//	Now, if a directory specified, open a storage so
		//	that the stream may be located in the proper place.
		//
		if( sStgDir != "" )
			{

			sFileName = sStream;				// use just the stream portion

			IStorage *pStg = NULL;
			HRESULT hRes = m_pStgRoot->OpenStorage( T2COLE( sStgDir), NULL,
					STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
					NULL, 0, &pStg );

			if( hRes != S_OK )
				{
				ASSERT( 0 );
				return( FALSE );
				}

			pStgRoot = pStg;					// use as new root for open
			m_pStgStreamOpen = pStg;			// save for release
			}



		//
		//	Create a new one to be used by the new open
		//
		m_pStreamOpen = new COleStreamFile;	// a new one for now


		//
		//	Determine if a create or open existing
		//
		if( nOpenFlags & modeCreate )
			{
			fSuccess = m_pStreamOpen->CreateStream( pStgRoot, sFileName );
			ASSERT( fSuccess );
			}
		else
			{
			fSuccess = m_pStreamOpen->OpenStream( pStgRoot, sFileName );
			ASSERT( fSuccess );
			}

		}
	else
		{
		fSuccess = CFile::Open( lpszFileName, nOpenFlags, pError );
		}

	return( fSuccess );
}

// ////////////////////////////////////////////////////////////
//
//	Open an existing compound file
//
BOOL	CStgFile::OpenStg( LPCTSTR pszFileStg )
{
    USES_CONVERSION;                // needed for OLE2CT

	BOOL	fSuccess = FALSE;

	CloseStg();						// close any previous
	ASSERT( m_pStgRoot == NULL );

    HRESULT hRes = StgOpenStorage( T2COLE(pszFileStg),
            NULL,           // priority
            STGM_TRANSACTED | STGM_READWRITE,
            NULL, 0, &m_pStgRoot );

    if( hRes == S_OK )
        {
		fSuccess = TRUE;
		ASSERT( m_pStgRoot != NULL );
        }

	return( fSuccess );
}
// ////////////////////////////////////////////////////////////
//
//	Create (and truncate) a new compound document
//
BOOL	CStgFile::CreateStg( LPCTSTR pszFileStg )
{
    USES_CONVERSION;                // needed for OLE2CT

	BOOL	fSuccess = FALSE;

	CloseStg();						// close any previous
	ASSERT( m_pStgRoot == NULL );


	//
	//	Create the specified file...
	//
    HRESULT hRes = StgCreateDocfile( T2COLE(pszFileStg),
			STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE ,
//            STGM_TRANSACTED | STGM_READWRITE | STGM_CREATE,
			0, &m_pStgRoot );

	if( hRes == S_OK )
		{
	    ASSERT( m_pStgRoot != NULL );
		fSuccess = TRUE;
		}

	return( fSuccess );
}
// ////////////////////////////////////////////////////////////
//
//	Close (and commit) any open IStorage interfaces
//
BOOL	CStgFile::CloseStg( void )
{

	BOOL	fSuccess = FALSE;

	//
	//	Cleanup any open streams
	//
	if( m_pStreamOpen != NULL )
		{
		m_pStreamOpen->Close();
		delete m_pStreamOpen;
		}

	//
	//	Commit and free if currently open
	//
	if( m_pStgStreamOpen != NULL )
		{
		m_pStgStreamOpen->Commit( STGC_OVERWRITE );
		m_pStgStreamOpen->Release();
		}

	if( m_pStgRoot != NULL )
		{
		m_pStgRoot->Commit( STGC_OVERWRITE );
		m_pStgRoot->Release();

		fSuccess = TRUE;
		}

	//
	//	Step on the current pointers
	//
	m_pStreamOpen = NULL;
	m_pStgStreamOpen = NULL;
	m_pStgRoot = NULL;


	return( fSuccess );
}
// ////////////////////////////////////////////////////////////
//
//	Create the specified sub-storage off of the root storage
//
BOOL	CStgFile::MkStg( LPCTSTR pszStg )
{
    USES_CONVERSION;                // needed for OLE2CT


	BOOL	fSuccess = FALSE;

	if( m_pStgRoot != NULL )		// only if currently open
		{
		IStorage	*pStg = NULL;

		if( *pszStg == '\\' )
			{
			pszStg++;					// step past prefix
			}

		//
		//	First, check to see if already in place since STGM_FAILIFTHERE
		//	seems to have a problem.
		//
		HRESULT hRes = S_FALSE;

		if( (hRes = m_pStgRoot->OpenStorage( T2COLE( pszStg ), NULL,
				STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg )) == S_OK )
			{
			ASSERT( pStg != NULL );	// already in place
			}
		else if( (hRes = m_pStgRoot->CreateStorage( T2COLE( pszStg ),
				STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE ,
///				STGM_FAILIFTHERE | STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE ,
//				STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
				NULL, NULL, &pStg ) ) == S_OK )
			{
			ASSERT( pStg != NULL );
			}
		else
			{
			ASSERT( 0 );
			}


		if( hRes == S_OK )
			{
			pStg->Commit( STGC_OVERWRITE );
			pStg->Release();

			fSuccess = TRUE;
			}
		else
			{
			ASSERT( 0 );
			}
		}
	else
		{
		ASSERT( 0 );
		}



	return( fSuccess );
}

// ////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////
