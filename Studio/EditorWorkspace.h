// EditorWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include <list>

class CProjectTreeCtrl;
class CMainFrame;
class CEditorProject;
class CToolBox;
class CPropertyTabPane;
class CZOrderListCtrl;
class COpenDCLDoc;
class COleControlObject;


#define theEditorWorkspace (*(CEditorWorkspace*)&theWorkspace)


class CEditorWorkspace : public CWorkspace
{
	CMainFrame* mpMainFrame;
	CEditorProject* mpActiveProject;
	CDocument* mpActiveDoc;

	//temporary kludge to save old projects (because destroying them immediately causes mondo exceptions)
	std::list< CProject* > mDiscardedProjects;

public:
	CEditorWorkspace();
	~CEditorWorkspace();

public:
	virtual void SetModified( bool bModified );
	virtual CProject* GetActiveProject() const;
	virtual RefCountedPtr< COleControlObject > GetOleControlFor( const AxPropertyDescriptor* pProperty ) const;
	virtual RefCountedPtr< COleControlObject > GetOleControlFor( const AxMethodDescriptor* pMethod ) const;

	//Attributes
	CMainFrame* GetMainFrame() const;
	//void SetMainFrame( CMainFrame* pMainFrame ) { mpMainFrame = pMainFrame; }
	void SetActiveProject( CEditorProject* pProject );
	CDocument* GetActiveDocument() const;
	void SetActiveDocument( CDocument* pDoc ) { mpActiveDoc = pDoc; } //kludge! [ORW]
	CProjectTreeCtrl* GetProjectTreeCtrl() const;
	CPropertyTabPane* GetPropertyTabs() const;
	CToolBox* GetToolBox() const;
	CZOrderListCtrl* GetZOrderListCtrl() const;
	CString GetActiveProjectName() const;
};
