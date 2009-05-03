// Methods_Tree.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Tree.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "ArxImageTreeCtrl.h"
#include "ControlTypes.h"
#include "ArxWorkspace.h"
#include "Resource.h"


void ReturnTreeItem( LPCTSTR pszLabel, int nImage, int nSelectedImage, LPCTSTR pszKey )
{
	resbuf rbKey = { NULL, RTSTR };
	rbKey.resval.rstring = (ACHAR*)pszKey;
	resbuf rbSelectedImage = { &rbKey, RTSHORT };
	rbSelectedImage.resval.rint = nSelectedImage;
	resbuf rbImage = { &rbSelectedImage, RTSHORT };
	rbImage.resval.rint = nImage;
	resbuf rbLabel = { &rbImage, RTSTR };
	rbLabel.resval.rstring = (ACHAR*)pszLabel;
	acedRetList( &rbLabel );
}

void ReturnTreeItem( LPCTSTR pszLabel, int nImage, int nSelectedImage, DWORD_PTR dwKey )
{
	resbuf rbKey = { NULL, RTENAME };
	rbKey.resval.rlname[0] = dwKey;
	resbuf rbSelectedImage = { &rbKey, RTSHORT };
	rbSelectedImage.resval.rint = nSelectedImage;
	resbuf rbImage = { &rbSelectedImage, RTSHORT };
	rbImage.resval.rint = nImage;
	resbuf rbLabel = { &rbImage, RTSTR };
	rbLabel.resval.rstring = (ACHAR*)pszLabel;
	acedRetList( &rbLabel );
}

ADSRESULT Tree::AddParent()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();

	bool bNestedLists = GetListBeginArgument( pArgs, true );

	if( bNestedLists && !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	bool bMultiple = false;
	HTREEITEM hItem = NULL;
	CString sLabel;
	while( GetStringArgument( pArgs, sLabel, true ) )
	{
		CString sKey;
		GetStringArgument( pArgs, sKey, true );

		int nImage = -1;
		GetIntArgument( pArgs, nImage, true );

		int nSelectedImage = nImage;
		GetIntArgument( pArgs, nSelectedImage, true );

		int nExpandedImage = -1;
		GetIntArgument( pArgs, nExpandedImage, true );

		if( bNestedLists && !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		bMultiple = (hItem != NULL);

		hItem = pCtrl->AddParent( sLabel, sKey, nImage, nSelectedImage, nExpandedImage );
		if( !hItem )
			return RSRSLT;

		if( !bNestedLists || !GetListBeginArgument( pArgs, true ) )
			break;
	}

	if( bNestedLists && !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( bMultiple )
		acedRetT();
	else
		theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	return RSRSLT;
}

ADSRESULT Tree::AddChild()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();

	bool bNestedLists = GetListBeginArgument( pArgs, true );

	if( bNestedLists && !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	bool bFailed = false;

	bool bMultiple = false;
	HTREEITEM hItem = NULL;
	CString sParentKey;
	DWORD_PTR dwParentKey = -1;
	while( GetStringArgument( pArgs, sParentKey, true ) ||
				 GetHandleArgument( pArgs, dwParentKey, true ) )
	{
		CString sLabel;
		if( !GetStringArgument( pArgs, sLabel ) )
			return RSERR;

		CString sKey;
		GetStringArgument( pArgs, sKey, true );

		int nImage = -1;
		GetIntArgument( pArgs, nImage, true );

		int nSelectedImage = nImage;
		GetIntArgument( pArgs, nSelectedImage, true );

		int nExpandedImage = -1;
		GetIntArgument( pArgs, nExpandedImage, true );

		if( bNestedLists && !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		bMultiple = (hItem != NULL);

		HTREEITEM hParent = (HTREEITEM)dwParentKey;
		if( dwParentKey == -1 )
			hParent = pCtrl->FindItem( sParentKey );
		if( !hParent )
			bFailed = true;
		else
			hItem = pCtrl->AddChild( hParent, sLabel, sKey, nImage, nSelectedImage, nExpandedImage );

		if( !bNestedLists || !GetListBeginArgument( pArgs, true ) )
			break;
	}

	if( bNestedLists && !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !bFailed )
	{
		if( bMultiple )
			acedRetT();
		else
			theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	}
	return RSRSLT;
}

ADSRESULT Tree::AddSibling()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	CString sLabel;
	if( !GetStringArgument( pArgs, sLabel ) )
		return RSERR;

	CString sKey;
	GetStringArgument( pArgs, sKey, true );

	int nImage = -1;
	GetIntArgument( pArgs, nImage, true );

	int nSelectedImage = nImage;
	GetIntArgument( pArgs, nSelectedImage, true );

	int nExpandedImage = -1;
	GetIntArgument( pArgs, nExpandedImage, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	HTREEITEM hItem = NULL;
	HTREEITEM hAddAfter = (HTREEITEM)dwRefKey;
	if( dwRefKey == -1 )
		hAddAfter = pCtrl->FindItem( sRefKey );
	if( hAddAfter )
		hItem = pCtrl->AddChild( NULL, sLabel, sKey, nImage, nSelectedImage, nExpandedImage, hAddAfter );

	if( hItem )
		theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	return RSRSLT;
}

ADSRESULT Tree::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Clear();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetCount() );
	return RSRSLT;
}

ADSRESULT Tree::SelectItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->SelectItem( hItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::GetParentItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hParentItem = pCtrl->GetParentItem( hItem );
		if( hParentItem )
		{
			CString sKey = pCtrl->GetItemKey( hParentItem );
			if( !sKey.IsEmpty() )
				acedRetStr( sKey );
			else
				theArxWorkspace.RetHandle( (DWORD_PTR)hParentItem );
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::ItemHasChildren()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->ItemHasChildren( hItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::GetNextSiblingItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hSiblingItem = pCtrl->GetNextSiblingItem( hItem );
		if( hSiblingItem )
		{
			CString sKey = pCtrl->GetItemKey( hSiblingItem );
			if( !sKey.IsEmpty() )
				acedRetStr( sKey );
			else
				theArxWorkspace.RetHandle( (DWORD_PTR)hSiblingItem );
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::GetPrevSiblingItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hSiblingItem = pCtrl->GetPrevSiblingItem( hItem );
		if( hSiblingItem )
		{
			CString sKey = pCtrl->GetItemKey( hSiblingItem );
			if( !sKey.IsEmpty() )
				acedRetStr( sKey );
			else
				theArxWorkspace.RetHandle( (DWORD_PTR)hSiblingItem );
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::GetFirstVisibleItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = pCtrl->GetFirstVisibleItem();
	if( hItem )
	{
		CString sKey = pCtrl->GetItemKey( hItem );
		if( !sKey.IsEmpty() )
			acedRetStr( sKey );
		else
			theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	}
	return RSRSLT;
}

ADSRESULT Tree::GetNextVisibleItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hNextItem = pCtrl->GetNextVisibleItem( hItem );
		if( hNextItem )
		{
			CString sKey = pCtrl->GetItemKey( hNextItem );
			if( !sKey.IsEmpty() )
				acedRetStr( sKey );
			else
				theArxWorkspace.RetHandle( (DWORD_PTR)hNextItem );
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::GetPrevVisibleItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hNextItem = pCtrl->GetPrevVisibleItem( hItem );
		if( hNextItem )
		{
			CString sKey = pCtrl->GetItemKey( hNextItem );
			if( !sKey.IsEmpty() )
				acedRetStr( sKey );
			else
				theArxWorkspace.RetHandle( (DWORD_PTR)hNextItem );
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::GetSelectedItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = pCtrl->GetSelectedItem();
	if( hItem )
	{
		CString sKey = pCtrl->GetItemKey( hItem );
		if( !sKey.IsEmpty() )
			acedRetStr( sKey );
		else
			theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	}
	return RSRSLT;
}

ADSRESULT Tree::GetRootItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = pCtrl->GetRootItem();
	if( hItem )
	{
		CString sKey = pCtrl->GetItemKey( hItem );
		if( !sKey.IsEmpty() )
			acedRetStr( sKey );
		else
			theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	}
	return RSRSLT;
}

ADSRESULT Tree::GetItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		CString sLabel = pCtrl->GetItemText( hItem );
		int nImage = -1;
		int nSelectedImage = -1;
		pCtrl->GetItemImage( hItem, nImage, nSelectedImage );
		if( dwRefKey != -1 )
			ReturnTreeItem( sLabel, nImage, nSelectedImage, dwRefKey );
		else
			ReturnTreeItem( sLabel, nImage, nSelectedImage, sRefKey );
	}
	return RSRSLT;
}

ADSRESULT Tree::GetItemImages()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		int nImage = -1;
		int nSelectedImage = -1;
		pCtrl->GetItemImage( hItem, nImage, nSelectedImage );
		int nExpandedImage = pCtrl->GetExpandedImage( hItem );
		resbuf rbExpandedImage = { NULL, RTSHORT };
		rbExpandedImage.resval.rint = nExpandedImage;
		resbuf rbSelectedImage = { &rbExpandedImage, RTSHORT };
		rbSelectedImage.resval.rint = nSelectedImage;
		resbuf rbImage = { &rbSelectedImage, RTSHORT };
		rbImage.resval.rint = nImage;
		acedRetList( &rbImage );
	}
	return RSRSLT;
}

ADSRESULT Tree::SetItemImages()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	int nImage = -1;
	if( !GetIntArgument( pArgs, nImage ) )
		return RSERR; //invalid input

	int nSelectedImage = nImage;
	int nExpandedImage = nImage;
	if( GetIntArgument( pArgs, nSelectedImage, true ) )
		GetIntArgument( pArgs, nExpandedImage, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		if( pCtrl->SetItemImage( hItem, nImage, nSelectedImage ) )
		{
			pCtrl->SetExpandedImage( hItem, nExpandedImage );
			pCtrl->OnNeedRepaint();
			acedRetT();
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::SetExpandedImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	int nExpandedImage = -1;
	if( !GetIntArgument( pArgs, nExpandedImage ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->SetExpandedImage( hItem, nExpandedImage ) )
	{
		pCtrl->OnNeedRepaint();
		acedRetT();
	}
	return RSRSLT;
}

ADSRESULT Tree::GetExpandedImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
		acedRetInt( pCtrl->GetExpandedImage( hItem ) );
	return RSRSLT;
}

ADSRESULT Tree::GetItemKey()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	DWORD_PTR dwRefKey = -1;
	if( !GetHandleArgument( pArgs, dwRefKey ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	if( dwRefKey )
	{
		CString sKey = pCtrl->GetItemKey( (HTREEITEM)dwRefKey );
		if( !sKey.IsEmpty() )
			acedRetStr( sKey );
	}
	return RSRSLT;
}

ADSRESULT Tree::GetItemHandle()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	if( !GetStringArgument( pArgs, sRefKey ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = pCtrl->FindItem( sRefKey );
	if( hItem )
		theArxWorkspace.RetHandle( (DWORD_PTR)hItem );
	return RSRSLT;
}

ADSRESULT Tree::GetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
		theArxWorkspace.RetHandle( pCtrl->GetItemData( hItem ) );
	return RSRSLT;
}

ADSRESULT Tree::SetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	DWORD_PTR dwData = -1;
	if( !GetHandleArgument( pArgs, dwData ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->SetItemData( hItem, dwData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::GetItemLabel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
		acedRetStr( pCtrl->GetItemText( hItem ) );
	return RSRSLT;
}

ADSRESULT Tree::SetItemLabel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	CString sLabel;
	if( !GetStringArgument( pArgs, sLabel ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->SetItemText( hItem, sLabel ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::GetVisibleCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetVisibleCount() );
	return RSRSLT;
}

ADSRESULT Tree::DeleteItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->DeleteItem( hItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::ExpandItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	int nExpandStyle;
	if( !GetIntArgument( pArgs, nExpandStyle ) )
		return RSERR; //invalid input
	UINT nStyle = 0;
	switch( nExpandStyle )
	{
	case -1:
		nStyle = TVE_COLLAPSE;
		break;
	case 0:
		nStyle = TVE_TOGGLE;
		break;
	case 1:
		nStyle = TVE_EXPAND;
		break;
	default:
		HandleArgValueError( pArgs );
		return RSERR;
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		if( pCtrl->Expand( hItem, nStyle ) )
		{
			pCtrl->OnNeedRepaint();
			acedRetT();
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::SelectSetFirstVisible()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->SelectSetFirstVisible( hItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::StartLabelEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->EditLabel( hItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::CancelLabelEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	CEdit* pEdit = pCtrl->GetEditControl();
	if( pEdit )
	{
		pEdit->DestroyWindow();
		acedRetT();
	}
	return RSRSLT;
}

ADSRESULT Tree::SortChildren()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->SortChildren( hItem ) )
	{
		pCtrl->OnNeedRepaint();
		acedRetT();
	}
	return RSRSLT;
}

ADSRESULT Tree::EnsureVisible()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem && pCtrl->EnsureVisible( hItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tree::GetFirstChildItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hChildItem = pCtrl->GetChildItem( hItem );
		if( hChildItem )
		{
			CString sKey = pCtrl->GetItemKey( hChildItem );
			if( !sKey.IsEmpty() )
				acedRetStr( sKey );
			else
				theArxWorkspace.RetHandle( (DWORD_PTR)hChildItem );
		}
	}
	return RSRSLT;
}

ADSRESULT Tree::IsItemExpanded()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTree))
		return RSERR; //invalid input

	CString sRefKey;
	DWORD_PTR dwRefKey = -1;
	if( !GetStringArgument( pArgs, sRefKey, true ) )
	{
		if( !GetHandleArgument( pArgs, dwRefKey ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxImageTreeCtrl* pCtrl = (CArxImageTreeCtrl*)pDlgControl->GetControlWnd();
	HTREEITEM hItem = ((dwRefKey != -1)? (HTREEITEM)dwRefKey : pCtrl->FindItem( sRefKey ) );
	if( hItem )
	{
		HTREEITEM hChildItem = pCtrl->GetChildItem( hItem );
		HTREEITEM hNextVisibleItem = pCtrl->GetNextVisibleItem( hItem );
		if( hChildItem == hNextVisibleItem )
			acedRetT();
	}
	return RSRSLT;
}
