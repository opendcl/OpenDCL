// Methods_DwgPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_DwgPreview.h"
#include "ArgumentsRetrieval.h"
#include "DwgPreviewCtrl.h"
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

	CDwgPreviewCtrl* pCtrl = (CDwgPreviewCtrl*)pDlgControl->GetControlWnd();

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

	CDwgPreviewCtrl* pCtrl = (CDwgPreviewCtrl*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->m_filename );
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

	CDwgPreviewCtrl* pCtrl = (CDwgPreviewCtrl*)pDlgControl->GetControlWnd();
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

	CDwgPreviewCtrl* pCtrl = (CDwgPreviewCtrl*)pDlgControl->GetControlWnd();
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

	CDwgPreviewCtrl* pCtrl = (CDwgPreviewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->m_filename.Empty();
	pCtrl->m_bSelectedRect = false;
	pCtrl->Invalidate();

	acedRetT();
	return RSRSLT;
}