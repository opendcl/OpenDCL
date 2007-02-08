// ArxWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include "ArxProjectCollection.h"
#include "AcadDocReactor.h"


class CMainFrame;
class CProject;
class CDialogObject;
class CArxProject;
class CDclFormObject;
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
	virtual const CDclControlObject* GetArxControlFor( const AxPropertyDescriptor* pProperty ) const;
	virtual const CDclControlObject* GetArxControlFor( const AxMethodDescriptor* pMethod ) const;
	virtual CString LoadResourceString(int nResId, HMODULE hmodRes = NULL) const;
	virtual bool DisplayAlert( LPCTSTR pszMessage ) const; //display alert dialog; returns true if displayed, false if suppressed
	virtual bool DisplayStatus( LPCTSTR pszMessage ) const; //display modeless status message; returns true if displayed, false if suppressed
	virtual CString FindFile( LPCTSTR pszFilePath ) const;

	//Searching
	CArxProject* FindProject( LPCTSTR pszKeyName ) const;
	//CDclFormObject* FindDialog( LPCTSTR pszFormName ) const;
	//CDclFormObject* FindDclForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const;
	CDclFormObject* FindDclFormControl( HWND hwndControl, /*out*/ CString& sControlName ) const;
	//CDialogObject* FindDialog( LPCTSTR pszFormName ) const;
	CDialogObject* FindDialog( LPCTSTR pszProjectName, LPCTSTR pszDialogName ) const;
	//CDialogObject* FindDialog( const CDclFormObject* pDclForm ) const;
	CDclControlObject* FindControl( LPCTSTR pszProject, LPCTSTR pszDialog, LPCTSTR pszControl ) const;
	//const CArxProject* GetDclFormProject( LPCTSTR pszFormName ) const;
	//const CArxProject* GetDialogProject( LPCTSTR pszDialogName ) const;
	CDclFormObject* GetForm( LPCTSTR pszProjectName, LPCTSTR pszFormName ) const;
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
	bool UpdateGlobalVariables() const;
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
