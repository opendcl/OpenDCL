// StudioProject.cpp : implementation file
//

#include "stdafx.h"
#include "StudioProject.h"
#include "OpenDCLDoc.h"
#include "StudioWorkspace.h"
#include "DclFormObject.h"
#include "StudioFrame.h"
#include "UndoActions.h"


/////////////////////////////////////////////////////////////////////////////
// CStudioProject

CStudioProject::CStudioProject( COpenDCLDoc* pDoc )
: CProject()
, mpDocument( pDoc )
, mUndoManager( this )
{
	TraceFmt( _T("> CStudioProject::CStudioProject(%s [%p]) [this: %p]\r\n"),
						(LPCTSTR)pDoc->GetTitle(), pDoc, this );
}

CStudioProject::CStudioProject( COpenDCLDoc* pDoc, LPCTSTR pszKeyName )
: CProject( pszKeyName )
, mpDocument( pDoc )
, mUndoManager( this )
{
	TraceFmt( _T("> CStudioProject::CStudioProject(%s [%p], %s) [this: %p]\r\n"),
						(LPCTSTR)pDoc->GetTitle(), pDoc, pszKeyName, this );
}

CStudioProject::~CStudioProject()
{
	TraceFmt( _T("< CStudioProject::~CStudioProject() [this: %p]\r\n"), this );
}

CDocument* CStudioProject::GetDocument() const
{
	return mpDocument;
}

bool CStudioProject::Undo( size_t ctActions /*= 1*/ )
{
	bool bSuccess = mUndoManager.Undo( ctActions );
	if( !bSuccess )
		MessageBeep( MB_ICONERROR );
	return bSuccess;
}

CDclFormView* CStudioProject::OpenDclFormView( TDclFormPtr pDclForm )
{
	CStudioFrame* pStudioFrame = theStudioWorkspace.GetStudioFrame();
	if( !pStudioFrame )
		return NULL;
	return pStudioFrame->OpenDclFormView( pDclForm );
}
