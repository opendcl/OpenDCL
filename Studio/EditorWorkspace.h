// EditorWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include "EditorProject.h"
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
	TEditorProjectPtr mpActiveProject;
	CDocument* mpActiveDoc;

public:
	CEditorWorkspace();
	~CEditorWorkspace();

public:
	virtual void SetModified( bool bModified );
	virtual TEditorProjectPtr GetActiveProject();
	virtual TOleControlPtr GetOleControlFor( const AxPropertyDescriptor* pProperty );
	virtual TOleControlPtr GetOleControlFor( const AxMethodDescriptor* pMethod );

	//Attributes
	CMainFrame* GetMainFrame() const;
	//void SetMainFrame( CMainFrame* pMainFrame ) { mpMainFrame = pMainFrame; }
	void SetActiveProject( TEditorProjectPtr pProject );
	CDocument* GetActiveDocument() const;
	void SetActiveDocument( CDocument* pDoc ) { mpActiveDoc = pDoc; } //kludge! [ORW]
	CProjectTreeCtrl* GetProjectTreeCtrl() const;
	CPropertyTabPane* GetPropertyTabs() const;
	CToolBox* GetToolBox() const;
	CZOrderListCtrl* GetZOrderListCtrl() const;
	CString GetActiveProjectName() const;
};
