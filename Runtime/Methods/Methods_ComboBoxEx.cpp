// Methods_ComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ComboBoxEx.h"
#include "DclControlObject.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "ErrorLexicon.h"
#include "ImageComboBoxCtrl.h"
#include "ControlTypes.h"


/////////////////////////////////////////////////////////////////////////////
// Methods_ComboBoxEx

//*****************************************************************************
// 
// Method: ComboBoxEx_AddString()
// 
// Purpose: [Add a string to a list box's list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ComboBoxEx_AddString()
{
	CString sStringArg;
	int nImage;
	int nSelectedImage = -1;
	int nIndent = 0;
	int nIndex = -1;

	TDclControlPtr pArx = GetLispInput(sComboBoxEx_AddString, sStringArg, nImage, nSelectedImage, nIndent, nIndex);
	
	if (pArx == NULL)	
	{
		acedRetNil();
		return 0;
	}
	
	CImageComboBoxCtrl *pCombo = (CImageComboBoxCtrl*)pArx->GetWindow();
	
	if (nSelectedImage == -1)
		nSelectedImage = nImage;

	if (nIndex == -1)
		nIndex = pCombo->GetCount();

	COMBOBOXEXITEM cbi;

	cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
     CBEIF_SELECTEDIMAGE | CBEIF_TEXT;


	if (pCombo->GetImageList() != NULL)
		pCombo->GetImageList()->SetBkColor(RGB(255,255,255));

	cbi.iItem = nIndex;
	cbi.pszText = sStringArg.LockBuffer();
	cbi.cchTextMax = sStringArg.GetLength();
	cbi.iImage = nImage;
	cbi.iSelectedImage = nSelectedImage;
	cbi.iOverlay = 2;
	cbi.iIndent = nIndent;

	int nRet = pCombo->InsertItem(&cbi);
	acedRetInt(nRet);
	return 0;
}

void ReturnItem(COMBOBOXEXITEM &cbi)
{	
	struct resbuf *list = acutBuildList(
		RTSTR, cbi.pszText,
		RTSHORT, cbi.iImage,
		RTSHORT, cbi.iSelectedImage,
		RTSHORT, cbi.iIndent,
		RTNONE);

	if (list != NULL)
	{ 	    
		acedRetList(list);		
		acutRelRb(list); 
	} 

}

int ComboBoxEx_GetLBText()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetLBText, &nArg);
	int nIndex;

	if (pControl == NULL ||
			!GetIntArgument(nArg, &nIndex, sComboBoxEx_GetLBText) ||
			nIndex < 0 ||
			nIndex >= ((CComboBoxEx*)pControl)->GetCount())
	{		
		acedRetNil();
		return 0;
	}	

	CString sString;
	((CComboBox*)pControl)->GetLBText(nIndex, sString);
	acedRetStr( sString );
	return 0;
}


//*****************************************************************************
// 
// Method: ComboBoxEx_SetItem()
// 
// Purpose: [sets the string to a list box's list]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int ComboBoxEx_SetItem()
{
	CString sStringArg;
	int nImage;
	int nSelectedImage;
	int nIndex;
	int nIndent;
	int nArg=0;
	
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetItem, &nArg);
	
	if (pControl == NULL ||
			!GetIntArgument(nArg, &nIndex, sComboBoxEx_SetItem) ||
			nIndex < 0 ||
			nIndex >= ((CComboBoxEx*)pControl)->GetCount())
	{		
		acedRetNil();
		return 0;
	}	
	nArg++;

	if (!GetStringArgument(nArg, &sStringArg, sComboBoxEx_SetItem))
	{
		acedRetNil();
		return 0;
	}
	nArg++;

	if (!GetIntArgument(nArg, &nImage, sComboBoxEx_SetItem))
	{		
		acedRetNil();
		return 0;
	}	
	nArg++;

	if (!GetIntArgument(nArg, &nSelectedImage, sComboBoxEx_SetItem))
	{		
		acedRetNil();
		return 0;
	}	
	nArg++;

	if (!FindOptionalIntArgument(nArg, &nIndent, sComboBoxEx_SetItem))
	{		
		nIndent = 0;
	}
	

	COMBOBOXEXITEM cbi;

	cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
     CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

	cbi.iItem = nIndex;
	cbi.pszText = sStringArg.LockBuffer();
	cbi.iImage = nImage;
	cbi.iSelectedImage = nSelectedImage;
	cbi.iOverlay = 2;
	cbi.iIndent = (nIndent & 0x03); 

	acedRetInt(((CComboBoxEx*)pControl)->SetItem(&cbi));
	return 0;
}

int ComboBoxEx_Clear()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_Clear);
	if (!pControl )
	{		
		acedRetNil();
		return 0;
	}
	
	((CComboBoxEx*)pControl)->ResetContent();
	acedRetT();
	return 0;
}

int ComboBoxEx_GetCurSel()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetCurSel);
	if (!pControl )
	{		
		acedRetNil();
		return 0;
	}

	acedRetInt(((CComboBoxEx*)pControl)->GetCurSel());
	return 0;
}

int ComboBoxEx_GetCount()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetCount);
	if (!pControl )
	{		
		acedRetNil();
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*)pControl)->GetCount());
	return 0;
}

int ComboBoxEx_DeleteString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_DeleteString, &nArg);
	int nIndex;
	
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sComboBoxEx_DeleteString) ||
			nIndex < 0 ||
			nIndex >= ((CComboBoxEx*)pControl)->GetCount())
	{		
		acedRetNil();
		return 0;
	}
	((CComboBoxEx*) pControl)->DeleteItem(nIndex);
	acedRetT();
	return 0;

}

int ComboBoxEx_SelectString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SelectString, &nArg);
	CString sSearchString;

	if (!pControl || !GetStringArgument(nArg, &sSearchString, sComboBoxEx_SelectString))
	{		
		acedRetNil();
		return 0;
	}
	
	int nIndex = ((CComboBoxEx*) pControl)->GetCurSel();
	acedRetInt(((CComboBoxEx*) pControl)->SelectString(nIndex, sSearchString));
	return 0;
}

int ComboBoxEx_FindString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_FindString, &nArg);
	CString sSearchString;

	if (!pControl || !GetStringArgument(nArg, &sSearchString, sComboBoxEx_FindString))
	{		
		acedRetNil();
		return 0;
	}
	
	int nIndex = ((CComboBoxEx*) pControl)->GetCurSel();
	acedRetInt(((CComboBoxEx*) pControl)->FindString(nIndex, sSearchString));
	return 0;
}

int ComboBoxEx_FindStringExact()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_FindStringExact, &nArg);
	CString sSearchString;

	if (!pControl || !GetStringArgument(nArg, &sSearchString, sComboBoxEx_FindStringExact))
	{		
		acedRetNil();
		return 0;
	}
	
	int nIndex = ((CComboBoxEx*) pControl)->GetCurSel();
	acedRetInt(((CComboBoxEx*) pControl)->FindStringExact(nIndex, sSearchString));
	return 0;
}

int ComboBoxEx_SetCurSel()
{
	int nArg=0;

	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetCurSel, &nArg);
	int nIndex;
	
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sComboBoxEx_SetCurSel) ||
			nIndex < 0 ||
			nIndex >= ((CComboBoxEx*)pControl)->GetCount())
	{		
		acedRetNil();
		return 0;
	}
	((CComboBoxEx*) pControl)->SetCurSel(nIndex);

	acedRetT();
	return 0;
}

int ComboBoxEx_GetEditSel()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetEditSel);
	
	if (!pControl)
	{		
		acedRetNil();
		return 0;
	}
	DWORD dwSel;
	int nStart;
	int nEnd;
	if ((dwSel=((CComboBoxEx*) pControl)->GetEditSel()) == CB_ERR)
	{
		acedRetNil();
		return 0;
	}
	else
	{
		nStart = HIWORD(dwSel);
		nEnd = LOWORD(dwSel);
	}

	// this code is for all other dialogs
	struct resbuf *list = acutBuildList(
		RTSHORT, nStart,
		RTSHORT, nEnd,
		RTNONE);

	if (list != NULL)
	{ 	    
		acedRetList(list);		
		acutRelRb(list); 
	} 

	return 0;	
}

int ComboBoxEx_SetEditSel()
{
	int nStart;
	int nEnd;
	
	CWnd *pControl = GetArgsControlIntInt(CtlImageComboBox, sComboBoxEx_SetEditSel, nStart, nEnd);

	if (!pControl)
	{		
		acedRetNil();
		return 0;
	}

	((CComboBoxEx*) pControl)->GetEditCtrl()->SetSel(nStart, nEnd);

	acedRetT();
	return 0;
}

int ComboBoxEx_SetItemData()
{
	int nIndex = -1;
	int nData = 0;
	CWnd *pControl = GetArgsControlIntInt(CtlImageComboBox, sComboBoxEx_SetItemData, nIndex, nData);

	if (!pControl || nIndex < 0 || nIndex >= ((CComboBoxEx*)pControl)->GetCount())
	{		
		acedRetNil();
		return 0;
	}
	
	((CComboBoxEx*) pControl)->SetItemData(nIndex, nData);

	acedRetT();
	return 0;

}
int ComboBoxEx_GetItemData()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetItemData, &nArg);
	int nIndex;
	
	if (!pControl ||
			!GetIntArgument(nArg, &nIndex, sComboBoxEx_GetItemData) ||
			nIndex < 0 ||
			nIndex >= ((CComboBoxEx*)pControl)->GetCount())
	{
		acedRetNil();
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*) pControl)->GetItemData(nIndex));
	return 0;

}
int ComboBoxEx_GetTopIndex()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetTopIndex);

	if (pControl == NULL)		
	{
		acedRetNil();
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*)pControl)->GetTopIndex());	
	return 0;

}
int ComboBoxEx_SetTopIndex()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetTopIndex, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBoxEx_SetTopIndex) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	((CComboBoxEx*) pControl)->SetTopIndex(nIndex);

	acedRetT();
	return 0;
}

int ComboBoxEx_GetDroppedWidth()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetDroppedWidth);

	if (pControl == NULL)		
	{
		acedRetNil();
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*)pControl)->GetDroppedWidth());	
	return 0;

}
int ComboBoxEx_SetDroppedWidth()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetDroppedWidth, &nArg);
	int nDroppedWidth;
	
	if (!GetIntArgument(nArg, &nDroppedWidth, sComboBoxEx_SetDroppedWidth) || pControl == NULL)
	{
		acedRetNil();
		return 0;
	}
	((CComboBoxEx*) pControl)->SetDroppedWidth(nDroppedWidth);

	acedRetT();
	return 0;
}

int ComboBoxEx_ClearEdit()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_ClearEdit);

	if (pControl == NULL)		
	{
		acedRetNil();
		return 0;
	}
	
	((CComboBoxEx*)pControl)->Clear();
	acedRetT();
	return 0;
}


int ComboBoxEx_GetEBText()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetEBText);
	if (pControl == NULL)
	{
		acedRetNil();
		return 0;
	}

	CString sString;
	CComboBoxEx *pCtrl = (CComboBoxEx*)pControl;
	CEdit *pEdit = pCtrl->GetEditCtrl();
	if (pEdit != NULL)
		pEdit->GetWindowText(sString);
	else
		pCtrl->GetLBText(pCtrl->GetCurSel(), sString);
	acedRetStr(sString);
	return 0;
}

int ComboBoxEx_GetItem()
{
	int nIndex = -1;
	TDclControlPtr pDclObject = GetLispInput(sGrid_SelectCell, nIndex);
	if (pDclObject == NULL || nIndex < 0 || nIndex >= ((CComboBoxEx*)pDclObject->GetWindow())->GetCount())
	{		
		acedRetNil();
		return 0;
	}

	COMBOBOXEXITEM cbi = { CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT, nIndex };
	CString sText;
	int cchText = ((CComboBoxEx*)pDclObject->GetWindow())->GetLBTextLen( nIndex ) + 1;
	cbi.pszText = sText.GetBuffer( cchText );
	cbi.cchTextMax = cchText;
	((CComboBoxEx*)pDclObject->GetWindow())->GetItem(&cbi);
	ReturnItem(cbi);

	return 0;
}