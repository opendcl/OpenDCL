#include "StdAfx.h"
#include "AcadBlockInsertDropTarget.h"
#include "AutoDocLock.h"


CAcadBlockInsertDropTarget::CAcadBlockInsertDropTarget( CDialogControl* pDlgControl )
: CArxControlAcadDropTarget( pDlgControl )
, mptLastDrag( -1, -1 )
, mLastEffect( DROPEFFECT_NONE )
{
}

CAcadBlockInsertDropTarget::~CAcadBlockInsertDropTarget(void)
{
}

UINT CAcadBlockInsertDropTarget::GetAcadBlockClipboardFormat()
{
	static const UINT CF_DclBlockRecId = RegisterClipboardFormat( _T("OpenDCL.DclBlockRecId") );
	return CF_DclBlockRecId;
}

BOOL CAcadBlockInsertDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	BOOL bHandled = __super::OnDrop( pWnd, pDataObject, dropEffect, point );
	if( bHandled )
		return bHandled;
	AcDbDatabase* pDb = AcApGetDatabase( (CView*)pWnd );
	if( !pDb )
		return bHandled;
	HGLOBAL hData = pDataObject->GetGlobalData( CF_TEXT );
	if( !hData )
		return bHandled;
	CString sBlockName( (char*)GlobalLock( hData ) );
	GlobalUnlock( hData );
	GlobalFree( hData );
	UINT CF_DclBlockRecId = CAcadBlockInsertDropTarget::GetAcadBlockClipboardFormat();
	hData = pDataObject->GetGlobalData( CF_DclBlockRecId );
	if( !hData )
		return bHandled;
	AcDbObjectId idBlock = *(AcDbObjectId*)GlobalLock( hData );
	GlobalUnlock( hData );
	GlobalFree( hData );
	AcDbDatabase* pSourceDb = idBlock.database();
	if( !pSourceDb )
		return bHandled;
	AcDbDatabase* pTempDb;
	if( Acad::eOk != pSourceDb->wblock( pTempDb, idBlock ) )
		return bHandled;
	CAutoDocWriteLock DocLock( pDb );
	AcDbObjectId idNewBlock;
	Acad::ErrorStatus es = pDb->insert( idNewBlock, sBlockName, pTempDb, false );
	delete pTempDb;
	if( es != Acad::eOk )
		return bHandled;
	int nViewport = acedGetWinNum( point.x, point.y );
	acedDwgPoint dwgPt;
	acedCoordFromPixelToWorld( nViewport, point, dwgPt );
	AcGePoint3d ptIns( dwgPt[X], dwgPt[Y], dwgPt[Z] );
	AcDbObjectId idSpace = pDb->currentSpaceId();
	AcDbBlockTableRecord* pBTR = NULL;
	es = acdbOpenObject( pBTR, idSpace, AcDb::kForWrite );
	if( es != Acad::eOk )
		return bHandled;
	AcDbBlockReference* pInsertEnt = new AcDbBlockReference( ptIns, idNewBlock );
	if( !pInsertEnt )
	{
		pBTR->close();
		return bHandled;
	}
	es = pBTR->appendAcDbEntity( pInsertEnt );
	pBTR->close();
	if( es != Acad::eOk )
	{
		delete pInsertEnt;
		return bHandled;
	}
	pInsertEnt->close();
	acedGetAcadFrame()->SetActiveWindow();
	return TRUE;
}

DROPEFFECT CAcadBlockInsertDropTarget::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	DROPEFFECT dwEffect = __super::OnDropEx( pWnd, pDataObject, dropDefault, dropList, point );
	if( dwEffect != DROPEFFECT_NONE )
		return dwEffect;
	AcDbDatabase* pDb = AcApGetDatabase( (CView*)pWnd );
	if( !pDb )
		return dwEffect;
	HGLOBAL hData = pDataObject->GetGlobalData( CF_TEXT );
	if( !hData )
		return dwEffect;
	CString sBlockName( (char*)GlobalLock( hData ) );
	GlobalUnlock( hData );
	GlobalFree( hData );
	UINT CF_DclBlockRecId = CAcadBlockInsertDropTarget::GetAcadBlockClipboardFormat();
	hData = pDataObject->GetGlobalData( CF_DclBlockRecId );
	if( !hData )
		return dwEffect;
	AcDbObjectId idBlock = *(AcDbObjectId*)GlobalLock( hData );
	GlobalUnlock( hData );
	GlobalFree( hData );
	AcDbDatabase* pSourceDb = idBlock.database();
	if( !pSourceDb )
		return dwEffect;
	AcDbDatabase* pTempDb;
	if( Acad::eOk != pSourceDb->wblock( pTempDb, idBlock ) )
		return dwEffect;
	CAutoDocWriteLock DocLock( pDb );
	AcDbObjectId idNewBlock;
	Acad::ErrorStatus es = pDb->insert( idNewBlock, sBlockName, pTempDb, false );
	delete pTempDb;
	if( es != Acad::eOk )
		return dwEffect;
	int nViewport = acedGetWinNum( point.x, point.y );
	acedDwgPoint dwgPt;
	acedCoordFromPixelToWorld( nViewport, point, dwgPt );
	AcGePoint3d ptIns( dwgPt[X], dwgPt[Y], dwgPt[Z] );
	AcDbObjectId idSpace = pDb->currentSpaceId();
	AcDbBlockTableRecord* pBTR = NULL;
	es = acdbOpenObject( pBTR, idSpace, AcDb::kForWrite );
	if( es != Acad::eOk )
		return dwEffect;
	AcDbBlockReference* pInsertEnt = new AcDbBlockReference( ptIns, idNewBlock );
	if( !pInsertEnt )
	{
		pBTR->close();
		return dwEffect;
	}
	es = pBTR->appendAcDbEntity( pInsertEnt );
	pBTR->close();
	if( es != Acad::eOk )
	{
		delete pInsertEnt;
		return dwEffect;
	}
	pInsertEnt->close();
	acedGetAcadFrame()->SetActiveWindow();
	return DROPEFFECT_COPY;
}
