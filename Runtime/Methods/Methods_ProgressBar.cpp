// Methods_ProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ProgressBar.h"
#include "ProgressBarCtrl.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"


ADSRESULT ProgressBar::Reset()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlProgressBar ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CProgressBarCtrl* pCtrl = (CProgressBarCtrl*)pDlgControl->GetControlWnd();

	pCtrl->Reset();
	acedRetT();
	return RSRSLT;
}

ADSRESULT ProgressBar::SetPos()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlProgressBar))
		return RSERR; //invalid input

	int nPos;
	if( !GetIntArgument( pArgs, nPos ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CProgressCtrl* pCtrl = (CProgressCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetPos( nPos );
	pCtrl->RedrawWindow();
	acedRetT();
	return RSRSLT;
}
