// ArxWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include "ArxProject.h"
#include "OleControlObject.h"
#include <list>


class CMainFrame;
class CProject;
class CDialogObject;
class CArxProject;
class AxPropertyDescriptor;
class AxMethodDescriptor;
struct DialogParams;


#define theArxWorkspace (*(CArxWorkspace*)&theWorkspace)

typedef CList< CDialogObject* > TDialogList;
typedef std::list< TArxProjectPtr > TProjectList;


class CArxWorkspace : public CWorkspace
{
	TDialogList mDialogs;
	TProjectList mProjects;
	class CDocReactor : public AcApDocManagerReactor
	{
		const CArxWorkspace* mpWorkspace;
	public:
		CDocReactor( const CArxWorkspace* pWorkspace )
			: mpWorkspace( pWorkspace )
			{ acDocManager->addReactor( this ); }
		~CDocReactor()
			{ acDocManager->removeReactor( this ); }
		virtual void documentBecameCurrent(AcApDocument* pDoc)
			{ if( mpWorkspace ) mpWorkspace->UpdateGlobalLispSymbols(); }
	} mDocReactor;

public:
	CArxWorkspace();
	~CArxWorkspace();

public:
	//CWorkspace overrides
	virtual CString GetLocalResourceModuleFilename() const { return _T("Runtime.Res.dll"); }
	virtual TOleControlPtr GetOleControlFor( const AxPropertyDescriptor* pProperty );
	virtual TOleControlPtr GetOleControlFor( const AxMethodDescriptor* pMethod );
	virtual CString GetUserProfilePrefix() const;
	virtual HMODULE GetThisModule(void) const;
	virtual HMODULE GetResourceModule() const;
	virtual FontSettings GetDefaultFontSettings() const;
	virtual bool DisplayAlert( LPCTSTR pszMessage ) const; //display alert dialog; returns true if displayed, false if suppressed
	virtual bool DisplayStatus( LPCTSTR pszMessage ) const; //display modeless status message; returns true if displayed, false if suppressed
	virtual CString FindFile( LPCTSTR pszFilePath ) const;

	//Searching
	TArxProjectPtr FindProject( LPCTSTR pszKeyName ) const;
	TDclFormPtr FindDclFormControl( HWND hwndControl, /*out*/ CString& sControlName ) const;
	CDialogObject* FindDialog( LPCTSTR pszProjectName, LPCTSTR pszDialogName ) const;
	TDclControlPtr FindControl( LPCTSTR pszProject, LPCTSTR pszFormName, LPCTSTR pszControl ) const;
	TDclFormPtr FindForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const;

	//Project management
	bool AddProject( TArxProjectPtr pProject );
	bool UnloadProject( TArxProjectPtr pProject, bool bForce = false );
	bool UnloadProject( LPCTSTR pszKeyName, bool bForce = false );
	bool RegisterDialog( CDialogObject* pDialog );
	bool UnregisterDialog( CDialogObject* pDialog );
	TArxProjectPtr ImportProject( CFile& src, LPCTSTR pszKeyName = NULL );
	bool ExportProject( TArxProjectPtr pProject, CFile& src );
	TArxProjectPtr LoadProjectFile( LPCTSTR pszFilePath, LPCTSTR pszKeyName = NULL, bool bReload = false );

	//Services
	bool IsModalFormOpen() const;
	HWND GetTopmostModalForm() const;
	ULONG CountOpenModalForms() const;
	int ActivateDclForm( TDclFormPtr pDclObject, DialogParams* pParams );
	void CloseAllDialogs( DWORD dwMask = (DWORD)-1 );
	void ResetLispSymbol( LPCTSTR pszLispSymbol ) const;
	void SetLispSymbol( LPCTSTR pszLispSymbol, UINT_PTR pValue ) const;
	bool UpdateGlobalLispSymbols() const;
	bool OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM );
	bool AddExtensionTab( TDclFormPtr pDclForm, CAdUiTabExtensionManager* pTabXM );
	void UnloadAllProjects();

	//Attributes
	const TDialogList& GetDialogList() const { return mDialogs; }
	const TProjectList& GetProjects() const { return mProjects; }

protected:
};
