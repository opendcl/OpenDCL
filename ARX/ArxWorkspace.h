// ArxWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include "ArxProjectCollection.h"
#include "AcadDocReactor.h"
#include "OleControlObject.h"


class CMainFrame;
class CProject;
class CDialogObject;
class CArxProject;
class AxPropertyDescriptor;
class AxMethodDescriptor;
struct DialogParams;


#define theArxWorkspace (*(CArxWorkspace*)&theWorkspace)


class CArxWorkspace : public CWorkspace
{
	CList<CDialogObject*> mDialogs;
	CArxProjectCollection mProjects;
	CStringList	mOdsProjects;
	CAcadDocReactor mDocReactor;

public:
	CArxWorkspace();
	~CArxWorkspace();

public:
	//CWorkspace overrides
	virtual CProject* GetActiveProject() const;
	virtual RefCountedPtr< COleControlObject > GetOleControlFor( const AxPropertyDescriptor* pProperty ) const;
	virtual RefCountedPtr< COleControlObject > GetOleControlFor( const AxMethodDescriptor* pMethod ) const;
	virtual CString GetUserProfilePrefix() const;
	virtual HMODULE GetResourceModule() const;
	virtual HMODULE GetLocalResourceModule() const;
	virtual bool DisplayAlert( LPCTSTR pszMessage ) const; //display alert dialog; returns true if displayed, false if suppressed
	virtual bool DisplayStatus( LPCTSTR pszMessage ) const; //display modeless status message; returns true if displayed, false if suppressed
	virtual CString FindFile( LPCTSTR pszFilePath ) const;

	//Searching
	CArxProject* FindProject( LPCTSTR pszKeyName ) const;
	CDclFormObject* FindDclFormControl( HWND hwndControl, /*out*/ CString& sControlName ) const;
	CDialogObject* FindDialog( LPCTSTR pszProjectName, LPCTSTR pszDialogName ) const;
	CDclControlObject* FindControl( LPCTSTR pszProject, LPCTSTR pszFormName, LPCTSTR pszControl ) const;
	CDclFormObject* FindForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const;
	CArxProject* GetProjectFor( const CDclFormObject* pDclForm ) const;

	//Project management
	bool AddProject( CArxProject* pProject );
	bool UnloadProject( CProject* pProject, bool bForce = false );
	bool UnloadProject( LPCTSTR pszKeyName, bool bForce = false );
	bool RegisterDialog( CDialogObject* pDialog );
	bool UnregisterDialog( CDialogObject* pDialog );
	CArxProject* LoadProjectFile( LPCTSTR pszFilePath, LPCTSTR pszKeyName = NULL, bool bReload = false );

	//Services
	bool IsModalFormOpen() const;
	HWND GetTopmostModalForm() const;
	ULONG CountOpenModalForms() const;
	int ActivateDclForm( CDclFormObject* pDclObject, DialogParams* pParams );
	void CloseAllDialogs( DWORD dwMask = (DWORD)-1 );
	void ResetLispSymbol( LPCTSTR pszLispSymbol ) const;
	void SetLispSymbol( LPCTSTR pszLispSymbol, long lValue ) const;
	bool UpdateGlobalLispSymbols() const;
	bool OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM );
	bool AddExtensionTab( CDclFormObject* pDclForm, CAdUiTabExtensionManager* pTabXM );

	//Attributes
	CString GetActiveProjectName() const;

	const CList<CDialogObject*>& GetDialogList() const { return mDialogs; }
	CList<CDialogObject*>& GetDialogList() { return mDialogs; }
	const CProjectCollection& GetProjectHolder() const { return mProjects; }
	CProjectCollection& GetProjectHolder() { return mProjects; }

protected:
};
