// Methods_ComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ComboBoxEx.h"
#include "DclControlObject.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "ErrorLexicon.h"
#include "VdclComboBoxEx.h"
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

	CDclControlObject *pArx = GetLispInput(sComboBoxEx_AddString, sStringArg, nImage, nSelectedImage, nIndent, nIndex);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	VdclComboBoxEx *pCombo = (VdclComboBoxEx*)pArx->m_pWnd;
	
	if (nSelectedImage == -1)
		nSelectedImage = nImage;

	if (nIndex == -1)
		nIndex = pCombo->GetComboBoxCtrl()->GetCount();

	COMBOBOXEXITEM cbi;

	cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
     CBEIF_SELECTEDIMAGE | CBEIF_TEXT;


	if (pCombo->GetImageList() != NULL)
		pCombo->GetImageList()->SetBkColor(RGB(255,255,255));

	cbi.iItem = nIndex;
	cbi.pszText = (LPTSTR)(LPCTSTR)sStringArg;
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
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSTR, CString(cbi.pszText),
		RTSHORT, cbi.iImage,
		RTSHORT, cbi.iSelectedImage,
		RTSHORT, cbi.iIndent,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 

}

int ComboBoxEx_GetLBText()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetLBText, &nArg);
	int nArgument;
	
	if (!GetIntArgument(nArg, &nArgument, sComboBoxEx_GetLBText) || pControl == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}
	
	COMBOBOXEXITEM cbi;

	cbi.iItem = nArgument;
	
	((CComboBoxEx*)pControl)->GetItem(&cbi);

	// return the string found
	CString sString;
	
	int n = ((CComboBox*)pControl)->GetLBTextLen(nArgument);      
	((CComboBox*)pControl)->GetLBText(nArgument, sString.GetBuffer(n));
	sString.ReleaseBuffer();

	cbi.pszText = (LPTSTR)(LPCTSTR)sString;
	cbi.cchTextMax = sString.GetLength();
	
	ReturnItem(cbi);
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
	
	if (pControl == NULL || !GetIntArgument(nArg, &nIndex, sComboBoxEx_SetItem))
	{		
		acedRetInt(-1);
		return 0;
	}	
	nArg++;

	if (!GetStringArgument(nArg, &sStringArg, sComboBoxEx_SetItem))
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	nArg++;

	if (!GetIntArgument(nArg, &nImage, sComboBoxEx_SetItem))
	{		
		acedRetInt(-1);
		return 0;
	}	
	nArg++;

	if (!GetIntArgument(nArg, &nSelectedImage, sComboBoxEx_SetItem))
	{		
		acedRetInt(-1);
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
	cbi.pszText = (LPTSTR)(LPCTSTR)sStringArg;
	cbi.cchTextMax = sStringArg.GetLength();
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

	int nArg = 0;
	CDclControlObject *pArxObject = GetControlArxObject(sComboBoxEx_Clear, &nArg);
	
	int nCount = ((CComboBoxEx*)pControl)->GetComboBoxCtrl()->GetCount();

	for (int i=0; i<nCount; i++)
	{
		((CComboBoxEx*)pControl)->DeleteItem(0);
	}
	((CComboBoxEx*)pControl)->GetComboBoxCtrl()->ResetContent();
	
	// return nil
	acedRetVoid();
	return 0;
}

int ComboBoxEx_GetCurSel()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetCurSel);
	
	int nSel = ((CComboBoxEx*)pControl)->GetComboBoxCtrl()->GetCurSel();
	
	// return nil
	acedRetInt(nSel);
	return 0;
}

int ComboBoxEx_GetCount()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetCount);
	
	int nCount = ((CComboBoxEx*)pControl)->GetComboBoxCtrl()->GetCount();
	
	// return nil
	acedRetInt(nCount);
	return 0;
}


int ComboBoxEx_DeleteString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_DeleteString, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBoxEx_DeleteString) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBoxEx*) pControl)->DeleteItem(nIndex);
	acedRetVoid();
	return 0;

}
int ComboBoxEx_SelectString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SelectString, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sComboBoxEx_SelectString) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CComboBoxEx*) pControl)->GetComboBoxCtrl()->GetCurSel();
	acedRetInt(((CComboBoxEx*) pControl)->GetComboBoxCtrl()->SelectString(nIndex, sSearchString));
	return 0;

}

int ComboBoxEx_FindString()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_FindString, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sComboBoxEx_FindString) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CComboBoxEx*) pControl)->GetComboBoxCtrl()->GetCurSel();
	acedRetInt(((CComboBoxEx*) pControl)->GetComboBoxCtrl()->FindString(nIndex, sSearchString));
	return 0;

}
int ComboBoxEx_FindStringExact()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_FindStringExact, &nArg);
	CString sSearchString;

	if (!GetStringArgument(nArg, &sSearchString, sComboBoxEx_FindStringExact) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	int nIndex = ((CComboBoxEx*) pControl)->GetComboBoxCtrl()->GetCurSel();
	acedRetInt(((CComboBoxEx*) pControl)->GetComboBoxCtrl()->FindStringExact(nIndex, sSearchString));
	return 0;

}
int ComboBoxEx_SetCurSel()
{
	int nArg=0;

	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetCurSel, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBoxEx_SetCurSel) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBoxEx*) pControl)->GetComboBoxCtrl()->SetCurSel(nIndex);

	acedRetVoid();
	return 0;
}
int ComboBoxEx_GetEditSel()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetEditSel);
	
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
	if ((dwSel=((CComboBoxEx*) pControl)->GetComboBoxCtrl()->GetEditSel()) == CB_ERR)
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
int ComboBoxEx_SetEditSel()
{
	//int nArg=0;
	//CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetEditSel, &nArg);
	int nStart;
	int nEnd;
	
	CWnd *pControl = GetArgsControlIntInt(CtlImageComboBox, sComboBoxEx_SetEditSel, nStart, nEnd);

	if (pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}

	((CComboBoxEx*) pControl)->GetEditCtrl()->SetSel(nStart, nEnd);

	acedRetVoid();
	return 0;
}
int ComboBoxEx_SetItemData()
{
	int nIndex;
	int nData;
	CWnd *pControl = GetArgsControlIntInt(CtlImageComboBox, sComboBoxEx_SetItemData, nIndex, nData);

	if (pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	((CComboBoxEx*) pControl)->GetComboBoxCtrl()->SetItemData(nIndex, nData);

	acedRetVoid();
	return 0;

}
int ComboBoxEx_GetItemData()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetItemData, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBoxEx_GetItemData) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*) pControl)->GetComboBoxCtrl()->GetItemData(nIndex));
	return 0;

}
int ComboBoxEx_GetTopIndex()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetTopIndex);

	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*)pControl)->GetComboBoxCtrl()->GetTopIndex());	
	return 0;

}
int ComboBoxEx_SetTopIndex()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetTopIndex, &nArg);
	int nIndex;
	
	if (!GetIntArgument(nArg, &nIndex, sComboBoxEx_SetTopIndex) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBoxEx*) pControl)->GetComboBoxCtrl()->SetTopIndex(nIndex);

	acedRetVoid();
	return 0;
}

int ComboBoxEx_GetDroppedWidth()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetTopIndex);

	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	acedRetInt(((CComboBoxEx*)pControl)->GetComboBoxCtrl()->GetDroppedWidth());	
	return 0;

}
int ComboBoxEx_SetDroppedWidth()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_SetDroppedWidth, &nArg);
	int nDroppedWidth;
	
	if (!GetIntArgument(nArg, &nDroppedWidth, sComboBoxEx_SetDroppedWidth) || pControl == NULL)
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	((CComboBoxEx*) pControl)->GetComboBoxCtrl()->SetDroppedWidth(nDroppedWidth);

	acedRetVoid();
	return 0;
}
int ComboBoxEx_ClearEdit()
{
	CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_ClearEdit);

	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	((CComboBoxEx*)pControl)->GetComboBoxCtrl()->Clear();
	acedRetVoid();
	return 0;

}


int ComboBoxEx_GetEBText()
{
		CWnd *pControl = GetControlPointer(CtlImageComboBox, sComboBoxEx_GetEBText);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetStr(CString());
		return 0;
	}

	
	CString sString;
	
	CComboBoxEx *pCtrl = (CComboBoxEx*)pControl;
	
	CEdit *pEdit = pCtrl->GetEditCtrl();
	
	if (pEdit != NULL)
	{	
		pEdit->GetWindowText(sString);
		// return the string found
		acedRetStr(sString);
	}
	else
	{
		int nSel = pCtrl->GetComboBoxCtrl()->GetCurSel();
	
		COMBOBOXEXITEM cbi;

		cbi.iItem = nSel;
		
		pCtrl->GetItem(&cbi);

		// return the string found
		CString sString;
		
		int n = pCtrl->GetLBTextLen(nSel);      
		pCtrl->GetLBText(nSel, sString.GetBuffer(n));
		sString.ReleaseBuffer();

		cbi.pszText = (LPTSTR)(LPCTSTR)sString;
		cbi.cchTextMax = sString.GetLength();

		// return the string found
		acedRetStr(sString);
	}
	return 0;
}

int ComboBoxEx_GetItem()
{
	int nIndex=-1;

	CDclControlObject *pArx = GetLispInput(sGrid_SelectCell, nIndex);

	if (pArx == NULL || nIndex == -1)
	{		
		acedRetNil();
		return 0;
	}

	CComboBoxEx *pCtrl = (CComboBoxEx*)pArx->m_pWnd;
	
	
	COMBOBOXEXITEM cbi;

	cbi.iItem = nIndex;
	
	pCtrl->GetItem(&cbi);

	// return the string found
	CString sString;
	
	int n = pCtrl->GetLBTextLen(nIndex);      
	pCtrl->GetLBText(nIndex, sString.GetBuffer(n));
	sString.ReleaseBuffer();

	cbi.pszText = (LPTSTR)(LPCTSTR)sString;
	cbi.cchTextMax = sString.GetLength();

	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSTR, sString,
		RTSHORT, cbi.iImage,
		RTSHORT, cbi.iSelectedImage,
		RTSHORT, cbi.iIndent,			
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 

	return 0;
}