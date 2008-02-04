// Methods_BlockList.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BlockList.h"
#include "ArgumentsRetrieval.h"
#include "ArxListViewCtrl.h"
#include "ControlTypes.h"
#include "Resource.h"
#include "Workspace.h"


static void ReturnRowCol( int nRow, int nCol )
{
	resbuf rbColumn = { NULL, RTSHORT };
	rbColumn.resval.rint = nCol;
	resbuf rbRow = { &rbColumn, RTSHORT };
	rbRow.resval.rint = nRow;
	acedRetList( &rbRow );
}


ADSRESULT BlockList::GetFileName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->m_pLoadedDwg )
		acedRetStr( pCtrl->m_FileName );
	return RSRSLT;
}

ADSRESULT BlockList::LoadDwg()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->LoadDwg( sFilename ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockList::Reset()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->m_pLoadedDwg )
	{
		delete pCtrl->m_pLoadedDwg;
		pCtrl->m_pLoadedDwg = NULL;
		pCtrl->m_FileName.Empty();
		pCtrl->RefreshBlockList();
		acedRetT();
	}
	return RSRSLT;
}
