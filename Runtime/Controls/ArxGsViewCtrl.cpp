// ArxGsViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGsViewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "Resource.h"
#include "AcadBlockReactor.h"
#include "AutoDocLock.h"

const int nOrbitOffset = 20;
const int nOrbitQuadCircleDia = 19;
const TCHAR sModelSpace[] = _T("*Model_Space");


/////////////////////////////////////////////////////////////////////////////
// CArxGsViewCtrl

CArxGsViewCtrl::CArxGsViewCtrl( TDclControlPtr pTemplate,
																CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mDocReactor( this )
, mColorService( long(-19), long(-22) )
, mArxServices( this )
, mDragDropService( this )
, mpDevice( NULL )
, mpView( NULL )
, mpGhostModel( NULL )
, mpModel( NULL )
, mbModelCreated( false )
, mbZooming( false )
, mbPanning( false )
, mbOrbiting( false )
, mhSavedCursor( NULL )
, mhZoomCursor( NULL )
, mhPanCursor( NULL )
, mhCrossCursor( NULL )
, mhOrbitCursor( NULL )
, mhArrowCursor( NULL )
{
	m_bSelectedRect = false;
	m_dPreviousZoomScale = 1.0;
	m_bBeingLDblClicked = false;
	m_bBeingRDblClicked = false;
	m_FileName = CString();
	m_pBlockReactor = NULL;	
	m_pLoadedDwg = NULL;
  m_Line[0] = NULL;
  m_Line[1] = NULL;
  m_Line[2] = NULL;
  m_Line[3] = NULL;

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxGsViewCtrl::~CArxGsViewCtrl()
{
	erasePreview(); 
	clearAll();

	if( m_pBlockReactor )
	{
		delete m_pBlockReactor;
		acedEditor->removeReactor( m_pBlockReactor );
	}

	if( mhSavedCursor )
		DeleteObject( mhSavedCursor );
	
	if( mhZoomCursor )
		DeleteObject( mhZoomCursor );
	
	if( mhPanCursor )
		DeleteObject( mhPanCursor );
	
	if( mhCrossCursor )
		DeleteObject( mhCrossCursor );
	
	if( mhOrbitCursor )
		DeleteObject( mhOrbitCursor );
	
	if( mhArrowCursor )
		DeleteObject( mhArrowCursor );		
}

bool CArxGsViewCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	mhArrowCursor = LoadCursor( NULL, IDC_ARROW );
    
	//Load our special cursors
	HMODULE hRes = theWorkspace.GetLocalResourceModule();
	mhZoomCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_ZOOM) );
	mhPanCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_PAN) );
	mhCrossCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_CROSS) );
	mhOrbitCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_ORBIT) );

	// create and add the new reactor
	m_pBlockReactor = new CAcadBlockReactor( NULL, this );
	acedEditor->addReactor( m_pBlockReactor );

	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CArxGsViewCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::RenderMode:
		SetRenderMode( pProp->GetLongValue() );
		break;
	case Prop::BlockName:
		DisplayBlock( pProp->GetStringValue() );
		break;
	}
	return !bFailed;
}

void CArxGsViewCtrl::SetRenderMode( long lMode ) 
{
	AcGsView* pView = view();	

	if (pView == NULL)
		return;

	/*
	pView->setMode(AcGsView::kNone);
	return;
	*/
	switch( lMode )
	{
	case 0:
		pView->setMode(AcGsView::k2DOptimized);
		break;
	case 1:
		pView->setMode(AcGsView::kBoundingBox);
		break;
	case 2:
		pView->setMode(AcGsView::kWireframe);
		break;
    case 3:
		pView->setMode(AcGsView::kHiddenLine);
		break;
    case 4:
		pView->setMode(AcGsView::kFlatShaded);
		break;
    case 5:
		pView->setMode(AcGsView::kGouraudShaded);
		break;
    case 6:
		pView->setMode(AcGsView::kFlatShadedWithWireframe);
		break;
	case 7:
		pView->setMode(AcGsView::kGouraudShadedWithWireframe);
		break;
	}
}

void CArxGsViewCtrl::SetHighlight( const COLORREF& clrHighlight )
{
	m_bSelectedRect = true;
	m_HighlightColor = clrHighlight;		
	Invalidate();
}

void CArxGsViewCtrl::RemoveHighlight()
{
	m_bSelectedRect = false;
	Invalidate();
}

void CArxGsViewCtrl::DoHighLight(CDC *pdc)
{
	try
	{
	if (m_bSelectedRect && !mbOrbiting && !mbPanning)
	{
		CRect rcCell;
		GetClientRect (&rcCell);
		CSize szValue(1,1);
		rcCell.DeflateRect(szValue);
		
		CPen Pen(PS_SOLID, 1, m_HighlightColor);
		CPen* pOldPen = pdc->SelectObject(&Pen);

		pdc->MoveTo(1, 1);
		pdc->LineTo(rcCell.Width(), 1);		
		pdc->LineTo(rcCell.Width(), rcCell.Height());		
		pdc->LineTo(1, rcCell.Height());		
		pdc->LineTo(1, 1);		
		
		//pdc->Rectangle(rcCell);

		pdc->SelectObject(pOldPen);			
		Pen.DeleteObject();
	}
	}
	catch(...)
	{
		clearAll();
	}
}

void CArxGsViewCtrl::DrawOrbitCircles(CDC* pDC)
{
	try
	{
	if (mbPanning)
		return;
    
	if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) != 1 )
		return;

	CDC* pdc;
	if (pDC == NULL)
		pdc = GetDC();
	else
		pdc = pDC;
		
	CRect rcThis;
	GetWindowRect(&rcThis);
	ScreenToClient(rcThis);

	int nSmallerSize;

	if (rcThis.Width() < rcThis.Height())
		nSmallerSize = rcThis.Width() / 2;
	else		
		nSmallerSize = rcThis.Height() / 2;

	int cX = rcThis.left + (rcThis.Width() / 2);
	int cY = rcThis.top + (rcThis.Height() / 2);
	
	int sX = cX - nSmallerSize + nOrbitOffset;
	int sY = cY - nSmallerSize + nOrbitOffset;
	int eX = cX + nSmallerSize - nOrbitOffset;
	int eY = cY + nSmallerSize - nOrbitOffset;
	
	CPen Pen(PS_SOLID, 1, RGB(0,255,0));
	CPen* pOldPen = pdc->SelectObject(&Pen);

	// draw the main circle
	pdc->MoveTo(sX, sY);
	pdc->Arc(sX, sY, eX, eY, sX, cY, eX, cY);		
	pdc->Arc(sX, sY, eX, eY, eX, cY, sX, cY);		

	// draw the outer circles
	DrawOrbitQuadCircle(pdc, cX, sY);
	DrawOrbitQuadCircle(pdc, cX, eY);
	DrawOrbitQuadCircle(pdc, sX, cY);
	DrawOrbitQuadCircle(pdc, eX, cY);
	
	pdc->SelectObject(pOldPen);			
	Pen.DeleteObject();

	if (pDC == NULL)
		ReleaseDC(pdc);
	}
	catch(...)
	{
		clearAll();
	}	
}

void CArxGsViewCtrl::DrawOrbitQuadCircle(CDC *pdc, int nX, int nY)
{
	int sX = nX - (nOrbitQuadCircleDia / 2);
	int sY = nY - (nOrbitQuadCircleDia / 2);
	int eX = nX + (nOrbitQuadCircleDia / 2);
	int eY = nY + (nOrbitQuadCircleDia / 2);

	
	pdc->MoveTo(sX, sY);
	pdc->Arc(sX, sY, eX, eY, sX, nY, eX, nY);		
	pdc->Arc(sX, sY, eX, eY, eX, nY, sX, nY);		

}

// Erase view and delete model
void CArxGsViewCtrl::erasePreview()
{
	try 
	{
		if (mpView)
			mpView->eraseAll();
		if (mpModel)
		{  
			AcGsManager* pManager = acgsGetGsManager();
			if( pManager )
				pManager->destroyAutoCADModel (mpModel);
			mpModel = NULL;
		}
	}
	catch(...)
	{
	}
}

void CArxGsViewCtrl::setModel(AcGsModel* pModel)
{
    erasePreview();
    mpModel = pModel;
    mbModelCreated =false;
}

void CArxGsViewCtrl::clearAll()
{	
	m_BlockName.Empty();
	try
	{
		if (mpView)
		{
			//mpView->eraseAll(); //causes crash if displayed block's host drawing was closed
			if (mpDevice)
				mpDevice->erase(mpView);
		}
		AcGsManager* pManager = acgsGetGsManager();
		if( pManager )
		{
			AcGsClassFactory* pFactory = pManager->getGSClassFactory();
			if (pFactory)    
			{
				if (mpView)  
				{
					pFactory->deleteView(mpView);
					mpView = NULL;
				}
				if (mpGhostModel) 
				{
					pFactory->deleteModel(mpGhostModel);
					mpGhostModel = NULL;
				}
			}
			if (mpModel) 
			{
				if (mbModelCreated)
					pManager->destroyAutoCADModel(mpModel);
				mpModel = NULL;
			}
    
			if (mpDevice)    
			{
				pManager->destroyAutoCADDevice(mpDevice);
				mpDevice = NULL;
			}
		}    
	}
	catch(...)
	{
	}
}

void CArxGsViewCtrl::init(HMODULE hRes, bool bCreateModel)
{
  //Instantiate view, a device and a model object
  AcGsManager* pManager = acgsGetGsManager();
	assert( pManager != NULL );
	if( !pManager )
		return;
  AcGsClassFactory* pFactory = pManager->getGSClassFactory();
  //a device with standard autocad color palette
  mpDevice = pManager->createAutoCADDevice(m_hWnd);
	TPropertyPtr pAcadColor = mpTemplate->GetPropertyObject(Prop::BackgroundColor);
	if (pAcadColor)
	{
		AcGsColor color = mpDevice->getBackgroundColor();
		COLORREF aColor = GetRGBColor(pAcadColor->GetLongValue());
		//AcGsColor color;
		color.m_red = GetRValue(aColor);
		color.m_green = GetGValue(aColor);
		color.m_blue = GetBValue(aColor);
		mpDevice->setBackgroundColor(color);
	}	
      
  CRect rect;
  GetClientRect( &rect);
  mpDevice->onSize(rect.Width(), rect.Height());   
  //a simple view
  mpView = pFactory->createView();
  // m_pLog->WriteString("\r\ninit 17");
  if (bCreateModel)
  {
    //a model with open/close protocol
    mpModel = pManager->createAutoCADModel();
		mbModelCreated = true;
  }
  //another model without open/close for the orbit gadget
  mpGhostModel = pFactory->createModel(AcGsModel::kDirect, 0, 0,	0);
  mpView->add(&mOrbitGadget,mpGhostModel);
  mOrbitGadget.setGsView(mpView);
  mpDevice->add(mpView);

	// get the view port information - see parameter list
	ads_real height = 0.0, width = 0.0, viewTwist = 0.0;
	AcGePoint3d targetView;
	AcGeVector3d viewDir;
	// m_pLog->WriteString("\r\ninit 25");
	try
	{
		GetActiveViewPortInfo (height, width, targetView, viewDir, viewTwist, true);
	}
	catch(...)
	{
	}
	try
	{
		mpView->setView(targetView + viewDir, targetView,
			AcGeVector3d(0.0, 1.0, 0.0),  // upvector
			1.0, 1.0);
	}
	catch(...)
	{
	}
	/*
      mpView->setView(AcGePoint3d(0.0, 0.0, 1.0),
                     AcGePoint3d(0.0, 0.0,  0.0),
                     AcGeVector3d(0.0, 1.0,  0.0),
                     1.0, 1.0); 
				   */
}

void CArxGsViewCtrl::ResizeHatch() 
{
	if (m_Line[0] != NULL)
	{
		AcGePoint3d vertexPts[4];
		
		CRect rc;
		GetClientRect(&rc);
		
		int nTheWidth = rc.Width();
		int nTheHeight = rc.Height();
		
		rc.left -= nTheWidth*0.5;
		rc.right -= nTheWidth*0.5;
		rc.top -= nTheHeight*0.5;
		rc.bottom -= nTheHeight*0.5;
		
		vertexPts[0].set(rc.left, rc.top, 0.0);
		vertexPts[1].set(rc.right, rc.top, 0.0);
		vertexPts[2].set(rc.right, rc.bottom, 0.0);
		vertexPts[3].set(rc.left, rc.bottom, 0.0);

		AcDbLine *pLine;

		if (acdbOpenObject(pLine, m_Line[0], AcDb::kForWrite) == Acad::eOk)
		{
			pLine->setStartPoint(vertexPts[0]);
			pLine->setEndPoint(vertexPts[1]);
			pLine->close();			
		}

		if (acdbOpenObject(pLine, m_Line[1], AcDb::kForWrite) == Acad::eOk)
		{
			pLine->setStartPoint(vertexPts[1]);
			pLine->setEndPoint(vertexPts[2]);
			pLine->close();
		}

		if (acdbOpenObject(pLine, m_Line[2], AcDb::kForWrite) == Acad::eOk)
		{
			pLine->setStartPoint(vertexPts[2]);
			pLine->setEndPoint(vertexPts[3]);
			pLine->close();
		}

		if (acdbOpenObject(pLine, m_Line[3], AcDb::kForWrite) == Acad::eOk)
		{
			pLine->setStartPoint(vertexPts[3]);
			pLine->setEndPoint(vertexPts[0]);
			pLine->close();
		}

		AcDbHatch *pHatch;
		if (acdbOpenObject(pHatch, m_HatchId, AcDb::kForWrite) == Acad::eOk)
		{
			for (int i=0; i<pHatch->numLoops(); i++)
			{
				pHatch->removeLoopAt(0);
			}

			AcDbObjectIdArray dbObjIds; 
			dbObjIds.append(m_Line[0]);
			dbObjIds.append(m_Line[1]);
			dbObjIds.append(m_Line[2]);
			dbObjIds.append(m_Line[3]);

			pHatch->appendLoop(AcDbHatch::kDefault, dbObjIds); 
			
			pHatch->evaluateHatch(); 
			pHatch->close();
		}
	}
}

void CArxGsViewCtrl::Zoom(double dZfactor)
{
	mpView->zoom(dZfactor);
	Invalidate();
	if (GetFocus() == this)
	{
		CDC *pdc = GetDC();
		// draw the solid rectangle
		pdc->DrawFocusRect(m_rcFocus);	
		ReleaseDC(pdc);
	}
}

bool CArxGsViewCtrl::PreLoadDwg(CString sFileName)
{
	try
	{
		CString sPath = theWorkspace.FindFile( sFileName ); 
		if( sPath.IsEmpty() )
		{				
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
			acedAlert(sLoadString + sFileName);
			return FALSE;
		}

		clearAll();

		// if a drawing has been previously loaded
		if (m_pLoadedDwg)
		{
			// delete it.
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}
			m_pLoadedDwg = NULL;
			m_BlockName.Empty();
			m_FileName.Empty();
		}

		// set the default block name
		m_BlockName = sModelSpace;

		Acad::ErrorStatus es;


		// This database is used to open the user specified file into if it exists
		m_pLoadedDwg = new AcDbDatabase(false, true);
		
		// Try to open the user specified file
		if (Acad::eOk != (es = m_pLoadedDwg->readDwgFile(sPath,_SH_DENYNO,false)))
		{
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
			acedAlert(sLoadString + sFileName);
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}					
			m_pLoadedDwg = NULL;
			return false;
		}
		// set the file name for future use
		m_FileName = sPath;
		
		if (m_pLoadedDwg==NULL)
		{
			return false;			
		}
		m_pLoadedDwg->closeInput( true );
	}
	catch(...)
	{
	}
	return true;			
		
}

bool CArxGsViewCtrl::LoadPreviewDwg(CString sFileName)
{
	return LoadPreviewDwg(sFileName, 1.0, true, 1, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
}

bool CArxGsViewCtrl::LoadPreviewDwg(
		CString sFileName, 
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ)
{
	AcDbExtents extents;

	try
	{
		CString sPath = theWorkspace.FindFile( sFileName ); 
		if( sPath.IsEmpty() )
		{				
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
			acedAlert(sLoadString + sFileName);
			return FALSE;
		}

		try
		{
			clearAll();
		}
		catch(...)
		{
		}

		// m_pLog->WriteString("\r\nLoadDwg 1a");	
		// if a drawing has been previously loaded
		if (m_pLoadedDwg != NULL)
		{
			// m_pLog->WriteString("\r\nLoadDwg 2");
			// delete it.
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}
			// m_pLog->WriteString("\r\nLoadDwg 3");
			m_pLoadedDwg = NULL;
			m_BlockName.Empty();
			m_FileName.Empty();
		}
		// m_pLog->WriteString("\r\nLoadDwg 1b");
		m_BlockName = sModelSpace;
		// m_pLog->WriteString("\r\nLoadDwg 1c");
		AcDbBlockTableRecord *pRec = NULL;
		// m_pLog->WriteString("\r\nLoadDwg 1d");
		AcDbBlockTable *pTab = NULL;
		// m_pLog->WriteString("\r\nLoadDwg 1e");
		Acad::ErrorStatus es;
		// m_pLog->WriteString("\r\nLoadDwg 4");
		// if a drawing has been previously loaded
		if (m_pLoadedDwg)
		{
			// m_pLog->WriteString("\r\nLoadDwg 5");
			// delete it.
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}
			// m_pLog->WriteString("\r\nLoadDwg 6");
			m_pLoadedDwg = NULL;
			m_BlockName.Empty();
		}

		try
		{
			// m_pLog->WriteString("\r\nLoadDwg 7");
			// This database is used to open the user specified file into if it exists
			m_pLoadedDwg = new AcDbDatabase(false, true);
			// m_pLog->WriteString("\r\nLoadDwg 8");
			es = m_pLoadedDwg->readDwgFile(sPath, _SH_DENYNO);
// m_pLog->WriteString("\r\nLoadDwg 9");
			// Try to open the user specified file
			if (es != Acad::eOk)
			{
				CString sLoadString;
				sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
				acedAlert(sLoadString + sFileName);
				try
				{
					delete m_pLoadedDwg;
				}
				catch(...)
				{
				}
				
				m_pLoadedDwg = NULL;
				return FALSE;
			}
			// m_pLog->WriteString("\r\nLoadDwg 11");
				
		}
		catch(...)
		{
		}
	
		// m_pLog->WriteString("\r\nLoadDwg 12");
				
		// set the file name to "" so that the block view control does not need to be reset when using the next DisplayBlock.
		m_FileName.Empty();
		
		if (m_pLoadedDwg==NULL)
		{
			m_BlockName.Empty();
			return FALSE;			
		}
		m_pLoadedDwg->closeInput( true );
// m_pLog->WriteString("\r\nLoadDwg 13");
				
		if ((es = m_pLoadedDwg->getBlockTable(pTab,AcDb::kForRead))!=Acad::eOk)    
		{
			// delete it.
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}			
			m_pLoadedDwg = NULL;
			m_BlockName.Empty();
			return FALSE;
		}
// m_pLog->WriteString("\r\nLoadDwg 14");
				
		if ((es = pTab->getAt(m_BlockName,pRec,AcDb::kForRead))!=Acad::eOk)
		{
		    pTab->close();			
		   // delete it.
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}
			m_pLoadedDwg = NULL;
			m_BlockName.Empty();
		    return FALSE;
		}
		pTab->close();
 //// m_pLog->WriteString("\r\nLoadDwg 15");
				
		DisplayTheBlock(
			pRec,		
			dZoomFactor, 
			bZoomExtents,
			nScaleType,
			dVectorX, 
			dVectorY, 
			dVectorZ,
			dCameraX, 
			dCameraY, 
			dCameraZ);	
	
	}
	catch(...)
	{
	}

	return TRUE; 
}

bool CArxGsViewCtrl::GetDwgSize( AcDbExtents& ext )
{
	if( !m_pLoadedDwg )
		return false;
	AcGePoint3d ptMin = m_pLoadedDwg->extmin();
	AcGePoint3d ptMax = m_pLoadedDwg->extmax();
	ext.set( ptMin, ptMax );
	return true;
}

bool CArxGsViewCtrl::GetBlockSize( LPCTSTR pszBlockName, AcDbExtents& ext )
{
	if( !pszBlockName || !*pszBlockName )
		return false;
	AcDbDatabase* pDb = (m_pLoadedDwg? m_pLoadedDwg : acdbCurDwg());
	if( !pDb )
		return false;
  AcDbBlockTable* pBlockTable;
	if( Acad::eOk != pDb->getBlockTable( pBlockTable, AcDb::kForRead ) )
		return false;

	AcDbBlockTableRecord* pBTR;
	Acad::ErrorStatus es = pBlockTable->getAt( pszBlockName, pBTR, AcDb::kForRead );
	pBlockTable->close();
	if( es != Acad::eOk )
		return false;
	es = ext.addBlockExt( pBTR );
	pBTR->close();
	if( es != Acad::eOk )
		return false;
	return true;
}

BOOL CArxGsViewCtrl::DisplayBlock(CString sBlockName)
{
	return DisplayBlock(sBlockName, 1.0, true, 1, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
}

BOOL CArxGsViewCtrl::DisplayBlock(
		CString sBlockName, 
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ)
{
	clearAll();
	// if a drawing has been previously loaded
	if (m_pLoadedDwg && m_FileName == CString())
	{
		// delete it.
		try
		{
			delete m_pLoadedDwg;
		}
		catch(...)
		{
		}
		m_pLoadedDwg = NULL;
	}
	m_BlockName = sBlockName;
    
	if (sBlockName.GetLength() == 0)
		return FALSE;
    AcDbBlockTableRecord *pRec;
    AcDbBlockTable *pTab;
    Acad::ErrorStatus es;
    AcDbDatabase* pDb;
	if (m_pLoadedDwg != NULL)
		pDb = m_pLoadedDwg;
	else
		pDb = acdbHostApplicationServices()->workingDatabase(); 

	if (pDb==NULL)
	{
		sBlockName.Empty();
		m_BlockName.Empty();
        return FALSE;
	}
	CAutoDocWriteLock CurDocLock;
	if ((es = pDb->getBlockTable(pTab,AcDb::kForRead)) !=Acad::eOk)    
	{
		sBlockName.Empty();
		m_BlockName.Empty();
		return FALSE;
	}
	if (!pTab->has(sBlockName))
	{
		pTab->close();
		sBlockName.Empty();
		m_BlockName.Empty();
		return FALSE;
	}
  if ((es = pTab->getAt(sBlockName,pRec,AcDb::kForWrite)) !=Acad::eOk)
  {
		sBlockName.Empty();
		m_BlockName.Empty();
		pTab->close();
		return FALSE;
	}
  pTab->close();
	DisplayTheBlock(
		pRec,		
		dZoomFactor, 
		bZoomExtents,
		nScaleType,
		dVectorX, 
		dVectorY, 
		dVectorZ,
		dCameraX, 
		dCameraY, 
		dCameraZ);
	return TRUE;
}

BOOL CArxGsViewCtrl::DisplayHatchPattern(CString sPattern)
{
	m_sHatchPattern = sPattern;

	AcDbBlockTable *pBlockTable = NULL;
	AcDbBlockTableRecord *pBlockRecord = NULL;
	
	m_pLoadedDwg = new AcDbDatabase(true, true);
	
	if (m_pLoadedDwg == NULL)
		return FALSE;
	
	Acad::ErrorStatus es = m_pLoadedDwg->getSymbolTable(pBlockTable,AcDb::kForRead);
	if( es != Acad::eOk )
	{
		delete m_pLoadedDwg;
		m_pLoadedDwg = NULL;
		return FALSE;
	}
	es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockRecord, AcDb::kForWrite);
	pBlockTable->close();
	if( es != Acad::eOk )
	{
		delete m_pLoadedDwg;
		m_pLoadedDwg = NULL;
		return FALSE;
	}


	// Construct database AcDbLines
    //
    AcGePoint3d vertexPts[4];
    AcDbObjectId lineId, cirId, hatchId;
    AcDbObjectIdArray dbObjIds;
    AcDbLine *line;

	
	int nTheWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nTheHeight =::GetSystemMetrics(SM_CYSCREEN);
	CRect rc(0,0,nTheWidth, nTheHeight);
		
	rc.left -= nTheWidth*0.5;
	rc.right -= nTheWidth*0.5;
	rc.top -= nTheHeight*0.5;
	rc.bottom -= nTheHeight*0.5;

    vertexPts[0].set(rc.left, rc.top, 0.0);
    vertexPts[1].set(rc.right, rc.top, 0.0);
    vertexPts[2].set(rc.right, rc.bottom, 0.0);
    vertexPts[3].set(rc.left, rc.bottom, 0.0);

	long lClr = mpTemplate->GetLongProperty(Prop::ForegroundColor);
	AcCmColor clr;

	if (lClr < 1 || lClr > 255)
	{
		COLORREF clrRef = GetRGBColor(lClr);
		clr.setColorMethod(AcCmEntityColor::kByColor);

		clr.setRGB(GetRValue(clrRef),
				   GetGValue(clrRef),
				   GetBValue(clrRef));
		
	}
  int i;
  for (i = 0; i < 4; i++) 
	{
    line = new AcDbLine();

		line->setStartPoint(vertexPts[i]) ;
        line->setEndPoint(vertexPts[(i == 3) ? 0 : i+1]) ;
		//line->setVisibility(AcDb::kInvisible);
		Acad::ErrorStatus es;
			
		if (lClr >= 1 && lClr <= 255)
			es = line->setColorIndex(lClr);
		else
			es = line->setColor(clr);

		es = pBlockRecord->appendAcDbEntity(lineId, line);
		if (es != Acad::eOk)
		{
			pBlockRecord->close();
			return 0;
		}
		m_Line[i] = lineId;
		line->close();

    dbObjIds.append(lineId);
	}

	double dblPatternScaleFactor = 1.0;
	AcDbDatabase* pCurDwg = acdbCurDwg();
	if( pCurDwg && pCurDwg->measurement() == AcDb::kEnglish )
		dblPatternScaleFactor = 25.4;

	AcDbHatch *pHatch = new AcDbHatch;
	AcGeVector3d normal(0.0, 0.0, 1.0);
	pHatch->setNormal(normal);
	pHatch->setElevation(0.0);
	pHatch->setPatternScale(dblPatternScaleFactor * mpTemplate->GetPropertyObject(Prop::HatchScale)->GetDoubleValue());
	pHatch->setPatternAngle(0.0);
	//pHatch->setAssociative(Adesk::kTrue);
	//pHatch->setHatchStyle(AcDbHatch::kNormal);
	pHatch->appendLoop(AcDbHatch::kExternal, dbObjIds);
	pHatch->setPattern(AcDbHatch::kPreDefined, sPattern);
	pHatch->evaluateHatch();
	if (lClr >= 1 && lClr <= 255)
		pHatch->setColorIndex(lClr);
	else
		pHatch->setColor(clr);
	
	es = pBlockRecord->appendAcDbEntity(hatchId, pHatch);
	pHatch->close();
	m_HatchId = hatchId;
	// Attach hatchId to all source boundary objects for notification.
    //
 //   AcDbEntity *pEnt;
 //   int numObjs = dbObjIds.length();
 //  
	//for (i = 0; i < numObjs; i++) {
 //       es = acdbOpenAcDbEntity(pEnt, dbObjIds[i], AcDb::kForWrite);
 //       if (es == Acad::eOk) {
 //           pEnt->addPersistentReactor(hatchId);
 //           pEnt->close();
 //       }
 //   }

	AcGePoint3d extMax = m_pLoadedDwg->extmax();
		
	//pBlockRecord->upgradeOpen();

	//

	m_BlockName = ACDB_MODEL_SPACE;
	pBlockRecord->close();
	if (acdbOpenObject(pBlockRecord, pBlockRecord->objectId(),  AcDb::kForRead) != Acad::eOk)
		return FALSE;

	DisplayTheBlock(
		pBlockRecord,		
		1.0, false, 1, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	pBlockRecord->close();

	return TRUE;
}

BOOL CArxGsViewCtrl::DisplayExternalBlock(
		CString sBlockName, 
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ)
{
	clearAll();
	

	m_BlockName = sBlockName;
    
	if (sBlockName.GetLength() == 0)
		return FALSE;

    AcDbBlockTableRecord *pRec;
    AcDbBlockTable *pTab;
    Acad::ErrorStatus es;
    AcDbDatabase* pDb;

	if (m_pLoadedDwg != NULL)
	{
		// delete it.
		pDb = m_pLoadedDwg;
	}
	else
		pDb = acdbHostApplicationServices()->workingDatabase(); 
    
	if (pDb==NULL)
	{
		sBlockName.Empty();
		m_BlockName.Empty();
        return FALSE;
	}
    
	if ((es = pDb->getBlockTable(pTab,AcDb::kForRead)) !=Acad::eOk)    
	{
		sBlockName.Empty();
		m_BlockName.Empty();
		return FALSE;
	}
	
	if (!pTab->has(sBlockName))
    {
        pTab->close();
	    sBlockName.Empty();
		m_BlockName.Empty();
        return FALSE;
    }

    if ((es = pTab->getAt(sBlockName,pRec,AcDb::kForRead)) !=Acad::eOk)
    {
		sBlockName.Empty();
		m_BlockName.Empty();
        pTab->close();
	    return FALSE;
    }
    pTab->close();	

	DisplayTheBlock(
		pRec,		
		dZoomFactor, 
		bZoomExtents,
		nScaleType,
		dVectorX, 
		dVectorY, 
		dVectorZ,
		dCameraX, 
		dCameraY, 
		dCameraZ);

	return TRUE;
}

void CArxGsViewCtrl::DisplayTheBlock(
		AcDbBlockTableRecord *pRec,
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,		
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ)
{
	//initialize the preview control
	init(theWorkspace.GetLocalResourceModule(),true);
	AcDbExtents extents;
	AcGePoint3d extMax;
	AcGePoint3d extMin;
	AcDbDatabase* pDb;

	/*
	if (sModelSpace == m_BlockName && m_pLoadedDwg != NULL)
	{
		//get the extension values for the model space block
		extMax = m_pLoadedDwg->extmax();
		extMin = m_pLoadedDwg->extmin();
		extents.addPoint(extMax);
		extents.addPoint(extMin);
	}
	else if (sModelSpace == m_BlockName && m_pLoadedDwg == NULL)
	{
		pDb = acdbHostApplicationServices()->workingDatabase(); 
		//get the extension values for the model space block
		extMax = pDb->extmax();
		extMin = pDb->extmin();
		extents.addPoint(extMax);
		extents.addPoint(extMin);
	}
	else
	{	*/
		// now create a way of looping through the entities in the block using iterators
		AcDbBlockTableRecordIterator *pBlockIterator;

		// loop thru them
		for (pRec->newIterator (pBlockIterator); !pBlockIterator->done(); pBlockIterator->step())
		{
			AcDbEntity *pEntity = NULL;  
			// open the entity for read
			if (pBlockIterator->getEntity (pEntity, AcDb::kForRead) != Acad::eOk)
				continue;

			AcDbExtents temp;
			// get the extents of the entity
			pEntity->getGeomExtents (temp);
			
			// sometimes mtext objects can be pressent and empty
			// if they are the wreck the extents calculation
			// so we must ignore empty MText entities.
			if (pEntity->isA() == AcDbMText::desc())
			{
				AcDbMText *pMText = (AcDbMText*)pEntity;
				CString sContents = pMText->contents();

				// if the Mext is empty, don't include it's extents.
				if (sContents.GetLength() == 0)
					continue;
			}

			// add this entity to the total extents
			extents.addExt (temp);

			// of course we must close it
			pEntity->close();
		}
	//}

	// Calculate the extents in WCS
	extMax = extents.maxPoint ();  
	extMin = extents.minPoint ();   

	// get the view port information - see parameter list
	ads_real height = 0.0, width = 0.0, viewTwist = 0.0;
	AcGePoint3d targetView(0,0,0);
	AcGeVector3d vCameraViewDir(dCameraX,dCameraY,dCameraZ);
	
	// we are only interested in the directions of the view, not the sizes, so we normalise. 
	vCameraViewDir = vCameraViewDir.normal();
	//**********************************************
	// Our view coordinate space consists of z direction 
	// get a perp vector off the z direction
	// Make sure its normalised
	AcGeVector3d viewXDir = vCameraViewDir.perpVector ().normal();
	
	// correct the x angle by applying the twist
	viewXDir = viewXDir.rotateBy (viewTwist, -vCameraViewDir);
	
	
	// now we can work out y, this is of course perp to the x and z directions. No need to normalise this, 
	// as we know that x and z are of unit length, and perpendicular, so their cross product must be on unit length
	AcGeVector3d viewYDir = vCameraViewDir.crossProduct (viewXDir);

	AcGeLineSeg3d calcLine(extMax, extMin);
	AcGePoint3d boxCenter = calcLine.midPoint();
	//**********************************************
	// create a transform from WCS to View space
	// this represents the transformation from WCS to the view space. (Actually not quite since 
	// we are keeping the fixed point as the center of the box for convenience )
	AcGeMatrix3d viewMat;
	viewMat = AcGeMatrix3d::alignCoordSys (boxCenter, AcGeVector3d::kXAxis, AcGeVector3d::kYAxis, AcGeVector3d::kZAxis,  
												boxCenter, viewXDir, viewYDir, vCameraViewDir).inverse();

	//AcDbExtents wcsExtents(extMin, extMax);
	// now we have the view Extents
	AcDbExtents viewExtents = extents;
	// transforms the extents in WCS->view space
	viewExtents.transformBy (viewMat);
	//**********************************************
	// get the extents of the AutoCAD view
	double xMax = fabs(viewExtents.maxPoint ().x - viewExtents.minPoint ().x);
	double yMax = fabs(viewExtents.maxPoint ().y - viewExtents.minPoint ().y);

	//**********************************************
	// setup the view
	AcGsView *pView = view();	
	AcGePoint3d eye = boxCenter + vCameraViewDir;

	if (yMax == 0.0)
		yMax = 0.00000000002;
	
	if (xMax == 0.0)
		xMax = 0.00000000002;

	if (mpTemplate->GetType() == CtlHatch)
	{
		struct resbuf   rb;
		acedGetVar(_T("CVPORT"), &rb);
		int nVPNum = rb.resval.rint;
		
		AcGsView::RenderMode eMode;

		eMode = AcGsView::k2DOptimized;
		
		AcGsView * pCurView  = acgsGetGsView (nVPNum, false);
		if( pCurView )
			pCurView->setMode(eMode);
	}

	try
	{
		pView->setView(eye, boxCenter, viewYDir, xMax , yMax );
	}
	catch(...)
	{
		/*identification number of the current viewport*/
		struct resbuf   rb;
		acedGetVar(_T("CVPORT"), &rb);
		int nVPNum = rb.resval.rint;
		acedGetVar(_T("TILEMODE"), &rb);
		// SHADEMODE is not available in non mspace layout viewports
		//
		AcGsView::RenderMode eMode;

		try
		{		
			if ((rb.resval.rint==0) && (nVPNum == 1))
			{
				eMode = AcGsView::k2DOptimized;
			}
			else
			{
				AcGsView * pCurView  = acgsGetGsView (nVPNum, false);
				//eMode  = (pCurView)? pCurView->mode(): AcGsView::k2DOptimized;
				eMode = AcGsView::kFlatShaded;
				pCurView->setMode(eMode);
			}
			pView->setMode(eMode);
			pView->setView(eye, boxCenter, viewYDir, xMax , yMax );
		}
		catch(...)
		{
			acedAlert(_T("An error occurred when trying to display the block."));
		}
	}

	// set the zoom to units per pixel as program requests
	if (!bZoomExtents)
	{
		if (nScaleType == 1)
		{
			CRect rect;
			GetClientRect(&rect);
			
			// get the current units per pixel			
			double dScale=xMax/rect.Width();
			if(dScale < yMax/rect.Height())
				dScale = yMax/rect.Height();
			
			// calc the scale factor to display the units per pixel as the program requests
			double x=dScale/dZoomFactor;		
			pView->zoom(x);	
		}
		else
		{
			pView->zoom(dZoomFactor);	
		}
	}
	SetRenderMode( mpTemplate->GetLongProperty( Prop::RenderMode ) );
	if (m_pLoadedDwg == NULL)
	{
		// remove all objects previously viewed
		pView->eraseAll();
	}

	// add the new object to the viewer
	m_pRec = pRec;
	pView->add(pRec,model()); 
	pRec->close();

	const ACHAR* pszName = NULL;
	pRec->getName( pszName );
	mpTemplate->SetStringProperty( Prop::BlockName, pszName );

	Invalidate();
	if (mpTemplate->GetBooleanProperty( Prop::ShowOrbitCircles ))
		DrawOrbitCircles();
}

void CArxGsViewCtrl::UpdateBlock()
{
	if (!m_hWnd)
		return;
	if (mpView == NULL)
		return;

	try
	{
		
		AcGePoint3d target = mpView->target();
		AcGePoint3d camera = mpView->position();
		AcGeVector3d upVector = mpView->upVector();
		double fieldHeight = mpView->fieldHeight();
		double fieldWidth = mpView->fieldWidth();
		//kParallel

		
		AcDbBlockTableRecord *pRec;
		AcDbBlockTable *pTab;
		Acad::ErrorStatus es;
		AcDbDatabase* pDb;

		if (m_pLoadedDwg != NULL)
		{
			// delete it.
			pDb = m_pLoadedDwg;
		}
		else
			pDb = acdbHostApplicationServices()->workingDatabase(); 
	    
		if (pDb==NULL)
			return;
	    
		if ((es = pDb->getBlockTable(pTab,AcDb::kForRead)) !=Acad::eOk)    
			return;
		
		if (!pTab->has(m_BlockName))
		{
			pTab->close();
			return;
		}

		if ((es = pTab->getAt(m_BlockName,pRec,AcDb::kForRead)) !=Acad::eOk)
		{
			pTab->close();
			return;
		}
		pTab->close();
		mpView->eraseAll();
		

		//pView->setView(eye, boxCenter, viewYDir, 
		//							xMax , yMax );
		
		//SetRenderMode();

		mpView->add(pRec,model()); 
		pRec->close();

		Invalidate();
		if (mpTemplate->GetBooleanProperty( Prop::ShowOrbitCircles ))
			DrawOrbitCircles();
	}
	catch(...)
	{
	}
}

// get the view port information - see parameter list
bool CArxGsViewCtrl::GetActiveViewPortInfo (ads_real &height, ads_real &width, AcGePoint3d &target, AcGeVector3d &vCameraViewDir, ads_real &viewTwist, bool getViewCenter)
{
	// make sure the active view port is uptodate
	/*
	try
	{
	acedVports2VportTableRecords();
	}
	catch(...)
	{
	}
	*/
	//acedVportTableRecords2Vports();
	// get the working db
  AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase(); 
	// if not ok
  if (pDb == NULL)
    return false;
	// open the view port records
	AcDbViewportTable *pVTable = NULL;
	Acad::ErrorStatus es = pDb->getViewportTable (pVTable, AcDb::kForRead);
	// if we opened them ok
	if (es == Acad::eOk)
	{
		AcDbViewportTableRecord *pViewPortRec = NULL;
		es = pVTable->getAt (_T("*Active"), pViewPortRec, AcDb::kForRead);
		if (es == Acad::eOk)
		{
			// get the height of the view
			height = pViewPortRec->height ();
			// get the width
			width = pViewPortRec->width ();
			// if the user wants the center of the viewport used
			if (getViewCenter == true)
			{
				struct resbuf rb;
				memset (&rb, 0, sizeof (struct resbuf));
				// get the system var VIEWCTR
				acedGetVar (_T("VIEWCTR"), &rb);
				// set that as the target
				target = AcGePoint3d (rb.resval.rpoint[X], rb.resval.rpoint[Y], rb.resval.rpoint[Z]);
			}
			// we want the viewports camera target setting
			else
			{
				// get the target of the view
				target = pViewPortRec->target ();
			}		

			// get the view direction
			vCameraViewDir = pViewPortRec->viewDirection ();
			// get the view twist of the viewport
			viewTwist = pViewPortRec->viewTwist ();
		}
		// close after use
		pVTable->close ();
		pViewPortRec->close();	
	}	
	
	return (true);
}


BEGIN_MESSAGE_MAP(CArxGsViewCtrl, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxGsViewCtrl message handlers

BOOL CArxGsViewCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( !mColorService.IsBackgroundTransparent() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		pDC->FillSolidRect( &rcClient, mColorService.GetBackgroundColor() );
	}
	return TRUE;
}

void CArxGsViewCtrl::OnPaint() 
{
  CPaintDC dc(this);

	if( m_BlockName.GetLength() > 0 || m_pLoadedDwg != NULL )
	{
		if( mpView )
		{
			try 
			{
				//update the gs view
				mpView->invalidate(); 
				mpView->update();
			}
			catch(...)
			{		
				clearAll();
			}
		}
		if( mbOrbiting || mpTemplate->GetBooleanProperty( Prop::ShowOrbitCircles ) )
			DrawOrbitCircles(&dc);
	}
	DoHighLight(&dc);
	if( GetFocus() == this && !mbOrbiting && !mbPanning )
		dc.DrawFocusRect( m_rcFocus );
}

void CArxGsViewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CRect rcThis;
	GetClientRect(&rcThis);

	// call the function to resize the hatch if one exists.
	//ResizeHatch();
	
	m_rcFocus.left = 2;
	m_rcFocus.top = 2;
	m_rcFocus.right = rcThis.Width() - 2,
	m_rcFocus.bottom = rcThis.Height() - 2;

    CRect rect;
    if (mpDevice) 
	{
        GetClientRect( &rect);
        mpDevice->onSize(rect.Width(), rect.Height());
    }
	if (m_BlockName.GetLength() > 0)
	{		
		if (mpView)
		{
			//update the gs view
			mpView->invalidate(); 
			mpView->update();
		}
		
		try{
			if (mpTemplate->GetBooleanProperty( Prop::ShowOrbitCircles ))
				DrawOrbitCircles();
		}
		catch(...)
		{
		}
		
	}
	else
	{
		CDC *pdc = GetDC();
		pdc->FillRect( rcThis, mColorService.GetBackgroundCBrush() );
		ReleaseDC(pdc);
	}
}

BOOL CArxGsViewCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 0 )
		return TRUE;

    if (mpView)
    {
        if (zDelta<0)
		{
            mpView->zoom(0.9);
			mdScale*=0.9;// mirror our own scale factor
		}
        else
		{
            mpView->zoom(1.1);
			mdScale*=1.1;// mirror our own scale factor
		}
        Invalidate();
		if (GetFocus() == this)
		{
			CDC *pdc = GetDC();
			// draw the solid rectangle
			pdc->DrawFocusRect(m_rcFocus);	
			ReleaseDC(pdc);
		}
    }
    return TRUE;
}

void CArxGsViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE && nFlags == MK_LBUTTON)
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
		if( dwDropEffect != DROPEFFECT_NONE )
			return;
	}

	SetFocus();
	if (m_BlockName.GetLength() > 0)
	{
		if (mpTemplate->GetType() == CtlHatch)
		{
			mbPanning = true;
			mbOrbiting = false;
			SetCapture();
			//set up the proper cursor
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			//::SetCursor(mhPanCursor);
			//store the start point
			mStartPt = point;		
			return;
		}
	
		// zoom mode
		if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 3 )
		{
			mbZooming = true;
			mbPanning = false;
			mbOrbiting = false;
			SetCapture();
			//set up the proper cursor
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhZoomCursor);
			//store the start point
			mStartPt = point;					
			// initialize the zoom scale factor
			m_dPreviousZoomScale = 1.0;
		}
		
		// pan mode
		if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 2 )
		{
			mbZooming = false;
			mbPanning = true;
			mbOrbiting = false;
			SetCapture();
			//set up the proper cursor
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhPanCursor);
			//store the start point
			mStartPt = point;
		}
		// orbit mode
		if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 1 )
		{
			//start orbit
			mbZooming = false;
			mbPanning = false;
			mbOrbiting = true;
			SetCapture();
			//set up the proper cursor
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhOrbitCursor);
			//store the start point
			mStartPt = point;
			return;
		}
	}
	else
	{
		::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
		::SetCursor(mhArrowCursor);	
	}
}

void CArxGsViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	SetFocus();

	
    //end orbit and pan
	mbOrbiting = false;
	mbZooming = false;
	mbPanning = false;
		
	// zoom mode
	switch (mpTemplate->GetLongProperty( Prop::InterfaceMode ))
	{
	case 2:
		{	
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhPanCursor);
			break;
		}
	case 3:
		{	
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhZoomCursor);
			break;
		}
	default:
		{	
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhArrowCursor);
			break;
		}
	}	

	ShowCursor(TRUE);
	Invalidate();
	ReleaseCapture();

	__super::OnLButtonUp(nFlags, point);

	if (m_bBeingLDblClicked)
	{
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());	
		m_bBeingLDblClicked = false;
	}

	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());	
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	if (!mpView || m_BlockName.GetLength() == 0)
	{
		return;
	}
}

void CArxGsViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
	if (mpTemplate->GetType() == CtlHatch)
	{
		//start pan
		mbPanning = true;
		SetCapture();
		//set up the proper cursor
		::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
		//::SetCursor(mhPanCursor);
		//store the start point
		mStartPt = point;
		return;
	}
	if (m_BlockName.GetLength() > 0 && 
		mpTemplate->GetLongProperty( Prop::InterfaceMode ) != 0)
	{
		//start pan
		mbPanning = true;
		SetCapture();
		//set up the proper cursor
		::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
		::SetCursor(mhPanCursor);
		//store the start point
		mStartPt = point;
	}
}

void CArxGsViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	SetFocus();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
    //end pan
	ReleaseCapture();
	mbPanning = false;
	//::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,(LONG_PTR)mhArrowCursor);
	::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
	::SetCursor(mhArrowCursor);
		
	ShowCursor(TRUE);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);
}

void CArxGsViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	try
	{
		if (mpTemplate->GetType() == CtlHatch)
		{
			mbOrbiting = false;
			mbPanning = false;
			//::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,(LONG_PTR)mhArrowCursor);
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			//::SetCursor(mhArrowCursor);
		
			
			ShowCursor(TRUE);
			//ReleaseCapture();
			return;
		}
		// static
		if (mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 0)
		{
			mbOrbiting = false;
			mbPanning = false;
			mbZooming = false;
			
			//::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,(LONG_PTR)mhArrowCursor);
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			::SetCursor(mhArrowCursor);
		
			
			ShowCursor(TRUE);
			
			return;
		}
		// zoom mode
		switch (mpTemplate->GetLongProperty( Prop::InterfaceMode ))
		{
		case 2:
			{	
				::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
				::SetCursor(mhPanCursor);
				ShowCursor(TRUE);
				break;
			}
		case 3:
			{	
				::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
				::SetCursor(mhZoomCursor);
				ShowCursor(TRUE);
				break;
			}
		default:
			{	
				::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
				::SetCursor(mhArrowCursor);
				ShowCursor(TRUE);
				//ReleaseCapture();		
				break;
			}
		}			
		/*
		if (mbZooming == false && mbPanning == false && mbOrbiting == false)
		{
			::SetClassLongPtr(m_hWnd,GCLP_HCURSOR,NULL);
			__super::SetCursor(mhArrowCursor);
			ShowCursor(TRUE);
			ReleaseCapture();
			return;
		}	
		*/
		if (mpView)
		{
			if (mbZooming)
			{
				CRect rcThis;
				GetClientRect(&rcThis);
				double zDelta = point.y - mStartPt.y;
				
				if (zDelta<0)
				{
					if (m_dPreviousZoomScale != 1.0)
					{
						double dZoomScale = 1.0 - m_dPreviousZoomScale;
						dZoomScale = 1.0 - dZoomScale;
						mpView->zoom(dZoomScale);			
					}
					zDelta = zDelta / rcThis.Height();
					m_dPreviousZoomScale = 1.0 - zDelta;
					mpView->zoom(m_dPreviousZoomScale);
				}
				else if (zDelta>0)
				{
					if (m_dPreviousZoomScale != 1.0)
					{
						double dZoomScale = 1.0 - m_dPreviousZoomScale;
						dZoomScale = 1.0 - dZoomScale;
						mpView->zoom(dZoomScale);			
					}
					zDelta = zDelta / rcThis.Height();
					m_dPreviousZoomScale = 1.0 - zDelta;
					mpView->zoom(m_dPreviousZoomScale);
				}
				
				mStartPt = point;
				
				if (zDelta != 0.0)
				{
					RedrawWindow();
				}
			}
    
		
			if (mbPanning)
			{
				//transform the point from device coordinates to
				//world coordinates
				AcGeVector3d pan_vec(-(point.x-mStartPt.x),point.y-mStartPt.y,0);
				pan_vec.transformBy(mpView->viewingMatrix() * mpView->worldToDeviceMatrix().inverse());
				mpView->dolly(pan_vec);
				RedrawWindow();
				mStartPt = point;
			}
			if (mbOrbiting)
			{
				const double Half_Pi =  1.570796326795;

				AcGsDCRect view_rect;
				mpView->getViewport (view_rect);

				int nViewportX = (view_rect.m_max.x - view_rect.m_min.x) + 1;
				int nViewportY = (view_rect.m_max.y - view_rect.m_min.y) + 1;

				int centerX = int(float(nViewportX) / 2.0f) + view_rect.m_min.x;
				int centerY = int(float(nViewportY) / 2.0f) + view_rect.m_min.y; 
      
				const double radius  = min (nViewportX, nViewportY) * 0.4f;

				// compute two vectors from last and new cursor positions:

				AcGeVector3d last_vector ((mStartPt.x - centerX) / radius,
										  -(mStartPt.y - centerY) / radius,
										  0.0);

				if (last_vector.lengthSqrd () > 1.0)     // outside the radius
					last_vector.normalize  ();
				else
					last_vector.z = sqrt (1.0 - last_vector.x * last_vector.x - last_vector.y * last_vector.y);

				AcGeVector3d new_vector((point.x - centerX) / radius,
										-(point.y - centerY) / radius,
										0.0);

				if (new_vector.lengthSqrd () > 1.0)     // outside the radius
					new_vector.normalize  ();
				else
					new_vector.z = sqrt (1.0 - new_vector.x * new_vector.x - new_vector.y * new_vector.y);

				// determine angles for proper sequence of camera manipulations:

				AcGeVector3d    rotation_vector (last_vector);
				rotation_vector = rotation_vector.crossProduct (new_vector);  // rotation_vector = last_vector x new_vector

				AcGeVector3d    work_vector (rotation_vector);
				work_vector.z = 0.0f;                      // projection of rotation_vector onto xy plane

				double          roll_angle      = atan2 (work_vector.x, 
														 work_vector.y);        // assuming that the camera's up vector is "up",
																				// this computes the angle between the up vector 
																				// and the work vector, which is the roll required
																				// to make the up vector coincident with the rotation_vector

				double length = rotation_vector.length ();
				double orbit_y_angle = (length != 0.0) ? acos (rotation_vector.z / length) + Half_Pi : Half_Pi;                   // represents inverse cosine of the dot product of the
				if (length > 1.0f)                                              // rotation_vector and the up_vector divided by the
					length = 1.0f;                                              // magnitude of both vectors.  We add pi/2 because we 
																				// are making the up-vector parallel to the the rotation
				double          rotation_angle  = asin (length);                // vector ... up-vector is perpin. to the eye-vector.

				// perform view manipulations

				mpView->roll( roll_angle);               // 1: roll camera to make up vector coincident with rotation vector
				mpView->orbit( 0.0f,  orbit_y_angle);     // 2: orbit along y to make up vector parallel with rotation vector
				mpView->orbit( rotation_angle, 0.0f);     // 3: orbit along x by rotation angle
				mpView->orbit( 0.0f, -orbit_y_angle);     // 4: orbit along y by the negation of 2
				mpView->roll(-roll_angle);               // 5: roll camera by the negation of 1
				
				RedrawWindow();
				mStartPt = point;
			}
		}
	}
	catch(...)
	{
	}
}

__UINT_LRESULT CArxGsViewCtrl::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

void CArxGsViewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CRect rcThis;
	GetClientRect(&rcThis);

	m_rcFocus.left = 2;
	m_rcFocus.top = 2;
	m_rcFocus.right = rcThis.Width() - 2,
	m_rcFocus.bottom = rcThis.Height() - 2;

	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);

	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxGsViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SetFocus();
			
	__super::OnLButtonDblClk(nFlags, point);

	m_bBeingLDblClicked = true;

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	// call methods to invoke the event
	//InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), false);//IsAsyncEvents());	
}

void CArxGsViewCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	SetFocus();
		
	__super::OnRButtonDblClk(nFlags, point);

	m_bBeingRDblClicked = true;

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	// call methods to invoke the event
	///InvokeMethod(mpTemplate->GetStringProperty(nEventRDblClick), false);//IsAsyncEvents());
}

void CArxGsViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	SetFocus();

	if (m_bBeingRDblClicked)
	{
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventRDblClick), IsAsyncEvents());	
		m_bBeingRDblClicked = false;
	}
	
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventRClick), IsAsyncEvents());	
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	
	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	__super::OnRButtonUp(nFlags, point);
}

void CArxGsViewCtrl::OnDestroy() 
{
	try
	{
		if (m_pLoadedDwg != NULL)
		{
			clearAll();
			m_BlockName.Empty();
			try
			{
				delete m_pLoadedDwg;
			}
			catch(...)
			{
			}
			m_pLoadedDwg = NULL;
		}

		if (m_pBlockReactor != NULL)
		{
			acedEditor->removeReactor(m_pBlockReactor);		
			delete m_pBlockReactor;
			m_pBlockReactor = NULL;
		}
	}
	catch(...)
	{
	}
	__super::OnDestroy();
}

void CArxGsViewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	ReleaseDC(pdc);
	
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxGsViewCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CArxGsViewCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxGsViewCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxGsViewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	__super::OnRButtonDown(nFlags, point);
}
