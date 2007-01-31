// EditorProject.h : header file
//
#pragma once

#include "Project.h"

#define activeEditorProject ((CEditorProject*)theWorkspace.GetActiveProject()) //shortcut to the active project


class CEditorProject : public CProject
{
public:
	CEditorProject();
	virtual ~CEditorProject();

protected:
	DECLARE_SERIAL(CEditorProject)
};
