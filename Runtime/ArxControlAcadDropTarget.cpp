// ArxControlAcadDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlAcadDropTarget.h"
#include "DragDropService.h"
#include "DialogControl.h"
#include "DclFormObject.h"
#include "InvokeMethod.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlAcadDropTarget

CArxControlAcadDropTarget::CArxControlAcadDropTarget( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
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
	CString sDropOnAcadWndPointEvent =
		mpDlgControl->GetTemplate()->GetStringProperty( Prop::DragnDropToAutoCAD );
	if( !sDropOnAcadWndPointEvent.IsEmpty() )
	{
		acedDwgPoint dwgPt;
		int nViewport = acedGetWinNum( point.x, point.y );
		acedCoordFromPixelToWorld( nViewport, point, dwgPt );
		InvokeMethodPoint3DInt( sDropOnAcadWndPointEvent,
														dwgPt,
														nViewport,
														mpDlgControl->IsAsyncEvents() );
		return TRUE;
	}
	return FALSE;
}

DROPEFFECT CArxControlAcadDropTarget::OnDropEx( CWnd* pWnd, COleDataObject* pDataObject,
																								DROPEFFECT dropDefault, DROPEFFECT dropList,
																								CPoint point )
{
	CString sDropOnAcadWndPointEvent =
		mpDlgControl->GetTemplate()->GetStringProperty( Prop::DragnDropToAutoCAD );
	if( !sDropOnAcadWndPointEvent.IsEmpty() )
	{
		acedDwgPoint dwgPt;
		int nViewport = acedGetWinNum( point.x, point.y );
		acedCoordFromPixelToWorld( nViewport, point, dwgPt );
		InvokeMethodPoint3DInt( sDropOnAcadWndPointEvent,
														dwgPt,
														nViewport,
														mpDlgControl->IsAsyncEvents() );
		return DROPEFFECT_NONE;
	}
	return DROPEFFECT_NONE; //returning DROPEFFECT_NONE will cause OnDrop to be called instead
}
