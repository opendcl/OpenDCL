// ArxWorkspace.cpp : implementation file
//

#include "stdafx.h"
#include "ArxWorkspace.h"
#include "ArxProject.h"
#include "Resource.h"
#include "ArchiveEx.h"
#include "ArxDialogObject.h"
#include "DclFormObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "InvokeMethod.h"
#include "DclControlObject.h"


CWorkspace* theWorkspacePtr()
{
	static CArxWorkspace workspace;
	return &workspace;
}

//Constant strings
#define S_DefaultFontName _T("DefaultFontName")
#define S_DefaultFontSize _T("DefaultFontSize")
#define S_DefaultFontItalic _T("DefaultFontItalic")
#define S_DefaultFontUnderLine _T("DefaultFontUnderLine")
#define S_DefaultFontBold _T("DefaultFontBold")
#define S_DefaultFontSizeStyle _T("DefaultFontSizeStyle")


//function used by CLocalResourceModuleOverride defined in StdAfx.h
HMODULE appLocalResModule(void)
{
	return theWorkspace.GetLocalResourceModule();
}


static CString StripPathFromFileName( LPCTSTR pszFilePath )
{
	CString sShortName = pszFilePath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


static CString CreateUniqueName() 
{
	CString sUniqueName;
	UUID uuid;
#ifdef _UNICODE
	RPC_WSTR pUUID;
#else
	RPC_CSTR pUUID;
#endif
	UuidCreate(&uuid);
	UuidToString(&uuid, &pUUID);
	sUniqueName = (LPCTSTR)pUUID;
	return sUniqueName;
}


CArxWorkspace::CArxWorkspace()
: mDocReactor( this )
{
}

CArxWorkspace::~CArxWorkspace()
{
	for( std::list< IUnknown* >::iterator iter =  mUnknowns.begin(); iter != mUnknowns.end(); ++iter )
		(*iter)->Release();
}

CString CArxWorkspace::GetUserProfilePrefix() const
{
	resbuf rbProfile = { NULL };
	acedGetVar( _T("CPROFILE"), &rbProfile );
	return CString( _T("Profiles\\") ) +  rbProfile.resval.rstring + _T("\\OpenDCL\\");
}

HMODULE CArxWorkspace::GetThisModule() const
{
	return _hdllInstance;
}

HMODULE CArxWorkspace::GetResourceModule() const
{
	return _hdllInstance;
}

CString CArxWorkspace::GetLanguage(void) const
{
	CString sLanguage = __super::GetLanguage();
	if( !sLanguage.IsEmpty() )
		return sLanguage;
	resbuf rbLocale = { NULL };
	if( RTNORM != acedGetVar( _T("LOCALE"), &rbLocale ) )
		return sLanguage;
	CString sLocale = rbLocale.resval.rstring;
	
	CString sRootFolder;
	int cchPath = ::GetModuleFileName( GetResourceModule(), sRootFolder.GetBuffer( MAX_PATH ), MAX_PATH );
	sRootFolder.ReleaseBuffer( cchPath );
	if( !sRootFolder.IsEmpty() )
	{
		sRootFolder.MakeReverse();
		sRootFolder = sRootFolder.Mid( sRootFolder.SpanExcluding( _T("\\/:") ).GetLength() );
		sRootFolder.MakeReverse();
		if( INVALID_FILE_ATTRIBUTES != GetFileAttributes( sRootFolder + sLocale ) )
			sLanguage = sLocale;
		else
		{ //search for something close
			CString sFind = sRootFolder + sLocale.Left( 2 ) + _T("?.");
			WIN32_FIND_DATA FD;
			HANDLE hFF = FindFirstFile( sFind, &FD );
			if( hFF != INVALID_HANDLE_VALUE )
			{
				sLanguage = FD.cFileName;
				FindClose( hFF );
			}
		}
	}

	return sLanguage;
}

CString CArxWorkspace::FindFile( LPCTSTR pszFilePath ) const
{
	CString sPath;
	if( acedFindFile( pszFilePath, sPath.GetBuffer( MAX_PATH ) ) != RTNORM )
		return CWorkspace::FindFile( pszFilePath );
	sPath.ReleaseBuffer();
	return sPath;
}

bool CArxWorkspace::IsModalFormOpen() const
{
	POSITION posDialog = mDialogs.GetHeadPosition();
	while (posDialog)
	{
		CDialogObject *pDialog = mDialogs.GetNext(posDialog);
		assert(pDialog != NULL);
		if (!pDialog->IsModeless())
			return true;
	}
	return false;
}

ULONG CArxWorkspace::CountOpenModalForms() const
{
	ULONG ctForms = 0;
	POSITION posDialog = mDialogs.GetHeadPosition();
	while (posDialog)
	{
		CDialogObject *pDialog = mDialogs.GetNext(posDialog);
		assert(pDialog != NULL);
		if (!pDialog)
			continue;
		if (!pDialog->IsModeless())
			++ctForms;
	}
	return ctForms;
}

HWND CArxWorkspace::GetTopmostModalForm() const
{
	POSITION pos = mDialogs.GetTailPosition();
	while (pos != NULL)
	{
		CDialogObject *pDialog = mDialogs.GetPrev(pos);
		assert(pDialog != NULL);
		if (pDialog && !pDialog->IsModeless() && pDialog->GetControlWnd()->IsWindowVisible())
			return pDialog->GetHWnd();
	}
	return adsw_acadMainWnd();
}

TArxProjectPtr CArxWorkspace::FindProject( LPCTSTR pszKeyName ) const
{
	for( TProjectList::const_iterator iter = mProjects.begin(); iter != mProjects.end(); ++iter )
	{
		if( (*iter)->GetKeyName().CompareNoCase( pszKeyName ) == 0 )
			return *iter;
	}
	return NULL;
}

bool CArxWorkspace::AddProject( TArxProjectPtr pProject )
{
	if( FindProject( pProject->GetKeyName() ) )
		return false; //can't allow duplicate keys
	pProject->SetProjectLispSymbols();
	mProjects.push_back( pProject );
	return true;
}

bool CArxWorkspace::UnloadProject( TArxProjectPtr pProject, bool bForce )
{
	const TDclFormList& Forms = pProject->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		CDialogObject* pDialog = (*iter)->GetFormInstance();
		if( pDialog )
		{
			if( !bForce )
				return false;
			pDialog->CloseDialog(); //there is an active dialog using this form
			if( (*iter)->GetFormInstance() )
				return false; //for some reason the form didn't close
		}
	}
	for( TProjectList::iterator iter = mProjects.begin(); iter != mProjects.end(); ++iter )
	{
		TArxProjectPtr pThisProject = *iter;
		if( pProject == pThisProject )
		{
			pThisProject->SetProjectLispSymbols( true );
			mProjects.erase( iter );
			return true;
		}
	}
	return true;
}

bool CArxWorkspace::RegisterDialog( CDialogObject* pDialog )
{
	mDialogs.AddTail( pDialog );
	return true;
}

bool CArxWorkspace::UnregisterDialog( CDialogObject* pDialog )
{
	POSITION posDialog = mDialogs.GetHeadPosition();
	while( posDialog != NULL )
	{
		POSITION posAt = posDialog;
		if( pDialog == mDialogs.GetNext( posDialog ) )
		{
			mDialogs.RemoveAt( posAt );
			return true;
		}
	}
	return true;
}

void CArxWorkspace::CloseAllDialogs( DWORD dwMask /*= (DWORD)-1*/ )
{
	POSITION posDialog = mDialogs.GetHeadPosition();
	while (posDialog)
	{
		CDialogObject* pDialog = mDialogs.GetNext(posDialog);
		assert(pDialog != NULL);
		if( dwMask == 0 || ((DWORD( 1 ) << pDialog->GetSourceForm()->GetType()) & dwMask) != 0 )
			pDialog->CloseDialog(); //this call should result in the dialog being removed from the list
	}
}

void CArxWorkspace::ResetLispSymbol( LPCTSTR pszLispSymbol ) const
{
	if( !pszLispSymbol )
		return; //no-op

	if( !acdbCurDwg() )
		return; //zero-doc state, or shutting down

	static struct resbuf rbNIL = { NULL, RTNIL };;
	acedPutSym( pszLispSymbol, &rbNIL );

	TraceFmt( _T("Lisp Symbol %s reset to NIL\r\n"), pszLispSymbol );
}

void CArxWorkspace::SetLispSymbol( LPCTSTR pszLispSymbol, const void* ptr, odcl::PtrType type ) const
{
	if( !pszLispSymbol )
		return; //no-op

	struct resbuf rbPtr = { NULL, RTENAME };;
	rbPtr.resval.rlname[0] = (LONG_PTR)ptr;
	rbPtr.resval.rlname[1] = type;
	acedPutSym( pszLispSymbol, &rbPtr );

	TraceFmt( _T("Lisp Symbol %s set to %p\r\n"), pszLispSymbol, ptr );
}

bool CArxWorkspace::UpdateGlobalLispSymbols() const
{
	bool bFailed = false;
	for( TProjectList::const_iterator iter = mProjects.begin(); iter != mProjects.end(); ++iter )
	{
		TArxProjectPtr pProject = *iter;
		if( !pProject->SetProjectLispSymbols() )
			bFailed = true;
	}
	
	POSITION posDialog = mDialogs.GetHeadPosition();
	while (posDialog != NULL)
	{
		CDialogObject *pDialog = mDialogs.GetNext(posDialog);
		assert( pDialog != NULL);
		TDclFormPtr pSourceForm = pDialog->GetSourceForm();
		assert( pSourceForm != NULL);
		if( !pDialog || !pSourceForm )
			continue;
		if( pSourceForm->GetParentForm() )
			continue; //child forms have the same key name as their parent, so ignore them
		CString sVarName = pSourceForm->GetVarName();
		if (!sVarName.IsEmpty())
			SetLispSymbol( sVarName, (const CDclControlObject*)pDialog->GetSourceForm()->GetControlProperties(), odcl::ptrDclControl );
		pDialog->GetControlPane()->SetGlobalLispSymbols();
	}

	return !bFailed;
}

bool CArxWorkspace::OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM )
{
	for( TProjectList::const_iterator iter = mProjects.begin(); iter != mProjects.end(); ++iter )
	{
		TArxProjectPtr pProject = *iter;
		if( !pProject->OnExtendTabbedDialog( pTabXM ) )
			return false;
	}
	return true;
}

bool CArxWorkspace::AddExtensionTab( TDclFormPtr pDclForm, CAdUiTabExtensionManager* pTabXM )
{
	if( !pTabXM )
		return false; //need to register the tab extension manager first!
	if( pDclForm->GetFormInstance() )
		return true; //it's already there
	assert( pDclForm->GetType() == FrmOptionsTab );

	CDialogObject* pDialog = CArxDialogObject::Create( pDclForm );
	assert( pDialog != NULL );
	if( !pDialog )
		return false;
	CString sTabCaption = pDclForm->GetControlProperties()->GetStringProperty(Prop::OptionsTabCaption);
	if( !pTabXM->AddTab( theWorkspace.GetLocalResourceModule(),
											 IDD_CFGTAB,
											 sTabCaption,
											 (CAdUiTabChildDialog*)pDialog->GetControlWnd() ) )
		return false;
	return true;
}

void CArxWorkspace::UnloadAllProjects()
{
	CloseAllDialogs();
	for( TProjectList::iterator iter = mProjects.begin(); iter != mProjects.end(); ++iter )
		(*iter)->SetProjectLispSymbols( true );
	mProjects.clear();
}

bool CArxWorkspace::UnloadProject( LPCTSTR pszKeyName, bool bForce )
{
	TArxProjectPtr pProject = FindProject( pszKeyName );
	if( !pProject )
		return true; //project is not loaded
	return UnloadProject( pProject, bForce );
}

TDclFormPtr CArxWorkspace::FindDclFormControl( HWND hwndControl, /*out*/ CString& sControlName ) const
{
	sControlName.Empty();
	POSITION pos = mDialogs.GetHeadPosition();
	while (pos != NULL)
	{
		CDialogObject *pDialog = mDialogs.GetNext(pos);
		if (pDialog != NULL)
		{
			if (hwndControl == pDialog->GetHWnd())
				return pDialog->GetControlPane()->GetSourceForm();
			CControlPane* pPane = pDialog->GetControlPane();
			if (pPane->FindControl(hwndControl, sControlName))
				return pPane->GetSourceForm();
		}
	}
	return NULL;
}

CDialogObject* CArxWorkspace::FindDialog( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const
{
	POSITION pos = mDialogs.GetHeadPosition();
	while (pos != NULL)
	{
		CDialogObject *pDialog = mDialogs.GetNext(pos);
		if (pDialog != NULL &&
				pDialog->GetSourceForm()->GetKeyName().CompareNoCase(pszFormName) == 0 &&
				pDialog->GetSourceForm()->GetProject()->GetKeyName().CompareNoCase( pszProjectName ) == 0)
			return pDialog;
	}
	return NULL;
}

TDclControlPtr CArxWorkspace::FindControl( LPCTSTR pszProject, LPCTSTR pszFormName, LPCTSTR pszControl ) const
{
	const TArxProjectPtr pProject = FindProject( pszProject );
	if( !pProject )
		return NULL;
	TDclFormPtr pDclForm = pProject->FindDclForm( pszFormName );
	if( pDclForm )
	{
		TDclControlPtr pControl = pDclForm->FindControl( pszControl );
		if( pControl )
			return pControl;
	}
	return pProject->FindControlWithVarName( CString( pszProject ) + _T('_') + pszFormName + _T('_') + pszControl );
}

TDclFormPtr CArxWorkspace::FindForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const
{
	TArxProjectPtr pProject = FindProject( pszProjectName );
	if( !pProject )
		return NULL;
	TDclFormPtr pDclForm = pProject->FindDclForm( pszFormName );
	if( pDclForm )
		return pDclForm;
	return pProject->FindDclFormWithVarName( CString( pszProjectName ) + _T('_') + pszFormName );
}

void CArxWorkspace::AddUnknown( IUnknown* pUnknown )
{
	if( !pUnknown )
		return;
	mUnknowns.push_back( pUnknown );
}

void CArxWorkspace::RemoveUnknown( IUnknown* pUnknown )
{
	for( std::list< IUnknown* >::iterator iter =  mUnknowns.begin(); iter != mUnknowns.end(); ++iter )
	{
		if( (*iter) == pUnknown )
		{
			(*iter)->Release();
			mUnknowns.erase( iter );
			return;
		}
	}
}

TArxProjectPtr CArxWorkspace::ImportProject( CFile& src, LPCTSTR pszKeyName /*= NULL*/ )
{
	// create a new project
	TArxProjectPtr pProject = new CArxProject( pszKeyName );
	try
	{
		CArchiveEx ar( &src, CArchive::load | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
		pProject->Serialize( ar );
	}
	catch( ... )
	{
		return NULL; 
	}
	if( pProject->GetDclFormList().empty() )
		return NULL; //file had nothing in it

	if( pProject->GetKeyName().IsEmpty() )
		pProject->SetKeyName( CreateUniqueName() );
	UnloadProject( pProject->GetKeyName(), true );
	if( !AddProject( pProject ) )
		return NULL; //couldn't add the new project, probably because of a name collision

	return pProject;
}


bool CArxWorkspace::ExportProject( TArxProjectPtr pProject, CFile& dest )
{
	try
	{
		CArchiveEx ar( &dest, CArchive::store, NULL, _T("ObjectDCL"), TRUE);
		pProject->Serialize( ar );
	}
	catch( ... )
	{
		return false; 
	}

	return true;
}


TArxProjectPtr CArxWorkspace::LoadProjectFile( LPCTSTR pszFilePath, LPCTSTR pszKeyName /*= NULL*/, bool bReload /*= false*/ )
{
	CString sFilePath( pszFilePath );
	if( sFilePath.IsEmpty() )
		return NULL;

	if( !bReload )
	{
		CString sKeyName( pszKeyName );
		if( sKeyName.IsEmpty() )
			sKeyName = StripPathFromFileName( sFilePath ).SpanExcluding( _T(".") );
		sKeyName.Replace( _T(' '), _T('_') );

		TArxProjectPtr pProject = FindProject( sKeyName );
		if( pProject )
			return pProject; //already loaded, just return it
	}

	CString sFoundFile;
	CString sFileName = StripPathFromFileName( sFilePath );
	if( sFileName.Find( _T('.') ) > 0 )
		sFoundFile = FindFile( sFilePath );
	if( sFoundFile.IsEmpty() )
		sFoundFile = FindFile( sFilePath + GetProjectFileExtension() );

	if( sFoundFile.IsEmpty() )
		return NULL; //file not found

	// create a new project
	TArxProjectPtr pProject = new CArxProject( pszKeyName );
	if( pProject->ReadFromFile(sFoundFile) != statOK || //failed to read file
			pProject->GetDclFormList().empty() ) //file had nothing in it
		return NULL; 

	if( bReload )
		UnloadProject( pProject->GetKeyName(), true );
	else if( !pszKeyName || *pszKeyName == _T('\0') )
	{
		TArxProjectPtr pExistingProject = FindProject( pProject->GetKeyName() );
		if( pExistingProject )
			return pExistingProject; //already loaded, just return it
	}
	if( !AddProject( pProject ) )
		return NULL; //couldn't add the new project, probably because of a name collision

	return pProject;
}

FontSettings CArxWorkspace::GetDefaultFontSettings() const
{
	CWinApp* pApp = AfxGetApp();
	static const CString sSection = theWorkspace.GetAppKey();
	FontSettings FS( pApp->GetProfileString( sSection, S_DefaultFontName, NULL ),
									 pApp->GetProfileInt( sSection, S_DefaultFontSize, -10 ),
									 pApp->GetProfileInt( sSection, S_DefaultFontBold, 0 ) != 0,
									 pApp->GetProfileInt( sSection, S_DefaultFontUnderLine, 0 ) != 0,
									 pApp->GetProfileInt( sSection, S_DefaultFontItalic, 0 ) != 0 );
	if( !FS )
		FS.setName( theWorkspace.GetDefaultFontName() );
	if( pApp->GetProfileInt( sSection, S_DefaultFontSizeStyle, 0 ) != 0 )
	{
		if( !FS.isScaled() )
			FS.setSize( -FS.size() ); //if "size style" is non-zero, make the size positive to indicate "point size"
	}
	return FS;
}

//display alert dialog; returns true if displayed, false if suppressed
bool CArxWorkspace::DisplayAlert( LPCTSTR pszMessage ) const
{
	assert(pszMessage != NULL);
	if( IsMessagesSuppressed() )
		return false;
	acedAlert( pszMessage );
	return true;
}

//display modeless status message; returns true if displayed, false if suppressed
bool CArxWorkspace::DisplayStatus( LPCTSTR pszMessage ) const
{
	assert(pszMessage != NULL);
	if( IsMessagesSuppressed() )
		return false;
	acutPrintf( pszMessage );
	return true;
}

int CArxWorkspace::ActivateDclForm( TDclFormPtr pDclForm, DialogParams* pParams /*= NULL*/ )
{
	assert (pDclForm != NULL);
	if( pDclForm->GetParentForm() )
		return -1; //can only activate top level forms from here!
	if( pDclForm->GetType() == FrmOptionsTab )
		return -1; //cannot directly activate config tabs
	CDialogObject* pDlgObject = pDclForm->GetFormInstance();
	if( pDlgObject ) //form already created?
	{ //I think this should result in failure, but for now it just shows the current dialog [ORW]
		pDlgObject->Show();
		if( !pDlgObject->IsModeless() )
			pDlgObject->Focus();
		return pDlgObject->GetID();
	}
	const TProjectPtr pProject = pDclForm->GetProject();
	assert (pProject != NULL);
	if( !pProject )
		return -1;

	CWnd *pParent = NULL;
	if( !pDclForm->IsModeless() )
	{
		::ReleaseCapture();
		POSITION posDialog = mDialogs.GetTailPosition();
		while( posDialog )
		{
			CDialogObject* pDlg = mDialogs.GetPrev( posDialog );
			assert (pDlg != NULL);
			if( !pDlg->IsModeless() )
			{
				pParent = pDlg->GetControlWnd();
				HWND hwndParent = pParent->m_hWnd;
				if( hwndParent)
				{
					MSG msg;
					while( PeekMessage( &msg, pParent->m_hWnd, 0, 0, PM_REMOVE ) )
					{
						TranslateMessage( &msg );
						DispatchMessage( &msg );
						if( msg.message == WM_QUIT )
							break;
					}
				}
				break;
			}
		}
	}
	if( !pParent )
		pParent = CWnd::FromHandle( adsw_acadMainWnd() );
	//if( GetCapture() )
	//	ReleaseCapture();

	CAcAppContextModuleResourceOverride resOverride;
	CDialogObject* pDialog = CArxDialogObject::Create( pDclForm, pParent, pParams );
	assert( pDialog != NULL );
	if( !pDialog )
		return -1;
#ifdef _DEBUG
	//check whether all event handlers are defined, and assert if not
	bool bMissingEventHandler = false;
	class CLispEventHandlerCheck
	{
		bool mbValid;
	public:
		CLispEventHandlerCheck( const ACHAR* pszHandlerName ) : mbValid( false )
			{
				if( !pszHandlerName || !*pszHandlerName )
				{
					mbValid = true;
					return;
				}
				resbuf* prbResult = NULL;
			#if (_ACADTARGET >= 17)
				int acedEvaluateLisp(ACHAR const *, struct resbuf * &result);
				CString sHandlerName( pszHandlerName );
				if( sHandlerName.Left( 2 ).CompareNoCase( _T("C:") ) != 0 )
				{
					sHandlerName = _T("c:");
					sHandlerName += pszHandlerName;
				}
				CString sLisp;
				sLisp.Format( _T("(if (member (type %s) '(SUBR EXSUBR LIST)) 'T)"), (LPCTSTR)sHandlerName );
				acedEvaluateLisp( sLisp, prbResult );
				if( prbResult && prbResult->restype == RTT )
					mbValid = true;
			#else
				if( RTNORM != acedGetSym( pszHandlerName, &prbResult ) ||
						(prbResult && (prbResult->rbnext || prbResult->restype == RTVOID)) )
					mbValid = true;
			#endif //(_ACADTARGET >= 17)
				acutRelRb( prbResult );
			}
		operator bool() const { return mbValid; }
	};
	TDclFormList Forms;
	pDclForm->GetProject()->FindChildForms( pDclForm, Forms );
	Forms.push_front( pDclForm );
	for( TDclFormList::const_iterator iterForm = Forms.begin();
			 iterForm != Forms.end();
			 ++iterForm )
	{
		const TDclControlList& Controls = (*iterForm)->GetControlList();
		for( TDclControlList::const_iterator iterControl = Controls.begin();
				 iterControl != Controls.end();
				 ++iterControl )
		{
			const TPropertyList& Props = (*iterControl)->GetPropertyList();
			for( TPropertyList::const_iterator iterProp = Props.begin();
					 iterProp != Props.end();
					 ++iterProp )
			{
				if( (*iterProp)->GetType() == PropEvent )
				{
					if( !CLispEventHandlerCheck( (*iterProp)->GetStringValue() ) )
					{
						TraceFmt( _T("* Event handler (%s) not defined for (%s / %s)\r\n"),
											(LPCTSTR)(*iterProp)->GetStringValue(),
											(LPCTSTR)(*iterControl)->GetKeyPath(),
											(LPCTSTR)(*iterProp)->GetApiName() );
						bMissingEventHandler = true;
					}
				}
				else if( (*iterProp)->GetType() == PropActiveXEvent )
				{
				}
			}
		}
	}
	assert( !bMissingEventHandler );
#endif //_DEBUG
	UINT nID = pDialog->GetID();
	if( pDialog->IsModeless() )
	{
		HWND hwndActiveWindow = pParent->IsWindowEnabled()? NULL : GetActiveWindow();
		if( !pDialog->CreateModeless( nID ) ) //when this call returns, pDialog is no longer safe! [ORW]
			return -1;
		if( hwndActiveWindow )
			SetActiveWindow( hwndActiveWindow );
		else
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		return nID;
	}
	return pDialog->DoModal();
}
