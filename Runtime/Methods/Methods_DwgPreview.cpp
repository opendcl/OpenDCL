// Methods_DwgPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_DwgPreview.h"
#include "ArgumentsRetrieval.h"
#include "ArxDwgPreviewCtrl.h"
#include "ControlTypes.h"
#include "Workspace.h"


ADSRESULT DwgPreview::LoadDwg()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgPreview ) )
		return RSERR; //invalid input

	CString sFileName;
	if( !GetStringArgument( pArgs, sFileName ) )
		return RSERR; //invalid input

	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
		return RSRSLT;

	CArxDwgPreviewCtrl* pCtrl = (CArxDwgPreviewCtrl*)pDlgControl->GetControlWnd();

	pCtrl->LoadDwg( sFileName );
	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgPreview::GetDwgName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgPreview ) )
		return RSERR; //invalid input

	CArxDwgPreviewCtrl* pCtrl = (CArxDwgPreviewCtrl*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetFilename() );
	return RSRSLT;
}

ADSRESULT DwgPreview::SetHighLight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgPreview ) )
		return RSERR; //invalid input

	int nColor = 0;
	if( !GetIntArgument( pArgs, nColor ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDwgPreviewCtrl* pCtrl = (CArxDwgPreviewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetHighLight( nColor );

	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgPreview::RemoveHighLight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgPreview ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDwgPreviewCtrl* pCtrl = (CArxDwgPreviewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->RemoveHighLight();

	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgPreview::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlDwgPreview))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDwgPreviewCtrl* pCtrl = (CArxDwgPreviewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Clear();

	acedRetT();
	return RSRSLT;
}