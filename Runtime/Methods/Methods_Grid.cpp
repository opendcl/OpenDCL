// Methods_Grid.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Grid.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"


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
// Method: Grid_AddColumn()
// 
// Purpose: [Add a column to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_AddColumn()
{
	CString sStringArg;
	struct resbuf *ListData;
	int nCol;
	CString sColumnHeading;
	int nFormat;
	int nWidth;
	int nImageIndex = -1;
	int nArg=0;
	CDclControlObject *pArx = GetControlArxObject(sGrid_AddColumn, &nArg);

	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();	
  RefCountedPtr< CPropertyObject > m_pColCaptions = pArx->GetPropertyObject(nColumnCaptions);	

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
				acedAlert(CString(ErrorListNotSet) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotInt) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			
			// get the first argument required
			nCol = ListData->resval.rint;

			// advance to the next list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			
			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotStr) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}

			// get the second argument required
			sColumnHeading = ListData->resval.rstring;

			// advance to the next list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
		

			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotInt) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			// get the third argument required
			nFormat = ListData->resval.rint;
			
			// advance to the next list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListArgNotInt) + sGrid_AddColumn);	
				acedRetInt(-1);  return 0; 
			}
			// get the fourth argument required
			nWidth = ListData->resval.rint;

			// advance to the next list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				acedAlert(CString(ErrorListNotSet) + sGrid_AddColumn);	
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
			int nRet = pGridCtrl->InsertColumn(
							nCol,
							sColumnHeading,
							nFormat,
							nWidth,
							nImageIndex);
			
			_RowData *pCells = pGridCtrl->GetRowData(nRet);
			
			if (pCells != NULL && pCells->m_Cells.GetCount() < nRet)
			{
				for (int i=0; i<pGridCtrl->GetItemCount(); i++)
				{
					_RowData *pData = pGridCtrl->GetRowData(i);

					_Style *pStyle = NULL;
					pData->m_Cells.InsertAt(nRet, pStyle);					
				}
			}
		
			if (nImageIndex > -1)
			{
				HD_ITEM curItem;
				LPTSTR  pStrTemp;
				CHeaderCtrl* pHdrCtrl= NULL;

				// retrieve embedded header control			
				pHdrCtrl = pGridCtrl->GetHeaderCtrl();

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
				acedAlert(CString(ErrorToManyItemsInList) + sGrid_AddColumn);	
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
			acedAlert(CString(ErrorListWasExpected) + sGrid_AddColumn);			
			acedRetInt(-1);  return 0; 
		}
	}
	
	// return nil
	acedRetVoid();
	return 0;
}



//*****************************************************************************
// 
// Method: Grid_AddRow()
// 
// Purpose: [Add a row to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_AddRow()
{
	Grid_ApplyNewRow(false, sGrid_AddRow);
	return 0;
}
//*****************************************************************************
// 
// Method: Grid_InsertRow()
// 
// Purpose: [inserts a row to a grid ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_InsertRow()
{
	Grid_ApplyNewRow(true, sGrid_InsertRow);
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_ApplyNewRow()
// 
// Purpose: [does the actual insertion of a new row to a list ctrl]
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_ApplyNewRow(bool bLookForInsertIndex, CString sMethodName)
{
	CString sStringArg;
	struct resbuf *ListData;
	int nIndex;
	int nImage = -1;
	CStringArray sTextArray;
	CArray<int, int> nIntArray;
	int nArg=0;

	CDclControlObject *pArx = GetControlArxObject(sMethodName, &nArg);
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();	

	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();

  int i;	
	for (i = 0; i < nArg; i++)
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
		nIndex = pGridCtrl->GetItemCount();
	

	// see if an image has been requested to be associated
	if (ListData->restype == RTSHORT) 
	{
		// get the first argument required
		nIntArray.Add(ListData->resval.rint);

		// advance to the next list item
		ListData = ListData->rbnext;	
	}
	else
	{
		// get the first argument required
		nIntArray.Add(-1);

	}

	// clear the array
	sTextArray.RemoveAll();

	// do loop to get all the strings passed by AutoLISP
	bool bDoLoop = true;
	while (bDoLoop)
	{
		if (ListData->restype == RTLB) 
		{				
			// advance to the first list item
			ListData = ListData->rbnext;
		}

		// if no more lists are found then exit here
		if (ListData->restype == RTSHORT)
		{
			nIntArray.Add(ListData->resval.rint);
			ListData = ListData->rbnext;
		}
		else if (ListData->restype == RTLONG)
		{
			nIntArray.Add(ListData->resval.rlong);
			ListData = ListData->rbnext;
		}
		else if (ListData->restype == RTREAL)
		{
			nIntArray.Add(ListData->resval.rreal);
			ListData = ListData->rbnext;
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
		
		lvItem.iImage = nIntArray[0];
		lvItem.iIndent = 0;
		
		// insert the row item
		nInsertIndex = pGridCtrl->InsertItem(&lvItem);
	}

	// now set the rest of the cells.
	for (i=1; i<sTextArray.GetSize(); i++)
	{
		pGridCtrl->SetItemText(nInsertIndex, i, sTextArray[i]);
		pGridCtrl->SetItemImage(nInsertIndex, i, (i < nIntArray.GetSize())? nIntArray[i] : nIntArray[0]);
	}
	
	// clear the text array
	sTextArray.RemoveAll();
	
	// return the insertion index
	acedRetInt(nInsertIndex);

	return 0;
}

//*****************************************************************************
// 
// Method: Grid_Clear()
// 
// Purpose: [return the item count]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_Clear()
{
	int nArg=0;
	CDclControlObject *pArx = GetControlArxObject(sGrid_Clear, &nArg);
	if (pArx == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();	
	for (int i=0; i<pGridCtrl->GetItemCount(); i++)
	{
		if (pGridCtrl->GetItemData(i) > 0)
		{
			delete (_RowData *)pGridCtrl->GetItemData(i);
		}
	}
	// delete all the items
	pGridCtrl->DeleteAllItems();

	// return nil
	acedRetVoid();
	return 0;
}
//*****************************************************************************
// 
// Method: Grid_GetItemData()
// 
// Purpose: [return the item data of an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_GetItemData()
{
	int nIndex;
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nIndex);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	long lData = 0;	
	_RowData *pData = pGridCtrl->GetRowData(nIndex);
	if (pData != NULL)
		lData = pData->nItemData;

	// return the count
	acedRetInt(lData);
	return 0;
}


//*****************************************************************************
// 
// Method: Grid_GetItemImage()
// 
// Purpose: [return the item image of an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_GetItemImage()
{
	int nIndex = 0;
	int nColIndex = 0;
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nIndex, nColIndex);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	// return the count
	acedRetInt(pGridCtrl->GetItemImage(nIndex, nColIndex));
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_SetItemData()
// 
// Purpose: [return the sets the item data of an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_SetItemData()
{
	int nIndex=0;
	long lItemData = 0;

	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nIndex, lItemData);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	_RowData *pData = pGridCtrl->GetRowData(nIndex);
	if (pData == NULL)
	{
		pData = new _RowData; 
		pGridCtrl->SetItemData(nIndex, (DWORD_PTR)pData);
	}

	pData->nItemData = lItemData;
	
	// return nil
	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_GetItemText()
// 
// Purpose: [return the item's Text]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_GetItemText()
{
	int nRowIndex;
	int nColIndex = 0;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nRowIndex, nColIndex);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		
	// return the count
	acedRetStr(pGridCtrl->GetItemText(nRowIndex, nColIndex));
	return 0;
}


//*****************************************************************************
// 
// Method: Grid_GetRow()
// 
// Purpose: [return the row Text in a list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_GetRow()
{
	int nRowIndex;
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nRowIndex);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	// Get the indexes of all the selected items.
	int nCount = pGridCtrl->GetItemCount();

	// if nothing selected
	if (nCount == 0)
	{
		// return nil
		acedRetNil();
		return 0;
	}
	
	if (pGridCtrl->GetHeaderCtrl() != NULL)
	{
		// get the column count
		int nColumnCount = pGridCtrl->GetHeaderCtrl()->GetItemCount();

		for (int j=0; j<nColumnCount; j++)
		{	
			// get the text name of the selected line number
			CString sTextItem = pGridCtrl->GetItemText(nRowIndex, j);

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
// Method: Grid_GetColumn()
// 
// Purpose: [return the column Text in a list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_GetColumn()
{
	int nColumnIndex;
	
	CDclControlObject *pArx = GetLispInput(sGrid_GetColumn, nColumnIndex);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	// Get the indexes of all the selected items.
	int nCount = pGridCtrl->GetItemCount();
	// get the column count
	int nColumnCount = pGridCtrl->GetHeaderCtrl()->GetItemCount();

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
		CString sTextItem = pGridCtrl->GetItemText(j, nColumnIndex);

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
// Method: Grid_SetItemText()
// 
// Purpose: [sets the item's Text]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_SetItemText()
{
	int nRowIndex;
	int nColIndex;
	CString sNewText;

	CDclControlObject *pArx = GetLispInput(sGrid_SetItemImage, nRowIndex, nColIndex, sNewText);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		
	pGridCtrl->SetItemText(nRowIndex, nColIndex, sNewText);
	
	acedRetVoid();
	return 0;
}

		
//*****************************************************************************
// 
// Method: Grid_Cell_SetStyle()
// 
// Purpose: [sets the cell editing style]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_Cell_SetStyle()
{
	int nRow;
	int nCol;
	int nStyle;
	int nData1 = 0;
	int nData2 = 0;
	CString sOptionalText;

	CDclControlObject *pArx = GetLispInput(sGrid_Cell_SetStyle, nRow, nCol, nStyle, nData1, nData2, sOptionalText);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	pGridCtrl->SetCellStyle(nRow, nCol, nStyle);

	if (pGridCtrl->GetCellData(nRow, nCol) != NULL)
	{
		pGridCtrl->GetCellData(nRow, nCol)->nData1 = nData1;
		pGridCtrl->GetCellData(nRow, nCol)->nData2 = nData2;
	}

	switch(nStyle)
	{
	case Grid_DwgFilesCell:
		{
			if (pGridCtrl->GetCellData(nRow, nCol) != NULL)
			{
				pGridCtrl->GetCellData(nRow, nCol)->sStrings.RemoveAll();
				pGridCtrl->GetCellData(nRow, nCol)->sStrings.Add(sOptionalText);
			}
			break;
		}
	case Grid_UpperCase:
	case Grid_UpperCase_Combo:
		{
			CString sText;
			sText = pGridCtrl->GetItemText(nRow, nCol);
			sText.MakeUpper();
			pGridCtrl->SetItemText(nRow, nCol, sText);
			break;
		}		
	case Grid_LowerCase:
	case Grid_LowerCase_Combo:
		{
			CString sText;
			sText = pGridCtrl->GetItemText(nRow, nCol);
			sText.MakeLower();
			pGridCtrl->SetItemText(nRow, nCol, sText);
			break;
		}
	}
	acedRetVoid();
	return 0;
}			


int Grid_HitPointTest()
{
	int nX;
	int nY;
	int nArg=0;

	CDclControlObject *pArx = GetLispInput(sGrid_HitPointTest, nX, nY);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CArxGridCtrl *pGrid = (CArxGridCtrl*)pArx->GetWindow();
	
	CPoint pt(nX, nY);
	int nRow, nCol;
	pGrid->CellHitTest(pt, nRow, nCol);
	
	ReturnRowCol(nRow, nCol);
	return 0;
	
}

int Grid_Cell_SetDropList()
{
	int nRow;
	int nCol;
	int nData1 = 0;
	int nData2 = 0;
	CStringArray sStrings;
	CArray<int, int> nInts;

	CDclControlObject *pArx = GetLispInput(sGrid_Cell_SetDropList, nRow, nCol, &nInts, &sStrings);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	if (pGridCtrl->GetCellData(nRow, nCol) != NULL)
	{
		pGridCtrl->GetCellData(nRow, nCol)->sStrings.RemoveAll();
		pGridCtrl->GetCellData(nRow, nCol)->nInts.RemoveAll();

    int i;		
		for (i=0; i<sStrings.GetSize(); i++)
		{
			CString &sText = sStrings[i];
			int nStyle = 0;
			if (pGridCtrl->GetCellData(nRow, nCol) != NULL)
				nStyle = pGridCtrl->GetCellData(nRow, nCol)->nStyle;

			switch(nStyle)
			{
			case Grid_UpperCase:
			case Grid_UpperCase_Combo:
				sText.MakeUpper();
				break;
			case Grid_LowerCase:
			case Grid_LowerCase_Combo:
				sText.MakeLower();					
				break;
			}
			
			pGridCtrl->GetCellData(nRow, nCol)->sStrings.Add(sText);
		}
		for (i=0; i<nInts.GetSize(); i++)
		{
			pGridCtrl->GetCellData(nRow, nCol)->nInts.Add(nInts[i]);
		}
	}
	
	acedRetVoid();
	return 0;
}			
//*****************************************************************************
// 
// Method: Grid_SetItemImage()
// 
// Purpose: [sets the item's Image]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_SetItemImage()
{
	int nArg1;
	int nArg2;
	int nArg3;
	CDclControlObject *pArx = GetLispInput(sGrid_SetItemImage, nArg1, nArg2, nArg3);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		
	pGridCtrl->SetItemImage(nArg1, nArg2, nArg3);

	
	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_SetColWidth()
// 
// Purpose: [sets the column width]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_GetColWidth()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sGrid_GetColWidth, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sGrid_GetColWidth, &nArg);

	int nColIndex;
	
	if (pArx == NULL || !GetIntArgument(nArg, &nColIndex, sGrid_GetColWidth))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	acedRetInt(pGridCtrl->GetColumnWidth(nColIndex));
	return 0;
}


//*****************************************************************************
// 
// Method: Grid_SetColWidth()
// 
// Purpose: [sets the column width]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_SetColWidth()
{
	int nColIndex = 0;
	int nNewWidth = 0;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nColIndex, nNewWidth);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	pGridCtrl->SetColumnWidth(nColIndex, nNewWidth);

	acedRetVoid();
	return 0;
}


//*****************************************************************************
// 
// Method: Grid_CalcColumnWidth()
// 
// Purpose: [returns the min required width of a column]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_CalcColumnWidth()
{
	CString sText;
	
	CDclControlObject *pArx = GetLispInput(sGrid_CalcColumnWidth, sText);
		
	if (pArx != NULL)
	{
		CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		acedRetInt(pGridCtrl->GetStringWidth(sText)*0.5*3);
	}
	else
		acedRetNil();

	return 0;
}
//*****************************************************************************
// 
// Method: Grid_DeleteItems()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_DeleteItems()
{
	int nRow;
	
	CDclControlObject *pArx = GetLispInput(sGrid_DeleteItems, nRow);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	_RowData *pData = pGridCtrl->GetRowData(nRow);

	if (pData != NULL)
		delete pData;

	// get the second argument required
	pGridCtrl->DeleteItem(nRow);

	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_DeleteItems()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_DeleteColumns()
{
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_DeleteColumns, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	// get the second argument required
	pGridCtrl->DeleteColumn(nCol);

	for (int i=0; i<pGridCtrl->GetItemCount(); i++)
	{
		_RowData *pCells = pGridCtrl->GetRowData(i);

		if (nCol < pCells->m_Cells.GetSize() && 
			nCol > pGridCtrl->m_pColCaptions->GetStringArrayPtr()->size()-1)
			pCells->m_Cells.RemoveAt(nCol);
	}
	
	acedRetVoid();
	return 0;
}
//*****************************************************************************
// 
// Method: Grid_FillGrid()
// 
// Purpose: [Adds multiple rows to a list ctrl]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_FillGrid()
{
	struct resbuf *ListData;
	int nImage = -1;
	CString sText;
	int nBeforeAdditionCount;
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sGrid_FillGrid, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sGrid_FillGrid, &nArg);
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();

  int i;	
	for (i = 0; i < nArg; i++)
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

	for (i=0; i<pGridCtrl->GetItemCount(); i++)
	{
		if (pGridCtrl->GetItemData(i) > 0)
		{
			delete (_RowData *)pGridCtrl->GetItemData(i);
		}
	}
	
	// delete all the items
	pGridCtrl->DeleteAllItems();
	
	// get the count before the addition of any rows
	nBeforeAdditionCount = pGridCtrl->GetItemCount();

	pGridCtrl->SetRedraw(FALSE); // turn drawing off regardless of list mode

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
			pGridCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pGridCtrl->Invalidate();
			pGridCtrl->UpdateWindow(); 
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sGrid_FillGrid);	
			acedRetInt(-1);  return 0; 
		}
		
		if (ListData->restype != RTSTR) 
		{
			pGridCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pGridCtrl->Invalidate();
			pGridCtrl->UpdateWindow(); 
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListArgNotStr) + sGrid_FillGrid);	
			acedRetInt(-1);  return 0; 
		}

		// get the first argument required
		sText = ListData->resval.rstring;
		
		// advance to the next list item
		ListData = ListData->rbnext;				
		
		if (ListData == NULL)
		{
			pGridCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pGridCtrl->Invalidate();
			pGridCtrl->UpdateWindow(); 
			// inform the programmer that he did not make the correct call
			acedAlert(CString(ErrorListNotSet) + sGrid_FillGrid);	
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

		int nCount = pGridCtrl->GetItemCount();

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
			int nRet = pGridCtrl->InsertItem(&lvItem);			
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
			int nRet = pGridCtrl->SetItem(&lvItem);	
		}
		

		if (ListData->restype == RTLE) 
		{
			// advance to the next list item
			ListData = ListData->rbnext;	

			if (ListData == NULL || ListData->restype == RTLE) 
			{
				pGridCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
				pGridCtrl->Invalidate();
				pGridCtrl->UpdateWindow(); 
			
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
			
			pGridCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
			pGridCtrl->Invalidate();
			pGridCtrl->UpdateWindow(); 
			
			// return nil
			acedRetVoid();
			return 0;
		}
	}
	
	pGridCtrl->SetRedraw(TRUE); // turn drawing back on and update the window
	pGridCtrl->Invalidate();
	pGridCtrl->UpdateWindow(); 
		
	// return nil
	acedRetVoid();
	return 0;
}

int Grid_GetSelectedCell()
{
	int nArg=0;
	CDclControlObject *pArx = GetControlArxObject(sGrid_GetSelectedCell, &nArg);
	if (pArx == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	ReturnRowCol(pGridCtrl->m_nRowSelected, pGridCtrl->m_nColSelected);
	return 0;
}


int Grid_CancelLabelEdit()
{	
	int nArg = 0;
	CDclControlObject *pArx = GetControlArxObject(sGrid_CancelLabelEdit, &nArg);
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	// Make sure the focus is set to the list view control.
	pGridCtrl->HideEditControls();

	acedRetVoid();
	return 0;
}


int Grid_SortColTextItems()
{
	int nCol=0;
	int nAscending;
	int nArg=0;

	//CWnd *pControl = GetControlPointer(CtlListView, sGrid_SortTextItems, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sGrid_SortTextItems, &nArg);
	if (pArx == NULL || !GetIntArgument(nArg, &nCol, sGrid_SortTextItems))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nAscending, sGrid_SortTextItems))
	{
		//nAscending = nCol;
	}

    try
	{
		pGridCtrl->SortTextItems(nCol, nAscending);
	}
	catch(...)
	{
	}
	
	acedRetVoid();
	return 0;
}

int Grid_SortColNumericItems()
{
	int nCol;
	int nAscending;
	int nArg=0;

	//CWnd *pControl = GetControlPointer(CtlListView, sGrid_SortNumericItems, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sGrid_SortNumericItems, &nArg);
	if (pArx == NULL || !GetIntArgument(nArg, &nCol, sGrid_SortNumericItems))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nAscending, sGrid_SortNumericItems))
	{
		//nAscending = nCol;
	}

	try
	{
		pGridCtrl->SortNumericItems(nCol, nAscending);
	}
	catch(...)
	{
	}
	
	acedRetVoid();
	return 0;
}




//*****************************************************************************
// 
// Method: Grid_SetColumnImage()
// 
// Purpose: [sets the column's Image]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_SetColumnImage()
{
	int nCol = -1;
	int nImage = -1;

	CDclControlObject *pArx = GetLispInput(sGrid_SetColumnImage, nCol, nImage);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	CHeaderCtrl *pHeader = pGridCtrl->GetHeaderCtrl();

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

int Grid_GetColumnImage()
{
	int nCol;

	CDclControlObject *pArx = GetLispInput(sGrid_GetColumnImage, nCol);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	CHeaderCtrl *pHeader = pGridCtrl->GetHeaderCtrl();

	HDITEM item;

	TCHAR  lpBuffer[256];
	item.mask = HDI_TEXT|HDI_IMAGE;
	item.pszText = lpBuffer;
	item.cchTextMax = 256;

	pHeader->GetItem(nCol, &item);

	acedRetInt(item.iImage);
	return 0;
}

int Grid_GetCount()
{
	int nArg;
	//CWnd *pControl = GetControlPointer(CtlListView, sGrid_SetItemImage, &nArg);
	CDclControlObject *pArx = GetControlArxObject(sGrid_GetCount, &nArg);

	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		
	acedRetInt(pGridCtrl->GetItemCount());
	return 0;
}

int Grid_GetColumnCount()
{
	int nArg;
	
	CDclControlObject *pArx = GetControlArxObject(sGrid_GetColumnCount, &nArg);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
		
	CHeaderCtrl *pHeader = pGridCtrl->GetHeaderCtrl();

	if (pHeader != NULL)
		acedRetInt(pHeader->GetItemCount());
	else
		acedRetInt(0);

	return 0;
}

//*****************************************************************************
// 
// Method: Grid_SelectCell()
// 
// Purpose: [Forces an item to be selected]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Grid_SelCurCell()
{
	int nRow;
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nRow, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	pGridCtrl->SetCurSel(nRow, nCol);
	pGridCtrl->EnsureVisible(nRow, false);
	acedRetVoid();
	return 0;
}



int Grid_SelCurRow()
{
	int nRow;
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelCurRow, nRow, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	pGridCtrl->SetCurSel(nRow, -1);
	pGridCtrl->EnsureVisible(nRow, false);
	acedRetVoid();
	return 0;
}

int Grid_Cell_TextBox()
{
	int nRow;
	int nCol;
	int nStyle;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nRow, nCol, nStyle);

	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();

	int nChar = 0;

	// ensure the programmer does not try to show a combo box style that
	// does not match the col pre-assigned setting.
	if (pGridCtrl->m_pColStyles->GetIntArrayPtr()->size()-1 > nCol)
	{
		if (pGridCtrl->m_pColStyles->GetIntArrayPtr()->at(nCol) != nStyle &&
			pGridCtrl->m_pColStyles->GetIntArrayPtr()->at(nCol) != 0)
			nStyle = pGridCtrl->m_pColStyles->GetIntArrayPtr()->at(nCol);
	}
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	if (nCellStyle != nStyle && nCellStyle > 0)
		nStyle = nCellStyle;
	
	pGridCtrl->SetCurSel(nRow, nCol);
	pGridCtrl->DoEditCellNow(nStyle);

	acedRetVoid();
	return 0;
}



int Grid_Cell_ComboBox()
{
	int nRow;
	int nCol;
	int nStyle;
	CStringArray sStrings;
	int nArg=0;

	CWnd *pControl = GetControlPointer(CtlListView, sGrid_Cell_ComboBox, &nArg);
	
	if (pControl == NULL || !GetIntArgument(nArg, &nRow, sGrid_Cell_ComboBox))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	if (!GetIntArgument(nArg, &nCol, sGrid_Cell_ComboBox))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	if (!GetIntArgument(nArg, &nStyle, sGrid_Cell_ComboBox))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	
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
				break;
			}
			
			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sGrid_Cell_ComboBox);	
				acedRetInt(-1);  return 0; 
			}

			// get the first argument required
			sNewString = ListData->resval.rstring;

			sStrings.Add(sNewString);
		}
	}	
	
	CArxGridCtrl *pList = (CArxGridCtrl*)pControl;
	
	if (sStrings.GetSize() > 1 && nRow > -1 && nCol > -1)
	{
		pList->m_nRowSelected = nRow;
		pList->m_nColSelected = nCol;

		// ensure the programmer does not try to show a combo box style that
		// does not match the col pre-assigned setting.
		if (pList->m_pColStyles->GetIntArrayPtr()->size()-1 > nCol)
		{
			if (pList->m_pColStyles->GetIntArrayPtr()->at(nCol) != nStyle &&
				pList->m_pColStyles->GetIntArrayPtr()->at(nCol) != 0)
				nStyle = pList->m_pColStyles->GetIntArrayPtr()->at(nCol);
		}
		
		
	}
	int nCellStyle = pList->GetCellStyle(nRow, nCol);

	if (nCellStyle != nStyle && nCellStyle > 0)
		nStyle = nCellStyle;
	
	pList->SetCurSel(nRow, nCol);
	//pGridCtrl->DoEditCellNow(nStyle);
	
	pList->ShowComboBox(nRow, nCol, nStyle-16, sStrings);

	acedRetVoid();
	return 0;
}



int Grid_Cell_ImageComboBox()
{
	int nRow;
	int nCol;
	CStringArray sStrings;
	CArray<int, int> nItems;
	int nArg = 0;

	CWnd *pControl = GetControlPointer(CtlListView, sGrid_Cell_ImageComboBox, &nArg);
	
	if (pControl == NULL || !GetIntArgument(nArg, &nRow, sGrid_Cell_ImageComboBox))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	if (!GetIntArgument(nArg, &nCol, sGrid_Cell_ImageComboBox))	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	nArg++;
	
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
			int nItem = 0;
			// advance to the first list item
			ListData = ListData->rbnext;

			// check to see if we are finished.
			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				break;
			}
			
			if (ListData->restype == RTSHORT) 
			{
				// get the first argument required
				nItem = ListData->resval.rint;
			}
			else if (ListData->restype == RTLONG) 
			{
				// get the first argument required
				nItem = ListData->resval.rlong;
			}
			else
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sGrid_Cell_ImageComboBox);	
				acedRetInt(-1);  return 0; 
			}

			

			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sGrid_Cell_ImageComboBox);	
				acedRetInt(-1);  return 0; 
			}

			// get the first argument required
			sNewString = ListData->resval.rstring;
		
			nItems.Add(nItem);
			sStrings.Add(sNewString);
		}
	}	
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pControl;
	
	// ensure the programmer does not try to show a combo box style that
	// does not match the col pre-assigned setting.
	if (pGridCtrl->m_pColStyles->GetIntArrayPtr()->size()-1 > nCol)
	{
		if (pGridCtrl->m_pColStyles->GetIntArrayPtr()->at(nCol) != Grid_ImageDropList && 
			pGridCtrl->m_pColStyles->GetIntArrayPtr()->at(nCol) != 0)
		{
			acedRetVoid();
			return 0;
		}
	}

	if (sStrings.GetSize() > 1 && nRow > -1 && nCol > -1)
	{
		int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

		if (nCellStyle != Grid_ImageDropList &&
			nCellStyle != 0)
		{
			acedRetVoid();
			return 0;
		}
		
		pGridCtrl->SetCurSel(nRow, nCol);
		pGridCtrl->ShowImageComboBox(nRow, nCol, sStrings, nItems);
	}

	acedRetVoid();
	return 0;
}



int Grid_Cell_EllipsesButton()
{
	int nRow;
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nRow, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	if (nCellStyle != Grid_EllipsesButtons && nCellStyle > 0)
	{
		acedRetVoid();
		return 0;
	}
	
	pGridCtrl->SetCurSel(nRow, nCol);
	pGridCtrl->DoEditCellNow(Grid_EllipsesButtons);
	//pGridCtrl->ShowEllipsesButton(nRow, nCol, false);

	acedRetVoid();
	return 0;
}


int Grid_Cell_PickButton()
{
	int nRow;
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nRow, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	if (nCellStyle != Grid_PickButtons && nCellStyle > 0)
	{
		acedRetVoid();
		return 0;
	}
	
	pGridCtrl->SetCurSel(nRow, nCol);
	pGridCtrl->DoEditCellNow(Grid_PickButtons);
	//pGridCtrl->ShowEllipsesButton(nRow, nCol, true);

	acedRetVoid();
	return 0;
}

int Grid_Cell_DoCellDlg()
{
	int nRow;
	int nCol;
	int nStyle;
	
	CDclControlObject *pArx = GetLispInput(sGrid_Cell_DoCellDlg, nRow, nCol, nStyle);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	if (nCellStyle != nStyle && nCellStyle > 0)
		nStyle = nCellStyle;
	
	pGridCtrl->SetCurSel(nRow, nCol);
	pGridCtrl->DoEditCellNow(nStyle);

	acedRetVoid();
	return 0;
}


int Grid_GetCheck()
{
	int nRow;
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_GetCheck, nRow, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	if (nCellStyle == Grid_CheckBoxes ||
		nCellStyle == Grid_OptionButtons)
	{
		if (pGridCtrl->GetItemImage(nRow, nCol) > 0)
		{
			acedRetT();
			return 0;
		}			
	}

	acedRetNil();
	return 0;
}

int Grid_SetCheck()
{
	int nRow;
	int nCol;
	int nChecked;
	
	CDclControlObject *pArx = GetLispInput(sGrid_SetCheck, nRow, nCol, nChecked);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	if (nCellStyle == Grid_CheckBoxes ||
		nCellStyle == Grid_OptionButtons)
	{
		pGridCtrl->SetItemImage(nRow, nCol, nChecked);
	}

	acedRetVoid();
	return 0;
}

int Grid_Cell_ToggleImages()
{
	int nRow;
	int nCol;
	
	CDclControlObject *pArx = GetLispInput(sGrid_Cell_ToggleImages, nRow, nCol);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();
	
	int nCellStyle = pGridCtrl->GetCellStyle(nRow, nCol);

	// test to see how to edit the cell each different kind of edit style
	// according to the column specification.
	if (nCellStyle != Grid_SwitchableIcons && nCellStyle > 0)
	{
		acedRetVoid();
		return 0;
	}
	
	int nImage = pGridCtrl->GetItemImage(nRow, nCol);

	if (nImage == pGridCtrl->m_pColDefault->GetIntArrayPtr()->at(nCol) || nImage == -1)
	{
		pGridCtrl->SetItemImage(nRow, nCol, pGridCtrl->m_pColAlternate->GetIntArrayPtr()->at(nCol));
	}
	else
	{
		pGridCtrl->SetItemImage(nRow, nCol, pGridCtrl->m_pColDefault->GetIntArrayPtr()->at(nCol));
	}

	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_AddString()
// 
// Purpose: [does the actual insertion of a new row to a list ctrl]
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_AddString()
{
	int nInsertIndex = -1;
	CString sStringArg;
	CString sDivider = _T("\t");
	CDclControlObject *pArx = GetLispInput(sGrid_AddString, sStringArg, sDivider);
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();	

	int nStart = -1;

	int nNext = sStringArg.Find(sDivider, 0);
	if (nNext == -1)
		nNext = sStringArg.GetLength();
	int nCol = 0;
	while (nNext > -1)
	{
		CString sCell = sStringArg.Mid(nStart + 1, nNext);
		sCell.Trim(_T(" "));

		if( nCol == 0 )
		{
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
			lvItem.iItem = pGridCtrl->GetItemCount() + 1;
			lvItem.iSubItem = 0;
			lvItem.pszText = sCell.LockBuffer();			
			lvItem.iImage = -1;
			lvItem.iIndent = 0;			
			nInsertIndex = pGridCtrl->InsertItem(&lvItem);
		}
		else
		{
			pGridCtrl->SetItemText(nInsertIndex, nCol, sCell);
		}
		++nCol;

		sStringArg = sStringArg.Mid(nNext + 1);
		nNext = sStringArg.Find(sDivider, 0);
		if (nNext == -1 && sStringArg.GetLength() > 0)
			nNext = sStringArg.GetLength();
	}

	acedRetInt(nInsertIndex);
	return 0;
}

//*****************************************************************************
// 
// Method: Grid_InsertString()
// 
// Purpose: [does the actual insertion of a new row to a list ctrl]
// 
// Returns:	int
// 
//*****************************************************************************

int Grid_InsertString()
{
	int nInsertIndex = -1;
	CString sStringArg;
	CString sDivider = _T("\t");
	CDclControlObject *pArx = GetLispInput(sGrid_InsertString, nInsertIndex, sStringArg, sDivider);
	if (pArx == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}
	
	CArxGridCtrl *pGridCtrl = (CArxGridCtrl*)pArx->GetWindow();	

	int nStart = -1;

	int nNext = sStringArg.Find(sDivider, 0);
	if (nNext == -1)
		nNext = sStringArg.GetLength();
	int nCol = 0;
	while (nNext > -1)
	{
		CString sCell = sStringArg.Mid(nStart + 1, nNext);
		sCell.Trim(_T(" "));

		if( nCol == 0 )
		{
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
			lvItem.iItem = pGridCtrl->GetItemCount() + 1;
			lvItem.iSubItem = 0;
			lvItem.pszText = sCell.LockBuffer();			
			lvItem.iImage = -1;
			lvItem.iIndent = 0;			
			nInsertIndex = pGridCtrl->InsertItem(&lvItem);
		}
		else
		{
			pGridCtrl->SetItemText(nInsertIndex, nCol, sCell);
		}
		++nCol;

		sStringArg = sStringArg.Mid(nNext + 1);
		nNext = sStringArg.Find(sDivider, 0);
		if (nNext == -1 && sStringArg.GetLength() > 0)
			nNext = sStringArg.GetLength();
	}

	acedRetInt(nInsertIndex);
	return 0;
}