// EditorProject.cpp : implementation file
//

#include "stdafx.h"
#include "EditorProject.h"


/////////////////////////////////////////////////////////////////////////////
// CEditorProject

IMPLEMENT_SERIAL(CEditorProject, CProject, 1)

CEditorProject::CEditorProject()
{
}

CEditorProject::CEditorProject( LPCTSTR pszKeyName )
: CProject( pszKeyName )
{
}

CEditorProject::~CEditorProject()
{
}
