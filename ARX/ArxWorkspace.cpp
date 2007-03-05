// ArxWorkspace.cpp : implementation file
//

#include "stdafx.h"
#include "ArxWorkspace.h"
#include "ArxProject.h"
#include "Resource.h"
#include "ArxDialogObject.h"
#include "DclFormObject.h"
#include "ErrorLexicon.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "InvokeMethod.h"
#include "DclControlObject.h"


CWorkspace* theWorkspacePtr()
{
	static CArxWorkspace workspace;
	return &workspace;
}


static CString StripPathFromFileName( LPCTSTR pszFilePath )
{
	CString sShortName = pszFilePath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


CArxWorkspace::CArxWorkspace()
{
	mDocReactor.m_bRefreshGlobalVariables = true;
	acDocManager->addReactor(&mDocReactor);
}

CArxWorkspace::~CArxWorkspace()
{
	acDocManager->removeReactor(&mDocReactor);
}

CString CArxWorkspace::GetUserProfilePrefix() const
{
	resbuf rbProfile = { NULL };
	acedGetVar( _T("CPROFILE"), &rbProfile );
	return CString( _T("Profiles\\") ) +  rbProfile.resval.rstring + _T("\\OpenDCL\\");
}

HMODULE CArxWorkspace::GetResourceModule() const
{
	return _hdllInstance;
}

HMODULE CArxWorkspace::GetLocalResourceModule() const
{
	return _hdllInstance;
}

CProject* CArxWorkspace::GetActiveProject() const
{
	return mProjects.mProjectCollection.GetTail();
}

CString CArxWorkspace::FindFile( LPCTSTR pszFilePath ) const
{
	CString sPath;
	if( acedFindFile( pszFilePath, sPath.GetBuffer( MAX_PATH ) ) == RTNORM )
		return CWorkspace::FindFile( pszFilePath );
	sPath.ReleaseBuffer();
	return sPath;
}

CString CArxWorkspace::GetActiveProjectName() const
{
	CString sShortFileName = StripPathFromFileName(GetActiveProject()->GetKeyName());
	if (sShortFileName[sShortFileName.GetLength()-4] == _T('.'))
		sShortFileName = sShortFileName.Left(sShortFileName.GetLength()-4);
	return sShortFileName;
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
		if (pDialog != NULL)
		{
			if (!pDialog->IsModeless())
				return pDialog->GetHWnd();
		}
	}
	return adsw_acadMainWnd();
}

CArxProject* CArxWorkspace::FindProject( LPCTSTR pszKeyName ) const
{
	POSITION posProject = mProjects.mProjectCollection.GetHeadPosition();
	while( posProject != NULL )
	{
		CProject* pProject = mProjects.mProjectCollection.GetNext( posProject );
		if( pProject->GetKeyName().CompareNoCase( pszKeyName ) == 0 )
			return static_cast<CArxProject*>(pProject);
	}
	return NULL;
}

bool CArxWorkspace::AddProject( CArxProject* pProject )
{
	if( FindProject( pProject->GetKeyName() ) )
		return false; //can't allow duplicate keys
	pProject->SetProjectLispSymbols();
	mProjects.mProjectCollection.AddTail( pProject );
	return true;
}

bool CArxWorkspace::UnloadProject( CProject *pProject, bool bForce )
{
	POSITION posForm = pProject->GetDclFormList().GetHeadPosition();
	while( posForm )
	{
		CDclFormObject* pForm = pProject->GetDclFormList().GetNext( posForm );
		assert( pForm != NULL );
		if( !pForm )
			continue;
		CDialogObject* pDialog = pForm->GetFormInstance();
		if( pDialog )
		{
			if( !bForce )
				return false;
			pDialog->CloseDialog(); //there is an active dialog using this form
			if( pForm->GetFormInstance() )
				return false; //for some reason the form didn't close
		}
	}
	POSITION posProject = mProjects.mProjectCollection.GetHeadPosition();
	while( posProject != NULL )
	{
		POSITION posAt = posProject;
		if( pProject == mProjects.mProjectCollection.GetNext( posProject ) )
		{
			mProjects.mProjectCollection.RemoveAt( posAt );
			((CArxProject*)pProject)->SetProjectLispSymbols( true );
			delete pProject;
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

#ifdef _DEBUG
	TraceFmt( _T("Lisp Symbol %s reset to NIL\r\n"), pszLispSymbol );
#endif
}

void CArxWorkspace::SetLispSymbol( LPCTSTR pszLispSymbol, long lValue ) const
{
	if( !pszLispSymbol )
		return; //no-op

	struct resbuf rbLong = { NULL, RTLONG };;
	rbLong.resval.rlong = lValue;
	acedPutSym( pszLispSymbol, &rbLong );

#ifdef _DEBUG
	TraceFmt( _T("Lisp Symbol %s set to %08x\r\n"), pszLispSymbol, lValue );
#endif
}

bool CArxWorkspace::UpdateGlobalLispSymbols() const
{
	bool bFailed = false;
	POSITION posProject = mProjects.mProjectCollection.GetHeadPosition();
	while (posProject != NULL)
	{
		CArxProject* pProject = (CArxProject*)mProjects.mProjectCollection.GetNext(posProject);
		assert( pProject != NULL);
		if( !pProject )
			continue;
		if( !pProject->SetProjectLispSymbols() )
			bFailed = true;
	}
	
	POSITION posDialog = mDialogs.GetHeadPosition();
	while (posDialog != NULL)
	{
		CDialogObject *pDialog = mDialogs.GetNext(posDialog);
		assert( pDialog != NULL);
		assert( pDialog->GetSourceForm() != NULL);
		if( !pDialog || !pDialog->GetSourceForm() )
			continue;
		CString sVarName = pDialog->GetSourceForm()->GetControlProperties()->GetStrProperty(nGlobalVarName);
		if (!sVarName.IsEmpty())
			SetLispSymbol(sVarName, (long)pDialog->GetSourceForm());
		pDialog->GetControlPane().SetGlobalLispSymbols();
	}

	return !bFailed;
}

bool CArxWorkspace::OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM )
{
	POSITION posProject = mProjects.mProjectCollection.GetHeadPosition();
	while (posProject)
	{
		CArxProject *pProject = (CArxProject*)mProjects.mProjectCollection.GetNext(posProject);
		if (pProject && !pProject->OnExtendTabbedDialog( pTabXM ))
			return false;
	}
	return true;
}

bool CArxWorkspace::AddExtensionTab( CDclFormObject* pDclForm, CAdUiTabExtensionManager* pTabXM )
{
	if( !pTabXM )
		return false; //need to register the tab extension manager first!
	if( pDclForm->GetFormInstance() )
		return true; //it's already there
	assert( pDclForm->GetType() == VdclConfigTab );

	CDialogObject* pDialog = CArxDialogObject::Create( pDclForm );
	assert( pDialog != NULL );
	if( !pDialog )
		return false;
	CString sTabCaption = pDclForm->GetControlProperties()->GetStrProperty(nCfgTabCaption);
	if (!pTabXM->AddTab(_hdllInstance,
											IDD_CFGTAB,
											sTabCaption,
											(CAdUiTabChildDialog*)pDialog->GetWindow()))
		return false;
	return true;
}

bool CArxWorkspace::UnloadProject( LPCTSTR pszKeyName, bool bForce )
{
	CProject* pProject = FindProject( pszKeyName );
	if( !pProject )
		return true; //project is not loaded
	return UnloadProject( pProject, bForce );
}

CDclFormObject* CArxWorkspace::FindDclFormControl( HWND hwndControl, /*out*/ CString& sControlName ) const
{
	sControlName.Empty();
	POSITION pos = mDialogs.GetHeadPosition();
	while (pos != NULL)
	{
		CDialogObject *pDialog = mDialogs.GetNext(pos);
		if (pDialog != NULL)
		{
			if (hwndControl == pDialog->GetHWnd())
				return pDialog->GetControlPane().GetSourceForm();
			CControlPane& Pane = pDialog->GetControlPane();
			if (Pane.FindControl(hwndControl, sControlName))
				return Pane.GetSourceForm();
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

CDclControlObject* CArxWorkspace::FindControl( LPCTSTR pszProject, LPCTSTR pszFormName, LPCTSTR pszControl ) const
{
	CArxProject* pProject = FindProject( pszProject );
	if( !pProject )
		return NULL;
	CDclFormObject* pDclForm = pProject->FindDclForm( pszFormName );
	if( pDclForm )
	{
		CDclControlObject* pControl = pDclForm->FindControl( pszControl );
		if( pControl )
			return pControl;
	}
	return pProject->FindControlWithVarName( CString( pszProject ) + _T('_') + pszFormName + _T('_') + pszControl );
}

CDclFormObject* CArxWorkspace::FindForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const
{
	CArxProject* pProject = FindProject( pszProjectName );
	if( !pProject )
		return NULL;
	CDclFormObject* pDclForm = pProject->FindDclForm( pszFormName );
	if( pDclForm )
		return pDclForm;
	return pProject->FindDclFormWithVarName( CString( pszProjectName ) + _T('_') + pszFormName );
}

CArxProject* CArxWorkspace::GetProjectFor( const CDclFormObject *pDclObject ) const
{
	CProject* pProject = const_cast<CProject*>(pDclObject->GetProject());
	return static_cast<CArxProject*>(pProject);
}

RefCountedPtr< COleControlObject > CArxWorkspace::GetOleControlFor( const AxPropertyDescriptor* pProperty ) const
{
	POSITION pos = mProjects.mProjectCollection.GetHeadPosition();
	while (pos != NULL)
	{
		CProject* pProject = mProjects.mProjectCollection.GetNext(pos);
		if (pProject != NULL)
		{
			RefCountedPtr< COleControlObject > pObject = pProject->GetOleObject(pProperty);
			if (pObject != NULL)
				return pObject;
		}
	}
	return NULL;
}

RefCountedPtr< COleControlObject > CArxWorkspace::GetOleControlFor( const AxMethodDescriptor* pMethod ) const
{
	POSITION pos = mProjects.mProjectCollection.GetHeadPosition();
	while (pos != NULL)
	{
		CProject* pProject = mProjects.mProjectCollection.GetNext(pos);
		if (pProject != NULL)
		{
			RefCountedPtr< COleControlObject > pObject = pProject->GetOleObject(pMethod);
			if (pObject != NULL)
				return pObject;
		}
	}
	return NULL;
}


CArxProject* CArxWorkspace::LoadProjectFile( LPCTSTR pszFilePath, LPCTSTR pszKeyName /*= NULL*/, bool bReload /*= false*/ )
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

		CArxProject* pProject = FindProject( sKeyName );
		if( pProject )
			return pProject; //already loaded, just return it
	}

	CString sFoundFile;
	CString sFileName = StripPathFromFileName( sFilePath );
	if( sFileName.Find( _T('.') ) > 0 )
		sFoundFile = FindFile( sFilePath );
	if( sFoundFile.IsEmpty() )
	{
		sFoundFile = FindFile( sFilePath + GetProjectFileExtension() );
		if( sFoundFile.IsEmpty() )
			sFoundFile = FindFile( sFilePath + GetDistributionFileExtension() );
	}

	if( sFoundFile.IsEmpty() )
		return NULL; //file not found

	// create a new project
	CArxProject* pProject = new CArxProject( pszKeyName );
	if (pProject->ReadFromFile(sFoundFile) != statOK || //failed to read file
			pProject->GetDclFormList().GetCount() == 0) //file had nothing in it
	{		
		delete pProject;
		return NULL; 
	}
	if( sFileName.Right( GetDistributionFileExtension().GetLength() ).CompareNoCase( GetDistributionFileExtension() ) == 0 )
		mOdsProjects.AddTail( CString( pProject->GetKeyName() ).MakeLower() );

	if( bReload )
		UnloadProject( pProject->GetKeyName(), true );
	if( !AddProject( pProject ) )
	{ //couldn't add the new project, probably because of a name collision
		delete pProject;
		return NULL;
	}

	return pProject;
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

int CArxWorkspace::ActivateDclForm( CDclFormObject* pDclForm, DialogParams* pParams /*= NULL*/ )
{
	assert (pDclForm != NULL);
	CDialogObject* pDlgObject = pDclForm->GetFormInstance();
	if( pDlgObject ) //form already created?
	{ //I think this should result in failure, but for now it just shows the current dialog [ORW]
		pDlgObject->Show();
		if( !pDlgObject->IsModeless() )
			pDlgObject->SetFocus();
		return pDlgObject->GetID();
	}
	if( pDclForm->GetParentForm() )
		return -1; //can only activate top level forms from here!
	const CProject* pProject = pDclForm->GetProject();
	assert (pProject != NULL);
	if (!pProject)
		return -1;

	CWnd *pParent = NULL;
	if (CountOpenModalForms() > 1)
	{
		POSITION posDialog = mDialogs.GetTailPosition();
		while (posDialog)
		{
			CDialogObject* pDlg = mDialogs.GetPrev(posDialog);
			assert (pDlg != NULL);
			if (!pDlg->IsModeless())
			{
				pParent = CWnd::FromHandle(pDlg->GetHWnd());
				break;
			}
		}
	}
	if( !pParent)
		pParent = CWnd::FromHandle(adsw_acadMainWnd());

	CAcModuleResourceOverride resOverride;
	CDialogObject* pDialog = CArxDialogObject::Create( pDclForm, pParent, pParams );
	assert( pDialog != NULL );
	if( !pDialog )
		return -1;
	UINT nID = pDialog->GetID();
	if( pDialog->IsModeless() )
	{
		if( !pDialog->CreateModeless( nID ) ) //when this call returns, pDialog is no longer safe! [ORW]
			return -1;
		return nID;
	}
	return pDialog->DoModal();
}
