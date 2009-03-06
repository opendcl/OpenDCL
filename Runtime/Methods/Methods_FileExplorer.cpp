// Methods_FileExplorer.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_FileExplorer.h"
#include "ArgumentsRetrieval.h"
#include "CustomFileDialog.h"


ADSRESULT FileExplorer::GetFileName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetFileName() );
	return RSRSLT;
}

ADSRESULT FileExplorer::GetFileTitle()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetFileTitle() );
	return RSRSLT;
}

ADSRESULT FileExplorer::GetFileExt()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetFileExt() );
	return RSRSLT;
}

ADSRESULT FileExplorer::GetPathName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetPathName() );
	return RSRSLT;
}

ADSRESULT FileExplorer::GetFolderPath()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetFolderPath() );
	return RSRSLT;
}
	
ADSRESULT FileExplorer::GetFolderName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetFolderPath().MakeReverse().SpanExcluding(_T("\\/:")).MakeReverse() );
	return RSRSLT;
}

ADSRESULT FileExplorer::GetSelCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	CWnd* pMainWnd = pCtrl->GetParent()->GetDlgItem(lst2);
	if( pMainWnd )
	{
		CListCtrl* pFileListCtrl = (CListCtrl*)pMainWnd->GetDlgItem( 1 );
		acedRetInt( pFileListCtrl->GetSelectedCount() );
	}
	return RSRSLT;
}

ADSRESULT FileExplorer::GetFileNameList()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	CWnd* pMainWnd = pCtrl->GetParent()->GetDlgItem(lst2);
	if( pMainWnd )
	{
		CListCtrl* pFileListCtrl = (CListCtrl*)(pMainWnd->GetDlgItem(1));
		struct resbuf* prbFileList = NULL;
		struct resbuf* prbTail = NULL;
		POSITION pos = pFileListCtrl->GetFirstSelectedItemPosition();
		while( pos )
		{
			resbuf* prbFile = acutNewRb( RTSTR );
			int idxSelected = pFileListCtrl->GetNextSelectedItem( pos );
			acutNewString( pFileListCtrl->GetItemText( idxSelected, 0 ), prbFile->resval.rstring );
			if( !prbTail )
			{
				prbFileList = prbFile;
				prbTail = prbFileList;
			}
			else
			{
				prbTail->rbnext = prbFile;
				prbTail = prbTail->rbnext;
			}
		}
		acedRetList( prbFileList );
		acutRelRb( prbFileList );
	}
	return RSRSLT;
}

ADSRESULT FileExplorer::SetOkButtonText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlFileExplorer ) )
		return RSERR; //invalid input

	CString sCaption;
	if( !GetStringArgument( pArgs, sCaption ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CCustomFileDialog* pCtrl = (CCustomFileDialog*)pDlgControl->GetControlWnd();

	CWnd* pOkButton = pCtrl->GetParent()->GetDlgItem( IDOK );
	if( pOkButton )
	{
		pOkButton->SetWindowText( sCaption );
		acedRetT();
	}
	return RSRSLT;
}
