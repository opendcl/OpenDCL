// EditorWorkspace.cpp : implementation file
//

#include "stdafx.h"
#include "EditorWorkspace.h"
#include "EditorProject.h"
#include "Editor.h"
#include "OpenDCLDoc.h"
#include "ZOrderPane.h"


CWorkspace* theWorkspacePtr()
{
	static CEditorWorkspace workspace;
	return &workspace;
}


static CString StripPathFromFileName(CString sFullPath)
{
	CString sShortName = sFullPath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


CEditorWorkspace::CEditorWorkspace()
: mpMainFrame( ((COpenDCLApp*)AfxGetApp())->m_pMainFrame )
, mpActiveProject( NULL )
{
}

CEditorWorkspace::~CEditorWorkspace()
{
}

CMainFrame* CEditorWorkspace::GetMainFrame() const
{
	if( mpMainFrame )
		return mpMainFrame;
	const_cast<CEditorWorkspace*>(this)->mpMainFrame = ((COpenDCLApp*)AfxGetApp())->m_pMainFrame;
	return mpMainFrame;
}

TEditorProjectPtr CEditorWorkspace::GetActiveProject()
{
	return mpActiveProject;
}

void CEditorWorkspace::SetActiveProject( TEditorProjectPtr pProject )
{
	mpActiveProject = pProject;
}


void CEditorWorkspace::SetModified( bool bModified )
{
	CDocument* pDoc = GetActiveDocument();
	if( pDoc )
		pDoc->SetModifiedFlag( bModified );
}

CDocument* CEditorWorkspace::GetActiveDocument() const
{
	return mpActiveDoc; //kludge! [ORW]
	//if( !GetMainFrame() )
	//	return NULL;
	//return GetMainFrame()->GetActiveDocument();
}

CProjectTreeCtrl* CEditorWorkspace::GetProjectTreeCtrl() const
{
	if( !GetMainFrame() )
		return NULL;
	return &GetMainFrame()->m_ProjectTree;
}

CPropertyTabPane* CEditorWorkspace::GetPropertyTabs() const
{
	if( !GetMainFrame() )
		return NULL;
	return &GetMainFrame()->m_PropertyTabPane;
}

CToolBox* CEditorWorkspace::GetToolBox() const
{
	if( !GetMainFrame() )
		return NULL;
	return &GetMainFrame()->m_ToolBox;
}

CZOrderListCtrl* CEditorWorkspace::GetZOrderListCtrl() const
{
	if( !GetMainFrame() )
		return NULL;
	return &GetMainFrame()->m_ZOrderPane.m_ZOrderList;
}

CString CEditorWorkspace::GetActiveProjectName() const
{
	if( !mpActiveProject )
		return CString();
	return mpActiveProject->GetKeyName();
}

TOleControlPtr CEditorWorkspace::GetOleControlFor( const AxPropertyDescriptor* pProperty )
{
	if( !mpActiveProject )
		return NULL;
	return mpActiveProject->GetOleObject( pProperty );
}

TOleControlPtr CEditorWorkspace::GetOleControlFor( const AxMethodDescriptor* pMethod )
{
	if( !mpActiveProject )
		return NULL;
	return mpActiveProject->GetOleObject( pMethod );
}
