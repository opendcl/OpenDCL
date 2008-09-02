// Methods_BlockView.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BlockView.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ArxGsViewCtrl.h"


static ADSRESULT LoadDwgImp( bool bToScale );
static ADSRESULT DisplayBlockImp( bool bToScale );


ADSRESULT BlockView::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->clearAll();				
	pCtrl->m_BlockName.Empty();
	pCtrl->m_FileName.Empty();
	pCtrl->m_bSelectedRect = false;
	if( pCtrl->m_pLoadedDwg )
	{
		delete pCtrl->m_pLoadedDwg;
		pCtrl->m_pLoadedDwg = NULL;
	}
	pCtrl->RedrawWindow();

	acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::SetHighLight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	int nColor = 0;
	if( !GetIntArgument( pArgs, nColor ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetHighLight( nColor );

	acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::RemoveHighLight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->RemoveHighLight();

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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->clearAll();				
	pCtrl->m_BlockName.Empty();
	pCtrl->m_FileName.Empty();
	pCtrl->m_bSelectedRect = false;
	if( pCtrl->m_pLoadedDwg )
	{
		delete pCtrl->m_pLoadedDwg;
		pCtrl->m_pLoadedDwg = NULL;
	}
	pCtrl->RedrawWindow();
	if( !pCtrl->PreLoadDwg( sFileName ) )
		return RSRSLT;

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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	AcDbDatabase* pDb = (pCtrl->m_pLoadedDwg? pCtrl->m_pLoadedDwg : acdbCurDwg());
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

ADSRESULT LoadDwgImp( bool bToScale )
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockView ) )
		return RSERR; //invalid input

	CString sFileName;
	if( !GetStringArgument( pArgs, sFileName ) )
		return RSERR; //invalid input

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();

	if( !pArgs )
	{
		if( pCtrl->LoadPreviewDwg( sFileName ) )
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
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 );
			break;
		case 1: // bottom
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0 );
			break;
		case 2: // right
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0 );
			break;
		case 3: // left
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0 );
			break;
		case 4: // front
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 );
			break;
		case 5: // back
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 );
			break;
		case 6: // SW
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, -1.0, 1.0 );
			break;
		case 7: // SE
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, -1.0, 1.0 );
			break;
		case 8: // NW
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 1.0, 1.0 );
			break;
		case 9: // NE
			bSuccess = pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 );
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
	if( pCtrl->LoadPreviewDwg( sFileName, dblZoom, bZoomExtents, nScaleType,
														 0.0, 1.0, 0.0, dblCameraX, dblCameraY, dblCameraZ ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockView::LoadDwg()
{
	return LoadDwgImp( false );
}

ADSRESULT BlockView::LoadDwgToScale()
{
	return LoadDwgImp( true );
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();

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
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 );
			break;
		case 1: // bottom
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0 );
			break;
		case 2: // right
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0 );
			break;
		case 3: // left
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0 );
			break;
		case 4: // front
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 );
			break;
		case 5: // back
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 );
			break;
		case 6: // SW
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, -1.0, 1.0 );
			break;
		case 7: // SE
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, -1.0, 1.0 );
			break;
		case 8: // NW
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 1.0, 1.0 );
			break;
		case 9: // NE
			bSuccess = pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 );
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
	if( pCtrl->DisplayBlock( sBlockName, dblZoom, bZoomExtents, nScaleType,
													 0.0, 1.0, 0.0, dblCameraX, dblCameraY, dblCameraZ ) )
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->DisplayBlock( ACDB_PAPER_SPACE, 1.0, false, 0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 ) )
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->UpdateBlock();
	acedRetT();
	return RSRSLT;
}

/*
ADSRESULT BlockView::GetViewInfo()
{
	int nIndex = 0;
	CArxGsViewCtrl *pControl = (CArxGsViewCtrl*)GetControlPointer(CtlBlockView, sBlockView::GetViewInfo, &nIndex);
	
	if (pControl != NULL && pControl->view() != NULL)
	{
		AcGePoint3d camera = pControl->view()->position();
		AcGePoint3d target = pControl->view()->target();
		AcGeVector3d up = pControl->view()->upVector();

		int stat;
		struct resbuf *list;    
		
		int nPerspective;
		if (pControl->view()->isPerspective())
			nPerspective = RTT;
		else
			nPerspective = RTNIL;

		list = acutBuildList(
			RTLB,
				RTLB,
					RTREAL, target[X], 
					RTREAL, target[Y],
					RTREAL, target[Z],
				RTLE,
				RTLB,
					RTREAL, camera[X],
					RTREAL, camera[Y],
					RTREAL, camera[Z],
				RTLE,
				RTLB,
					RTREAL, up[X],
					RTREAL, up[Y],
					RTREAL, up[Z],
				RTLE,
				RTREAL, pControl->view()->fieldWidth(),
				RTREAL, pControl->view()->fieldHeight(),
				nPerspective,
			RTLE,
			RTNONE);

		if (list != NULL) { 	    
			stat = acedRetList(list);		
			acutRelRb(list); 
		} 
		return 0;
	}

	acedRetNil();
	return 0;
}


ADSRESULT BlockView::SetView()
{
	TDclControlPtr pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sBlockView::SetView, pArxObject);

	
	if (pArxObject == NULL)
	{
		acedRetVoid();
		return 0;
	}

	CArxGsViewCtrl *pControl = (CArxGsViewCtrl*)pArxObject->GetWindow();
	
	ListData = ListData->rbnext;
	
	AcGePoint3d camera;
	AcGePoint3d target;
	AcGeVector3d up;

	if (ListData->restype == RT3DPOINT)
	{		
		target[X] = ListData->resval.rpoint[X];
		target[Y] = ListData->resval.rpoint[Y];
		target[Z] = ListData->resval.rpoint[Z];
	}
	else
	{
		acedRetVoid();
		return 0;
	}
	ListData = ListData->rbnext;

	if (ListData->restype == RT3DPOINT)
	{
		camera[X] = ListData->resval.rpoint[X];
		camera[Y] = ListData->resval.rpoint[Y];
		camera[Z] = ListData->resval.rpoint[Z];
	}
	else
	{
		acedRetVoid();
		return 0;
	}

	ListData = ListData->rbnext;

	if (ListData->restype == RT3DPOINT)
	{
		up[X] = ListData->resval.rpoint[X];
		up[Y] = ListData->resval.rpoint[Y];
		up[Z] = ListData->resval.rpoint[Z];
	}
	else
	{
		acedRetVoid();
		return 0;
	}

	double fieldWidth;
	double fieldHeight;

	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		acedRetVoid();
		return 0;
	}
	GetLispRealInput(ListData, fieldWidth);

	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		acedRetVoid();
		return 0;
	}
	bool bIsPerspective;
	GetLispRealInput(ListData, fieldHeight);

	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		bIsPerspective = false;
	}
	else	
		GetLispBoolInput(ListData, bIsPerspective);

	if (pControl != NULL && pControl->view() != NULL)
	{
		AcGePoint3d camera = pControl->view()->position();
	}

	if (pControl->view() != NULL)
	{
		pControl->view()->setView(camera, target, up, fieldWidth, fieldHeight, (bIsPerspective ? AcGsView::kPerspective : AcGsView::kParallel));
		pControl->Invalidate();
	}
	
	acedRetVoid();
	return 0;
}
*/
