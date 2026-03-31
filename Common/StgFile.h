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
			printf( "CStgFile:: OnEnumStg( %ls\\%ls )\n",
					pszStgPrefix, (LPCTSTR)CString( pStat->pwcsName ) );

			return( TRUE );
			}

	virtual	BOOL	OnEnumStream( LPCTSTR pszStgPrefix, STATSTG *pStat )
			{
			printf( "CStgFile:: OnEnumStream( %ls\\%ls )\n",
					pszStgPrefix, (LPCTSTR)CString( pStat->pwcsName ) );

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
	virtual BOOL Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL ) override;
	virtual void Close( void ) override;
	virtual void Write( const void* lpBuf, UINT nCount ) override;
	virtual UINT Read( void* lpBuf, UINT nCount ) override;
	virtual ULONGLONG GetLength() const override;
	virtual void Flush( ) override;
	virtual ULONGLONG Seek( LONGLONG lOff, UINT nFrom ) override;

	void SeekToBegin();


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
