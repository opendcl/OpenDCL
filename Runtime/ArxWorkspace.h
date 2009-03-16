// ArxWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include "ArxProject.h"
#include <list>


class CMainFrame;
class CProject;
class CDialogObject;
class CArxProject;
class AxPropertyDescriptor;
class AxMethodDescriptor;
struct DialogParams;


namespace odcl
{
	enum PtrType
	{
		ptrHandle,
		ptrDclProject,
		ptrDclControl,
		ptrIUnknown,
		ptrImageList,
		ptrBinFile,
	};
};


#define theArxWorkspace (*(CArxWorkspace*)&theWorkspace)

typedef CList< CDialogObject* > TDialogList;
typedef std::list< TArxProjectPtr > TProjectList;


class CArxWorkspace : public CWorkspace
{
	TDialogList mDialogs;
	TProjectList mProjects;
	std::list< IUnknown* > mUnknowns;
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
	virtual CString GetSettingsRegPath(void) const { return _T("Software\\OpenDCL\\Runtime"); }
	virtual CString GetUserProfilePrefix() const;
	virtual HMODULE GetThisModule(void) const;
	virtual HMODULE GetResourceModule() const;
	virtual CString GetLanguage(void) const;
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
	void SetLispSymbol( LPCTSTR pszLispSymbol, const void* ptr, odcl::PtrType type ) const;
	bool UpdateGlobalLispSymbols() const;
	bool OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM );
	bool AddExtensionTab( TDclFormPtr pDclForm, CAdUiTabExtensionManager* pTabXM );
	void UnloadAllProjects();
	void AddUnknown( IUnknown* pUnknown );
	void RemoveUnknown( IUnknown* pUnknown );

	//Lisp I/O
	void RetPointer( void* ptr, odcl::PtrType type )
	{
		struct resbuf rbRet = {NULL,RTENAME};
		rbRet.resval.rlname[0] = (LONG_PTR)ptr;
		rbRet.resval.rlname[1] = type;
		acedRetVal( &rbRet );
	}
	void RetLong( LONG lValue )
	{
		struct resbuf rbRet = {NULL,RTLONG};
		rbRet.resval.rlong = lValue;
		acedRetVal( &rbRet );
	}
	void RetHandle( DWORD_PTR hdl )
	{
		if( hdl == (DWORD_PTR)(long)hdl )
			RetLong( (long)hdl );
		else
			RetPointer( (void*)hdl, odcl::ptrHandle );
	}
	void RetIUnknown( IUnknown* pUnknown )
	{
		if( !pUnknown )
			acedRetNil();
		else
		{
			AddUnknown( pUnknown );
			RetPointer( pUnknown, odcl::ptrIUnknown );
		}
	}

	//Attributes
	const TDialogList& GetDialogList() const { return mDialogs; }
	const TProjectList& GetProjects() const { return mProjects; }
};
