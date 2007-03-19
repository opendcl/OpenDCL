// ObjectDCLDoc.cpp : implementation of the CObjectDCLDoc class
//

#include "stdafx.h"
#include "ObjectDCLDoc.h"
#include "Resource.h"
#include "Project.h"
#include "MainFrm.h"
#include "ObjectDCL.h"
#include "ArchiveEx.h"
#include "StgFile.h"
#include "DclFormObject.h"
#include "EditorWorkspace.h"


#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLDoc

IMPLEMENT_DYNCREATE(CObjectDCLDoc, CDocument)

BEGIN_MESSAGE_MAP(CObjectDCLDoc, CDocument)
	//{{AFX_MSG_MAP(CObjectDCLDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLDoc construction/destruction

CObjectDCLDoc::CObjectDCLDoc()
{
	m_strPathName = CString();
	SetTitle(m_strPathName);
}

CObjectDCLDoc::~CObjectDCLDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CObjectDCLDoc serialization

void CObjectDCLDoc::Serialize(CArchive& ar)
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
// CObjectDCLDoc diagnostics

#ifdef _DEBUG
void CObjectDCLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CObjectDCLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLDoc commands

BOOL CObjectDCLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontNames.EnableWindow(TRUE);
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontSizes.EnableWindow(TRUE);
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_Buttons.EnableWindow(TRUE);
	theEditorWorkspace.GetMainFrame()->m_wndToolBar.EnableWindow(TRUE);
	CEditorProject* pProject = new CEditorProject( m_strTitle );
	theEditorWorkspace.SetActiveProject( pProject );
	theEditorWorkspace.SetActiveDocument(this); //kludge [ORW]
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	pProjTree->SetupProjectTree( pProject );
	pProjTree->SetDocument(this);

	CToolBarCtrl *pCtrl = &theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl();
	pCtrl->EnableButton(ID_ADDMODAL, TRUE);
	pCtrl->EnableButton(ID_PICTUREFOLDER, TRUE);
	return TRUE;
}

BOOL CObjectDCLDoc::OnOpenDocument(LPCTSTR lpszPathName) 
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
  theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontNames.EnableWindow(TRUE);
  theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontSizes.EnableWindow(TRUE);
  theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_Buttons.EnableWindow(TRUE);
  theEditorWorkspace.GetMainFrame()->m_wndToolBar.EnableWindow(TRUE);
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	pProjTree->SetupProjectTree(pProject);
	pProjTree->SetDocument(this);

  CToolBarCtrl *pCtrl = &theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl();
  pCtrl->EnableButton(ID_ADDMODAL, TRUE);
  pCtrl->EnableButton(ID_PICTUREFOLDER, TRUE);

  return TRUE;
}

BOOL CObjectDCLDoc::OnSaveDocument(LPCTSTR lpszPathName) 
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
		ReportSaveLoadException(sFileName, NULL, TRUE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	SetModifiedFlag(FALSE);     // back to unmodified
	SetTitle(lpszPathName);
	CFrameWnd* pActiveFrame = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetActiveFrame();
	if (pActiveFrame)
		pActiveFrame->SetTitle(lpszPathName);
	//activeProject->SaveDistFile();

	return TRUE;
}

void CObjectDCLDoc::SetGlobalVariableNames( LPCTSTR pszRootName )
{
	if( pszRootName )
		activeProject->SetGlobalVariableNames( pszRootName );
	else
		activeProject->ClearGlobalVariableNames();

	// force the redraw of the property list box so any (VarName) will be updated.
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
}

void CObjectDCLDoc::OnCloseDocument() 
{
	theEditorWorkspace.GetProjectTreeCtrl()->ClearTree();		
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
	CToolBarCtrl *pCtrl = &theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl();
	pCtrl->EnableButton(ID_FILE_SAVE, FALSE);
	pCtrl->EnableButton(ID_ADDMODAL, FALSE);
	pCtrl->EnableButton(ID_PICTUREFOLDER, FALSE);
	CDocument::OnCloseDocument();
	theEditorWorkspace.SetActiveDocument(NULL);
	CProject *pProject = activeProject;
	if (pProject)
		pProject->ClearProject();	
}
