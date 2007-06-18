// Methods_ComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ComboBox.h"
#include "DclControlObject.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "ErrorLexicon.h"
#include "VdclColorComboBox.h"
#include "VdclLineWeightComboBox.h"
#include "VdclArrowHeadComboBox.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "PrinterComboBox.h"
#include "VdclComboBox.h"
#include "FontCombo.h"
#include "ComboBoxFolder.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// Methods_ComboBox

int ComboBox_AddPath()
{
	int nArg = 0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_AddPath, &nArg);

	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pArxObject->GetLngProperty(nComboBoxStyle) != CmboStyle_DirPicker)
	{
		theWorkspace.DisplayAlert(_T("This ComboBox is not a Directory Picker ComboBox."));
		// return nil
		acedRetInt(-1);
		return 0;
	}

	
	CString sPath;
	CWnd *pControl = pArxObject->GetWindow();

	if (!GetStringArgument(nArg, &sPath, sComboBox_AddPath) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CComboBoxFolder*)pControl)->AddPath(sPath);

	// return nil
	acedRetVoid();
	return 0;
}


int ComboBox_AddColor()
{
	int nArg = 0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_AddColor, &nArg);

	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pArxObject->GetLngProperty(nComboBoxStyle) != CmboStyle_Color)
	{
		theWorkspace.DisplayAlert(ErrorWrongColorStyle);
		// return nil
		acedRetInt(-1);
		return 0;
	}

	
	int nColorIndex;
	CWnd *pControl = pArxObject->GetWindow();

	if (!GetIntArgument(nArg, &nColorIndex, sComboBox_AddColor) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((VdclColorComboBox*)pControl)->AddColorToMRU(nColorIndex);

	// return nil
	acedRetVoid();
	return 0;
}


int ComboBox_FindLineWeight()
{
	int nArg=0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_FindLineWeight, &nArg);
	
	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pArxObject->GetLngProperty(nComboBoxStyle) != CmboStyle_LineWeight)
	{
		theWorkspace.DisplayAlert(ErrorWrongColorStyle);
		// return nil
		acedRetInt(-1);
		return 0;
	}

	
	int nColorIndex;
	CWnd *pControl = pArxObject->GetWindow();

	if (!GetIntArgument(nArg, &nColorIndex, sComboBox_FindLineWeight) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	int nIndex = ((VdclLineWeightComboBox*)pControl)->FindItemByLineWeight(nColorIndex);

	if (nIndex > -1)
		// return the index
		acedRetInt(nIndex);
	else
		// return nil
		acedRetNil();
	return 0;
}

int ComboBox_FindColor()
{
	int nArg=0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_FindColor, &nArg);
	
	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pArxObject->GetLngProperty(nComboBoxStyle) != CmboStyle_Color)
	{
		theWorkspace.DisplayAlert(ErrorWrongColorStyle);
		// return nil
		acedRetInt(-1);
		return 0;
	}

	
	int nColorIndex;
	CWnd *pControl = pArxObject->GetWindow();

	if (!GetIntArgument(nArg, &nColorIndex, sComboBox_FindColor) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	int nIndex = ((VdclColorComboBox*)pControl)->FindItemByColorIndex(nColorIndex);

	if (nIndex > -1)
		// return the index
		acedRetInt(nIndex);
	else
		// return nil
		acedRetNil();
	return 0;
}

//*****************************************************************************
// 
// Method: ComboBox_AddString()
// 
// Purpose: [Add a string to a list box's list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ComboBox_AddString()
{
	CString sStringArg;
	CWnd *pControl =  GetArgsControlString(CtlComboBox, sComboBox_AddString, sStringArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	acedRetInt(((CComboBox*)pControl)->AddString(sStringArg));
	return 0;
}

//*****************************************************************************
// 
// Method: ComboBox_AddList()
// 
// Purpose: [Adds a list of a strings to a list box's list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ComboBox_AddList()
{
	CString sStringArg;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_AddList, &nArg);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	struct resbuf *ListData;
	CString sNewString;
	
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			// check to see if we are finished.
			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				// return nil
				acedRetVoid();
				return 0;
			}
			
			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sComboBox_AddList);	
				acedRetInt(-1);  return 0; 
			}

			// get the first argument required
			sNewString = ListData->resval.rstring;

			((CComboBox*)pControl)->AddString(sNewString);
		}
	}
	// return nil
	acedRetVoid();
	return 0;
}


int ComboBox_Clear()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_Clear);

	int nArg = 0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_GetDir, &nArg);
	
	switch (pArxObject->GetLngProperty(nComboBoxStyle))
	{
		case CmboStyle_Combo:
		case CmboStyle_Simple:
		case CmboStyle_DropDown:
		{
			((CComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_ArrowHead:			
		{
			((VdclArrowHeadComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_Color:
		{
			((VdclColorComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_LineWeight:
		{
			((VdclLineWeightComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_PlotNames:
		{
			((VdclPlotStyleNamesComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_PlotTables:
		{
			((VdclPlotStyleTablesComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_FontDropList:
		case CmboStyle_FontSimpleList:
		{
			((CFontCombo*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_Plotters:
		{
			((CPrinterComboBox*)pControl)->ResetContent();
			break;
		}
		case CmboStyle_PlotterPaperSizes:
		{
			((VdclComboBox*)pControl)->ResetContent();
			break;
		}

		case CmboStyle_DirPicker:
		{
			((CComboBoxFolder*)pControl)->ResetContent();
			break;
		}
	}
	
	// return nil
	acedRetVoid();
	return 0;
}

int ComboBox_GetCurSel()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetCurSel);
	
	int nSel = ((CComboBox*)pControl)->GetCurSel();
	
	// return nil
	acedRetInt(nSel);
	return 0;
}

int ComboBox_GetCount()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetCount);
	
	int nCount = ((CComboBox*)pControl)->GetCount();
	
	// return nil
	acedRetInt(nCount);
	return 0;
}


int ComboBox_DeleteString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_DeleteString, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBox_DeleteString) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBox*) pControl)->DeleteString(nIndex);
	acedRetVoid();
	return 0;

}
int ComboBox_InsertString()
{
	int nIndex;
	CString sInsString;
	CWnd *pControl = GetArgsControlIntString(CtlComboBox, sComboBox_InsertString, nIndex, sInsString);
	
	if (pControl == NULL)
	{
		acedRetInt(-1);
		return 0;
	}

	((CComboBox*) pControl)->InsertString(nIndex, sInsString);
	acedRetVoid();
	return 0;

}
int ComboBox_SelectString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_SelectString, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sComboBox_SelectString) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CComboBox*) pControl)->GetCurSel();
	acedRetInt(((CComboBox*) pControl)->SelectString(nIndex, sSearchString));
	return 0;

}

int ComboBox_FindString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_FindString, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sComboBox_FindString) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CComboBox*) pControl)->GetCurSel();
	acedRetInt(((CComboBox*) pControl)->FindString(nIndex, sSearchString));
	return 0;

}
int ComboBox_FindStringExact()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_FindStringExact, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sComboBox_FindStringExact) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CComboBox*) pControl)->GetCurSel();
	acedRetInt(((CComboBox*) pControl)->FindStringExact(nIndex, sSearchString));
	return 0;

}
int ComboBox_SetCurSel()
{
	int nArg=0;

	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_SetCurSel, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBox_SetCurSel) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBox*) pControl)->SetCurSel(nIndex);

	acedRetVoid();
	return 0;
}
int ComboBox_GetEditSel()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetEditSel);
	
	if (pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	DWORD dwSel;
	int nStart;
	int nEnd;
	// Set the selection to be all characters after the current selection.
	if ((dwSel=((CComboBox*) pControl)->GetEditSel()) == CB_ERR)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	else
	{
		nStart = HIWORD(dwSel);
		nEnd = LOWORD(dwSel);
	}

	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, nStart,
		RTSHORT, nEnd,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 

	return 0;	
}
int ComboBox_SetEditSel()
{
	//int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_SetEditSel, &nArg);
	int nStart;
	int nEnd;
	
	CWnd *pControl = GetArgsControlIntInt(CtlComboBox, sComboBox_SetEditSel, nStart, nEnd);

	if (pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}

	((CComboBox*) pControl)->SetEditSel(nStart, nEnd);

	acedRetVoid();
	return 0;
}
int ComboBox_SetItemData()
{
	int nIndex;
	int nData;
	CWnd *pControl = GetArgsControlIntInt(CtlComboBox, sComboBox_SetItemData, nIndex, nData);

	if (pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	((CComboBox*) pControl)->SetItemData(nIndex, nData);

	acedRetVoid();
	return 0;

}
int ComboBox_GetItemData()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetItemData, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBox_GetItemData) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CComboBox*) pControl)->GetItemData(nIndex));
	return 0;

}
int ComboBox_GetTopIndex()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetTopIndex);

	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CComboBox*)pControl)->GetTopIndex());	
	return 0;

}
int ComboBox_SetTopIndex()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_SetTopIndex, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBox_SetTopIndex) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBox*) pControl)->SetTopIndex(nIndex);

	acedRetVoid();
	return 0;
}

int ComboBox_GetDroppedWidth()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_SetTopIndex);

	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CComboBox*)pControl)->GetDroppedWidth());	
	return 0;

}
int ComboBox_SetDroppedWidth()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_SetDroppedWidth, &nArg);
	int nDroppedWidth;
	
	if (!GetIntArgument(nArg, &nDroppedWidth, sComboBox_SetDroppedWidth) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBox*) pControl)->SetDroppedWidth(nDroppedWidth);

	acedRetVoid();
	return 0;
}
int ComboBox_ClearEdit()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_ClearEdit);

	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	((CComboBox*)pControl)->Clear();
	acedRetVoid();
	return 0;

}
int ComboBox_GetLBText()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetLBText, &nArg);
	int nArgument;
	
	if (!GetIntArgument(nArg, &nArgument, sComboBox_GetLBText) || pControl == NULL)
	{
		// return nil
		acedRetStr(_T(""));
		return 0;
	}

	
	CString sString;
	
	int n = ((CComboBox*)pControl)->GetLBTextLen(nArgument);      
	((CComboBox*)pControl)->GetLBText(nArgument, sString.GetBuffer(n));
	sString.ReleaseBuffer();

	// return the string found
	acedRetStr(sString);
	return 0;
}

int ComboBox_GetEBText()
{
	CWnd *pControl = GetControlPointer(CtlComboBox, sComboBox_GetEBText);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetStr(_T(""));
		return 0;
	}

	
	CString sString;
	
	((CComboBox*)pControl)->GetWindowText(sString);
	//sString.ReleaseBuffer();


	// return the string found
	acedRetStr(sString);
	return 0;
}

int ComboBox_GetDir()
{
	int nArg = 0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_GetDir, &nArg);
	if (pArxObject == NULL)
	{
		acedRetNil();
		return 0;
	}
	CWnd *pControl = pArxObject->GetWindow();
	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	acedRetStr(((CComboBoxFolder*)pControl)->GetSelectedPath());
	return 0;
}

int ComboBox_Dir()
{
	int nArg = 0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBox_Dir, &nArg);
	if (pArxObject == NULL)
	{
		acedRetNil();
		return 0;
	}
	CWnd *pControl = pArxObject->GetWindow();
	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	CString sDir;
	CString sExtension;

	// check the control type to determine which control to create
	if (pArxObject->GetLngProperty(nComboBoxStyle) != CmboStyle_DropDown &&
		pArxObject->GetLngProperty(nComboBoxStyle) != CmboStyle_DirPicker)
	{
		CString sLoadString = _T("The combo box is not a directory combo box.");
		theWorkspace.DisplayAlert(sLoadString);
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	if (!GetStringArgument(nArg, &sDir, sComboBox_Dir) || pControl == NULL)
	{
		acedRetInt(-1);
		return 0;
	}
	nArg++;
	
	if (!FindOptionalStringArgument(nArg, &sExtension, sComboBox_Dir))
	{
		sExtension = _T("*.*");
	}

	if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_DropDown)
	{
		// Add all the files and directories in the windows directory.
		TCHAR lpszOldPath[MAX_PATH];
		TCHAR lpszWinPath[MAX_PATH];
		
		//::GetWindowsDirectory(lpszWinPath, MAX_PATH);
		
		::GetCurrentDirectory(MAX_PATH, lpszOldPath);
		_tcscpy( lpszWinPath, sDir );
		::SetCurrentDirectory(lpszWinPath);
	
		((CComboBox*)pControl)->ResetContent();
		((CComboBox*)pControl)->Dir(DDL_READWRITE|DDL_DIRECTORY, sExtension);
	
		::SetCurrentDirectory(lpszOldPath);
	}
	else if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_DirPicker)
	{
		((CComboBoxFolder*)pControl)->AddPath(sDir);
	}
	
	acedRetVoid();
	return 0;
}

