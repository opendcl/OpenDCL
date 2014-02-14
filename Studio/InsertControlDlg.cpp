// InsertControlDlg.Cpp : implementation file
//

#include "StdAfx.h"
#include "InsertControlDlg.h"
#include "AxTypeUtils.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CInsertControlDlg dialog


CInsertControlDlg::CInsertControlDlg( CWnd* pParent ) :
	 CDialog( CInsertControlDlg::IDD, pParent ),
	 m_clsid( CLSID_NULL )
{
	//{{AFX_DATA_INIT(CInsertControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInsertControlDlg::DoDataExchange(CDataExchange* pDX)
{
	int iItem;
	POSITION posControl;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertControlDlg)
	DDX_Control(pDX, IDC_SERVERPATH, m_staticServerPath);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDC_CONTROLS, m_lbControls);
	//}}AFX_DATA_MAP

	 if( pDX->m_bSaveAndValidate )
	 {
		iItem = m_lbControls.GetCurSel();
		if( iItem == LB_ERR )
		{
		 m_clsid = CLSID_NULL;
		}
		else
		{
		 posControl = POSITION( m_lbControls.GetItemDataPtr( iItem ) );
		 ASSERT( posControl != NULL );
		 m_clsid = m_lControls.GetAt( posControl );
		}
	 }
}


BEGIN_MESSAGE_MAP(CInsertControlDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertControlDlg)
	ON_LBN_DBLCLK(IDC_CONTROLS, OnControlsDblClk)
	ON_LBN_SELCHANGE(IDC_CONTROLS, OnControlsSelChange)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDREGISTER, OnRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertControlDlg message handlers

BOOL CInsertControlDlg::OnInitDialog()
{
	HRESULT hResult;
	CATID catid;

	BeginWaitCursor();
	hResult = m_pCatInfo.CreateInstance( CLSID_StdComponentCategoriesMgr );
	if( FAILED( hResult ) )
	{
		CString sErr;
		sErr.Format( _T("Failed to create Component Categories Manager!\r\nHRESULT: %X"), hResult );
		MessageBox( sErr, _T("Error") );
		EndDialog( IDCANCEL );
		return( TRUE );
	}

	CDialog::OnInitDialog();

	catid = CATID_Control;
	m_aImplementedCategories.Add( catid );

	m_lbControls.ModifyStyle( 0, WS_HSCROLL );

	RefreshControlList();

	m_lbControls.SetFocus();

	EndWaitCursor();
	return( FALSE );
}

void GetClassServerPath( REFCLSID clsid, CString& strServerPath )
{
	HKEY hKey = NULL;
	HKEY hServerKey = NULL;

	try
	{
		OLECHAR szCLSID[64];
		StringFromGUID2( clsid, szCLSID, 64 );
		LONG nResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, CString( _T("CLSID\\") ) + szCLSID, 0, KEY_READ, &hKey );
		if( nResult != ERROR_SUCCESS )
			throw( E_FAIL );

		nResult = RegOpenKeyEx( hKey, _T("InprocServer32"), 0, KEY_READ, &hServerKey );
		if( nResult != ERROR_SUCCESS )
		{
			nResult = RegOpenKeyEx( hKey, _T("InprocHandler32"), 0, KEY_READ, &hServerKey );
			if( nResult != ERROR_SUCCESS )
			{
				nResult = RegOpenKeyEx( hKey, _T("LocalServer32"), 0, KEY_READ, &hServerKey );
				if( nResult != ERROR_SUCCESS )
					throw( E_FAIL );
			}
		}
		RegCloseKey( hKey );
		hKey = NULL;

		ULONG nBytes = 0;
		DWORD dwType;
		nResult = RegQueryValueEx( hServerKey, NULL, NULL, &dwType, NULL, &nBytes );
		if( nResult != ERROR_SUCCESS )
			throw( E_FAIL );
		if( dwType != REG_EXPAND_SZ && dwType != REG_SZ )
			throw( E_FAIL );
		LPTSTR pszServerPath = LPTSTR( _alloca( nBytes ) );
		nResult = RegQueryValueEx( hServerKey, NULL, NULL, &dwType, LPBYTE( pszServerPath ), &nBytes );
		if( nResult != ERROR_SUCCESS )
			throw( E_FAIL );
		if( dwType != REG_EXPAND_SZ && dwType != REG_SZ )
			throw( E_FAIL );

		RegCloseKey( hServerKey );
		hServerKey = NULL;

		if( dwType == REG_EXPAND_SZ )
		{
			ExpandEnvironmentStrings( pszServerPath, strServerPath.GetBuffer( MAX_PATH + 2 ), MAX_PATH + 1 );
			strServerPath.ReleaseBuffer();
		}
		else
			strServerPath = pszServerPath;
	}
	catch( HRESULT )
	{
		if( hKey )
			RegCloseKey( hKey );
		if( hServerKey )
			RegCloseKey( hServerKey );
		strServerPath = theWorkspace.LoadResourceString( IDS_SERVERNOTFOUND );
		return;
	}
}

void CInsertControlDlg::RefreshControlList()
{
	 BOOL tDone;
	 HRESULT hResult;
	 IEnumGUIDPtr pEnum;
	 ULONG nImplementedCategories;
	 CATID* pcatidImpl;
	 ULONG nRequiredCategories;
	 CATID* pcatidReq;
	 CLSID clsid;
	 CString strName;
	 ULONG iCategory;
	 int iItem;
	 POSITION posControl;
	 CString strServerPath;
	 CString strString;

	 m_lbControls.ResetContent();
	 m_lControls.RemoveAll();

	 nImplementedCategories = (ULONG)m_aImplementedCategories.GetSize();
	 if( nImplementedCategories == 0 )
	 {
		nImplementedCategories = ULONG( -1 );
		pcatidImpl = NULL;
	 }
	 else
	 {
		pcatidImpl = (CATID*)_alloca( nImplementedCategories*sizeof( CATID ) );
		for( iCategory = 0; iCategory < nImplementedCategories; iCategory++ )
		{
		 pcatidImpl[iCategory] = m_aImplementedCategories[iCategory];
		}
	 }
	 nRequiredCategories = m_aRequiredCategories.GetSize();  
	 if( nRequiredCategories == 0 )
	 {
		 nRequiredCategories = ULONG( -1 );
		 pcatidReq = NULL;
	 }
	 else
	 {
		pcatidReq = (CATID*)_alloca( nRequiredCategories*sizeof( CATID ) );
		for( iCategory = 0; iCategory < nRequiredCategories; iCategory++ )
		{
		 pcatidReq[iCategory] = m_aRequiredCategories[iCategory];
		}
	 }

	 hResult = m_pCatInfo->EnumClassesOfCategories( nImplementedCategories,
		pcatidImpl, nRequiredCategories, pcatidReq, &pEnum );
	 if( FAILED( hResult ) )
	 {
		return;
	 }

	 tDone = FALSE;
	 while( !tDone )
	 {
		hResult = pEnum->Next( 1, &clsid, NULL );
		if( hResult == S_OK )
		{
		 strName = GetAxLongTypeName( clsid );
		 if( !strName.IsEmpty() )
		 {
			 iItem = m_lbControls.AddString( strName );
			 posControl = m_lControls.AddTail( clsid );
			 m_lbControls.SetItemDataPtr( iItem, posControl );
		 }
		}
		else
		{
		 tDone = TRUE;
		}
	 }

	 OnControlsSelChange();
}


void CInsertControlDlg::OnControlsDblClk()
{
	OnOK();
}


void CInsertControlDlg::OnControlsSelChange()
{
	 int iItem;
	 POSITION posControl;
	 CString strServerPath;
	 CLSID clsid;

	 iItem = m_lbControls.GetCurSel();
	 if( iItem != LB_ERR )
	 {
		m_butOK.EnableWindow( TRUE );
		posControl = POSITION( m_lbControls.GetItemDataPtr( iItem ) );
		clsid = m_lControls.GetAt( posControl );
		GetClassServerPath( clsid, strServerPath );
		m_staticServerPath.SetWindowText( strServerPath );
		m_FileName = strServerPath;
	 }
	 else
	 {
		m_butOK.EnableWindow( FALSE );
		m_staticServerPath.SetWindowText( NULL );
	 }
}

typedef HRESULT (STDAPICALLTYPE* PDLLREGISTERSERVER)( void );

static void RegisterServer( LPCTSTR pszPathName )
{
	HINSTANCE hLibrary = LoadLibrary( pszPathName );
	if( hLibrary )
	{
		PDLLREGISTERSERVER pDllRegisterServer =
			(PDLLREGISTERSERVER)GetProcAddress( hLibrary, "DllRegisterServer" );
		if( pDllRegisterServer )
			pDllRegisterServer();
		FreeLibrary( hLibrary );
	}
}

void CInsertControlDlg::OnRegister() 
{
	CFileDialog dlg( TRUE, NULL, NULL,
									 OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
									 theWorkspace.LoadResourceString(IDS_AXFILEFILTER) );

	if( dlg.DoModal() != IDOK )
		return;

	CWaitCursor wait;
	POSITION posPathName = dlg.GetStartPosition();
	while( posPathName != NULL )
		RegisterServer(dlg.GetNextPathName( posPathName ));
	RefreshControlList();
}
