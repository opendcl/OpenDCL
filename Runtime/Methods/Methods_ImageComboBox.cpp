// Methods_ImageComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ImageComboBox.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ImageComboBoxCtrl.h"
#include "ArxWorkspace.h"


static void ReturnItem( const COMBOBOXEXITEM& cbi )
{	
	resbuf* prbRet = acutBuildList( RTSTR, cbi.pszText,
																	RTSHORT, cbi.iImage,
																	RTSHORT, cbi.iSelectedImage,
																	RTSHORT, cbi.iIndent,
																	RTNONE );

	if( prbRet )
	{ 	    
		acedRetList( prbRet );		
		acutRelRb( prbRet ); 
	} 
}

ADSRESULT ImageComboBox::AddString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	CString sToAdd;
	if( !GetStringArgument( pArgs, sToAdd ) )
		return RSERR; //invalid input

	int nImage;
	if( !GetIntArgument( pArgs, nImage ) )
		return RSERR; //invalid input

	int nSelectedImage = nImage;
	GetIntArgument( pArgs, nSelectedImage, true );

	int nIndent = 0;
	GetIntArgument( pArgs, nIndent, true );

	int nItem = -1;
	GetIntArgument( pArgs, nItem, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	COMBOBOXEXITEM cbi =
	{
		(CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY | CBEIF_SELECTEDIMAGE | CBEIF_TEXT),
		nItem,
		sToAdd.LockBuffer(),
		-1,
		nImage,
		nSelectedImage,
		2,
		nIndent
	};
	int idx = pCtrl->InsertItem( &cbi );
	if( idx != CB_ERR )
		acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	COMBOBOXEXITEM cbi =
	{
		(CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT),
		nItem,
	};
	if( pCtrl->GetItem( &cbi ) )
		ReturnItem( cbi );
	return RSRSLT;
}

ADSRESULT ImageComboBox::SetItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	int nImage;
	if( !GetIntArgument( pArgs, nImage ) )
		return RSERR; //invalid input

	int nSelectedImage = nImage;
	GetIntArgument( pArgs, nSelectedImage, true );

	int nIndent = 0;
	GetIntArgument( pArgs, nIndent, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	COMBOBOXEXITEM cbi =
	{
		(CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY | CBEIF_SELECTEDIMAGE | CBEIF_TEXT),
		nItem,
		sText.LockBuffer(),
		-1,
		nImage,
		nSelectedImage,
		2,
		nIndent
	};
	int idx = pCtrl->SetItem( &cbi );
	if( idx != CB_ERR )
		acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ImageComboBox::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();
	pCtrl->ResetContent();
	acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCurSel() );
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCount() );
	return RSRSLT;
}

ADSRESULT ImageComboBox::DeleteItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	pCtrl->DeleteItem( nIndex );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::SelectString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	int nStartIndex = pCtrl->GetCurSel();
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->SelectString( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ImageComboBox::FindString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	int nStartIndex = -1;
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->FindString( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ImageComboBox::FindStringExact()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	int nStartIndex = -1;
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->FindStringExact( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ImageComboBox::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	int idx = pCtrl->SetCurSel( nIndex );
	if( idx == CB_ERR && nIndex != -1 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetEditSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	DWORD dwSel = pCtrl->GetEditSel();
	if( dwSel != CB_ERR )
	{
		resbuf rbEnd = { NULL, RTSHORT };
		rbEnd.resval.rint = LOWORD(dwSel);
		resbuf rbStart = { &rbEnd, RTSHORT };
		rbStart.resval.rint = HIWORD(dwSel);
		acedRetList( &rbStart );
	}
	return RSRSLT;
}

ADSRESULT ImageComboBox::SetEditSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nStart = -1;
	if( !GetIntArgument( pArgs, nStart ) )
		return RSERR; //invalid input

	int nEnd = -1;
	if( !GetIntArgument( pArgs, nEnd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();
	CEdit* pEditCtrl = pCtrl->GetEditCtrl();

	if( pEditCtrl )
	{
		pEditCtrl->SetSel( nStart, nEnd );
		acedRetT();
	}
	return RSRSLT;
}

ADSRESULT ImageComboBox::SetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	DWORD_PTR dwData = -1;
	if( !GetHandleArgument( pArgs, dwData ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	if( CB_ERR != pCtrl->SetItemData( nItem, dwData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	DWORD_PTR dwData = pCtrl->GetItemData( nItem );
	if( dwData != CB_ERR  )
		theArxWorkspace.RetHandle( dwData );
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetTopIndex() );
	return RSRSLT;
}

ADSRESULT ImageComboBox::SetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	if( CB_ERR != pCtrl->SetTopIndex( nItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetDroppedWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetDroppedWidth() );
	return RSRSLT;
}

ADSRESULT ImageComboBox::SetDroppedWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nWidth = -1;
	if( !GetIntArgument( pArgs, nWidth ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	if( CB_ERR != pCtrl->SetDroppedWidth( nWidth ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::ClearEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	pCtrl->Clear();
	acedRetT();
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetLBText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	CString sText;
	pCtrl->GetLBText( nItem, sText );
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT ImageComboBox::GetEBText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlImageComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBoxEx* pCtrl = (CComboBoxEx*)pDlgControl->GetControlWnd();

	CString sText;
	CEdit* pEditCtrl = pCtrl->GetEditCtrl();
	if( pEditCtrl )
		pEditCtrl->GetWindowText( sText );
	else
		pCtrl->GetLBText( pCtrl->GetCurSel(), sText );
	acedRetStr( sText );
	return RSRSLT;
}
