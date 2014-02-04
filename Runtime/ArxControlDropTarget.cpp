// ArxControlDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlDropTarget.h"
#include "ArxControlServices.h"
#include "DragDropService.h"
#include "DialogControl.h"
#include "DclFormTemplate.h"
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
	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	const CArxControlServices* pArxServices = mpDlgControl->GetArxServices();
	if( pArxServices && pArxServices->HandleDragOverControl( pDataObject, dwKeyState, point, dwEffect ) )
		return dwEffect;
	dwEffect = mpDlgControl->OnDragOver( point, pDataObject, dwKeyState );
	return dwEffect;
	//return __super::OnDragOver( pWnd, pDataObject, dwKeyState, point );
}

// 
// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
//
BOOL CArxControlDropTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, 
																		DROPEFFECT dropEffect, CPoint point )
{
	OnDragLeave( pWnd ); //to make sure everything gets cleaned up
	const CArxControlServices* pArxServices = mpDlgControl->GetArxServices();
	if( pArxServices && pArxServices->HandleDropOnControl( pDataObject, dropEffect, point ) )
		return TRUE;
	if( mpDlgControl->OnDrop( point, pDataObject, dropEffect ) )
		return TRUE;
	return __super::OnDrop( pWnd, pDataObject, dropEffect, point );
}
