// Methods_ListBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ListBox.h"
#include "DclControlObject.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ArxListBoxCtrl.h"
#include "ArxOptionListCtrl.h"
#include "ArxDwgListCtrl.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// ListBoxMethods

//*****************************************************************************
// 
// Method: ListBox_AddString()
// 
// Purpose: [Add a string to a list box's list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListBox_AddString()
{
	CString sStringArg;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_AddString, &nArg);

	if (!GetStringArgument(nArg, &sStringArg, sListBox_AddString) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	acedRetInt(((CArxListBoxCtrl*)pControl)->AddString(sStringArg));

	return 0;
}
//*****************************************************************************
// 
// Method: ListBox_AddList()
// 
// Purpose: [Adds a list of a strings to a list box's list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListBox_AddList()
{
	CString sStringArg;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_AddList, &nArg);

	if (pControl == NULL)
	{
		
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
				
				acedRetVoid();
				return 0;
			}
			
			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sListBox_AddList);	
				acedRetInt(-1);  return 0; 
			}

			// get the first argument required
			sNewString = ListData->resval.rstring;

			((CArxListBoxCtrl*)pControl)->AddString(sNewString);
		}
	}
	
	acedRetVoid();
	return 0;
}


int ListBox_GetText()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetText, &nArg);
	int nArgument;
	
	if (!GetIntArgument(nArg, &nArgument, sListBox_GetText) || pControl == NULL)
	{
		
		acedRetStr(CString());
		return 0;
	}

	
	CString sString;
	
	int n = ((CListBox*)pControl)->GetTextLen(nArgument);      
	((CListBox*)pControl)->GetText(nArgument, sString.GetBuffer(n));
	sString.ReleaseBuffer();

	acedRetStr(sString);

	return 0;
}

int ListBox_GetSelectedItems()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetSelectedItems);
	
	if (pControl == NULL)
	{
		
		acedRetVoid();
		return 0;
	}

	
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	// Get the indexes of all the selected items.
	int nCount = ((CListBox*)pControl)->GetSelCount();

	// if nothing selected
	if (nCount == 0)
	{
		
		acedRetNil();
		return 0;
	}

	// if one item selected
	if (nCount == -1)
	{
		CString sString;
	
		int nSel = ((CListBox*)pControl)->GetCurSel();
		int n = ((CListBox*)pControl)->GetTextLen(nSel);
		((CListBox*)pControl)->GetText(nSel, sString.GetBuffer(n));
		sString.ReleaseBuffer();

		
		acedRetStr(sString);
		return 0;
	}

	CArray<int,int> aryListBoxSel;

	// get an array of all the selected line numbers
	aryListBoxSel.SetSize(nCount);
	((CListBox*)pControl)->GetSelItems(nCount, aryListBoxSel.GetData());


	for (int i=0; i<nCount; i++)
	{
		//long nLineTagId = ((CListBox*)pControl)->GetItemData(aryListBoxSel[i]);

		// get the text name of the selected line number
		CString sTextItem;
		int n = ((CListBox*)pControl)->GetTextLen(aryListBoxSel[i]);
		((CListBox*)pControl)->GetText(aryListBoxSel[i], sTextItem.GetBuffer(n));
		sTextItem.ReleaseBuffer();

		acutNewString(sTextItem, rbpTail->resval.rstring);
		if ((i+1) < nCount)
		{
			rbpTail->rbnext = acutNewRb(RTSTR);
			rbpTail = rbpTail->rbnext;
		}
 
	}


	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);
	return 0;
}

int ListBox_GetSelectedNths()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetSelectedNths);
	
	if (pControl == NULL)
	{
		
		acedRetVoid();
		return 0;
	}

	
	// Convert the array to a list that can be returned
	//struct resbuf* rbpRetList = acutNewRb(RTSTR);
	//struct resbuf* rbpTail = rbpRetList;
	struct resbuf *pRBList = NULL, *pRB = NULL, *pRBTrav = NULL;
	
	// Get the indexes of all the selected items.
	int nCount = ((CListBox*)pControl)->GetSelCount();

	// if nothing selected
	if (nCount == 0)
	{
		
		acedRetNil();
		return 0;
	}
	// if one item selected
	if (nCount == -1)
	{
		CString sString;
	
		int nSel = ((CListBox*)pControl)->GetCurSel();
		
		
		acedRetInt(nSel);
		return 0;
	}

	CArray<int,int> aryListBoxSel;

	// get an array of all the selected line numbers
	aryListBoxSel.SetSize(nCount);
	((CListBox*)pControl)->GetSelItems(nCount, aryListBoxSel.GetData());


	for (int i=0; i<nCount; i++)
	{
		pRB = acutNewRb(RTSHORT);	//	create new resbuf
		pRB->resval.rint = aryListBoxSel[i]; //	set resbuf value to


		if(i == 0)
		{
			//	If this is first resbuf in list,
			//	we need to store its location in pRBList
			pRBList = pRB;
			pRBList->rbnext = NULL;
			pRBTrav = pRBList;
		}
		else
		{
			// Build up the linked list
			// Remember that pRBList still points
			// to the beginning of the liked list.
			pRBTrav->rbnext = pRB;
			pRBTrav = pRB;
		}

	}


	acedRetList(pRBList);
	acutRelRb(pRBList);
	return 0;
}


int ListBox_GetCurSel()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetCurSel);

	if (pControl == NULL)		
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	int nSel = ((CListBox*)pControl)->GetCurSel();
	
	if (nSel == -1)
		
		acedRetNil();
	else
		// return the index
		acedRetInt(nSel);
	return 0;
}

int ListBox_GetCount()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetCount);

	if (pControl == NULL)		
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	int nCount = ((CListBox*)pControl)->GetCount();
	
	
	acedRetInt(nCount);
	return 0;
}

int ListBox_Clear()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_Clear);

	if (pControl == NULL)
	{
		acedRetInt(-1);
		return 0;
	}
	((CListBox*) pControl)->ResetContent();

	acedRetVoid();
	return 0;
}


int ListBox_SetCurSel()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_SetCurSel, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_SetCurSel) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	((CListBox*) pControl)->SetCurSel(nIndex);	
	
	acedRetVoid();
	return 0;
}


int ListBox_GetTopIndex()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetTopIndex);

	if (pControl == NULL)		
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CListBox*)pControl)->GetTopIndex());	
	return 0;

}
int ListBox_SetTopIndex()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_SetTopIndex, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_SetTopIndex) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	((CListBox*) pControl)->SetTopIndex(nIndex);

	acedRetVoid();
	return 0;
}

int ListBox_SetItemData()
{
	int nIndex;
	int nData;

	CWnd *pControl = GetArgsControlIntInt(CtlListBox, sListBox_SetItemData, nIndex, nData);
	
	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}	

	((CListBox*) pControl)->SetItemData(nIndex, nData);

	acedRetVoid();
	return 0;

}


int OptionList_SetEnabled()
{
	int nIndex;
	int nData;

	CWnd *pControl = GetArgsControlIntInt(CtlListBox, sOptionList_SetEnabled, nIndex, nData);
	
	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}	

	// we actually have to use 2 as the disabled identifier flag and 0 as enabled but not selected.
	if (nData == 0)
		((CArxOptionListCtrl*) pControl)->SetItemData(nIndex, 2);
	else
		((CArxOptionListCtrl*) pControl)->SetItemData(nIndex, 0);
	CRect rc;
	((CArxOptionListCtrl*) pControl)->GetItemRect(nIndex, &rc);
	((CArxOptionListCtrl*) pControl)->InvalidateRect(rc, TRUE);

	acedRetVoid();
	return 0;

}

int ListBox_GetItemData()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_SetItemData, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_SetItemData) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CListBox*) pControl)->GetItemData(nIndex));
	return 0;

}

int ListBox_DeleteString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_DeleteString, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_DeleteString) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	((CListBox*) pControl)->DeleteString(nIndex);
	acedRetVoid();
	return 0;

}
int ListBox_InsertString()
{
	int nIndex;
	CString sInsString;

	CWnd *pControl = GetArgsControlIntString(CtlListBox, sListBox_InsertString, nIndex, sInsString);


	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	((CListBox*) pControl)->InsertString(nIndex, sInsString);
	acedRetVoid();
	return 0;

}

int ListBox_SelectString()
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_SelectString, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sListBox_SelectString) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CListBox*) pControl)->GetCurSel();
	acedRetInt(((CListBox*) pControl)->SelectString(nIndex, sSearchString));
	return 0;

}

int ListBox_FindString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_FindString, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sListBox_FindString) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CListBox*) pControl)->GetCurSel();
	acedRetInt(((CListBox*) pControl)->FindString(nIndex, sSearchString));

	return 0;

}
int ListBox_FindStringExact()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_FindStringExact, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sListBox_FindStringExact) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CListBox*) pControl)->GetCurSel();
	acedRetInt(((CListBox*) pControl)->FindStringExact(nIndex, sSearchString));
	return 0;

}

int ListBox_SetSel()
{
	int nIndex;
	int nBoolean;

	CWnd *pControl = GetArgsControlIntInt(CtlListBox, sListBox_SetSel, nIndex, nBoolean);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	if(nBoolean == 1)
	{
		acedRetInt(((CListBox*) pControl)->SetSel(nIndex, TRUE));
	}
	else
	{
		acedRetInt(((CListBox*) pControl)->SetSel(nIndex, FALSE));
	}
	return 0;
}
int ListBox_GetSel()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetSel, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_GetSel) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	if (((CListBox*) pControl)->GetSel(nIndex) == 1)
		acedRetT();
	else
		acedRetNil();
	return 0;
}
int ListBox_SetFocusIndex()// SetCaretIndex
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_SetFocusIndex, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_SetFocusIndex) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	((CListBox*) pControl)->SetCaretIndex(nIndex, TRUE);
	pControl->SetFocus();
	acedRetVoid();
	return 0;
}
int ListBox_GetFocusIndex()// GetCaretIndex
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetFocusIndex);
	
	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	int nFocusIndex = ((CListBox*) pControl)->GetCaretIndex();
	
	if (nFocusIndex != -1)
		acedRetInt(nFocusIndex);
	else
		acedRetNil();
	return 0;
}
int ListBox_GetSelCount()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetSelCount);
	
	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CListBox*) pControl)->GetSelCount());
	return 0;
}
int ListBox_SelItemRange()
{
	int nStart;
	int nEnd;
	int nBoolean;
	
	CWnd *pControl = GetArgsControlIntIntInt(CtlListBox, sListBox_GetSelCount, nStart, nEnd, nBoolean);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	if (nBoolean == 1)
		acedRetInt(((CListBox*) pControl)->SelItemRange(TRUE, nStart, nEnd));
	else		
		acedRetInt(((CListBox*) pControl)->SelItemRange(FALSE, nStart, nEnd));
	
	return 0;	

}
int ListBox_Dir()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_Dir, &nArg);
	CString sDir;
	CString sExtension;

	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	if (!GetStringArgument(nArg, &sDir, sListBox_Dir) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	if (((CListBoxCtrl*)pControl)->GetTemplate()->GetType() == CtlOptionList)
	{
		acedRetInt(-1); 
		return 0;
	}
	nArg++;

	if (!FindOptionalStringArgument(nArg, &sExtension, sListBox_Dir))
	{
		sExtension = _T("*.*");
	}

	// Add all the files and directories in the windows directory.
	TCHAR lpszOldPath[MAX_PATH];
	TCHAR lpszWinPath[MAX_PATH];
	
	//::GetWindowsDirectory(lpszWinPath, MAX_PATH);
	
	::GetCurrentDirectory(MAX_PATH, lpszOldPath);
	_tcscpy( lpszWinPath, sDir );
	::SetCurrentDirectory(lpszWinPath);

	((CListBox*)pControl)->ResetContent();
	((CListBox*)pControl)->Dir(DDL_READWRITE|DDL_DIRECTORY, sExtension);

	::SetCurrentDirectory(lpszOldPath);

	acedRetVoid();
	return 0;
}

int DwgList_GetType()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sDwgList_GetType, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sDwgList_GetType) || pControl == NULL)
	{	
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CListBox*) pControl)->GetItemData(nIndex));
	return 0;

}

int DwgList_Dir()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlDwgList, sDwgList_Dir, &nArg);
	CString sDir;

	if (pControl == NULL || !GetStringArgument(nArg, &sDir, sDwgList_Dir))
	{
		
		acedRetNil();
		return 0;
	}
	
	((CArxDwgListCtrl*)pControl)->ResetContent();
	((CArxDwgListCtrl*)pControl)->Dir(sDir);
	
	acedRetVoid();
	return 0;
}

int DwgList_GetDir()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlDwgList, sDwgList_GetDir, &nArg);
	CString sDir;

	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	acedRetStr(((CArxDwgListCtrl*)pControl)->m_sPath);
	return 0;
}

int DwgList_GetFileName()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlDwgList, sDwgList_GetFileName, &nArg);
	CString sDir;

	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	CString sString;
	
	int nArgument = ((CListBox*)pControl)->GetCurSel();
	int n = ((CListBox*)pControl)->GetTextLen(nArgument);      
	((CListBox*)pControl)->GetText(nArgument, sString.GetBuffer(n));
	sString.ReleaseBuffer();
	
	acedRetStr(
		((CArxDwgListCtrl*)pControl)->m_sPath + _T("\\") +
		sString);
	return 0;
}

int ListBox_SetAnchorIndex()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_SetAnchorIndex, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sListBox_SetAnchorIndex) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	((CListBox*) pControl)->SetAnchorIndex(nIndex);
	acedRetVoid();
	return 0;
}
int ListBox_GetAnchorIndex()
{
	CWnd *pControl = GetControlPointer(CtlListBox, sListBox_GetAnchorIndex);
	
	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CListBox*) pControl)->GetAnchorIndex());	
	return 0;
}


int OptionList_SetTttTitle()
{
	int nItem;
	CString sText;
	CDclControlObject *pArx = GetLispInput(sOptionList_SetTttTitle, nItem, sText);

	if (pArx == NULL || nItem < 0)
	{
		acedRetNil();
		return 0;
	}

	if (pArx->GetType() != CtlOptionList || // if this list box is not an option list box
			nItem >= pArx->GetPropertyObject(Prop::BtnCaption)->size()) // or the specified item does not exist
	{
		acedRetNil();
		return 0;
	}

	TPropertyPtr pTTTProperty = pArx->GetPropertyObject( Prop::BtnTTText );
	if( !pTTTProperty )
	{
		acedRetNil();
		return 0;
	}
	PropVal::TCStringArray* prsTTT = pTTTProperty->GetStringArrayPtr();
	if( !prsTTT )
	{
		pTTTProperty->AddStringItem( CString() ); //make sure the string array is created
		prsTTT = pTTTProperty->GetStringArrayPtr();
		assert( prsTTT != NULL );
	}
	if( nItem >= prsTTT->size() )
		prsTTT->resize( nItem + 1 );
	prsTTT->at( nItem ) = sText;

	CArxOptionListCtrl* pOptionLst = (CArxOptionListCtrl*)pArx->GetWindow();
	pOptionLst->ResetTooltips();
	
	acedRetT();
	return 0;
}
