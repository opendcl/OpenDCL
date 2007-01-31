// Methods_ListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ListCtrl.h"
#include "DclControlObject.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "SpreadSheet.h"
#include "ControlTypes.h"


static void ReturnRowCol(int nRow, int nCol)
{
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, nRow,
		RTSHORT, nCol,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
}


//*****************************************************************************
// 
// Method: ListCtrl_AddColumn()
// 
// Purpose: [Add a column to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_AddColumn()
{
	CString sStringArg;
	struct resbuf *ListData;
	int nCol;
	CString sColumnHeading;
	int nFormat;
	int nWidth;
	int nImageIndex = -1;
	int nArg=0;
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_AddColumn, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;	
	
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	//ensure AutoLisp has passed Arguments	
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

	
	bool bDoLoop = true;
	while (bDoLoop)
	{
		if (ListData->restype == RTLB) 
		{			
			// advance to the first list item
			ListData = ListData->rbnext;

			// start of inner list
			if (ListData->restype == RTLB) 
			{				
				// advance to the first list item
				ListData = ListData->rbnext;
			}

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotInt) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			
			// get the first argument required
			nCol = ListData->resval.rint;

			// advance to the next list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			
			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotStr) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}

			// get the second argument required
			sColumnHeading = ListData->resval.rstring;

			// advance to the next list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
		

			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotInt) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			// get the third argument required
			nFormat = ListData->resval.rint;
			
			// advance to the next list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotInt) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			// get the fourth argument required
			nWidth = ListData->resval.rint;

			// advance to the next list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			if (ListData->restype == RTSHORT) 
			{
				// get the next argument required
				nImageIndex = ListData->resval.rint;
	
				// advance to the next list item
				ListData = ListData->rbnext;			
			}
			else
				nImageIndex = -1;
			
			// convert the format to the required setting
			switch(nFormat)
			{
				case 1:
					nFormat = LVCFMT_CENTER;
					break;
				case 2:
					nFormat = LVCFMT_RIGHT;
					break;
				case 0:
				default:
					nFormat = LVCFMT_LEFT;
					break;
			}
			// insert the column
			int nRet = pListCtrl->InsertColumn(
							nCol,
							sColumnHeading,
							nFormat,
							nWidth,
							nImageIndex);
			
			if (nImageIndex > -1)
			{
				HD_ITEM curItem;
				LPTSTR  pStrTemp;
				CHeaderCtrl* pHdrCtrl= NULL;

				// retrieve embedded header control			
				pHdrCtrl = pListCtrl->GetHeaderCtrl();

				// add bmaps to each header item
				pHdrCtrl->GetItem(nRet, &curItem);
				curItem.mask= HDI_TEXT | HDI_IMAGE | HDI_FORMAT;
				pStrTemp = sColumnHeading.GetBuffer(sColumnHeading.GetLength());
				curItem.pszText = pStrTemp;
				curItem.iImage= nImageIndex;
			
				switch(nFormat)
				{					
				case 1:
					curItem.fmt= HDF_CENTER | HDF_STRING |HDF_IMAGE;
					break;
				case 2:
					curItem.fmt= HDF_RIGHT | HDF_STRING | HDF_IMAGE;
					break;				
				case 0:
				default:
					curItem.fmt= HDF_LEFT | HDF_STRING | HDF_IMAGE;
					break;
				}
			
				pHdrCtrl->SetItem(nRet, &curItem);
			}
			
			if (ListData->restype != RTLE) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorToManyItemsInList) + sListCtrl_AddColumn);	
				acedRetInt(-1);  return 0; 
			}	
			else
			{
				// advance to the next list item
				ListData = ListData->rbnext;

				// if no more lists are found then exit here
				if (ListData == NULL)
				{
					// return nil
					acedRetVoid();
					return 0;
				}
				// start of inner list
				if (ListData->restype == RTLE) 
				{
					// advance to the first list item
					ListData = ListData->rbnext;
					// if no more lists are found then exit here
					if (ListData == NULL)
					{
						// return nil
						acedRetVoid();
						return 0;
					}
				}
			}
			// if no more lists are found then exit here
			if (ListData == NULL)
			{
				// return nil
				acedRetVoid();
				return 0;
			}
		}
		else
		{	
			// inform the programer that he did not make the correct call
			acedAlert(CString(ErrorListWasExpected) + sListCtrl_AddColumn);			
			acedRetInt(-1);  return 0; 
		}
	}
	
	// return nil
	acedRetVoid();
	return 0;
}



//*****************************************************************************
// 
// Method: ListCtrl_AddRow()
// 
// Purpose: [Add a row to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_AddRow()
{
	ListCtrl_ApplyNewRow(false, sListCtrl_AddRow);
	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_InsertRow()
// 
// Purpose: [inserts a row to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_InsertItem()
{
	ListCtrl_ApplyNewRow(true, sListCtrl_InsertItem);
	return 0;
}


//*****************************************************************************
// 
// Method: ListCtrl_ApplyNewRow()
// 
// Purpose: [does the actual insertion of a new row to a list ctrl]
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_ApplyNewRow(bool bLookForInsertIndex, CString sMethodName)
{
	CString sStringArg;
	struct resbuf *ListData;
	int nIndex;
	int nImage = -1;
	CStringArray sTextArray;
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sMethodName, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sMethodName, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;	
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}


	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	//ensure AutoLisp has passed Arguments	
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

	if (bLookForInsertIndex)
	{
		if (ListData->restype != RTSHORT) 
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListArgNotInt) + sMethodName);	
			acedRetInt(-1);  return 0; 
		}

		// get the first argument required
		nIndex = ListData->resval.rint;

		// advance to the next list item
		ListData = ListData->rbnext;
	}
	else
		nIndex = pListCtrl->GetItemCount();
	

	// see if an image has been requested to be associated
	if (ListData->restype == RTSHORT) 
	{
		// get the first argument required
		nImage = ListData->resval.rint;

		// advance to the next list item
		ListData = ListData->rbnext;	
	}

	// clear the array
	sTextArray.RemoveAll();

	// do loop to get all the strings passed by AutoLisp
	bool bDoLoop = true;
	while (bDoLoop)
	{
		if (ListData->restype == RTLB) 
		{				
			// advance to the first list item
			ListData = ListData->rbnext;
		}

		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sMethodName);	
			acedRetInt(-1);  return 0; 
		}
		
		if (ListData->restype != RTSTR) 
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListArgNotStr) + sMethodName);	
			acedRetInt(-1);  return 0; 
		}

		// get the second argument required
		sTextArray.Add(ListData->resval.rstring);

		// advance to the next list item
		ListData = ListData->rbnext;
		
		// if no more lists are found then exit here
		if (ListData == NULL || ListData->restype == RTLE)
		{
			bDoLoop = false;
		}
	}

	int nInsertIndex = -1;
	if (sTextArray.GetSize() > 0)
	{
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT;
		lvItem.iItem = nIndex;
		lvItem.iSubItem = 0;		
		TCHAR sValue [256];
		_tcscpy(sValue, sTextArray[0]);
		lvItem.pszText = sValue;
		
		lvItem.iImage = nImage;
		lvItem.iIndent = 0;
		
		// insert the row item
		nInsertIndex = pListCtrl->InsertItem(&lvItem);
		
		for (int i=1; i<sTextArray.GetSize(); i++)
		{
			if (!pListCtrl->SetItemText(nInsertIndex, i, sTextArray[i]))
				break;
		}
	}


	// clear the text array
	sTextArray.RemoveAll();

	// return the insertion index
	acedRetInt(nInsertIndex);

	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_Clear()
// 
// Purpose: [return the item count]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_Clear()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_Clear);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_Clear, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;	
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	// delete all the items
	pListCtrl->DeleteAllItems();
	
	if (pListCtrl->GetHeaderCtrl() != NULL)
	{
		// Delete all of the columns.
		while (pListCtrl->GetHeaderCtrl()->GetItemCount() > 0)
		{
		   pListCtrl->DeleteColumn(0);
		}
	}

	// return nil
	acedRetVoid();
	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_CountItems()
// 
// Purpose: [return the item count]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_CountItems()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_CountItems);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_CountItems, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	// return the count
	acedRetInt(pListCtrl->GetItemCount());
	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_GetSelectedCount()
// 
// Purpose: [return the selected item count]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetSelectedCount()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetSelectedCount);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetSelectedCount, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	// return the count
	acedRetInt(pListCtrl->GetSelectedCount());
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_GetItemData()
// 
// Purpose: [return the item data of an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetItemData()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetItemData, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetItemData, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nIndex;
	if (pArx == NULL || !GetIntArgument(nArg, &nIndex, sListCtrl_GetItemData))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
	// return the count
	acedRetInt(pListCtrl->GetItemData(nIndex));
	return 0;
}


//*****************************************************************************
// 
// Method: ListCtrl_GetItemImage()
// 
// Purpose: [return the item image of an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetItemImage()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetItemImage, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetItemImage, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nIndex = 0;
	int nColIndex = 0;

	if (pArx == NULL || !GetIntArgument(nArg, &nIndex, sListCtrl_GetItemImage))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nColIndex, sListCtrl_GetItemImage))
		nColIndex = 0;
	
	// return the count
	acedRetInt(pListCtrl->GetItemImage(nIndex, nColIndex));
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_SetItemData()
// 
// Purpose: [return the sets the item data of an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_SetItemData()
{
	int nIndex=0;
	int nItemData = 0;
	int nArg=0;
	//CWnd *pControl = GetArgsControlIntInt(CtlListView,sListCtrl_SetItemData, nIndex, nItemData);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SetItemData, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL || pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	pListCtrl->SetItemData(nIndex, nItemData);
	// return nil
	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_GetItemText()
// 
// Purpose: [return the item's Text]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetItemText()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetItemText, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetItemText, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nRowIndex;
	int nColIndex;
	if (pArx == NULL || !GetIntArgument(nArg, &nRowIndex, sListCtrl_GetItemText))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nColIndex, sListCtrl_GetItemText))
		nColIndex = 0;
		
	// return the count
	acedRetStr(pListCtrl->GetItemText(nRowIndex, nColIndex));
	return 0;
}


//*****************************************************************************
// 
// Method: ListCtrl_GetRow()
// 
// Purpose: [return the row Text in a list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetRow()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetRow, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetRow, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nRowIndex;
	if (pArx == NULL || !GetIntArgument(nArg, &nRowIndex, sListCtrl_GetRow))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	// Get the indexes of all the selected items.
	int nCount = pListCtrl->GetItemCount();

	// if nothing selected
	if (nCount == 0)
	{
		// return nil
		acedRetNil();
		return 0;
	}
	
	if (pListCtrl->GetHeaderCtrl() != NULL)
	{
		// get the column count
		int nColumnCount = pListCtrl->GetHeaderCtrl()->GetItemCount();

		for (int j=0; j<nColumnCount; j++)
		{	
			// get the text name of the selected line number
			CString sTextItem = pListCtrl->GetItemText(nRowIndex, j);

			acutNewString(sTextItem, rbpTail->resval.rstring);
			if ((j+1) < nColumnCount)
			{
				rbpTail->rbnext = acutNewRb(RTSTR);
				rbpTail = rbpTail->rbnext;
			}
		}
	}
	
	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);
	return 0;
}


//*****************************************************************************
// 
// Method: ListCtrl_LoadDwg()
// 
// Purpose: [load a dwg for block listing.]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_LoadDwg()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_LoadDwg, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_LoadDwg, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	CString sFileName;
	if (pArx == NULL || !GetStringArgument(nArg, &sFileName, sListCtrl_LoadDwg))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	if (pListCtrl->m_ArxControl->GetType() != CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	pListCtrl->LoadDwg(sFileName);

	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_Reset()
// 
// Purpose: [Resets the block list to display the blocks of the current dwg]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_Reset()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_Reset, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_Reset, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	if (pListCtrl->m_ArxControl->GetType() != CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	// if a drawing has been previously loaded
	if (pListCtrl->m_pLoadedDwg)
	{
		// delete it.
		delete pListCtrl->m_pLoadedDwg;
		pListCtrl->m_pLoadedDwg = NULL;
		pListCtrl->m_FileName = CString();
	}

	pListCtrl->RefreshBlockList();

	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_GetFileName()
// 
// Purpose: [Retrieves the file name of the loaded dwg]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetFileName()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetFileName, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetFileName, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;


	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	if (pListCtrl->m_ArxControl->GetType() != CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	// if a drawing has been previously loaded
	if (pListCtrl->m_pLoadedDwg)
	{
		acedRetStr(pListCtrl->m_FileName);
	}
	else
		acedRetNil();
	
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_GetColumn()
// 
// Purpose: [return the column Text in a list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetColumn()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetColumn, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetColumn, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;


	int nColumnIndex;
	if (pArx == NULL || !GetIntArgument(nArg, &nColumnIndex, sListCtrl_GetColumn))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	// Get the indexes of all the selected items.
	int nCount = pListCtrl->GetItemCount();
	// get the column count
	int nColumnCount = pListCtrl->GetHeaderCtrl()->GetItemCount();

	// if nothing selected
	if (nCount == 0 || nColumnCount == 0 || nColumnCount <= nColumnIndex)
	{
		// return nil
		acedRetNil();
		return 0;
	}

	
	for (int j=0; j<nCount; j++)
	{	
		// get the text name of the selected line number
		CString sTextItem = pListCtrl->GetItemText(j, nColumnIndex);

		acutNewString(sTextItem, rbpTail->resval.rstring);
		if ((j+1) < nCount)
		{
			rbpTail->rbnext = acutNewRb(RTSTR);
			rbpTail = rbpTail->rbnext;
		}
	}


	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_SetItemText()
// 
// Purpose: [sets the item's Text]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_SetItemText()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SetItemText, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SetItemText, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nRowIndex;
	int nColIndex;
	CString sNewText;

	if (pArx == NULL || !GetStringArgument(nArg, &sNewText, sListCtrl_SetItemText))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
	nArg++;
	if (pArx == NULL || !GetIntArgument(nArg, &nRowIndex, sListCtrl_SetItemText))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	nArg++;	
	if (!FindOptionalIntArgument(nArg, &nColIndex, sListCtrl_GetItemText))
		nColIndex = 0;
		
	pListCtrl->SetItemText(nRowIndex, nColIndex, sNewText);
	
	acedRetVoid();
	return 0;
}



//*****************************************************************************
// 
// Method: ListCtrl_SetItemImage()
// 
// Purpose: [sets the item's Image]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_SetItemImage()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SetItemImage, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SetItemImage, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nArg1;
	int nArg2;
	int nArg3;

	if (pArx == NULL || !GetIntArgument(nArg, &nArg1, sListCtrl_SetItemImage))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
	nArg++;
	if (pArx == NULL || !GetIntArgument(nArg, &nArg2, sListCtrl_SetItemImage))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	nArg++;	
	if (!FindOptionalIntArgument(nArg, &nArg3, sListCtrl_SetItemImage))
	{
		nArg3 = nArg2;
		nArg2 = 0;
	}
		
	pListCtrl->SetItemImage(nArg1, nArg2, nArg3);

	
	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_SetColWidth()
// 
// Purpose: [sets the column width]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_GetColWidth()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetColWidth, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetColWidth, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nColIndex;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nColIndex, sListCtrl_GetColWidth))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	acedRetInt(pListCtrl->GetColumnWidth(nColIndex));
	return 0;
}


//*****************************************************************************
// 
// Method: ListCtrl_SetColWidth()
// 
// Purpose: [sets the column width]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_SetColWidth()
{
	int nColIndex = 0;
	int nArg=0;
	int nNewWidth = 0;
	//CWnd *pControl = GetArgsControlIntInt(CtlListView, sListCtrl_SetColWidth, nColIndex, nNewWidth);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SetColWidth, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL || pListCtrl->m_ArxControl->GetType() == CtlBlockList) 	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	if (!GetIntArgument(nArg, &nColIndex, sListCtrl_SetCurSel))
	{
		
		acedRetInt(-1);
		return 0;
	}
	nArg++;

	if (!GetIntArgument(nArg, &nNewWidth, sListCtrl_SetCurSel))
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	pListCtrl->SetColumnWidth(nColIndex, nNewWidth);

	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_SetCurSel()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_SetCurSel()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SetCurSel, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SetCurSel, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nIndex;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nIndex, sListCtrl_SetCurSel))
	{
		
		acedRetInt(-1);
		return 0;
	}

	if (nIndex > -1)
	{
		pListCtrl->SetItem(nIndex, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);		
		pListCtrl->EnsureVisible(nIndex, TRUE);
	}
	else
	{
		int nCount = pListCtrl->GetItemCount();
		for (int i=0; i<nCount; i++)
		{
			pListCtrl->SetItemState(i, 0, LVIS_SELECTED);
			pListCtrl->SetItemState(i, 0, LVIS_FOCUSED);
		}		
	}

	acedRetVoid();
	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_CalcColumnWidth()
// 
// Purpose: [returns the min required width of a column]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_CalcColumnWidth()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_CalcColumnWidth, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_CalcColumnWidth, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	CString sText;
	
	if (pArx == NULL || !GetStringArgument(nArg, &sText, sListCtrl_CalcColumnWidth))
	{
		
		acedRetInt(-1);
		return 0;
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

    acedRetInt(int(pListCtrl->GetStringWidth(sText)*0.5*3));

	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_Arrange()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_Arrange()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_Arrange, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_Arrange, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	

	int nArrangeStyle;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nArrangeStyle, sListCtrl_Arrange))
	{
		
		acedRetInt(-1);
		return 0;
	}
	switch(nArrangeStyle)
	{
	case 0:
		nArrangeStyle = LVA_ALIGNLEFT;
		break;
	case 1:
		nArrangeStyle = LVA_ALIGNTOP;
		break;
	case 2:
		nArrangeStyle = LVA_DEFAULT;
		break;
	case 3:
		nArrangeStyle = LVA_SNAPTOGRID;
		break;
	}
	pListCtrl->Arrange(nArrangeStyle);

	acedRetVoid();
	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_DeleteItems()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_DeleteItems()
{
	int nArg;
	struct resbuf *ListData;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_DeleteItems, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_DeleteItems, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	//ensure AutoLisp has passed Arguments	
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
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	// do loop to get all the strings passed by AutoLisp
	bool bDoLoop = true;
	while (bDoLoop)
	{
		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sListCtrl_DeleteItems);	
			acedRetInt(-1);  return 0; 
		}
		
		if (ListData->restype == RTSHORT) 
			pListCtrl->DeleteItem(ListData->resval.rint);
		else if (ListData->restype == RTREAL) 
			pListCtrl->DeleteItem((int)ListData->resval.rreal);
		else if (ListData->restype == RT3DPOINT) 
		{
			pListCtrl->DeleteItem((int)ListData->resval.rpoint[X]);
			pListCtrl->DeleteItem((int)ListData->resval.rpoint[Y]);
			pListCtrl->DeleteItem((int)ListData->resval.rpoint[Z]);
		}
		else if (ListData->restype == RTPOINT) 
		{
			pListCtrl->DeleteItem((int)ListData->resval.rpoint[X]);
			pListCtrl->DeleteItem((int)ListData->resval.rpoint[Y]);
		}
		else
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListArgNotInt) + sListCtrl_DeleteItems);	
			acedRetInt(-1);  return 0; 
		}
		// advance to the next list item
		ListData = ListData->rbnext;
		
		// if no more lists are found then exit here
		if (ListData == NULL || ListData->restype == RTLE)
		{
			bDoLoop = false;
		}
	}

	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: ListCtrl_DeleteItems()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_DeleteColumns()
{
	struct resbuf *ListData;
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_DeleteColumns, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_DeleteColumns, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	//ensure AutoLisp has passed Arguments	
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
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	// do loop to get all the strings passed by AutoLisp
	bool bDoLoop = true;
	while (bDoLoop)
	{
		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sListCtrl_DeleteColumns);	
			acedRetInt(-1);  return 0; 
		}
		
			if (ListData->restype == RTSHORT) 
			pListCtrl->DeleteColumn(ListData->resval.rint);
		else if (ListData->restype == RTREAL) 
			pListCtrl->DeleteColumn((int)ListData->resval.rreal);
		else if (ListData->restype == RT3DPOINT) 
		{
			pListCtrl->DeleteColumn((int)ListData->resval.rpoint[X]);
			pListCtrl->DeleteColumn((int)ListData->resval.rpoint[Y]);
			pListCtrl->DeleteColumn((int)ListData->resval.rpoint[Z]);
		}
		else if (ListData->restype == RTPOINT) 
		{
			pListCtrl->DeleteColumn((int)ListData->resval.rpoint[X]);
			pListCtrl->DeleteColumn((int)ListData->resval.rpoint[Y]);
		}
		else
		{
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListArgNotInt) + sListCtrl_DeleteItems);	
			acedRetInt(-1);  return 0; 
		}
		// advance to the next list item
		ListData = ListData->rbnext;
		
		// if no more lists are found then exit here
		if (ListData == NULL || ListData->restype == RTLE)
		{
			bDoLoop = false;
		}
	}

	acedRetVoid();
	return 0;
}
//*****************************************************************************
// 
// Method: ListCtrl_FillGrid()
// 
// Purpose: [Adds multiple rows to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ListCtrl_FillGrid()
{
	struct resbuf *ListData;
	int nImage = -1;
	CString sText;
	int nBeforeAdditionCount;
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_FillGrid, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_FillGrid, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	//ensure AutoLisp has passed Arguments	
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

	int nCurCol = 0;
	int nIndex = -1;
	bool bDoLoop = true;

	// delete all the items
	pListCtrl->DeleteAllItems();
	
	// get the count before the addition of any rows
	nBeforeAdditionCount = pListCtrl->GetItemCount();

	pListCtrl->SetRedraw(FALSE); // turn drawing off regardless of list mode

	while (bDoLoop)
	{
		// start of main list
		if (ListData->restype == RTLB) 
		{				
			// advance to the first list item
			ListData = ListData->rbnext;
			nIndex = 1;
			nCurCol++;
		}
		// start of inner list
		if (ListData->restype == RTLB) 
		{				
			// advance to the first list item
			ListData = ListData->rbnext;
		}
		if (ListData == NULL)
		{
			pListCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pListCtrl->Invalidate();
			pListCtrl->UpdateWindow(); 
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sListCtrl_FillGrid);	
			acedRetInt(-1);  return 0; 
		}
		
		if (ListData->restype != RTSTR) 
		{
			pListCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pListCtrl->Invalidate();
			pListCtrl->UpdateWindow(); 
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListArgNotStr) + sListCtrl_FillGrid);	
			acedRetInt(-1);  return 0; 
		}

		// get the first argument required
		sText = ListData->resval.rstring;
		
		// advance to the next list item
		ListData = ListData->rbnext;				
		
		if (ListData == NULL)
		{
			pListCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pListCtrl->Invalidate();
			pListCtrl->UpdateWindow(); 
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sListCtrl_FillGrid);	
			acedRetInt(-1);  return 0; 
		}
		
		if (ListData->restype == RTSHORT) 
		{
			// get the second argument required
			nImage = ListData->resval.rint;

			// advance to the next list item
			ListData = ListData->rbnext;				
		}
		//else
		//	nImage = -1;

		int nCount = pListCtrl->GetItemCount();

		if (nCurCol == 1)
		{
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT;
			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;
			TCHAR sValue [256];
			_tcscpy(sValue, sText);
			lvItem.pszText = sValue;
			lvItem.iImage = nImage;
			lvItem.iIndent = 0;
			// insert the column
			int nRet = pListCtrl->InsertItem(&lvItem);
			/*
						nCount,
						sText,
						nImage);
						*/
		}
		else
		{
			
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
			lvItem.iItem = nIndex + nBeforeAdditionCount-1;
			lvItem.iSubItem = nCurCol-1;
			TCHAR sValue [256];
			_tcscpy(sValue, sText);
			lvItem.pszText = sValue;
			lvItem.iImage = nImage;
			
			// insert the column
			int nRet = pListCtrl->SetItem(&lvItem);				
			/*
						nIndex + nBeforeAdditionCount-1,
						nCurCol-1,
						sText);
			*/			
		}
		

		if (ListData->restype == RTLE) 
		{
			// advance to the next list item
			ListData = ListData->rbnext;	

			if (ListData == NULL || ListData->restype == RTLE) 
			{
				pListCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
				pListCtrl->Invalidate();
				pListCtrl->UpdateWindow(); 
			
				// return nil
				acedRetVoid();
				return 0;
			}	
		}	

		// increment the index counter
		nIndex++;
		
		// if no more lists are found then exit here
		if (ListData == NULL)
		{
			
			pListCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pListCtrl->Invalidate();
			pListCtrl->UpdateWindow(); 
			
			// return nil
			acedRetVoid();
			return 0;
		}
	}
	
	pListCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
	pListCtrl->Invalidate();
	pListCtrl->UpdateWindow(); 
		
	// return nil
	acedRetVoid();
	return 0;
}

int ListCtrl_GetSelectedItems()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetSelectedItems);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetSelectedItems, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}

	
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;

	
	
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}
	else
	{
	   while (pos)
	   {
			int nItem = pListCtrl->GetNextSelectedItem(pos);
			CString sItemText = pListCtrl->GetItemText(nItem, 0);
			
			acutNewString(sItemText, rbpTail->resval.rstring);
			if (pos != NULL)
			{
				rbpTail->rbnext = acutNewRb(RTSTR);
				rbpTail = rbpTail->rbnext;
			}
	   }
	}

	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);
	return 0;
}

int ListCtrl_GetSelectedNths()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_GetSelectedNths);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetSelectedNths, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}

	
	// Convert the array to a list that can be returned
	//struct resbuf* rbpRetList = acutNewRb(RTSTR);
	//struct resbuf* rbpTail = rbpRetList;
	struct resbuf *pRBList = NULL, *pRB = NULL, *pRBTrav = NULL;
	
	int nCount = 0;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}
	else
	{
	   while (pos)
	   {
			int nItem = pListCtrl->GetNextSelectedItem(pos);
			pRB = acutNewRb(RTSHORT);	//	create new resbuf
			pRB->resval.rint = nItem;


			if(nCount == 0)
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
			nCount++;
	   }
	}

	acedRetList(pRBList);
	acutRelRb(pRBList);
	return 0;
}


int ListCtrl_StartLabelEdit()
{
	int nArg = 0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_StartLabelEdit, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_StartLabelEdit, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	int nItem = 0;
	int nSubItem = 0;

	if (pArx == NULL || !GetIntArgument(nArg, &nItem, sListCtrl_StartLabelEdit))
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlListView)
	{
		// Make sure the focus is set to the list view control.
		pListCtrl->SetFocus();

		pListCtrl->m_nEditSubItem = nSubItem;		

		// Show the edit control on the label of the indicated item
		CEdit *pEdit = pListCtrl->EditLabel(nItem);
		
		
	}
	acedRetVoid();
	return 0;
}



int ListCtrl_CancelLabelEdit()
{
	int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_CancelLabelEdit);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_CancelLabelEdit, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlListView)
	{
		// Make sure the focus is set to the list view control.
		CEdit *pEdit = pListCtrl->GetEditControl();

		pEdit->ShowWindow(FALSE);
	}
	acedRetVoid();
	return 0;
}


int ListCtrl_SortColTextItems()
{
	int nCol=0;
	int nAscending;
	int nArg=0;

	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SortTextItems, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SortTextItems, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nCol, sListCtrl_SortTextItems))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nAscending, sListCtrl_SortTextItems))
	{
		//nAscending = nCol;
	}

    try
	{
		pListCtrl->SortTextItems(nCol, nAscending);
	}
	catch(...)
	{
	}
	
	acedRetVoid();
	return 0;
}

int ListCtrl_SortColNumericItems()
{
	int nCol;
	int nAscending;
	int nArg=0;

	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SortNumericItems, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SortNumericItems, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nCol, sListCtrl_SortNumericItems))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nAscending, sListCtrl_SortNumericItems))
	{
		//nAscending = nCol;
	}

	try
	{
		pListCtrl->SortNumericItems(nCol, nAscending);
	}
	catch(...)
	{
	}
	
	acedRetVoid();
	return 0;
}


int ListCtrl_HitPointTest()
{
	int nX;
	int nY;
	int nArg=0;

	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_HitPointTest, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_HitPointTest, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nX, sListCtrl_HitPointTest))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	if (pArx == NULL || !GetIntArgument(nArg, &nY, sListCtrl_HitPointTest))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CPoint pt(nX, nY);
	UINT Flags;

	int nRow = pListCtrl->HitTest(pt, &Flags);
	
	LVHITTESTINFO lvhti;

	// Clear the subitem text the user clicked on.
	lvhti.pt = pt;
	pListCtrl->SubItemHitTest(&lvhti);

	if (lvhti.flags & LVHT_ONITEMLABEL)
	{
		ReturnRowCol(lvhti.iItem, lvhti.iSubItem);
		return 0;
	}
	else
	{
		acedRetInt(nRow);
		return 0;
	}


	acedRetNil();
	return 0;
}



//*****************************************************************************
// 
// Method: ListCtrl_SetColumnImage()
// 
// Purpose: [sets the column's Image]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int ListCtrl_SetColumnImage()
{
	int nArg;
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_SetColumnImage, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nCol;
	int nImage;

	if (pArx == NULL || !GetIntArgument(nArg, &nCol, sListCtrl_SetColumnImage))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
	nArg++;
	if (pArx == NULL || !GetIntArgument(nArg, &nImage, sListCtrl_SetColumnImage))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	CHeaderCtrl *pHeader = pListCtrl->GetHeaderCtrl();

	if (pHeader == NULL)
	{
		acedRetVoid();
		return 0;
	}
	
	if (pHeader->GetImageList() == NULL)
	{
		acedRetVoid();
		return 0;
	}
	
	HDITEM item;

	TCHAR  lpBuffer[256];
	item.mask = HDI_TEXT|HDI_IMAGE;
	item.pszText = lpBuffer;
	item.cchTextMax = 256;

	try
	{
		if (pHeader->GetItem(nCol, &item) == FALSE)
		{
			acedRetVoid();
			return 0;
		}
	}
	catch(...)
	{
	}
	
	item.iImage = nImage;

	try
	{
		pHeader->SetItem(nCol, &item);
	}
	catch(...)
	{
	}

	acedRetVoid();
	return 0;
}

int ListCtrl_GetColumnImage()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SetItemImage, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetColumnImage, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	int nCol;
	int nImage;

	if (pArx == NULL || !GetIntArgument(nArg, &nCol, sListCtrl_SetItemImage))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
		
	CHeaderCtrl *pHeader = pListCtrl->GetHeaderCtrl();

	HDITEM item;

	TCHAR  lpBuffer[256];
	item.mask = HDI_TEXT|HDI_IMAGE;
	item.pszText = lpBuffer;
	item.cchTextMax = 256;


	pHeader->GetItem(nCol, &item);
		
	nImage = item.iImage;

	acedRetInt(nImage);
	return 0;
}
int ListCtrl_GetCount()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SetItemImage, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetCount, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
		
	acedRetInt(pListCtrl->GetItemCount());
	return 0;
}
int ListCtrl_GetColumnCount()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sListCtrl_SetItemImage, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sListCtrl_GetColumnCount, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}
		
	CHeaderCtrl *pHeader = pListCtrl->GetHeaderCtrl();

	acedRetInt(pHeader->GetItemCount());
	return 0;
}


//*****************************************************************************
// 
// Method: ListCtrl_AddString()
// 
// Purpose: [does the actual insertion of a new row to a list ctrl]
// 
// Returns:	int
// 
//*****************************************************************************

const TCHAR sSeperator[] = _T("\t");
const TCHAR sSpace[] = _T(" ");

int ListCtrl_AddString()
{
	int nArg=0;
	CString sStringArg;
	CString sDivider;
	int nInsertIndex;

	CDclControlObject *pArx = GetControlArxObject(sListCtrl_AddString, &nArg);
	OdclListCtrl *pListCtrl = (OdclListCtrl*)pArx->m_pWnd;	
	pListCtrl->m_ArxControl = pArx;

	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	if (pListCtrl->m_ArxControl->GetType() == CtlBlockList)
	{
		acedRetInt(-1); 
		return 0;
	}

	
	if (!GetStringArgument(nArg, &sStringArg, sListCtrl_AddString))
	{		
		acedRetInt(-1);
		return 0;
	}

	nArg++;
	
	if (!FindOptionalStringArgument(nArg, &sDivider, sListCtrl_AddString))
	{
		sDivider = sSeperator;
	}

	int nStart = 0;
	int nNext = sStringArg.Find(sDivider, nStart);
	int nCol = 0;

	while (nNext > -1)
	{
		CString sCell;

		if (nStart > 0)
			sCell = sStringArg.Mid(nStart+1, nNext);
		else
			sCell = sStringArg.Mid(nStart, nNext);

		sCell.TrimLeft(sSpace);
		sCell.TrimRight(sSpace);

		if (sCell.GetLength() > 0 && nCol == 0)
		{
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT|LVIF_INDENT|LVIF_IMAGE;
			lvItem.iItem = pListCtrl->GetItemCount()+1;
			lvItem.iSubItem = 0;
			TCHAR sValue [256];
			_tcscpy(sValue, sCell);
			lvItem.pszText = sValue;
			lvItem.iImage = -1;
			lvItem.iIndent = 0;
			
			
			// insert the row item
			nInsertIndex = pListCtrl->InsertItem(&lvItem);
			
/*
			
			lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
			lvItem.iItem = nInsertIndex;
			lvItem.iSubItem = 0;	
			lvItem.iImage = -1;	
			lvItem.pszText = sValue;

			pListCtrl->SetItem(&lvItem);
*/
			nCol++;
		}
		else
		{
			if (!pListCtrl->SetItemText(nInsertIndex, nCol, sCell))
			{
				acedRetVoid();
				return 0;
			}
			nCol++;
		}

		sStringArg = sStringArg.Mid(nNext+1);
		//nStart = nNext;
		nNext = sStringArg.Find(sDivider, nStart+1);

		if (nNext == -1 && sStringArg.GetLength() > 0)
			nNext = sStringArg.GetLength();
	}

	

	acedRetInt(nInsertIndex);
	return 0;
}
