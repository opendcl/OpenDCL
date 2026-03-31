// StudioProject.h : header file
//
#pragma once

#include "Project.h"
#include "StudioUndoManager.h"

#define activeProject (theStudioWorkspace.GetActiveProject()) //shortcut to the active project

class COpenDCLDoc;
class CDclFormView;


class CStudioProject : public CProject
{
	COpenDCLDoc* mpDocument;
	CStudioUndoManager mUndoManager;

public:
	CStudioProject( COpenDCLDoc* pDoc );
	CStudioProject( COpenDCLDoc* pDoc, LPCTSTR pszKeyName );
	virtual ~CStudioProject();

	CDocument* GetDocument() const override;
	CUndoManager* GetUndoManager() override { return &mUndoManager; }
	void ClearDocument() { mpDocument = NULL; }
	size_t UndoCount() const { return mUndoManager.size(); }
	bool Undo( size_t ctActions = 1 );
	CDclFormView* OpenDclFormView( TDclFormPtr pDclForm );
};

typedef RefCountedPtr< CStudioProject > TStudioProjectPtr;
typedef LockedPtr< CStudioProject > TStudioProjectLockedPtr;
