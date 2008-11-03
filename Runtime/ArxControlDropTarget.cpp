// ArxControlDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlDropTarget.h"
#include "DragDropService.h"
#include "DialogControl.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlDropTarget

CArxControlDropTarget::CArxControlDropTarget( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
{
}

CArxControlDropTarget::~CArxControlDropTarget() {}

//
// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT CArxControlDropTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject,
																							 DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = mpDlgControl->OnDragEnter( point, pDataObject, dwKeyState );
	if( dwEffect != DROPEFFECT_NONE )
		return dwEffect;
	return DROPEFFECT_NONE;
	//return __super::OnDragEnter( pWnd, pDataObject, dwKeyState, point );
}

//
// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
//
void CArxControlDropTarget::OnDragLeave(CWnd* pWnd)
{
	mpDlgControl->OnDragLeave();
	//__super::OnDragLeave( pWnd );
}

// 
// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT CArxControlDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject,
																							DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = mpDlgControl->OnDragOver( point, pDataObject, dwKeyState );
	if( dwEffect != DROPEFFECT_NONE )
		return dwEffect;
	return DROPEFFECT_NONE;
	//return __super::OnDragOver( pWnd, pDataObject, dwKeyState, point );
}

// 
// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
//
BOOL CArxControlDropTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, 
																		DROPEFFECT dropEffect, CPoint point )
{
	if( mpDlgControl->OnDrop( point, pDataObject, dropEffect ) )
		return TRUE;
	TDclControlPtr pDclControl = mpDlgControl->GetTemplate();
	if( pDclControl->GetBooleanProperty( Prop::DragnDropAllowDrop ) )
	{
		CString sDropControlEvent = pDclControl->GetStringProperty( Prop::DragnDropFromControl );
		if( !sDropControlEvent.IsEmpty() && pDataObject->IsDataAvailable( CDragDropService::GetDclControlClipboardFormat() ) )
		{
			HGLOBAL hData = pDataObject->GetGlobalData( CDragDropService::GetDclControlClipboardFormat() );
			if( !hData )
				return FALSE;
			CDclControlObject* pSourceDclControl = *(CDclControlObject**)GlobalLock( hData );
			GlobalUnlock( hData );
			GlobalFree( hData );
			if( !pSourceDclControl )
				return FALSE;
			CString sProject = pSourceDclControl->GetOwnerProject()->GetKeyName();
			CString sForm = pSourceDclControl->GetOwnerForm()->GetKeyName();
			CString sControl;
			if( pSourceDclControl->GetType() != _CtlForm )
				sControl = pSourceDclControl->GetKeyName();
			InvokeMethod3StringsPoint( sDropControlEvent,
																 sProject,
																 sForm,
																 sControl,
																 point,
																 mpDlgControl->IsAsyncEvents() );
			return TRUE;
		}

		CString sDropAcadWndPointEvent = pDclControl->GetStringProperty( Prop::DragnDropFromOther );
		if( !sDropAcadWndPointEvent.IsEmpty() )
		{
			InvokeMethodPoint( sDropAcadWndPointEvent,
												 point,
												 mpDlgControl->IsAsyncEvents() );
			return TRUE;
		}
	}

	return __super::OnDrop( pWnd, pDataObject, dropEffect, point );
/*
	CWnd* pDwgView = acedGetAcadDwgView();

	bool bUseSendString = true;
	if (mpDclControl->GetOwnerForm()->GetType() == FrmFileDlg)
		bUseSendString = false;
	
	TDclControlPtr pControl = PasteArxObjectFromClipboard();
	if (mpDclControl->GetType() == CtlDwgList)
		CArxDwgListCtrl *pDwgList = (CArxDwgListCtrl*)pControl->GetWindow();
	
	if (mpDclControl->GetType() == CtlTree)
	{
		HTREEITEM	hitem;
		UINT		flags = 0;
		CString		sTreeItemKey;
		CImageTreeCtrl *pTree = (CImageTreeCtrl*)mpDclControl->GetWindow();

		if ((hitem = pTree->HitTest(point, &flags)) != NULL)
		{
			sTreeItemKey = pTree->GetItemKey(hitem);			
		}
		
		if (pControl == NULL)
		{
			if (sTreeItemKey.GetLength() > 0)
			{
				// From AcadDwgView
				InvokeMethodLong(
					mpDclControl->GetStringProperty(Prop::DragnDropFromOther),
					(DWORD_PTR)hitem,
					(mpDclControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
			else
			{
				// From AcadDwgView
				InvokeMethodString(
					mpDclControl->GetStringProperty(Prop::DragnDropFromOther),
					sTreeItemKey,
					(mpDclControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
		}
		else
		{
			if (sTreeItemKey.GetLength() == 0)
			{
				// From Control
				InvokeMethod3StringsLong(
					mpDclControl->GetStringProperty(Prop::DragnDropFromControl), 
					pControl->GetOwnerProject()->GetKeyName(),
					pControl->GetOwnerForm()->GetKeyName(),
					pControl->GetStringProperty(Prop::Name),
					(DWORD_PTR)hitem,
					(mpDclControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
			else
			{
				// From Control
				InvokeMethod4Strings(
					mpDclControl->GetStringProperty(Prop::DragnDropFromControl), 
					pControl->GetOwnerProject()->GetKeyName(),
					pControl->GetOwnerForm()->GetKeyName(),
					pControl->GetStringProperty(Prop::Name),
					sTreeItemKey,
					(mpDclControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
		}
	}
	else
	{
		
		if (pControl == NULL)
		{
			// From AcadDwgView
			InvokeMethodPoint(
				mpDclControl->GetStringProperty(Prop::DragnDropFromOther),
				point,
				(mpDclControl->GetLongProperty(Prop::EventInvoke) == 1));
		}
		else
		{
			// From Control
			InvokeMethod3StringsPoint(
				mpDclControl->GetStringProperty(Prop::DragnDropFromControl), 
				pControl->GetOwnerProject()->GetKeyName(),
				pControl->GetOwnerForm()->GetKeyName(),
				pControl->GetStringProperty(Prop::Name),
				point,
				(mpDclControl->GetLongProperty(Prop::EventInvoke) == 1));
		}
	}
	
	if (pControl != NULL)
		pControl = NULL; // we must delete the arx object now!!!

	return TRUE;
*/
}

/*
TDclControlPtr  CArxControlDropTarget::PasteArxObjectFromClipboard()
{
	TDclControlPtr pControl = NULL; 
			
	// CG: This block was added by the Clipboard Assistant component
	if (mpParentWnd->OpenClipboard())
	{
		UINT m_nClipboardFormat = RegisterClipboardFormat(formatname);
		HANDLE hData = ::GetClipboardData(m_nClipboardFormat);
		if (hData != NULL)
		{
			pControl = new CDclControlObject(mpDclControl->GetOwnerForm()); 
			CSharedFile memFile;
			memFile.SetHandle(hData,FALSE);
			CArchive ar(&memFile, CArchive::load);
			
			// Serialize data to document
			pControl->Serialize(ar);
			ar.Close();
			
			//Resolution for CSharedFile::Detach() problem
			//Check <LINK TYPE="ARTICLE" VALUE="Q148455">Q148455</LINK> for current status.
			// _MFC_VER might need to be updated.
			#if _MFC_VER <= 0x0420
				::GlobalUnlock(memFile.Detach());
			#else
				memFile.Detach();
			#endif
		}

		CloseClipboard();
	}
	else
		AfxMessageBox(_T("Error, cannot open clipboard."));
	
	ClearTheClipboard();

	return pControl;
}

void CArxControlDropTarget::ClearTheClipboard()
{
   // CG: This block was added by the Clipboard Assistant component
	CSharedFile memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);
	UINT m_nClipboardFormat = RegisterClipboardFormat(formatname);

	ar.Flush();
	HGLOBAL hData = memFile.Detach();

	//Resolution for CSharedFile::Detach() problem
	//Check <LINK TYPE="ARTICLE" VALUE="Q148455">Q148455</LINK> for current status.
	// _MFC_VER might need to be updated.
	#if _MFC_VER <= 0x0420
		::GlobalUnlock(hData);
	#endif

	if (mpParentWnd->OpenClipboard())
	{		
		::SetClipboardData(m_nClipboardFormat, NULL);
		CloseClipboard();
	}
	else
		AfxMessageBox(_T("Error, cannot open clipboard."));

	EmptyClipboard();
}
/*

void CopyArxObjectToClipboard(TDclControlPtr pControl)
{
   // CG: This block was added by the Clipboard Assistant component
	CSharedFile memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);
	UINT m_nClipboardFormat = RegisterClipboardFormat(formatname);

	// serialize data to archive object       
	pControl->Serialize(ar);

	ar.Flush();
	HGLOBAL hData = memFile.Detach();

	//Resolution for CSharedFile::Detach() problem
	//Check <LINK TYPE="ARTICLE" VALUE="Q148455">Q148455</LINK> for current status.
	// _MFC_VER might need to be updated.
	#if _MFC_VER <= 0x0420
		::GlobalUnlock(hData);
	#endif

	if (pControl->GetWindow()->OpenClipboard())
	{
		::SetClipboardData(m_nClipboardFormat, hData);
		CloseClipboard();
	}
	else
		AfxMessageBox(_T("Error, cannot open clipboard."));
}

DROPEFFECT BeginDragnDrop(TDclControlPtr pControl, CPoint point, bool bInvokeWithSendString)
{
	// copy this control's Arx control object to the clip board
	CopyArxObjectToClipboard(pControl);

	// call methods to invoke the event
	InvokeMethod(pControl->GetStringProperty(Prop::DragnDropBegin), bInvokeWithSendString);

	int objType = 0;
	COleDataSource *pSource = generateDataSource(objType, CRect(point, CSize(0,0)));
	
	CMyOverrideDropTarget myDT;
	CDropSource myDS;

	myDT.mpDclControl = pControl;
	// Start overriding AutoCAD's Droptarget
	if (!acedStartOverrideDropTarget(&myDT))
		TRACE("Error in overriding Custom drop target!\n");

	DROPEFFECT dwEffect = pSource->DoDragDrop( DROPEFFECT_MOVE | DROPEFFECT_COPY, &CRect(point, CSize(3,3)), &myDS);

	// End overriding AutoCAD default droptarget
	if (!acedEndOverrideDropTarget(&myDT))
		TRACE("Error in ending override drop target\n");
	
	pSource->Empty(); 
	delete pSource;
	return dwEffect;
}

DROPEFFECT BeginDragnDropToInsertBlocks(TDclControlPtr pControl, CPoint point, bool bInvokeWithSendString, CStringArray &BlockNames)
{
	// copy this control's Arx control object to the clip board
	CopyArxObjectToClipboard(pControl);

	// call methods to invoke the event
	InvokeMethod(pControl->GetStringProperty(Prop::DragnDropBegin), bInvokeWithSendString);

	int objType = 0;
	COleDataSource *pSource = generateDataSource(objType, CRect(point, CSize(0,0)));
	
	CMyOverrideDropTarget myDT;
	CDropSource myDS;

	myDT.m_pBlockNames = &BlockNames;

	myDT.mpDclControl = pControl;
	// Start overriding AutoCAD's Droptarget
	if (!acedStartOverrideDropTarget(&myDT))
		TRACE("Error in overriding Custom drop target!\n");

	DROPEFFECT dwEffect = pSource->DoDragDrop( DROPEFFECT_MOVE | DROPEFFECT_COPY, &CRect(point, CSize(3,3)), &myDS);

	// End overriding AutoCAD default droptarget
	if (!acedEndOverrideDropTarget(&myDT))
		TRACE("Error in ending override drop target\n");
	
	pSource->Empty(); 
	delete pSource;
	return dwEffect;
}

COleDataSource* generateDataSource(int objType, const CRect& rect)
{
	COleDataSource* pSource = new COleDataSource();
	HGLOBAL temp = GlobalAlloc(GHND, 64);
	char* data = (char*) GlobalLock(temp);
	_snprintf(data, 64,"%d %d %d %d %d", objType, rect.left, rect.top, rect.right, rect.bottom);
	GlobalUnlock(temp);
	pSource->CacheGlobalData(CF_TEXT, temp);
	return pSource;                               
}
*/
