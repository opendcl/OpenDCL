// ArxControlAcadDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlAcadDropTarget.h"
#include "ArxControlServices.h"
#include "DragDropService.h"
#include "DialogControl.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlAcadDropTarget

CArxControlAcadDropTarget::CArxControlAcadDropTarget( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
, mbDropExCalled( false )
{
}

CArxControlAcadDropTarget::~CArxControlAcadDropTarget() {}


//
// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT CArxControlAcadDropTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject,
																									 DWORD dwKeyState, CPoint point )
{
	return DROPEFFECT_COPY;
}

//
// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
//
void CArxControlAcadDropTarget::OnDragLeave(CWnd* pWnd)
{
}

// 
// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT CArxControlAcadDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject,
																									DWORD dwKeyState, CPoint point )
{
	return DROPEFFECT_COPY;
}

// 
// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
//
BOOL CArxControlAcadDropTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, 
																				DROPEFFECT dropEffect, CPoint point )
{
	if( mbDropExCalled )
		return FALSE;
	CString sDropOnAcadWndPointEvent =
		mpDlgControl->GetTemplate()->GetStringProperty( Prop::DragnDropToAutoCAD );
	const CArxControlServices* pArxServices = mpDlgControl->GetArxServices();
	if( pArxServices && !sDropOnAcadWndPointEvent.IsEmpty() )
	{
		acedDwgPoint dwgPt;
		int nViewport = acedGetWinNum( point.x, point.y );
		acedCoordFromPixelToWorld( nViewport, point, dwgPt );
		pArxServices->HandleEvent( sDropOnAcadWndPointEvent, args_P3N( dwgPt, nViewport ) );
		return TRUE;
	}
	return FALSE;
}

DROPEFFECT CArxControlAcadDropTarget::OnDropEx( CWnd* pWnd, COleDataObject* pDataObject,
																								DROPEFFECT dropDefault, DROPEFFECT dropList,
																								CPoint point )
{
	mbDropExCalled = true;
	CString sDropOnAcadWndPointEvent =
		mpDlgControl->GetTemplate()->GetStringProperty( Prop::DragnDropToAutoCAD );
	const CArxControlServices* pArxServices = mpDlgControl->GetArxServices();
	if( pArxServices && !sDropOnAcadWndPointEvent.IsEmpty() )
	{
		acedDwgPoint dwgPt;
		int nViewport = acedGetWinNum( point.x, point.y );
		acedCoordFromPixelToWorld( nViewport, point, dwgPt );
		pArxServices->HandleEvent( sDropOnAcadWndPointEvent, args_P3N( dwgPt, nViewport ) );
		return DROPEFFECT_COPY;
	}
	return DROPEFFECT_NONE; //returning DROPEFFECT_NONE will cause OnDrop to be called instead
}
