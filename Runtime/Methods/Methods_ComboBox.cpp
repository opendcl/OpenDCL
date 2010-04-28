// Methods_ComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ComboBox.h"
#include "ArgumentsRetrieval.h"
#include "ArxColorComboBoxCtrl.h"
#include "ArxLineweightComboBoxCtrl.h"
#include "ArxFolderComboCtrl.h"
#include "ControlTypes.h"
#include "TextBoxFilterTypes.h"
#include "ComboStyles.h"
#include "ArxWorkspace.h"
#include "Resource.h"


ADSRESULT ComboBox::AddPath()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( pDlgControl->GetTemplate()->GetLongProperty( Prop::ComboBoxStyle ) != ComboStyle::DirPicker )
		return HandleArgValueError( pArgs, IDS_ERRNOTDIRPICKERCOMBO );

	CString sPath;
	if( !GetStringArgument( pArgs, sPath ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxFolderComboCtrl* pCtrl = (CArxFolderComboCtrl*)pDlgControl->GetControlWnd();

	pCtrl->AddPath( sPath );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::AddColor()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( pDlgControl->GetTemplate()->GetLongProperty( Prop::ComboBoxStyle ) != ComboStyle::Color )
		return HandleArgValueError( pArgs, IDS_ERRNOTCOLORCOMBO );

	int nColorIndex;
	if( !GetIntArgument( pArgs, nColorIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxColorComboBoxCtrl* pCtrl = (CArxColorComboBoxCtrl*)pDlgControl->GetControlWnd();

	pCtrl->AddColorToMRU( nColorIndex );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::FindLineWeight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( pDlgControl->GetTemplate()->GetLongProperty( Prop::ComboBoxStyle ) != ComboStyle::LineWeight )
		return HandleArgValueError( pArgs, IDS_ERRNOTLINEWEIGHTCOMBO );

	int nLineweight;
	if( !GetIntArgument( pArgs, nLineweight ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxLineweightComboBoxCtrl* pCtrl = (CArxLineweightComboBoxCtrl*)pDlgControl->GetControlWnd();

	int idx = pCtrl->FindItemByLineWeight( nLineweight );
	if( idx >= 0 )
		acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ComboBox::FindColor()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( pDlgControl->GetTemplate()->GetLongProperty( Prop::ComboBoxStyle ) != ComboStyle::Color )
		return HandleArgValueError( pArgs, IDS_ERRNOTCOLORCOMBO );

	int nColorIndex;
	if( !GetIntArgument( pArgs, nColorIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxColorComboBoxCtrl* pCtrl = (CArxColorComboBoxCtrl*)pDlgControl->GetControlWnd();

	int idx = pCtrl->FindItemByColorIndex( nColorIndex );
	if( idx >= 0 )
		acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ComboBox::AddString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	PropVal::TCStringArray rsToAdd;
	if( !GetStringArrayArgument( pArgs, rsToAdd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idxNewItem = -1;
	TPropertyPtr pListProp = pDlgControl->GetTemplate()->GetPropertyObject( Prop::List );
	if( pListProp )
	{
		PropVal::TCStringArray* prsList = pListProp->GetStringArrayPtr();
		prsList->insert( prsList->end(), rsToAdd.begin(), rsToAdd.end() );
		if( pDlgControl->ApplyProperty( pListProp ) )
			idxNewItem = prsList->size();
	}

	if( idxNewItem >= 0 )
		acedRetInt( idxNewItem );
	return RSRSLT;
}

ADSRESULT ComboBox::AddList()
{
	return ComboBox::AddString();
}

ADSRESULT ComboBox::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();
	pCtrl->ResetContent();
	acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCurSel() );
	return RSRSLT;
}

ADSRESULT ComboBox::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCount() );
	return RSRSLT;
}

ADSRESULT ComboBox::DeleteItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();
	int nResult = pCtrl->DeleteString( nIndex );
	if( nResult >= 0 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::InsertString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	CString sToAdd;
	if( !GetStringArgument( pArgs, sToAdd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();
	int idxNewItem = pCtrl->InsertString( nIndex, sToAdd );
	if( idxNewItem >= 0 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::SelectString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	int nStartIndex = pCtrl->GetCurSel();
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->SelectString( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ComboBox::FindString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	int nStartIndex = -1;
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->FindString( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ComboBox::FindStringExact()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	int nStartIndex = -1;
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->FindStringExact( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ComboBox::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	int idx = pCtrl->SetCurSel( nIndex );
	if( idx == nIndex )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::GetEditSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

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

ADSRESULT ComboBox::SetEditSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nStart = -1;
	if( !GetIntArgument( pArgs, nStart ) )
		return RSERR; //invalid input

	int nEnd = -1;
	if( !GetIntArgument( pArgs, nEnd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	if( pCtrl->SetEditSel( nStart, nEnd ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::SetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	DWORD_PTR dwData = -1;
	if( !GetHandleArgument( pArgs, dwData ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	if( CB_ERR != pCtrl->SetItemData( nItem, dwData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::GetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	DWORD_PTR dwData = pCtrl->GetItemData( nItem );
	theArxWorkspace.RetHandle( dwData );
	return RSRSLT;
}

ADSRESULT ComboBox::GetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetTopIndex() );
	return RSRSLT;
}

ADSRESULT ComboBox::SetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	if( CB_ERR != pCtrl->SetTopIndex( nItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::GetDroppedWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetDroppedWidth() );
	return RSRSLT;
}

ADSRESULT ComboBox::SetDroppedWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nWidth = -1;
	if( !GetIntArgument( pArgs, nWidth ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	if( CB_ERR != pCtrl->SetDroppedWidth( nWidth ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::ClearEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	pCtrl->Clear();
	acedRetT();
	return RSRSLT;
}

ADSRESULT ComboBox::GetLBText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	CString sText;
	if( nItem >= 0 && nItem < pCtrl->GetCount() )
		pCtrl->GetLBText( nItem, sText );
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT ComboBox::GetEBText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CComboBox* pCtrl = (CComboBox*)pDlgControl->GetControlWnd();

	CString sText;
	pCtrl->GetWindowText( sText );
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT ComboBox::GetDir()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	if( pDlgControl->GetTemplate()->GetLongProperty( Prop::ComboBoxStyle ) != ComboStyle::DirPicker )
		return HandleArgValueError( pArgs, IDS_ERRNOTDIRPICKERCOMBO );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxFolderComboCtrl* pCtrl = (CArxFolderComboCtrl*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetSelectedPath() );
	return RSRSLT;
}

ADSRESULT ComboBox::Dir()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlComboBox ) )
		return RSERR; //invalid input

	CString sDirectory;
	if( !GetStringArgument( pArgs, sDirectory ) )
		return RSERR; //invalid input

	CString sFilter = _T("*.*");
	GetStringArgument( pArgs, sFilter, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxFolderComboCtrl* pCtrl = (CArxFolderComboCtrl*)pDlgControl->GetControlWnd();

	TCHAR szOldPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, szOldPath );
	::SetCurrentDirectory( sDirectory );
	pCtrl->ResetContent();
	pCtrl->Dir( DDL_READWRITE | DDL_DIRECTORY, sFilter );
	::SetCurrentDirectory( szOldPath );
	acedRetT();
	return RSRSLT;
}

