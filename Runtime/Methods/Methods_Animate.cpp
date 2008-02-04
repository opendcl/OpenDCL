// Methods_Animate.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Animate.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "Workspace.h"


ADSRESULT Animate::Load()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlAnimate ) )
		return RSERR; //invalid input

	CString sFileName;
	if( !GetStringArgument( pArgs, sFileName, true ) )
	{
		const TProjectPtr pProject = pDlgControl->GetTemplate()->GetOwnerProject();
		if( !pProject )
			return RSERR; //invalid input
		sFileName = pProject->GetBaseFileName();
		if( sFileName.Find( _T('.') ) > 0 )
		{
			sFileName.MakeReverse();
			sFileName = sFileName.Mid( sFileName.SpanExcluding( _T(".") ).GetLength() );
			sFileName.MakeReverse();
			sFileName += _T("avi");
		}
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CAnimateCtrl* pCtrl = (CAnimateCtrl*)pDlgControl->GetControlWnd();

	if( sFileName.Right( 4 ).CompareNoCase( _T(".avi") ) != 0 )
		sFileName += _T(".avi");
	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
	{
		const TProjectPtr pProject = pDlgControl->GetTemplate()->GetOwnerProject();
		if( pProject )
		{
			CString sAvi = pProject->GetProjectFilePath();
			sAvi.MakeReverse();
			sAvi = sAvi.Mid( sAvi.SpanExcluding( _T("\\/:") ).GetLength() );
			sAvi.MakeReverse();
			sAvi += sFileName;
			sPath = sAvi;
		}
	}	

	if( pCtrl->Open( sPath ) )
		acedRetT();
	return RSRSLT;
}


ADSRESULT Animate::Seek()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlAnimate ) )
		return RSERR; //invalid input

	int nFrame;
	if( !GetIntArgument( pArgs, nFrame ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CAnimateCtrl* pCtrl = (CAnimateCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->Seek( nFrame ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Animate::Close()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlAnimate ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CAnimateCtrl* pCtrl = (CAnimateCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->Close() )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Animate::Stop()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlAnimate ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CAnimateCtrl* pCtrl = (CAnimateCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->Stop() )
		acedRetT();
	return RSRSLT;
}