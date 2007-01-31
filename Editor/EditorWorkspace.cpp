// EditorWorkspace.cpp : implementation file
//

#include "stdafx.h"
#include "EditorWorkspace.h"
#include "EditorProject.h"
#include "ObjectDCL.h"
#include "ObjectDCLDoc.h"
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
: mpMainFrame( ((CObjectDCLApp*)AfxGetApp())->m_pMainFrame )
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
	const_cast<CEditorWorkspace*>(this)->mpMainFrame = ((CObjectDCLApp*)AfxGetApp())->m_pMainFrame;
	return mpMainFrame;
}

CProject* CEditorWorkspace::GetActiveProject() const
{
	return mpActiveProject;
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
	//if( !mpMainFrame )
	//	return NULL;
	//return mpMainFrame->GetActiveDocument();
}

CProjectTreeCtrl* CEditorWorkspace::GetProjectTreeCtrl() const
{
	if( !mpMainFrame )
		return NULL;
	return &mpMainFrame->m_ProjectTree;
}

CPropertyTabPane* CEditorWorkspace::GetPropertyTabs() const
{
	if( !mpMainFrame )
		return NULL;
	return &mpMainFrame->m_PropertyTabPane;
}

CToolBox* CEditorWorkspace::GetToolBox() const
{
	if( !mpMainFrame )
		return NULL;
	return &mpMainFrame->m_ToolBox;
}

CZOrderListCtrl* CEditorWorkspace::GetZOrderListCtrl() const
{
	if( !mpMainFrame )
		return NULL;
	return &mpMainFrame->m_ZOrderPane.m_ZOrderList;
}

CString CEditorWorkspace::GetActiveProjectName() const
{
	CString sShortFileName = StripPathFromFileName(GetActiveProject()->GetKeyName());
	if (sShortFileName.GetLength() > 4)
	{
		if (sShortFileName[sShortFileName.GetLength()-4] == _T('.'))
			sShortFileName = sShortFileName.Left(sShortFileName.GetLength()-4);
	}
	return sShortFileName;
}

const CDclControlObject* CEditorWorkspace::GetArxControlFor( const AxPropertyDescriptor* pProperty ) const
{
	if( !mpActiveProject )
		return NULL;
	return mpActiveProject->GetOleObject( pProperty );
}

const CDclControlObject* CEditorWorkspace::GetArxControlFor( const AxMethodDescriptor* pMethod ) const
{
	if( !mpActiveProject )
		return NULL;
	return mpActiveProject->GetOleObject( pMethod );
}
