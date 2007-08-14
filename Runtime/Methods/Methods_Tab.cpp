// Methods_Tab.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Tab.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "MethodLexicon.h"
#include "VdclTab.h"
#include "ControlTypes.h"


int TabControl_SetTabText()
{
	CString sString;
	int nIndex = -1;
	int nArg;

	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_SetTabText, &nArg);
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sTabControl_SetTabText) ||
			(nIndex = ((VdclTab*)pControl)->GetTabItemIndex( nIndex )) < 0)
	{
		acedRetNil();
		return 0;
	}

	nArg++;
	
	if (!GetStringArgument(nArg, &sString, sTabControl_SetTabText))
	{
		acedRetNil();
		return 0;
	}
	
	TCITEM tcItem = { TCIF_TEXT };
	tcItem.pszText = sString.LockBuffer();
	((VdclTab*)pControl)->GetTabCtrl().SetItem(nIndex, &tcItem);

	acedRetT();
	return 0;
}

int TabControl_ShowTab()
{
	int nIndex = -1;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_ShowTab, &nArg);
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sTabControl_ShowTab) ||
			nIndex < 0 ||
			nIndex >= ((VdclTab*)pControl)->GetTabPageCount())
	{
		acedRetNil();
		return 0;
	}
	
	((VdclTab*)pControl)->ShowTab(nIndex);
	acedRetT();
	return 0;
}

int TabControl_HideTab()
{
	int nIndex = -1;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_HideTab, &nArg);
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sTabControl_HideTab) ||
			nIndex < 0 ||
			nIndex >= ((VdclTab*)pControl)->GetTabPageCount())
	{
		acedRetNil();
		return 0;
	}

	((VdclTab*)pControl)->HideTab(nIndex);
	acedRetT();
	return 0;
}

int TabControl_SetCurSel()
{
	int nIndex = -1;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_SetCurSel, &nArg);
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sTabControl_SetCurSel) ||
			((VdclTab*)pControl)->GetTabItemIndex( nIndex ) < 0)
	{
		acedRetNil();
		return 0;
	}

	((VdclTab*)pControl)->SetCurSel(((VdclTab*)pControl)->GetTabItemIndex( nIndex ));
	((VdclTab*)pControl)->ActivateTabPage(nIndex, TRUE, TRUE);
	acedRetT();
	return 0;
}

int TabControl_GetCurSel()
{
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_GetCurSel);
	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	acedRetInt(((VdclTab*)pControl)->GetCurTabPage());
	return 0;
}

int TabControl_GetRowCount()
{
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_GetRowCount);
	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	acedRetInt(((VdclTab*)pControl)->GetTabCtrl().GetRowCount());
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Methods_Tab
