// OpenDCLDoc.cpp : implementation of the COpenDCLDoc class
//

#include "stdafx.h"
#include "OpenDCL.h"
#include "OpenDCLDoc.h"
#include "Resource.h"
#include "StudioProject.h"
#include "StudioFrame.h"
#include "ArchiveEx.h"
#include "StgFile.h"
#include "StudioWorkspace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenDCLDoc

IMPLEMENT_DYNCREATE(COpenDCLDoc, CDocument)

BEGIN_MESSAGE_MAP(COpenDCLDoc, CDocument)
END_MESSAGE_MAP()


// COpenDCLDoc construction/destruction

COpenDCLDoc::COpenDCLDoc()
{
	// TODO: add one-time construction code here

}

COpenDCLDoc::~COpenDCLDoc()
{
}

void COpenDCLDoc::SetModifiedFlag(BOOL bModified /*= TRUE*/)
{
	if( bModified )
	{ //save document state
	}
	__super::SetModifiedFlag( bModified );
}


BOOL COpenDCLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	
	if( mpProject )
		mpProject->ClearDocument();
	mpProject = new CStudioProject( this, m_strTitle );
	SetModifiedFlag(FALSE);     // start off with unmodified
	theStudioWorkspace.ActivateProject( mpProject, this );

	return TRUE;
}

void COpenDCLDoc::OnCloseDocument()
{
	theStudioWorkspace.ActivateProject( NULL, NULL );
	if( mpProject )
		mpProject->ClearDocument();
	CDocument::OnCloseDocument();
}

BOOL COpenDCLDoc::OnOpenDocument( LPCTSTR lpszPathName )
{
	if( !CDocument::OnOpenDocument( lpszPathName ) )
		return FALSE;

	SetTitle( lpszPathName );
	SetModifiedFlag();  // dirty during de-serialize

	// begin reading
	CWaitCursor wait;

	if( mpProject )
		mpProject->ClearDocument();
	mpProject = new CStudioProject( this );
	IOStatus stat = mpProject->ReadFromFile( lpszPathName );
	if( stat != statOK )
	{
		ReportSaveLoadException( lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC );
		return FALSE;
	}

	SetModifiedFlag( FALSE );     // start off with unmodified
	theStudioWorkspace.ActivateProject( mpProject, this );
  return TRUE;
}

BOOL COpenDCLDoc::OnSaveDocument( LPCTSTR lpszPathName ) 
{
  //Get and open text file with basic information
	CString sFileName = lpszPathName;
	if( sFileName.IsEmpty() )
	{
		if( !AfxGetApp()->DoPromptFileName( sFileName, AFX_IDS_SAVEFILE,
																				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
																				FALSE, NULL ) )
			return FALSE;	
	}
  CopyFile( sFileName, sFileName + _T(".bak"), FALSE );

	// begin saving.
	CWaitCursor wait;

	mpProject->SetKeyName( lpszPathName ); //updating the key name *before* saving
	if( mpProject->WriteToFile( sFileName ) != statOK )
	{
		ReportSaveLoadException( sFileName, NULL, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC );
		return FALSE;
	}
	m_strTitle = mpProject->GetKeyName();
	SetModifiedFlag( FALSE );     // back to unmodified
	((CStudioFrame*)AfxGetApp()->GetMainWnd())->DelayUpdateFrameTitle();

	return TRUE;
}



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


// COpenDCLDoc commands
