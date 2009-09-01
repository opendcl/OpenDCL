// ArxControlDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlDropTarget.h"
#include "ArxControlServices.h"
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
		const CArxControlServices* pArxServices = mpDlgControl->GetArxServices();
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
			if( pArxServices )
				pArxServices->HandleEvent( sDropControlEvent, args_SSSP( sProject, sForm, sControl, point ) );
			return TRUE;
		}

		CString sDropAcadWndPointEvent = pDclControl->GetStringProperty( Prop::DragnDropFromOther );
		if( !sDropAcadWndPointEvent.IsEmpty() )
		{
			if( pArxServices )
				pArxServices->HandleEvent( sDropAcadWndPointEvent, args_P( point ) );
			return TRUE;
		}
	}

	return __super::OnDrop( pWnd, pDataObject, dropEffect, point );
}
