// Methods_BlockView.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BlockView.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ArxBlockViewCtrl.h"


static ADSRESULT DisplayDwgImp( bool bToScale );
static ADSRESULT DisplayBlockImp( bool bToScale );


ADSRESULT BlockView::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Clear();				
	pCtrl->RedrawWindow();

	acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::SetHighlight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	COLORREF clrHighlight;
	if( !GetColorArgument( pArgs, clrHighlight ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetHighlight( clrHighlight );

	acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::RemoveHighlight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->RemoveHighlight();

	acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::Zoom()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	double dblZoomFactor;
	if( !GetDoubleArgument( pArgs, dblZoomFactor ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Zoom(dblZoomFactor);

	acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::PreLoadDwg()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	CString sFileName;
	if( !GetStringArgument( pArgs, sFileName ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Clear();				
	pCtrl->RedrawWindow();
	if( pCtrl->PreLoadDwg( sFileName ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::GetBlockSize()	
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	CString sBlockName;
	if( !GetStringArgument( pArgs, sBlockName ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	AcDbExtents ext;
	if( !pCtrl->GetBlockSize( sBlockName, ext ) )
		return RSRSLT;
	AcGePoint3d ptMin = ext.minPoint();
	AcGePoint3d ptMax = ext.maxPoint();
	resbuf rbHeight = { NULL, RTREAL };
	rbHeight.resval.rreal = ptMax.y - ptMin.y;
	resbuf rbWidth = { &rbHeight, RTREAL };
	rbWidth.resval.rreal = ptMax.x - ptMin.x;
	acedRetList( &rbWidth );
	return RSRSLT;
}

ADSRESULT BlockView::GetDwgSize()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	AcDbExtents ext;
	if( !pCtrl->GetDwgSize( ext ) )
		return RSRSLT;
	AcGePoint3d ptMin = ext.minPoint();
	AcGePoint3d ptMax = ext.maxPoint();
	resbuf rbHeight = { NULL, RTREAL };
	rbHeight.resval.rreal = ptMax.y - ptMin.y;
	resbuf rbWidth = { &rbHeight, RTREAL };
	rbWidth.resval.rreal = ptMax.x - ptMin.x;
	acedRetList( &rbWidth );
	return RSRSLT;
}


ADSRESULT BlockView::GetBlockList()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	AcDbDatabase* pDb = pCtrl->GetSourceDb();
	if( !pDb )
		pDb = acdbCurDwg();
	if( !pDb )
		return RSRSLT;
	AcDbBlockTable* pBlockTable;
	if( Acad::eOk != pDb->getBlockTable( pBlockTable, AcDb::kForRead ) )
		return RSRSLT;

	AcDbBlockTableIterator* pIter;
	Acad::ErrorStatus es = pBlockTable->newIterator( pIter );
	pBlockTable->close();
	if( es != Acad::eOk )
		return RSRSLT;

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	for( pIter->start(); !pIter->done(); pIter->step() )
	{
		AcDbBlockTableRecord* pBTR;
		Acad::ErrorStatus es = pIter->getRecord( pBTR, AcDb::kForRead );
		if( es != Acad::eOk )
			continue;
		const ACHAR* pszBlockName = NULL;
		if( Acad::eOk == pBTR->getName( pszBlockName ) )
		{
			if( prbTail )
			{
				prbTail->rbnext = acutNewRb( RTSTR );
				prbTail = prbTail->rbnext;
			}
			else
			{
				prbTail = acutNewRb( RTSTR );
				prbResult = prbTail;
			}
			acutNewString( pszBlockName, prbTail->resval.rstring );
		}
		pBTR->close();
	}
	delete pIter;
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT DisplayDwgImp( bool bToScale )
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	CString sFileName;
	if( !GetStringArgument( pArgs, sFileName ) )
		return RSERR; //invalid input

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();

	if( !pArgs )
	{
		if( pCtrl->DisplayDwg( sFileName ) )
			acedRetT();
		return RSRSLT;
	}

	int nPresetView = 0;
	if( GetIntArgument( pArgs, nPresetView, true ) )
	{
		double dblZoom = 0;
		GetDoubleArgument( pArgs, dblZoom, true );
		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		bool bZoomExtents = (dblZoom == 0);
		if( bZoomExtents )
			dblZoom = 1.0;
		int nScaleType = (bToScale? 1 : 0);
		bool bSuccess = false;
		switch( nPresetView )
		{
		case 0: // top
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d::kZAxis );
			break;
		case 1: // bottom
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, -AcGeVector3d::kZAxis );
			break;
		case 2: // right
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d::kXAxis );
			break;
		case 3: // left
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, -AcGeVector3d::kXAxis );
			break;
		case 4: // front
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, -AcGeVector3d::kYAxis );
			break;
		case 5: // back
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d::kYAxis );
			break;
		case 6: // SW
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( -1.0, -1.0, 1.0 ) );
			break;
		case 7: // SE
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( 1.0, -1.0, 1.0 ) );
			break;
		case 8: // NW
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( -1.0, 1.0, 1.0 ) );
			break;
		case 9: // NE
			bSuccess = pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( 1.0, 1.0, 1.0 ) );
			break;
		default:
			HandleArgValueError( pArgs );
			return RSERR;
		}
		if( bSuccess )
			acedRetT();
		return RSRSLT;
	}

	double dblZoom = 0;
	double dblCameraX = 0;
	double dblCameraY = 0;
	double dblCameraZ = 0;
	if( !GetDoubleArgument( pArgs, dblCameraX ) )
		return RSERR;
	if( !GetDoubleArgument( pArgs, dblCameraY ) )
		return RSERR;
	if( !GetDoubleArgument( pArgs, dblCameraZ ) )
		return RSERR;
	if( !GetDoubleArgument( pArgs, dblZoom ) )
	{
		dblZoom = dblCameraZ;
		dblCameraZ = 0;
	}
	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	bool bZoomExtents = (dblZoom == 0);
	if( bZoomExtents )
		dblZoom = 1.0;
	int nScaleType = (bToScale? 1 : 0);
	if( pCtrl->DisplayDwg( sFileName, dblZoom, bZoomExtents, nScaleType,
												 AcGeVector3d( dblCameraX, dblCameraY, dblCameraZ ) ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::DisplayDwg()
{
	return DisplayDwgImp( false );
}

ADSRESULT BlockView::DisplayDwgToScale()
{
	return DisplayDwgImp( true );
}

ADSRESULT DisplayBlockImp( bool bToScale )
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	CString sBlockName;
	if( !GetStringArgument( pArgs, sBlockName ) )
		return RSERR; //invalid input

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();

	if( !pArgs )
	{
		if( pCtrl->DisplayBlock( sBlockName ) )
			acedRetT();
		return RSRSLT;
	}

	int nPresetView = 0;
	if( GetIntArgument( pArgs, nPresetView, true ) )
	{
		double dblZoom = 0;
		GetDoubleArgument( pArgs, dblZoom, true );
		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		bool bZoomExtents = (dblZoom == 0);
		if( bZoomExtents )
			dblZoom = 1.0;
		int nScaleType = (bToScale? 1 : 0);
		bool bSuccess = false;
		switch( nPresetView )
		{
		case 0: // top
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d::kZAxis );
			break;
		case 1: // bottom
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, -AcGeVector3d::kZAxis );
			break;
		case 2: // right
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d::kXAxis );
			break;
		case 3: // left
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, -AcGeVector3d::kXAxis );
			break;
		case 4: // front
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, -AcGeVector3d::kYAxis );
			break;
		case 5: // back
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d::kYAxis );
			break;
		case 6: // SW
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( -1.0, -1.0, 1.0 ) );
			break;
		case 7: // SE
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( 1.0, -1.0, 1.0 ) );
			break;
		case 8: // NW
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( -1.0, 1.0, 1.0 ) );
			break;
		case 9: // NE
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, AcGeVector3d( 1.0, 1.0, 1.0 ) );
			break;
		default:
			HandleArgValueError( pArgs );
			return RSERR;
		}
		if( bSuccess )
			acedRetT();
		return RSRSLT;
	}

	double dblZoom = 0;
	double dblCameraX = 0;
	double dblCameraY = 0;
	double dblCameraZ = 0;
	if( !GetDoubleArgument( pArgs, dblCameraX ) )
		return RSERR;
	if( !GetDoubleArgument( pArgs, dblCameraY ) )
		return RSERR;
	if( !GetDoubleArgument( pArgs, dblCameraZ ) )
		return RSERR;
	if( !GetDoubleArgument( pArgs, dblZoom, true ) )
	{
		dblZoom = dblCameraZ;
		dblCameraZ = 0;
	}
	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	bool bZoomExtents = (dblZoom == 0);
	if( bZoomExtents )
		dblZoom = 1.0;
	int nScaleType = (bToScale? 1 : 0);
	if( pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType,
													 AcGeVector3d( dblCameraX, dblCameraY, dblCameraZ ) ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::DisplayBlock()
{
	return DisplayBlockImp( false );
}

ADSRESULT BlockView::DisplayBlockToScale()
{
	return DisplayBlockImp( true );
}


ADSRESULT BlockView::DisplayPaperSpace()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input
	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->DisplayBlock( ACDB_PAPER_SPACE, 1.0, false, 0, AcGeVector3d::kZAxis ) )
		acedRetT();
	return RSRSLT;
}


ADSRESULT BlockView::RefreshBlock()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input
	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockViewCtrl* pCtrl = (CArxBlockViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->RefreshBlock();
	acedRetT();
	return RSRSLT;
}
