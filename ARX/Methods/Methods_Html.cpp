// Methods_Html.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Html.h"
#include "ControlTypes.h"
#include "ArgumentsRetrieval.h"
#include "MethodLexicon.h"
#include "HtmlCtrl.h"


int Html_Navigate()
{
	CString sNavigateDest;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_Navigate, &nArg);

	if (!GetStringArgument(nArg, &sNavigateDest, sHtml_Navigate) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->Navigate2(sNavigateDest, navNoHistory, NULL, NULL);

	// return nil
	acedRetVoid();
	return 0;
}



int Html_Stop()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_Stop);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->Stop();

	// return nil
	acedRetVoid();
	return 0;
}

int Html_Refresh()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_Refresh);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->Refresh();

	// return nil
	acedRetVoid();
	return 0;
}

int Html_GoBack()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GoBack);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->GoBack();

	// return nil
	acedRetVoid();
	return 0;
}



int Html_GoForward()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GoForward);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->GoForward();

	// return nil
	acedRetVoid();
	return 0;
}
int Html_GoHome()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GoHome);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->GoHome();

	// return nil
	acedRetVoid();
	return 0;
}
int Html_GoSearch()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GoSearch);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->GoSearch();

	// return nil
	acedRetVoid();
	return 0;
}
int Html_GetLocationName()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GetLocationName);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CString sLocation = ((CHtmlCtrl*)pControl)->GetLocationName();

	// return nil
	acedRetStr(sLocation);
	return 0;
}
int Html_GetLocationURL()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GetLocationURL);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CString sLocation = ((CHtmlCtrl*)pControl)->GetLocationURL();

	// return nil
	acedRetStr(sLocation);
	return 0;
}
int Html_GetOffline()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GetOffline);

	if (pControl == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}

	BOOL bOffline = ((CHtmlCtrl*)pControl)->GetOffline();

	if (bOffline == 1)
		acedRetT();
	else
		acedRetNil();
	return 0;
}
int Html_SetOffline()
{
	int nOffLine;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_SetOffline, &nArg);

	if (!GetIntArgument(nArg, &nOffLine, sHtml_SetOffline) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->SetOffline(nOffLine);

	// return nil
	acedRetVoid();
	return 0;
}
int Html_GetBusy()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GetBusy);

	if (pControl == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}

	BOOL bBusy = ((CHtmlCtrl*)pControl)->GetBusy();

	// return nil
	if (bBusy == 1)
		acedRetT();
	else
		acedRetNil();
	return 0;
}

int Html_GetFullName()
{
	CString sNavigateDest;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GetFullName);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CString sFullName = ((CHtmlCtrl*)pControl)->GetFullName();

	// return nil
	acedRetStr(sFullName);
	return 0;
}

int Html_GetHtmlDocument()
{
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_GetHtmlDocument);
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

  CString sHtmlText = ((CHtmlCtrl*)pControl)->GetHtmlText();

	acedRetStr(sHtmlText);
	return 0;

}

int Html_ReplaceText()
{
	CString sOldText;
	CString sNewText;

	CWnd *pControl =  GetArgsControlStringString(CtlHtmlCtrl, sHtml_ReplaceText, sOldText, sNewText);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->ReplaceText(sOldText, sNewText);
	acedRetVoid(); 
	return 0;
}


int Html_UpdateHtmlCode()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlHtmlCtrl, sHtml_UpdateHtmlCode, &nArg);
	CString sHtmlCode;
	
	if (pControl == NULL || !GetStringArgument(nArg, &sHtmlCode, sHtml_UpdateHtmlCode))
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CHtmlCtrl*)pControl)->LoadHtmlCode(sHtmlCode);
	acedRetVoid();
	return 0;
}
