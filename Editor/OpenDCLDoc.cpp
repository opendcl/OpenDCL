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
#include "Base64.h"


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

	CString sExt( lpszPathName );
	sExt = sExt.MakeReverse().Left( 5 ).MakeReverse();
	if( sExt.CompareNoCase( _T(".odce") ) == 0 )
	{
		TRY
		{
			CStdioFile SourceFile( lpszPathName, CFile::modeRead );
			if (!SourceFile)
				return FALSE;
			CStringA sRawData;
			UINT cchData = SourceFile.Read( sRawData.GetBuffer( (int)SourceFile.GetLength() ), (UINT)SourceFile.GetLength() );
			sRawData.ReleaseBufferSetLength( cchData );
			if( cchData == 0 )
				return FALSE;
			std::string sData = base64_decode( (LPCTSTR)sRawData );
			if( sData.length() == 0 )
				return FALSE;
			CMemFile Data( sData.length() );
			Data.Write( sData.c_str(), sData.length() );
			Data.SeekToBegin();
			CArchiveEx ar( &Data, CArchive::load, NULL, _T("ObjectDCL"), TRUE);
			activeProject->Serialize( ar );
			ar.Close();
		}
		CATCH_ALL(e)
		{
			ReportSaveLoadException(lpszPathName, e, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
			DELETE_EXCEPTION(e);
			return FALSE;
		}
		END_CATCH_ALL;
	}
	else 
	{
		IOStatus stat = pProject->ReadFromFile( lpszPathName );
		if( stat != statOK )
		{
			ReportSaveLoadException(lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
			return FALSE;
		}
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
	CString sExt( lpszPathName );
	sExt = sExt.MakeReverse().Left( 5 ).MakeReverse();
	if( sExt.CompareNoCase( _T(".odce") ) == 0 )
	{
		TRY
		{
			CStdioFile DestFile( lpszPathName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate );
			if (!DestFile)
				return FALSE;
			CMemFile Data( 0x10000 );
			CArchiveEx ar( &Data, CArchive::store | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
			activeProject->Serialize( ar );
			ar.Close();
			UINT cbData = (UINT)Data.GetLength();
			if( cbData == 0 )
				return FALSE;
			BYTE* pbData = Data.Detach();
			std::string sRawData = base64_encode( pbData, cbData );
			Data.Attach( pbData, cbData );
			Data.Abort();
			DestFile.Write( sRawData.c_str(), sRawData.length() );
			DestFile.Flush();
		}
		CATCH_ALL(e)
		{
			ReportSaveLoadException(sFileName, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
			DELETE_EXCEPTION(e);
			return FALSE;
		}
		END_CATCH_ALL;
	}
	else if (activeProject->WriteToFile(sFileName) != statOK)
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
