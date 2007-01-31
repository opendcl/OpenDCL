// EditorWorkspace.h : header file
//

#pragma once

#include "Workspace.h"


class CProjectTreeCtrl;
class CMainFrame;
class CProject;
class CEditorProject;
class CToolBox;
class CPropertyTabPane;
class CZOrderListCtrl;
class CObjectDCLDoc;


#define theEditorWorkspace (*(CEditorWorkspace*)&theWorkspace)


class CEditorWorkspace : public CWorkspace
{
	CMainFrame* mpMainFrame;
	CEditorProject* mpActiveProject;
	CDocument* mpActiveDoc;

public:
	CEditorWorkspace();
	~CEditorWorkspace();

public:
	virtual CProject* GetActiveProject() const;
	virtual const CDclControlObject* GetArxControlFor( const AxPropertyDescriptor* pProperty ) const;
	virtual const CDclControlObject* GetArxControlFor( const AxMethodDescriptor* pMethod ) const;
	virtual void SetModified( bool bModified );

	//Attributes
	CMainFrame* GetMainFrame() const;
	//void SetMainFrame( CMainFrame* pMainFrame ) { mpMainFrame = pMainFrame; }
	void SetActiveProject( CEditorProject* pProject ) { mpActiveProject = pProject; }
	CDocument* GetActiveDocument() const;
	void SetActiveDocument( CDocument* pDoc ) { mpActiveDoc = pDoc; } //kludge! [ORW]
	CProjectTreeCtrl* GetProjectTreeCtrl() const;
	CPropertyTabPane* GetPropertyTabs() const;
	CToolBox* GetToolBox() const;
	CZOrderListCtrl* GetZOrderListCtrl() const;
	CString GetActiveProjectName() const;
};
