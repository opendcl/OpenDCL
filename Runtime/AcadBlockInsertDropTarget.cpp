#include "StdAfx.h"
#include "AcadBlockInsertDropTarget.h"


CAcadBlockInsertDropTarget::CAcadBlockInsertDropTarget( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
, mptLastDrag( -1, -1 )
, mLastEffect( DROPEFFECT_NONE )
{
}

CAcadBlockInsertDropTarget::~CAcadBlockInsertDropTarget(void)
{
}

DROPEFFECT CAcadBlockInsertDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if( !pDataObject->IsDataAvailable( CF_TEXT ) )
		return DROPEFFECT_NONE;
	return OnDragOver( pWnd, pDataObject, dwKeyState, point );
}

DROPEFFECT CAcadBlockInsertDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CSize trackerSize;
	if( !pDataObject )
		return DROPEFFECT_NONE;
	//if( point == mptLastDrag )
	//	return DROPEFFECT_COPY;
	mptLastDrag = point;
	return DROPEFFECT_COPY;
}

BOOL CAcadBlockInsertDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if( !pDataObject )
		return FALSE;
	OnDragLeave(pWnd);
/*
	CRect rSize;
	int objType;
	if( !::PasteFromData( pDataObject, rSize, objType ) )
		return FALSE;
	if( rSize.IsRectEmpty() )
		return FALSE;
	//toggle lock mode (I'm not sure why, but I left it here just in case it has a purpose -- ORW [2007-09-03])
	{ CAutoDocWriteLock CurDocLock( AcApGetDatabase( (CView*)pWnd ) ); }
*/
	// Set Focus to AutoCAD because AutoCAD doesn't update its
	// display if it's not in focus.
	acedGetAcadFrame()->SetActiveWindow();

	return TRUE;
}

void CAcadBlockInsertDropTarget::OnDragLeave(CWnd* pWnd)
{
	//if( mLastEffect != DROPEFFECT_NONE)
	//{
	//	CDC* pDC = pWnd->GetDC();
	//	ASSERT(pDC);
	//	pDC->DrawFocusRect( CRect( mptLastDrag, mszTracker ) ); 
	//	pDC->DeleteDC();
	//	mLastEffect = DROPEFFECT_NONE;
	//}
}

DROPEFFECT CAcadBlockInsertDropTarget::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	return DROPEFFECT_NONE;
}
