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

CString CArxWorkspace::LoadResourceString(int nResId, HMODULE hmodRes /*= NULL*/) const
{
	//get it from this ARX module if hmodRes is NULL
	return CWorkspace::LoadResourceString( nResId, (hmodRes? hmodRes : _hdllInstance) );
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
	//if( !pDialog->CreateModeless() ) //when this call returns, pDialog is no longer safe! [ORW]
	//	return false;
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

//CDclFormObject* CArxWorkspace::FindDialog( LPCTSTR pszFormName ) const
//{
//	POSITION pos = mDialogs.GetHeadPosition();
//	while (pos != NULL)
//	{
//		CDialogObject *pDialog = mDialogs.GetNext(pos);
//		if (pDialog != NULL && pDialog->GetSourceForm()->GetKeyName().CompareNoCase(pszFormName) == 0)
//			return pDialog->GetSourceForm();
//	}
//	return NULL;
//}
//
//CDclFormObject* CArxWorkspace::FindDclForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const
//{
//	POSITION pos = mDialogs.GetHeadPosition();
//	while (pos != NULL)
//	{
//		CDialogObject *pDialog = mDialogs.GetNext(pos);
//		if (pDialog != NULL &&
//				pDialog->GetSourceForm()->GetKeyName().CompareNoCase(pszFormName) == 0 &&
//				pDialog->GetSourceForm()->mpOwner->GetKeyName().CompareNoCase(pszProjectName) == 0)
//			return pDialog->GetSourceForm();
//	}
//	return NULL;
//}

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

//CDialogObject* CArxWorkspace::FindDialog( LPCTSTR pszFormName ) const
//{
//	POSITION pos = mDialogs.GetHeadPosition();
//	while (pos != NULL)
//	{
//		CDialogObject *pDialog = mDialogs.GetNext(pos);
//		if (pDialog != NULL && pDialog->GetSourceForm()->GetKeyName().CompareNoCase(pszFormName) == 0)
//			return pDialog;
//	}
//	return NULL;
//}

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

//CDialogObject* CArxWorkspace::FindDialog( const CDclFormObject* pDclForm ) const
//{
//	if (!pDclForm)
//		return NULL;
//	POSITION pos = mDialogs.GetHeadPosition();
//	while (pos != NULL)
//	{
//		CDialogObject *pDialog = mDialogs.GetNext(pos);
//		if (pDialog != NULL && pDialog->GetSourceForm() == pDclForm)
//			return pDialog;
//	}
//	return NULL;
//}

CDclControlObject* CArxWorkspace::FindControl( LPCTSTR pszProject, LPCTSTR pszDialog, LPCTSTR pszControl ) const
{
	CArxProject* pProject = FindProject( pszProject );
	if( !pProject )
		return NULL;
	CDclFormObject* pDclForm = pProject->GetDclForm( pszDialog );
	if( !pDclForm )
		return NULL;
	return pDclForm->FindControl( pszControl );
}

//const CArxProject* CArxWorkspace::GetDclFormProject( LPCTSTR pszFormName ) const
//{
//	const CDclFormObject* pForm = FindDclForm( pszFormName );
//	if( !pForm )
//		return NULL;
//	return reinterpret_cast<const CArxProject*>(pForm->mpOwner);
//}

//const CArxProject* CArxWorkspace::GetDialogProject( LPCTSTR pszDialogName ) const
//{
//	const CDialogObject* pDialog = FindDialog( pszDialogName );
//	if( !pDialog )
//		return NULL;
//	return reinterpret_cast<const CArxProject*>(pDialog->m_pProject);
//}

CDclFormObject* CArxWorkspace::GetForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const
{
	CArxProject* pProject = FindProject( pszProjectName );
	if( !pProject )
		return NULL;
	return pProject->GetDclForm( pszFormName );
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
	pProject->m_ShortFileName = sFileName;
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

	CDialogObject* pDialog = CArxDialogObject::Create( pDclForm, pParent, pParams );
	assert( pDialog != NULL );
	if( !pDialog )
		return -1;
	UINT nID = pDialog->GetID();
	CAcModuleResourceOverride resOverride;
	if( pDialog->IsModeless() )
	{
		if( !pDialog->CreateModeless() ) //when this call returns, pDialog is no longer safe! [ORW]
			return -1;
		return nID;
	}
	return pDialog->DoModal();


	//CDialogObject* pDialog = NULL;
	//// determine what type of dlg to show and show it.
	//switch (pDclForm->GetType())
	//{
	//case VdclDockable:
	//	{
	//		pDialog = FindDialog( pDclForm );
	//		if (pDialog)
	//		{
	//			bool bCancel = true;
	//			if (pDialog->m_pResizableDockingDialog != NULL)
	//			{
	//				if (pDialog->m_pResizableDockingDialog->m_bClosing == true)
	//				{
	//					pDialog->m_pResizableDockingDialog->CloseWindow();
	//					delete pDialog->m_pResizableDockingDialog;		
	//					pDialog->m_pResizableDockingDialog = NULL;		
	//					RemoveDialog(pDialog); //added 2007-01-28 [ORW]
	//					bCancel = false;
	//				}
	//			}
	//			if (pDialog->m_pDockingDialog != NULL)
	//			{
	//				if (pDialog->m_pDockingDialog->m_bClosing == true)
	//				{
	//					pDialog->m_pDockingDialog->CloseWindow();
	//					delete pDialog->m_pDockingDialog;		
	//					pDialog->m_pDockingDialog = NULL;		
	//					RemoveDialog(pDialog); //added 2007-01-28 [ORW]
	//					bCancel = false;
	//				}
	//			}
	//			if (bCancel)
	//			{					
	//				AcApDocument* pDoc = acDocManager->curDocument();
	//				if (pDoc)
	//				{
	//					// give the command line focus {I'm not sure why this is necessary or desirable [ORW]}
	//					CWnd* wndCommandLine = acedGetAcadDockCmdLine();
	//					if (wndCommandLine)
	//						wndCommandLine->SetFocus();		

	//					// send the string to the current document
	//					acDocManager->sendStringToExecute(pDoc, _T("\x1B\x1B"), false, true, false); //send double cancel
	//				}
	//				return 0;	
	//			}
	//		}

	//		// create and add a new dialog object to the form holder
	//		// the mDialogs keeps track of the open dialogs for future
	//		// reference and modification
	//		pDialog = new CDialogObject( pDclForm, GetNextDialogId() );			
	//		mDialogs.AddTail(pDialog);
	//	
	//		// get the AutoCAD Frame to be passed in the create method
	//		CMDIFrameWnd* pAcadFrame = acedGetAcadFrame();
	//		
	//		// set the new dialog box	
	//		CDclControlObject* pProperties = pDclForm->GetControlProperties();
	//		if (!pProperties)
	//			return -1;

	//		BOOL bResizable = pProperties->GetBoolProperty(nResizable);
	//		int nDocHeight = pProperties->GetLngProperty(nHeight);
	//	
	//		DWORD dwDockableSides = 0;
	//		DWORD dwDefaultDockableSide = 0;

	//		switch (pProperties->GetLngProperty(nDockableSides))
	//		{
	//		case 1:
	//			// set the form to only dock on the top side
	//			dwDockableSides = CBRS_ALIGN_TOP;
	//			dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
	//			nDocHeight += 8;
	//			break;
	//		case 2:
	//			// set the form to only dock on the bottom side
	//			dwDockableSides = CBRS_ALIGN_BOTTOM;
	//			dwDefaultDockableSide = AFX_IDW_DOCKBAR_BOTTOM;
	//			nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
	//			break;
	//		case 3:
	//			// set the form to only dock on the top or bottom sides
	//			dwDockableSides = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;				
	//			dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
	//			nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
	//			break;
	//		case 4:
	//			// set the form to only dock on the any side
	//			dwDockableSides = AFX_IDW_DOCKBAR_TOP | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP;				
	//			dwDefaultDockableSide = CBRS_ALIGN_LEFT;
	//			nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
	//			break;
	//		case 5:
	//			// set the form to only dock on the any side
	//			dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;
	//			dwDefaultDockableSide = AFX_IDW_DOCKBAR_TOP;
	//			nDocHeight -= ::GetSystemMetrics(SM_CYSMCAPTION) - 4;
	//			break;
	//		default:
	//			// set the form to only dock on the left or right sides
	//			dwDockableSides = CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT;
	//			dwDefaultDockableSide = AFX_IDW_DOCKBAR_LEFT;
	//			break;
	//		}
	//		
	//		if (bResizable)
	//		{
	//			pDialog->m_pResizableDockingDialog = new CResizableDockingDialog(pDclForm);
	//			pDialog->m_pResizableDockingDialog->m_pFontCollection = &theWorkspace.GetFontCollection();

	//			// call the form's method to set the pointer to the loaded dcl form
	//			pDialog->m_pResizableDockingDialog->SetDclForm(pDclForm);
	//			
	//			// set the names for the dlg
	//			pDialog->m_pResizableDockingDialog->m_sProjectName = pProject->GetKeyName();
	//			pDialog->m_pResizableDockingDialog->m_sDialogName = pDclForm->GetKeyName();
	//			
	//			CRect rect = CFrameWnd::rectDefault;
	//			if (rect.left < 0)
	//				rect.left = 0;
	//			if (rect.top < 0)
	//				rect.top = 0;				
	//			rect.bottom = rect.top + nDocHeight;
	//			rect.right = rect.left + pProperties->GetLngProperty(nWidth);
	//			
	//			// create the docking dialog
	//			CAcModuleResourceOverride resOverride;
	//			pDialog->m_pResizableDockingDialog->Create(pAcadFrame, _T("ObjectDCLDock"), rect);		
	//			if (pDclForm->m_UUID.GetLength() == 0)
	//			{
	//				UUID uuid;
	//				UuidCreate(&uuid);
	//				pDialog->m_pResizableDockingDialog->SetToolID(&uuid);
	//			}
	//			else
	//			{
	//				UUID uuid = pDclForm->getUUID();
	//				pDialog->m_pResizableDockingDialog->SetToolID (&uuid);
	//			}
	//			// set the form to only dock on the set side(s)
	//			pDialog->m_pResizableDockingDialog->EnableDocking(dwDockableSides);
	//			pDialog->m_pResizableDockingDialog->m_bClosing = false;
	//			// loads the dockable form but does not display it
	//			pDialog->m_pResizableDockingDialog->RestoreControlBar(dwDefaultDockableSide);				
	//		}
	//		else /* non resizable dialog */
	//		{
	//			pDialog->m_pDockingDialog = new CDockingDialog(pDclForm);
	//			pDialog->m_pDockingDialog->m_pFontCollection = &theWorkspace.GetFontCollection();

	//			pDialog->m_pDockingDialog->SetDclForm(pDclForm);
	//			
	//			// set the names for the dlg
	//			pDialog->m_pDockingDialog->m_sProjectName = pProject->GetKeyName();
	//			pDialog->m_pDockingDialog->m_sDialogName = pDclForm->GetKeyName();
	//			
	//			CRect rect = CFrameWnd::rectDefault;
	//			if (rect.left < 0)
	//				rect.left = 0;
	//			if (rect.top < 0)
	//				rect.top = 0;				
	//			rect.bottom = rect.top + nDocHeight;
	//			rect.right = rect.left + pProperties->GetLngProperty(nWidth);

	//			// create the docking dialog
	//			CAcModuleResourceOverride resOverride;
	//			pDialog->m_pDockingDialog->Create( pAcadFrame, _T("ObjectDCLDock"), rect);		
	//			if (pDclForm->m_UUID.GetLength() == 0)
	//			{
	//				UUID uuid;
	//				UuidCreate(&uuid);
	//				pDialog->m_pDockingDialog->SetToolID (&uuid);
	//			}
	//			else
	//			{
 //         UUID uuid = pDclForm->getUUID();
	//				pDialog->m_pDockingDialog->SetToolID (&uuid);
	//			}			
	//			// set the form to only dock on the set side(s)
	//			pDialog->m_pDockingDialog->EnableDocking(dwDockableSides);
	//			pDialog->m_pDockingDialog->m_bClosing = false;
	//			// loads the dockable form but does not display it
	//			pDialog->m_pDockingDialog->RestoreControlBar(dwDefaultDockableSide);
	//		}
	//		break;
	//	}
	//case VdclModeless:
	//	{
	//		// create and add a new dialog object to the form holder
	//		// mDialogs keeps track of the open dialogs for future
	//		// reference and modification
	//		pDialog = new CDialogObject( pDclForm, GetNextDialogId() );			
	//		mDialogs.AddTail(pDialog);

	//		// set the new dialog box	
	//		pDialog->m_pModelessDialog = new CModelessDlg(pDclForm, CWnd::FromHandle(adsw_acadMainWnd()), nX, nY);
	//		
	//		pDialog->m_pModelessDialog->m_pFontCollection = &theWorkspace.GetFontCollection();
	//		
	//		// set the names for the dlg
	//		pDialog->m_pModelessDialog->m_sProjectName = pProject->GetKeyName();
	//		pDialog->m_pModelessDialog->m_sDialogName = pDclForm->GetKeyName();
	//		
	//		// call the form's method to set the pointer to the loaded dcl form
	//		pDialog->m_pModelessDialog->SetDclForm(pDclForm);
	//		
	//		// get the arx object that stores the dcl form's properties
	//		CDclControlObject *pDclProperties = pDclForm->GetControlProperties();

	//		// invoke as modeless with the right dialog resource
	//		CAcModuleResourceOverride resOverride;		
	//		if (pDclProperties->GetBoolProperty(nResizable))
	//		{
	//			BOOL bReturnValue = pDialog->m_pModelessDialog->Create(IDD_RESIZEABLE);
	//			pDialog->m_pModelessDialog->m_bShowGrip = true;
	//		}
	//		else
	//		{
	//			BOOL bReturnValue = pDialog->m_pModelessDialog->Create(IDD_MODALDIALOG);
	//			pDialog->m_pModelessDialog->m_bShowGrip = false;
	//		}

	//		pDialog->m_pModelessDialog->ShowWindow(TRUE);
	//		break;
	//	}
	//case VdclFileDialog:
	//	{
	//		// create and add a new dialog object to the form holder
	//		// mDialogs keeps track of the open dialogs for future
	//		// reference and modification
	//		pDialog = new CDialogObject( pDclForm, GetNextDialogId() );			
	//		mDialogs.AddTail(pDialog);
	//		
	//		// get the arx object that stores the dcl form's properties
	//		CDclControlObject* pDclProperties = pDclForm->GetControlProperties();
	//		if (!pDclProperties)
	//			return -1;

	//		CDclControlObject* pFileDlgProperties = pDclForm->FindFirstControlOfType(CtlFileDlgCtrl);
	//		if (!pFileDlgProperties)
	//			return -1;

	//		// set the new dialog box	
	//		CWnd *pParent = NULL;
	//		if (CountOpenModalForms() > 1)
	//		{
	//			POSITION posDialog = mDialogs.GetTailPosition();
	//			while (posDialog)
	//			{
	//				CDialogObject* pDlg = mDialogs.GetPrev(posDialog);
	//				assert (pDlg != NULL);
	//				if (!pDlg)
	//					continue;
	//				if (pDlg && pDlg->m_pModalDialog != NULL)
	//				{
	//					pParent = CWnd::FromHandle(pDlg->m_pModalDialog->m_hWnd);
	//					break;
	//				}
	//			}
	//		}
	//		if( !pParent)
	//			pParent = CWnd::FromHandle(adsw_acadMainWnd());
	//		
	//		// get the flags for the CFileDialog
	//		DWORD dwStyle = GetFileDlgFlags(pDclProperties, pFileDlgProperties);
	//
	//		BOOL bOpen = pFileDlgProperties->GetLngProperty(nFileDlgStyle); 

	//		// create the open dialog box
	//		CAcModuleResourceOverride resOverride;		
	//		pDialog->m_pFileDialog = new CParentFileDialog(
	//			pDclForm,
	//			bOpen, 
	//			NULL,
	//			pszDefaultFileName, 
	//			dwStyle,
	//			pFileDlgProperties->GetStrProperty(nFilter),
	//			pParent);

	//		pDialog->m_pFileDialog->m_pFontCollection = &theWorkspace.GetFontCollection();
	//		pDialog->m_pFileDialog->m_pFileDlgProps = pFileDlgProperties;
	//		
	//		// set the names for the dlg
	//		pDialog->m_pFileDialog->m_sProjectName = pProject->GetKeyName();
	//		pDialog->m_pFileDialog->m_sDialogName = pDclForm->GetKeyName();
	//		
	//		// call the form's method to set the pointer to the loaded dcl form
	//		pDialog->m_pFileDialog->SetDclForm(pDclForm);
	//		
	//		if (pszDefaultDirectory)
	//			pDialog->m_pFileDialog->m_ofn.lpstrInitialDir = pszDefaultDirectory;

	//		CString sTitle = pDclProperties->GetStrProperty(nTitleBarText);
	//		if (sTitle.GetLength() > 0)
	//			pDialog->m_pFileDialog->m_ofn.lpstrTitle = sTitle;

	//		// proceed to setup the file buffer size
	//		pDialog->m_pFileDialog->m_ofn.nMaxFile = MAX_PATH;
	//		TCHAR* pc = new TCHAR[MAX_PATH];
	//		pDialog->m_pFileDialog->m_ofn.lpstrFile = pc;
	//		pDialog->m_pFileDialog->m_ofn.lpstrFile[0] = NULL;
	//		
	//		CStringArray sListFileName;
	//		sListFileName.SetSize(0,1);
	//		// set the pointer to the string array object
	//		pDialog->m_pFileDialog->m_pStrList = &sListFileName;
	//		
	//		int nReturnValue = pDialog->m_pFileDialog->DoModal();
	//		
	//		if (nReturnValue != IDOK)
	//			return 0;

	//		if (pFileDlgProperties->GetLngProperty(nFileDlgStyle) == 1) // as open
	//		{
	//			if (pFileDlgProperties->GetBoolProperty(nMultiSelect) == TRUE)
	//			{
	//				// Convert the array to a list that can be returned
	//				struct resbuf* prbRetList = acutNewRb(RTSTR);
	//				struct resbuf* prbTail = prbRetList;
 //
	//				int nCount = sListFileName.GetSize();
	//				for (int i=0; i<nCount; i++)
	//				{
	//					// get the text name of the selected line number
	//					CString sFileName = sListFileName.GetAt(i);
	//					prbTail->rbnext = acutNewRb(RTSTR);
	//					prbTail = prbTail->rbnext;
	//					acutNewString(sFileName, prbTail->resval.rstring);
	//				}

	//				acedRetList(prbRetList);
	//				acutRelRb(prbRetList);

	//				// cleanup
	//				delete pDialog->m_pFileDialog;
	//				delete pDialog->m_pModalDialog;
	//				RemoveDialog(pDialog);
	//				return 0;
	//			}
	//		}
	//		
	//		acedRetStr(pDialog->m_pFileDialog->m_sFileName);

	//		// cleanup
	//		delete pDialog->m_pFileDialog;
	//		delete pDialog->m_pModalDialog;
	//		RemoveDialog(pDialog);
	//		break;
	//	}
	//
	//case VdclModal:
	//	{			
	//		// create and add a new dialog object to the form holder
	//		// the theArxWorkspace.GetDialogList() keeps track of the open dialogs for future
	//		// reference and modification
	//		pDialog = new CDialogObject( pDclForm, GetNextDialogId() );			
	//		mDialogs.AddTail(pDialog);
	//		
	//		// get the arx object that stores the dcl form's properties
	//		CDclControlObject *pDclProperties = pDclForm->GetControlProperties();
	//		// set the form's resizing using the dialog resource
	//		if (pDclProperties->GetBoolProperty(nResizable))
	//			mnSnapDlgId = IDD_RESIZEABLE;
	//		else
	//			mnSnapDlgId = IDD_MODALDIALOG;

	//		// set the new dialog box	
	//		CWnd *pParent = NULL;
	//		if (CountOpenModalForms() > 1)
	//		{
	//			POSITION posDialog = mDialogs.GetTailPosition();
	//			while (posDialog)
	//			{
	//				CDialogObject* pDlg = mDialogs.GetPrev(posDialog);
	//				assert (pDlg != NULL);
	//				if (!pDlg)
	//					continue;
	//				if (pDlg && pDlg->m_pModalDialog != NULL)
	//				{
	//					pParent = CWnd::FromHandle(pDlg->m_pModalDialog->m_hWnd);
	//					break;
	//				}
	//			}
	//		}
	//		if( !pParent)
	//			pParent = CWnd::FromHandle(adsw_acadMainWnd());

	//		pDialog->m_pModalDialog = new CModalVDcl(pDclForm, pParent, nX, nY);
	//		
	//		// set the form's resizing
	//		if (pDclProperties->GetBoolProperty(nResizable))
	//			pDialog->m_pModalDialog->m_bShowGrip = true;
	//		else
	//			pDialog->m_pModalDialog->m_bShowGrip = false;			

	//		pDialog->m_pModalDialog->m_pFontCollection = &theWorkspace.GetFontCollection();
	//		
	//		// set the names for the dlg
	//		pDialog->m_pModalDialog->m_sProjectName = pProject->GetKeyName();
	//		pDialog->m_pModalDialog->m_sDialogName = pDclForm->GetKeyName();
	//		
	//		// call the form's method to set the pointer to the loaded dcl form
	//		pDialog->m_pModalDialog->SetDclForm(pDclForm);			
	//		
	//		// invoke as modal
	//		CAcModuleResourceOverride resOverride;		
	//		int nReturnValue = pDialog->m_pModalDialog->DoModal();

	//		// cleanup
	//		delete pDialog->m_pFileDialog;
	//		delete pDialog->m_pModalDialog;
	//		RemoveDialog(pDialog);
	//		break;
	//	}
	//}
	//if (pDialog != NULL)
	//	return pDialog->GetID();
	//return -1;
}
