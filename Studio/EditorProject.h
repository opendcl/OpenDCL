// EditorProject.h : header file
//
#pragma once

#include "Project.h"

#define activeProject (theEditorWorkspace.GetActiveProject()) //shortcut to the active project


class CEditorProject : public CProject
{
public:
	CEditorProject();
	CEditorProject( LPCTSTR pszKeyName );
	virtual ~CEditorProject();

protected:
	DECLARE_SERIAL(CEditorProject)
};

typedef RefCountedPtr< CEditorProject > TEditorProjectPtr;
