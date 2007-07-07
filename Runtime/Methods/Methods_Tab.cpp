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
	CString pszString;
	int nNewSelIndex;
	int nArg;

	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_SetTabText, &nArg);

	if (!GetIntArgument(nArg, &nNewSelIndex, sTabControl_SetTabText) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	nArg++;
	
	if (!GetStringArgument(nArg, &pszString, sTabControl_SetTabText))
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	TCITEM tcItem;

	//  Get the current tab item text.
    tcItem.mask = TCIF_TEXT;
	((VdclTab*)pControl)->GetTabCtrl().GetItem(nNewSelIndex, &tcItem);

	//  Set the new text for the item.
	tcItem.pszText = pszString.GetBuffer(256);

	//  Set the item in the tab control.
	((VdclTab*)pControl)->GetTabCtrl().SetItem(nNewSelIndex, &tcItem);

	acedRetVoid();
	return 0;
}


int TabControl_ShowTab()
{
	int nIndex;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_ShowTab, &nArg);

	if (!GetIntArgument(nArg, &nIndex, sTabControl_ShowTab) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	
	((VdclTab*)pControl)->ShowTab(nIndex);
	
	acedRetVoid();
	return 0;
}

int TabControl_HideTab()
{
	int nIndex;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_HideTab, &nArg);

	if (!GetIntArgument(nArg, &nIndex, sTabControl_HideTab) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	
	((VdclTab*)pControl)->HideTab(nIndex);
	
	acedRetVoid();
	return 0;
}
int TabControl_SetCurSel()
{
	int nNewSelIndex;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_SetCurSel, &nArg);

	if (!GetIntArgument(nArg, &nNewSelIndex, sTabControl_SetCurSel) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	((VdclTab*)pControl)->SetCurSel(nNewSelIndex);
	((VdclTab*)pControl)->ActivateTabPage(nNewSelIndex, TRUE, TRUE);
	
	acedRetVoid();
	return 0;
}
int TabControl_GetCurSel()
{
	
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_GetCurSel);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	acedRetInt(((VdclTab*)pControl)->m_nCurrentSelectedTab);
	return 0;
}
int TabControl_GetRowCount()
{
		
	CWnd *pControl = GetControlPointer(CtlTabStrip, sTabControl_GetRowCount);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	acedRetInt(((VdclTab*)pControl)->GetTabCtrl().GetRowCount());
	return 0;

}


/////////////////////////////////////////////////////////////////////////////
// Methods_Tab
