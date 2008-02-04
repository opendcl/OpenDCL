// Methods_OptionList.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_OptionList.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ArxOptionListCtrl.h"
#include "Workspace.h"


ADSRESULT OptionList::SetEnabled()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	bool bEnabled = true;
	if( !GetBoolArgument( pArgs, bEnabled ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	DWORD_PTR dwData = pCtrl->GetItemData( nItem );
	if( LB_ERR == dwData )
		return RSRSLT;
	if( bEnabled )
		dwData &= ~2;
	else
		dwData |= 2;
	if( LB_ERR != pCtrl->SetItemData( nItem, dwData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT OptionList::AddString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	CStringArray rsToAdd;
	if( !GetStringArrayArgument( pArgs, rsToAdd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int idxLastAdded = -1;
	INT_PTR ctArgs = rsToAdd.GetSize();
	for( INT_PTR idx = 0; idx < ctArgs; ++idx )
		idxLastAdded = pCtrl->AddString( rsToAdd.GetAt( idx ) );
	if( idxLastAdded != CB_ERR )
		acedRetInt( idxLastAdded );
	return RSRSLT;
}

ADSRESULT OptionList::AddList()
{
	return OptionList::AddString();
}

ADSRESULT OptionList::GetText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	CString sText;
	pCtrl->GetText( nItem, sText );
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT OptionList::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nSel = pCtrl->GetCurSel();
	if( nSel >= 0 )
		acedRetInt( nSel );
	return RSRSLT;
}

ADSRESULT OptionList::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCount() );
	return RSRSLT;
}

ADSRESULT OptionList::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nCurSel = -1;
	if( !GetIntArgument( pArgs, nCurSel ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SetCurSel( nCurSel ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT OptionList::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	pCtrl->ResetContent();
	acedRetT();
	return RSRSLT;
}

ADSRESULT OptionList::GetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetTopIndex() );
	return RSRSLT;
}

ADSRESULT OptionList::SetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SetTopIndex( nItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT OptionList::DeleteString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	pCtrl->DeleteString( nIndex );
	acedRetT();
	return RSRSLT;
}

ADSRESULT OptionList::InsertString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	CString sToAdd;
	if( !GetStringArgument( pArgs, sToAdd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int idx = pCtrl->InsertString( nIndex, sToAdd );
	if( idx >= 0 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT OptionList::SetTttTitle()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlOptionList ) )
		return RSERR; //invalid input

	int nItem;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxOptionListCtrl* pCtrl = (CArxOptionListCtrl*)pDlgControl->GetControlWnd();

	TPropertyPtr pCaptionProp = pDlgControl->GetTemplate()->GetPropertyObject( Prop::BtnCaption );
	if( !pCaptionProp || nItem >= pCaptionProp->size() )
		return RSRSLT;

	TPropertyPtr pTTTProp = pDlgControl->GetTemplate()->GetPropertyObject( Prop::BtnTTText );
	if( !pTTTProp )
		return RSRSLT;

	PropVal::TCStringArray* prsTTT = pTTTProp->GetStringArrayPtr();
	if( !prsTTT )
		return RSRSLT;
	if( nItem >= prsTTT->size() )
		prsTTT->resize( nItem + 1 );
	prsTTT->at( nItem ) = sText;

	pCtrl->ResetTooltips();
	acedRetT();
	return RSRSLT;
}
