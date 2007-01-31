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
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
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
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	CProject* pProject = activeProject;
	if (pProject)
		pProject->ClearProject();
	pProjTree->SetupProjectTree(pProject);
	pProjTree->SetDocument(this);
	theEditorWorkspace.SetActiveDocument(this); //kludge [ORW]

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

	CProject* pProject = activeProject;
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

  RenameUntitledGlobalVars(lpszPathName);	

	// begin saving.
	CWaitCursor wait;

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

void CObjectDCLDoc::RenameUntitledGlobalVars(CString sPathName) 
{
	CString sShortName = sPathName;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:")).MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("."));

	POSITION pos = activeProject->GetDclFormList().GetHeadPosition();
	while (pos != NULL)
	{
		CDclFormObject *pDcl = activeProject->GetDclFormList().GetNext(pos);
		if (pDcl != NULL)
			pDcl->UpdateGlobalVariable(sShortName);
	}

	// force the redraw of the property list box so any (VarName) will be updated.
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.m_PropertyList.Invalidate();
}

void CObjectDCLDoc::OnFileClose() 
{
	CDocument::OnFileClose();

  CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
	pApp->CloseAllDocuments(FALSE);
	
	theEditorWorkspace.GetProjectTreeCtrl()->ClearTree();		
	theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ADDMODAL, FALSE);
	
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontNames.EnableWindow(FALSE);
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontSizes.EnableWindow(FALSE);
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_Buttons.EnableWindow(FALSE);
	theEditorWorkspace.GetMainFrame()->m_wndToolBar.EnableWindow(FALSE);

	CToolBarCtrl *pCtrl = &theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl();
	pCtrl->EnableButton(0, FALSE);
	pCtrl->EnableButton(ID_ADDMODAL, FALSE);
	pCtrl->EnableButton(ID_PICTUREFOLDER, FALSE);
}

void CObjectDCLDoc::OnCloseDocument() 
{
	//if( this == ((CObjectDCLApp*)AfxGetApp())->m_pDoc )
	//{
	//	((CObjectDCLApp*)AfxGetApp())->m_pDoc = NULL;
	//	theEditorWorkspace.GetMainFrame()->SetActiveView(NULL);
	//}

	theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ADDMODAL, FALSE);	
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontNames.EnableWindow(FALSE);
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_FontSizes.EnableWindow(FALSE);
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.m_Buttons.EnableWindow(FALSE);
	theEditorWorkspace.GetMainFrame()->m_wndToolBar.EnableWindow(FALSE);

	CToolBarCtrl *pCtrl = &theEditorWorkspace.GetMainFrame()->m_wndToolBar.GetToolBarCtrl();
	pCtrl->EnableButton(0, FALSE);
	pCtrl->EnableButton(ID_ADDMODAL, FALSE);
	pCtrl->EnableButton(ID_PICTUREFOLDER, FALSE);
	CDocument::OnCloseDocument();
	theEditorWorkspace.SetActiveDocument(NULL);

	//AfxGetApp()->m_pMainWnd->SetWindowText(_T(""));
}
