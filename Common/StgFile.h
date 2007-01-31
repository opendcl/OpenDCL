//
//	CStgFile.h
//+
//	header file
//-
//	rev 11/04/98 gls added isOpen()
//	rev 11/03/98 gls
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CStgFile window

class CStgFile : public CFile
{

	IStorage		*m_pStgRoot;		// open storage
	
	IStorage		*m_pStgStreamOpen;	// storage for current open stream
	COleStreamFile	*m_pStreamOpen;		// pointer if stream is open

	//int	printf( const char *format, ... ) { return(0); }


// Construction
public:
	CStgFile();

// Attributes
public:

// Operations
public:

	BOOL	OpenStg( LPCTSTR pszFileStg );
	BOOL	CreateStg( LPCTSTR pszFileStg );
	BOOL	CloseStg( void );
	BOOL	MkStg( LPCTSTR pszStg );

	BOOL	isOpen( void )
			{
			return( m_pStgRoot != NULL ? TRUE : FALSE );
			}



	BOOL	Enum( IStorage *pStg = NULL, LPCTSTR pszStgStart = NULL,
				BOOL (*pFunc)(void *pContext, LPCTSTR pszStgPrefix, STATSTG *pStat ) = NULL,
				void *pContextCallback = NULL );

	BOOL	EnumStreams( CStringArray &rgStreams, LPCSTR pszPattern = NULL );

	virtual	BOOL	OnEnumStg( LPCTSTR pszStgPrefix, STATSTG *pStat )
			{
			printf( "CStgFile:: OnEnumStg( %s\\%s )\n",
					pszStgPrefix,  CString( pStat->pwcsName ) );

			return( TRUE );
			}

	virtual	BOOL	OnEnumStream( LPCTSTR pszStgPrefix, STATSTG *pStat )
			{
			printf( "CStgFile:: OnEnumStream( %s\\%s )\n",
					pszStgPrefix,  CString( pStat->pwcsName ) );

			return( TRUE );
			}

	IStorage	*getStgRoot()
	{
		return( m_pStgRoot );
	}

	
public:
	//
	//	Overrides to switch between external and document stream
	//
	virtual BOOL Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL );
	virtual void Close( void );
	virtual void Write( const void* lpBuf, UINT nCount );
	virtual UINT Read( void* lpBuf, UINT nCount );
	virtual ULONGLONG GetLength();
	virtual void SeekToBegin();
	virtual void Flush( );
	virtual ULONGLONG Seek( LONG lOff, UINT nFrom );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStgFile)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStgFile();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStgFile)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
};
