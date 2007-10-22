// Methods_Tree.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Tree.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "VdclTree.h"
#include "ControlTypes.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// Methods_Tree

void ReturnTreeItem(CString sText, int nImage, int nSelectedImage, CString sKey)
{
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSTR, sText,
		RTSHORT, nImage,
		RTSHORT, nSelectedImage,
		RTSTR, sKey,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
	
}

void ReturnTreeItem(CString sText, int nImage, int nSelectedImage, DWORD_PTR uLong)
{
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;
	ads_name ptr;
	ptr[0] = uLong;
	ptr[1] = 0;

	list = acutBuildList(
		RTSTR, sText,
		RTSHORT, nImage,
		RTSHORT, nSelectedImage,
		RTENAME, ptr,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
	
}

bool Tree_AddParentList(CWnd *pControl, int nArg)
{
	CString sParentText;
	CString sKey;
	ULONG uKey = 0;
	
	int nImage;
	int nSelectedImage;
	int nExpandedImage;

	struct resbuf *ListData;
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}

	if (ListData->restype != RTLB) 
		return false;

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE) 
			{
				return true;
			}
			if (ListData->restype == NULL) 
			{
				return false;
			}
			
			if (ListData->restype == RTLB) 
			{		
			
				
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddParent);	
					return false;
				}
			
				if (ListData->restype == RTSTR) 
				{
					// get the next argument required
					sParentText = ListData->resval.rstring;
				}
				else if (ListData->restype != RTSTR) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sTree_AddChild);	
					return false;
				}				
								
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddParent);	
					return false;
				}
			
				// get the optional string Key
				if (ListData->restype == RTSTR) 
				{
					// get the next argument required
					sKey = ListData->resval.rstring;
					ListData = ListData->rbnext;				
				}
				
				bool bListEndFound = false;

				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddParent);	
					return false;
				}
				if (ListData->restype == RTLE) 
				{
					bListEndFound = true;
					nExpandedImage = -1;
					nImage = -1;
					nSelectedImage = -1;
				}
				
				if (ListData->restype != RTLE) 
				{
					if (ListData->restype != RTSHORT) 
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sTree_AddParent);	
						return false;
					}
					// get the first argument required
					nImage = ListData->resval.rint;

					// advance to the next list item
					ListData = ListData->rbnext;
					if (ListData == NULL)
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddParent);	
						return false;
					}
					if (ListData->restype != RTSHORT) 
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sTree_AddParent);	
						return false;
					}
					
					// get the second argument required
					nSelectedImage = ListData->resval.rint;
					
					
					// advance to the next list item
					ListData = ListData->rbnext;
					if (ListData->restype != RTLE) 
					{
						if (ListData->restype != RTSHORT) 
						{
							// inform the programmer that he did not make the correct call
							theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sTree_AddChild);	
							return false;
						}
						
						// get the second argument required
						nExpandedImage = ListData->resval.rint;
					}			
					else
					{
						bListEndFound = true;
						nExpandedImage = -1;
					}
				}
				
				((VdclTree*)pControl)->m_bAutoChangeSelection = false;
				((VdclTree*)pControl)->AddParent(sParentText, sKey, nImage, nSelectedImage, nExpandedImage);
				((VdclTree*)pControl)->m_bAutoChangeSelection = false;
				
				
				if (bListEndFound == false)
				{
					// advance to the next list item
					ListData = ListData->rbnext;

					if (ListData->restype != RTLE) 
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sTree_AddChild);	
						return false;
					}			
				}			
			}
		}
	}
	else
	{	
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sTree_AddParent);			
        return false;
	}	
	return true;
}

int Tree_AddParent()
{
	CString sParentText ;
	CString sKey ;
	int nImage = -1;
	int nSelectedImage = -1;
	int nExpandedImage = -1;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_AddParent, &nArg);

	if (!GetStringArgument(nArg, &sParentText, "") || pControl == NULL)
	{
		if (Tree_AddParentList(pControl, nArg))
			acedRetNil();
		else
		{
			CString sArg;
			sArg.Format(_T("%d"), nArg);
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(
				CString(ErrorTheErrorStart) + sArg + ErrorTheErrorMiddle + sTree_AddParent +
				ErrorEndErrorNotaString	);	
			acedRetNil();
		}
		return 0;
	}
	nArg++;
	if (!FindOptionalStringArgument(nArg, &sKey, sTree_AddParent))
	{
		sKey ;
	}
	else
	{		
		nArg++;
	}
	if (FindOptionalIntArgument(nArg, &nImage, sTree_AddParent))
	{
		nArg++;
		if (FindOptionalIntArgument(nArg, &nSelectedImage, sTree_AddParent))
		{
			nArg++;
			if (!FindOptionalIntArgument(nArg, &nExpandedImage, sTree_AddParent))
				nExpandedImage = -1;
		}
		else 
			nSelectedImage = nImage;
	}
	HTREEITEM hItem = NULL;
	((VdclTree*)pControl)->m_bAutoChangeSelection = false;	
	hItem = ((VdclTree*)pControl)->AddParent(sParentText, sKey, nImage, nSelectedImage, nExpandedImage);
	((VdclTree*)pControl)->m_bAutoChangeSelection = false; 
	
	
	acedRetHandle((DWORD_PTR)hItem);
	return 0;

}

bool Tree_AddChildList(CWnd *pControl, int nArg)
{
	CString sParentKey;
	DWORD_PTR uParentKey = 0;
	CString sChildText;
	CString sKey;
	DWORD_PTR uKey = 0;
	int nImage;
	int nSelectedImage;
	int nExpandedImage;
	
	struct resbuf *ListData;
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}

	if (ListData->restype != RTLB) 
		return false;

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE) 
			{
				return true;
			}
			if (ListData->restype == NULL) 
			{
				return false;
			}
			if (ListData->restype == RTLB) 
			{		
			
				
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddChild);	
					return false;
				}
			
				if (ListData->restype == RTLONG) 
				{
					// get the next argument required
					uParentKey = ListData->resval.rlong;
				}
				else if (ListData->restype == RTENAME) 
				{
					// get the next argument required
					uParentKey = ListData->resval.rlname[0];
				}
				else if (ListData->restype == RTSHORT) 
				{
					// get the next argument required
					uParentKey = ListData->resval.rint;
				}
				else if (ListData->restype == RTSTR) 
				{
					// get the next argument required
					sParentKey = ListData->resval.rstring;
				}
				else if (ListData->restype != RTSTR) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sTree_AddChild);	
					return false;
				}				
				
				
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddChild);	
					return false;
				}
			
				if (ListData->restype != RTSTR) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sTree_AddChild);	
					return false;
				}				
				
				// get the next argument required
				sChildText = ListData->resval.rstring;
								
			
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddChild);	
					return false;
				}
			
				if (ListData->restype == RTSTR) 
				{
					// get the next argument required
					sKey = ListData->resval.rstring;
					ListData = ListData->rbnext;				
				}
				
				// advance to the first list item
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddChild);	
					return false;
				}

				bool bListEndFound = false;

				if (ListData->restype == RTLE) 
				{
					bListEndFound = true;
					nExpandedImage = -1;
					nImage = -1;
					nSelectedImage = -1;
				}
				
				if (ListData->restype != RTLE) 
				{
				
					if (ListData->restype != RTSHORT) 
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sTree_AddChild);	
						return false;
					}
					// get the first argument required
					nImage = ListData->resval.rint;

					// advance to the next list item
					ListData = ListData->rbnext;
					if (ListData == NULL)
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sTree_AddChild);	
						return false;
					}
					
					if (ListData->restype != RTSHORT) 
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sTree_AddChild);	
						return false;
					}
					
					// get the second argument required
					nSelectedImage = ListData->resval.rint;
					
					// advance to the next list item
					ListData = ListData->rbnext;
					if (ListData->restype != RTLE) 
					{
						if (ListData->restype != RTSHORT) 
						{
							// inform the programmer that he did not make the correct call
							theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sTree_AddChild);	
							return false;
						}
						
						// get the second argument required
						nExpandedImage = ListData->resval.rint;
					}			
					else
					{
						bListEndFound = true;
						nExpandedImage = -1;
					}
				}
				
				HTREEITEM hItem = NULL;
				((VdclTree*)pControl)->m_bAutoChangeSelection = false;
				
				if (nImage == -1)
				{
					nSelectedImage = -1;
					nExpandedImage = -1;
				}

				if (uParentKey != 0)
					hItem = ((VdclTree*)pControl)->AddChild(sChildText, uParentKey, nImage, nSelectedImage, nExpandedImage);
				else
					hItem = ((VdclTree*)pControl)->AddChild(sChildText, sParentKey, sKey, nImage, nSelectedImage, nExpandedImage);
				
				((VdclTree*)pControl)->m_bAutoChangeSelection = false;
				if (hItem == NULL)
				{
					return false;
				}
				
				if (bListEndFound == false)
				{
					// advance to the next list item
					ListData = ListData->rbnext;

					if (ListData->restype != RTLE) 
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sTree_AddChild);	
						return false;
					}			
				}
			}
		}
	}
	else
	{	
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sTree_AddChild);			
        return false;
	}

	return true;
}


int Tree_AddChild()
{
	CString sParentKey;
	CString sChildText;
	CString sKey;
	int nImage;
	int nSelectedImage = -1;
	int nExpandedImage = -1;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_AddChild, &nArg);

	DWORD_PTR uParentKey;
	if (!GetStringOrHandleArgument(nArg, sParentKey, uParentKey, sTree_AddChild) || pControl == NULL)
	{
		if (Tree_AddChildList(pControl, nArg))
			acedRetNil();
		else
		{
			acedRetNil();
		}
		return 0;
	}
	nArg++;
	if (!GetStringArgument(nArg, &sChildText, sTree_AddChild) || pControl == NULL)
	{
		
		acedRetNil();
		return 0;
	}

	nArg++;	
	if (FindOptionalStringArgument(nArg, &sKey, sTree_AddChild))
		nArg++;
	if (FindOptionalIntArgument(nArg, &nImage, sTree_AddChild))
	{
		nArg++;
		if (FindOptionalIntArgument(nArg, &nSelectedImage, sTree_AddChild))
		{
			nArg++;
			FindOptionalIntArgument(nArg, &nExpandedImage, sTree_AddChild);
		}
		else 
			nSelectedImage = nImage;
	}
	
	HTREEITEM hItem = NULL;
	
	if (nImage == -1)
	{
		nSelectedImage = -1;
		nExpandedImage = -1;
	}
	// if a string key has been specified
	if (uParentKey != 0)
	{
		hItem = ((VdclTree*)pControl)->AddChild(sChildText, uParentKey, nImage, nSelectedImage, nExpandedImage);		
	}
	else
		hItem = ((VdclTree*)pControl)->AddChild(sChildText, sParentKey, sKey, nImage, nSelectedImage, nExpandedImage);
	
	//if (hItem)
	//	((VdclTree*)pControl)->m_ChildTree.Invalidate();

	if (hItem == NULL)
	{
		
		acedRetNil();
		return 0;
	}
	acedRetHandle((DWORD_PTR)hItem);
	
	return 0;

}

int Tree_InsertAfter()
{
	CString sRefKey;
	CString sChildText;
	CString sKey;
	int nImage = -1;
	int nSelectedImage = -1;
	int nExpandedImage = -1;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_InsertAfter, &nArg);

	DWORD_PTR uRefKey;
	if (!GetStringOrHandleArgument(nArg, sRefKey, uRefKey, sTree_InsertAfter) || pControl == NULL)
	{
		
		acedRetNil();
		return 0;
	}
	nArg++;
	
	if (!GetStringArgument(nArg, &sChildText, sTree_InsertAfter))
	{
		
		acedRetNil();
		return 0;
	}

	nArg++;
	if (!FindOptionalStringArgument(nArg, &sKey, sTree_InsertAfter))
	{
		sKey ;
	}
	else
	{		
		nArg++;
	}
	if (FindOptionalIntArgument(nArg, &nImage, sTree_InsertAfter))
	{
		nArg++;
		if (FindOptionalIntArgument(nArg, &nSelectedImage, sTree_InsertAfter))
		{
			nArg++;
			if (!FindOptionalIntArgument(nArg, &nExpandedImage, sTree_InsertAfter))
				nExpandedImage = -1;
		}
		else 
			nSelectedImage = nImage;
	}
	
	HTREEITEM hItem = NULL;
	
	if (uRefKey != 0)
		hItem = ((VdclTree*)pControl)->AddAfter(sChildText, uRefKey, nImage, nSelectedImage, nExpandedImage);
	else
		hItem = ((VdclTree*)pControl)->AddAfter(sChildText, sRefKey, sKey, nImage, nSelectedImage, nExpandedImage);
	
	if (hItem == NULL)
	{
		
		acedRetNil();
		return 0;
	}
	
	acedRetHandle((DWORD_PTR)hItem);
	
	return 0;

}

int Tree_Clear()
{
	CWnd *pControl = GetControlPointer(CtlTree, sTree_Clear);

	if (pControl == NULL)		
	{
		
		acedRetNil();
		return 0;
	}
	
	((VdclTree*)pControl)->Clear();
	
	
	acedRetNil();
	return 0;
}

//*****************************************************************************
// 
// Method: Tree_CountItems()
// 
// Purpose: [Get the item count]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Tree_CountItems()
{
	CWnd *pControl = GetControlPointer(CtlTree, sTree_CountItems);

	if (pControl == NULL)		
	{
		
		acedRetNil();
		return 0;
	}
	
	int nCount = ((VdclTree*)pControl)->m_ChildTree.GetCount();

	
	acedRetInt(nCount);
	return 0;
}

//*****************************************************************************
// 
// Method: Tree_SelectItem()
// 
// Purpose: [Select an item]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Tree_SelectItem()
{
	CString sSelectKey ;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SelectItem, &nArg);
	VdclTree* pTree = (VdclTree*)pControl;
	
	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SelectItem) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}

	HTREEITEM hOldItem = ((VdclTree*)pControl)->m_ChildTree.GetSelectedItem();

	HTREEITEM hItem = NULL;

	if (sSelectKey.GetLength() > 0)
	{
		pTree->SelectItem(sSelectKey);
		hItem = pTree->Get_hItem(sSelectKey);
	}
	if (uLong != 0)
	{
		hItem = (HTREEITEM)uLong;
		pTree->m_ChildTree.SelectItem(hItem);
	}
	/*
	if (hOldItem != NULL)
	{
		pTree->m_ChildTree.SetItemState(hOldItem, NULL, NULL);
		pTree->m_ChildTree.SetItemState(hOldItem, NULL, TVIS_DROPHILITED);
	}
	pTree->m_ChildTree.SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED );
	*/
	pTree->m_ChildTree.SetFocus();
		
	acedRetNil();
	return 0;
}

//*****************************************************************************
// 
// Method: Tree_GetParent()
// 
// Purpose: [Get the parent info]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int Tree_GetParent()
{
	CString sParentKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetParent, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sParentKey, uLong, sTree_GetParent) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}
	
	if (sParentKey.GetLength() > 0)
	{		
		if (!((VdclTree*)pControl)->GetParentInfo(sParentKey))
		{
			// return and error code
			acedRetNil();
			return 0;
		}
	}
	if (uLong != 0)
	{
		if (!((VdclTree*)pControl)->GetParentInfo((HTREEITEM)uLong))
		{
			// return and error code
			acedRetNil();
			return 0;
		}
	}
	
	// we don't do a acadRet... here becase the GetParentInfo takes care of that for us
	return 0;
	
}

int Tree_ItemHasChildren()
{
	CString sSelectKey ;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_ItemHasChildren, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_ItemHasChildren) || pControl == NULL)
	{
		
		acedRetNil();
		return 0;
	}

	BOOL bChildren = false;
	if (sSelectKey.GetLength() > 0)
	{		
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		bChildren = ((VdclTree*)pControl)->m_ChildTree.ItemHasChildren(hItem);
	}
	if (uLong != 0)
	{
		bChildren = ((VdclTree*)pControl)->m_ChildTree.ItemHasChildren((HTREEITEM)uLong);
	}
	
	if (bChildren)
		acedRetT();
	else
		acedRetNil();	

	return 0;
}
int Tree_GetNextSiblingItem()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetNextSiblingItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetNextSiblingItem) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{				
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		HTREEITEM hNextItem = ((VdclTree*)pControl)->m_ChildTree.GetNextSiblingItem(hItem);
		
		CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hNextItem);
		if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())			
			acedRetStr(_T(""));
		else
			acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hNextItem));
	}
	if (uLong != 0)
	{
		HTREEITEM hItem = (HTREEITEM)uLong;
		HTREEITEM hNextItem = ((VdclTree*)pControl)->m_ChildTree.GetNextSiblingItem(hItem);
		
		acedRetHandle((DWORD_PTR)hNextItem);
	}

	return 0;
}

int Tree_GetPrevSiblingItem()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetPrevSiblingItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetPrevSiblingItem) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{	
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		HTREEITEM hPrevItem = ((VdclTree*)pControl)->m_ChildTree.GetPrevSiblingItem(hItem);

		CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hPrevItem);
		if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())			
			acedRetStr(_T(""));
		else
			acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hPrevItem));
	}
	if (uLong != 0)
	{
		HTREEITEM hItem = (HTREEITEM)uLong;
		HTREEITEM hPrevItem = ((VdclTree*)pControl)->m_ChildTree.GetPrevSiblingItem(hItem);

		acedRetHandle((DWORD_PTR)hPrevItem);
	}
	return 0;
}
int Tree_GetFirstVisibleItem()
{
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetFirstVisibleItem);

	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	HTREEITEM hItem = ((VdclTree*)pControl)->m_ChildTree.GetFirstVisibleItem();

	CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hItem);
	if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())
		acedRetHandle((DWORD_PTR)hItem);
	else	
		acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hItem));

	return 0;
}

int Tree_GetNextVisibleItem()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetNextVisibleItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetNextVisibleItem) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		HTREEITEM hNextItem = ((VdclTree*)pControl)->m_ChildTree.GetNextVisibleItem(hItem);

		CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hNextItem);
		if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())			
			acedRetStr(_T(""));
		else	
			acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hNextItem));
	}
	if (uLong != 0)
	{
		HTREEITEM hItem = (HTREEITEM)uLong;
		HTREEITEM hNextItem = ((VdclTree*)pControl)->m_ChildTree.GetNextVisibleItem(hItem);

		acedRetHandle((DWORD_PTR)hNextItem);
	}
	return 0;
}

int Tree_GetPrevVisibleItem()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetPrevVisibleItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetPrevVisibleItem) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{		
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		HTREEITEM hPrevItem = ((VdclTree*)pControl)->m_ChildTree.GetPrevVisibleItem(hItem);

		CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hPrevItem);
		if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())
			acedRetStr(_T(""));
		else	
			acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hPrevItem));
	}
	if (uLong != 0)
	{
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		HTREEITEM hPrevItem = ((VdclTree*)pControl)->m_ChildTree.GetPrevVisibleItem(hItem);
			
		acedRetHandle((DWORD_PTR)hPrevItem);
	}
	return 0;
}
int Tree_GetSelectedItem()
{
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetSelectedItem);

	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	HTREEITEM hItem = ((VdclTree*)pControl)->m_ChildTree.GetSelectedItem();

	if (hItem == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hItem);
	if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())
		acedRetHandle((DWORD_PTR)hItem);
	else	
		acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hItem));

	return 0;
}

int Tree_GetRootItem()
{
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetRootItem);

	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	HTREEITEM hItem = ((VdclTree*)pControl)->m_ChildTree.GetRootItem();

	CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hItem);
	if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())		
		acedRetHandle((DWORD_PTR)hItem);
	else	
		acedRetStr(((VdclTree*)pControl)->Get_hItemKey(hItem));

	return 0;
}

int Tree_GetItem()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetItem) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
	{	
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	}
	if (uLong != 0)
		hItem = (HTREEITEM)uLong;

	if (hItem == NULL)
	{		
		acedRetNil();
		return 0;
	}
	CString sText = ((VdclTree*)pControl)->m_ChildTree.GetItemText(hItem);
	int nImage;
	int nSelectedImage;
	((VdclTree*)pControl)->m_ChildTree.GetItemImage(hItem, nImage, nSelectedImage);
	
	if (sSelectKey.GetLength() > 0)
		ReturnTreeItem(sText, nImage, nSelectedImage, sSelectKey);
	else if (uLong != 0)
		ReturnTreeItem(sText, nImage, nSelectedImage, uLong);
	
	return 0;
}

int Tree_GetItemImages()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetItemImages, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetItemImages) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
	{
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	}
	if (uLong != 0)
		hItem = (HTREEITEM)uLong;
	if (hItem == NULL)
	{		
		acedRetNil();
		return 0;
	}
	
	int nImage;
	int nSelectedImage;
	((VdclTree*)pControl)->m_ChildTree.GetItemImage(hItem, nImage, nSelectedImage);
	
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, nImage,
		RTSHORT, nSelectedImage,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 

	return 0;
}

int Tree_SetItemImages()
{
	CString sSelectKey;
	int nImage;
	int nSelectedImage;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SetItemImages, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SetItemImages) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}
	nArg++;
	if (!GetIntArgument(nArg, &nImage, sTree_SetItemImages))
	{
		acedRetNil();
		return 0;
	}
	nArg++;
	if (!FindOptionalIntArgument(nArg, &nSelectedImage, sTree_SetItemImages))
	{		
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
	{
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);	
		int nIndex = ((VdclTree*)pControl)->FindItemIndex(sSelectKey);
		OdclTreeItem *pItem = ((VdclTree*)pControl)->GetTreeItem(nIndex);
		if( pItem )
		{
			pItem->m_ImageIndex = nImage;
			pItem->m_SelectedImageIndex = nSelectedImage;
		}
	}
	if (uLong != 0)
		hItem = (HTREEITEM)uLong;

	if (hItem == NULL)
	{
		acedRetNil();
		return 0;
	}
	((VdclTree*)pControl)->m_ChildTree.SetItemImage(hItem, nImage, nSelectedImage);
	((VdclTree*)pControl)->m_ChildTree.Invalidate();
	acedRetNil();
	return 0;
}

int Tree_SetExpanedImage()
{
	CString sSelectKey;
	int nExpandedImage;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SetExpanedImage, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SetExpanedImage) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	nArg++;
	if (!GetIntArgument(nArg, &nExpandedImage, sTree_SetExpanedImage))
	{
		acedRetNil();
		return 0;
	}
	
	if (sSelectKey.GetLength() > 0)
	{		
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);	
		int nIndex = ((VdclTree*)pControl)->FindItemIndex(sSelectKey);
		OdclTreeItem *pItem = ((VdclTree*)pControl)->GetTreeItem(nIndex);
		pItem->m_ExpandedImageIndex = nExpandedImage;
		((VdclTree*)pControl)->m_ChildTree.Invalidate();
	}
	
	acedRetNil();
	return 0;
}

int Tree_GetExpanedImage()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SetExpanedImage, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetExpanedImage) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	if (sSelectKey.GetLength() > 0)
	{		
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);	
		int nIndex = ((VdclTree*)pControl)->FindItemIndex(sSelectKey);
		OdclTreeItem *pItem = ((VdclTree*)pControl)->GetTreeItem(nIndex);
		
		acedRetInt(pItem->m_ExpandedImageIndex);
	}
	else
		acedRetNil();

	return 0;
}


int Tree_GetItemData()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetItemData, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetItemData) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
	{	
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	}
	if (uLong != 0)
	{	
		hItem = (HTREEITEM)uLong;
	}

	if (hItem == NULL)
	{		
		acedRetNil();
		return 0;
	}
	
	DWORD_PTR lData = ((VdclTree*)pControl)->m_ChildTree.GetItemData(hItem);
	acedRetHandle(lData);	
	return 0;
}

int Tree_SetItemData()
{
	CString sSelectKey;
	int nData;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SetItemData, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SetItemData) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	nArg++;
	if (!GetIntArgument(nArg, &nData, sTree_SetItemData))
	{
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{	
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		((VdclTree*)pControl)->m_ChildTree.SetItemData(hItem, nData);
	}
	if (uLong != NULL)
	{
		((VdclTree*)pControl)->m_ChildTree.SetItemData((HTREEITEM)uLong, nData);
	}
	acedRetNil();
	return 0;
}

int Tree_GetItemText()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetItemText, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetItemText) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	if (uLong != NULL)
		hItem = (HTREEITEM)uLong;

	if (hItem == NULL)
	{		
		acedRetNil();
		return 0;
	}	
	
	CString sRetVal = ((VdclTree*)pControl)->m_ChildTree.GetItemText(hItem);
	acedRetStr( ((VdclTree*)pControl)->m_ChildTree.GetItemText(hItem));
	
	return 0;
}

int Tree_SetItemText()
{
	CString sSelectKey;
	CString sNewText;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SetItemText, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SetItemText) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	nArg++;
	if (!GetStringArgument(nArg, &sNewText, sTree_SetItemText) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	
	if (sSelectKey.GetLength() > 0)
	{
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		((VdclTree*)pControl)->m_ChildTree.SetItemText(hItem, sNewText);
	}
	if (uLong != NULL)
		((VdclTree*)pControl)->m_ChildTree.SetItemText((HTREEITEM)uLong, sNewText);

	((VdclTree*)pControl)->m_ChildTree.Invalidate();
	acedRetNil();
	return 0;
}
int Tree_GetVisibleCount()
{
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetVisibleCount);

	if (pControl == NULL)
	{
		
		acedRetNil();
		return 0;
	}

	acedRetInt( ((VdclTree*)pControl)->m_ChildTree.GetVisibleCount() );
	return 0;
}

int Tree_DeleteItem()
{
	CString sSelectKey;
	int nArg;
	
	CWnd *pControl = GetControlPointer(CtlTree, sTree_DeleteItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_DeleteItem) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	if (!sSelectKey.IsEmpty())
	{
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		((VdclTree*)pControl)->m_ChildTree.DeleteItem(hItem);
		
		int nIndex = ((VdclTree*)pControl)->FindItemIndex(sSelectKey);
		
		POSITION pos = ((VdclTree*)pControl)->m_TreeItems.FindIndex(nIndex);
		OdclTreeItem *pItem = ((VdclTree*)pControl)->m_TreeItems.GetAt(pos);
		((VdclTree*)pControl)->m_TreeItems.RemoveAt(pos);
		delete pItem;
	}
	else if (uLong != 0)
		((VdclTree*)pControl)->m_ChildTree.DeleteItem((HTREEITEM)uLong);
	else
	{
		theWorkspace.DisplayAlert(ErrorKeyMustBeSet);
		
		acedRetNil();
		return 0;
	}
	acedRetNil();
	return 0;
}

int Tree_ExpandItem()
{
	CString sSelectKey;
	int nExpandStyle;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_ExpandItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_ExpandItem) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}
	nArg++;
	if (!GetIntArgument(nArg, &nExpandStyle, sTree_ExpandItem))
	{		
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	OdclTreeItem *pItem = NULL;
	if (!sSelectKey.IsEmpty())
	{
		pItem = ((VdclTree*)pControl)->GetTreeItem(
			((VdclTree*)pControl)->FindItemIndex(sSelectKey));		
		hItem = pItem->hItem;
	}
	if (uLong !=0)
		hItem = (HTREEITEM)uLong;
	
	((VdclTree*)pControl)->m_bAutoChangeSelection = false;
	switch (nExpandStyle)
	{
	case -1:
		{
		((VdclTree*)pControl)->m_ChildTree.Expand(hItem, TVE_COLLAPSE);				
		break;
		}
	case 0:
		((VdclTree*)pControl)->m_ChildTree.Expand(hItem, TVE_TOGGLE);
		break;
	case 1:
		((VdclTree*)pControl)->m_ChildTree.Expand(hItem, TVE_EXPAND);
		break;
	}
	((VdclTree*)pControl)->m_bAutoChangeSelection = false;

	
	if (!sSelectKey.IsEmpty())
		// lets make sure the image is updated correctly. If we don't it will show the previous image.
		((VdclTree*)pControl)->m_ChildTree.SetItemImage(hItem, pItem->m_ImageIndex, pItem->m_SelectedImageIndex);
	
	((VdclTree*)pControl)->m_ChildTree.Invalidate();

	acedRetNil();
	return 0;
}

int Tree_SelectSetFirstVisible()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SelectSetFirstVisible, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SelectSetFirstVisible) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		((VdclTree*)pControl)->m_ChildTree.SelectSetFirstVisible(hItem);
	}
	if (uLong != 0)
		((VdclTree*)pControl)->m_ChildTree.SelectSetFirstVisible((HTREEITEM)uLong);

	acedRetNil();
	return 0;
}

int Tree_EditLabel()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_EditLabel, &nArg);

	
	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_EditLabel) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	if (uLong != 0)
		hItem = (HTREEITEM)uLong;
	if (hItem != NULL)
	{
		CEdit* pEdit = ((VdclTree*)pControl)->m_ChildTree.EditLabel(hItem);
		if (pEdit == NULL)
		{
			acedRetNil();
			return 0;
		}
	}
	acedRetNil();
	return 0;

}

int Tree_CancelEditLabel()
{
	CString sSelectKey ;
	
	CWnd *pControl = GetControlPointer(CtlTree, sTree_CancelEditLabel);

	if (pControl == NULL)
	{
		
		acedRetNil();
		return 0;
	}

	
	CEdit* pEdit = ((VdclTree*)pControl)->m_ChildTree.GetEditControl();
	
	if (pEdit == NULL)
	{
		acedRetNil();
		return 0;
	}
	pEdit->ShowWindow(FALSE);

	acedRetNil();
	return 0;

}

int Tree_SortChildren()
{
	CString sSelectKey ;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_SortChildren, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_SortChildren) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	HTREEITEM hItem = NULL;
	
	if (sSelectKey.GetLength() > 0)
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	if (uLong != 0)
		hItem = (HTREEITEM)uLong;
	if (hItem != NULL)
	{
		((VdclTree*)pControl)->m_ChildTree.SortChildren(hItem);
		((VdclTree*)pControl)->m_ChildTree.Invalidate();
	}
	acedRetNil();
	return 0;
}

int Tree_EnsureVisible()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_EnsureVisible, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_EnsureVisible) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	if (uLong != 0)
		hItem = (HTREEITEM)uLong;
	if (hItem != NULL)
		((VdclTree*)pControl)->m_ChildTree.EnsureVisible(hItem);

	acedRetNil();
	return 0;
}

int Tree_GetFirstChildItem()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_GetFirstChildItem, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_GetFirstChildItem) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	if (sSelectKey.GetLength() > 0)
	{
		HTREEITEM hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
		HTREEITEM hChildItem = ((VdclTree*)pControl)->m_ChildTree.GetChildItem(hItem);
		
		CString sRetVal = ((VdclTree*)pControl)->Get_hItemKey(hChildItem);
		if (sRetVal.Left(1) == _T("?") || sRetVal.IsEmpty())			
			acedRetStr(_T(""));
		else	
			acedRetStr( ((VdclTree*)pControl)->Get_hItemKey(hChildItem));
	}
	if (uLong != 0)
	{
		HTREEITEM hItem = (HTREEITEM)uLong;
		HTREEITEM hChildItem = ((VdclTree*)pControl)->m_ChildTree.GetChildItem(hItem);
		acedRetHandle((DWORD_PTR)hChildItem);
	}
	return 0;
}


int Tree_IsItemExpanded()
{
	CString sSelectKey;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTree, sTree_IsItemExpanded, &nArg);

	DWORD_PTR uLong;
	if (!GetStringOrHandleArgument(nArg, sSelectKey, uLong, sTree_IsItemExpanded) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	HTREEITEM hItem = NULL;
	if (sSelectKey.GetLength() > 0)
		hItem = ((VdclTree*)pControl)->Get_hItem(sSelectKey);
	if (uLong != NULL)
		hItem = (HTREEITEM)uLong;

	if (hItem == NULL)
	{		
		acedRetNil();
		return 0;
	}	
	
	//HTREEITEM hItem = ((VdclTree*)pControl)->m_ChildTree.GetSelectedItem();
	BOOL bHasChildren = ((VdclTree*)pControl)->m_ChildTree.ItemHasChildren(hItem);

	if (bHasChildren == FALSE)
	{
		acedRetNil();
		return 0;
	}

	HTREEITEM hChildItem = ((VdclTree*)pControl)->m_ChildTree.GetChildItem(hItem);
	HTREEITEM hNextVisibleItem = ((VdclTree*)pControl)->m_ChildTree.GetNextVisibleItem(hItem);
	
	if (hChildItem == hNextVisibleItem)
		acedRetT();
	else
		acedRetNil();
	
	return 0;
}

