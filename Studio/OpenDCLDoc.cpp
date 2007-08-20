// OpenDCLDoc.cpp : implementation of the COpenDCLDoc class
//

#include "stdafx.h"
#include "OpenDCLDoc.h"
#include "Resource.h"
#include "Project.h"
#include "MainFrm.h"
#include "OpenDCL.h"
#include "ArchiveEx.h"
#include "StgFile.h"
#include "DclFormObject.h"
#include "EditorWorkspace.h"


#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)

/////////////////////////////////////////////////////////////////////////////
// COpenDCLDoc

IMPLEMENT_DYNCREATE(COpenDCLDoc, CDocument)

BEGIN_MESSAGE_MAP(COpenDCLDoc, CDocument)
	//{{AFX_MSG_MAP(COpenDCLDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDCLDoc construction/destruction

COpenDCLDoc::COpenDCLDoc()
{
	m_strPathName = CString();
	SetTitle(m_strPathName);
}

COpenDCLDoc::~COpenDCLDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// COpenDCLDoc serialization

void COpenDCLDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
	
}

/////////////////////////////////////////////////////////////////////////////
// COpenDCLDoc diagnostics

#ifdef _DEBUG
void COpenDCLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COpenDCLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenDCLDoc commands

BOOL COpenDCLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	CEditorProject* pProject = new CEditorProject( m_strTitle );
	theEditorWorkspace.SetActiveProject( pProject );
	theEditorWorkspace.SetActiveDocument(this); //kludge [ORW]
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	pProjTree->SetupProjectTree( pProject );
	pProjTree->SetDocument(this);
	return TRUE;
}

BOOL COpenDCLDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	SetTitle(lpszPathName);
	SetModifiedFlag();  // dirty during de-serialize

	// begin reading
	CWaitCursor wait;

	CEditorProject* pProject = new CEditorProject;
	theEditorWorkspace.SetActiveProject( pProject );
	theEditorWorkspace.SetActiveDocument(this); //kludge [ORW]

	IOStatus stat = pProject->ReadFromFile( lpszPathName );
	if( stat != statOK )
	{
		ReportSaveLoadException(lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	SetModifiedFlag(FALSE);     // start off with unmodified
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	pProjTree->SetupProjectTree(pProject);
	pProjTree->SetDocument(this);
  return TRUE;
}

BOOL COpenDCLDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
  //Get and open text file with basic information
	CString sFileName = lpszPathName;
	if (sFileName.IsEmpty())
	{
		if (!AfxGetApp()->DoPromptFileName(sFileName, AFX_IDS_SAVEFILE,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, FALSE, NULL)) return FALSE;	
	}
  CopyFile(sFileName, sFileName + _T(".bak"), FALSE);

	// begin saving.
	CWaitCursor wait;

	activeProject->SetKeyName( lpszPathName ); //updating the key name *before* saving
	if (activeProject->WriteToFile(sFileName) != statOK)
	{
		ReportSaveLoadException(sFileName, NULL, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
		return FALSE;
	}

	theEditorWorkspace.GetProjectTreeCtrl()->SetWindowText( lpszPathName );
	SetModifiedFlag(FALSE);     // back to unmodified
	((CMainFrame*)AfxGetApp()->GetMainWnd())->DelayUpdateFrameTitle();

	return TRUE;
}

void COpenDCLDoc::SetGlobalVariableNames( LPCTSTR pszRootName )
{
	if( pszRootName )
		activeProject->SetGlobalVariableNames( pszRootName );
	else
		activeProject->ClearGlobalVariableNames();

	// force the redraw of the property list box so any (VarName) will be updated.
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
	theWorkspace.SetModified(true);
}

void COpenDCLDoc::OnCloseDocument() 
{
	CDocument::OnCloseDocument();
	theEditorWorkspace.GetProjectTreeCtrl()->ClearTree();		
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
	CToolBarCtrl *pCtrl = &theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl();
	theEditorWorkspace.SetActiveDocument(NULL);
	CProject *pProject = activeProject;
	if (pProject)
		pProject->ClearProject();
	theEditorWorkspace.SetActiveProject( NULL );
}
