// Methods_Html.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Html.h"
#include "ArgumentsRetrieval.h"
#include "HtmlCtrl.h"
#include "ControlTypes.h"


ADSRESULT Html::Navigate()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	CString sNavigateDest;
	if (!GetStringArgument (pArgs, sNavigateDest))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Navigate2(sNavigateDest, 0, NULL, NULL);
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::Stop()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Stop();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::Refresh()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Refresh();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::GoBack()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->GoBack();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::GoForward()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->GoForward();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::GoHome()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->GoHome();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::GoSearch()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->GoSearch();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::GetLocationName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	acedRetStr( pCtrl->GetLocationName() );
	return RSRSLT;
}

ADSRESULT Html::GetLocationURL()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	acedRetStr( pCtrl->GetLocationURL() );
	return RSRSLT;
}

ADSRESULT Html::GetOffline()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->GetOffline() )
		acedRetT();
	else
		acedRetNil();
	return RSRSLT;
}

ADSRESULT Html::SetOffline()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	bool bOffline = true;
	if (!GetBoolArgument (pArgs, bOffline))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetOffline( bOffline );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::GetBusy()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->GetBusy() )
		acedRetT();
	else
		acedRetNil();
	return RSRSLT;
}

ADSRESULT Html::GetFullName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	acedRetStr( pCtrl->GetFullName() );
	return RSRSLT;
}

ADSRESULT Html::GetHtmlDocument()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	acedRetStr( pCtrl->GetHtmlText() );
	return RSRSLT;
}

ADSRESULT Html::ReplaceText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	CString sOldText;
	if (!GetStringArgument (pArgs, sOldText))
		return RSERR; //invalid input

	CString sNewText;
	if (!GetStringArgument (pArgs, sNewText))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->ReplaceText( sOldText, sNewText );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Html::UpdateHtmlCode()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHtmlCtrl))
		return RSERR; //invalid input

	CString sHtmlCode;
	if (!GetStringArgument (pArgs, sHtmlCode))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CHtmlCtrl* pCtrl = (CHtmlCtrl*)pDlgControl->GetControlWnd();
	pCtrl->LoadHtmlCode( sHtmlCode );
	acedRetT();
	return RSRSLT;
}
