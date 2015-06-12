// ArxBlockViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxBlockViewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "Resource.h"
#include "AutoDocLock.h"
#include <math.h>

const int nOrbitOffset = 20;
const int nOrbitQuadCircleDia = 19;

#if (_ARXTARGET >= 20)
static AcGiVisualStyle::Type RenderModeToVisualStyle( long nRenderMode )
{
	switch( nRenderMode )
	{
	case 0: return AcGiVisualStyle::kFlat;
	case 1: return AcGiVisualStyle::kFlat;
	case 2: return AcGiVisualStyle::k3DWireframe;
	case 3: return AcGiVisualStyle::kHidden;
	case 4: return AcGiVisualStyle::kFlat;
	case 5: return AcGiVisualStyle::kGouraud;
	case 6: return AcGiVisualStyle::kFlatWithEdges;
	case 7: return AcGiVisualStyle::kGouraudWithEdges;
	};
	return AcGiVisualStyle::kFlat;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CArxBlockViewCtrl

CArxBlockViewCtrl::CArxBlockViewCtrl( TDclControlPtr pTemplate,
																			CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CArxGsViewCtrl( pTemplate, pPane, nID, false )
, mDocReactor( this )
, mEdReactor( this )
, mpSourceDb( NULL )
, mbPreloaded( false )
, mbZooming( false )
, mbPanning( false )
, mbOrbiting( false )
, mhSavedCursor( NULL )
, mhArrowCursor( NULL )
, mhZoomCursor( NULL )
, mhPanCursor( NULL )
, mhCrossCursor( NULL )
, mhOrbitCursor( NULL )
, mdScale( 1.0 )
, mdExtentsScale( 1.0 )
, mdPreviousZoomScale( 1.0 )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxBlockViewCtrl::~CArxBlockViewCtrl()
{
	if( mhSavedCursor )
		DeleteObject( mhSavedCursor );
	if( mhArrowCursor )
		DeleteObject( mhArrowCursor );		
	if( mhZoomCursor )
		DeleteObject( mhZoomCursor );
	if( mhPanCursor )
		DeleteObject( mhPanCursor );
	if( mhCrossCursor )
		DeleteObject( mhCrossCursor );
	if( mhOrbitCursor )
		DeleteObject( mhOrbitCursor );
	delete mpSourceDb;
}

bool CArxBlockViewCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	//Load our special cursors
	HMODULE hRes = theWorkspace.GetLocalResourceModule();
	mhArrowCursor = LoadCursor( NULL, IDC_ARROW );
	mhZoomCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_ZOOM) );
	mhPanCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_PAN) );
	mhCrossCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_CROSS) );
	mhOrbitCursor = LoadCursor( hRes, MAKEINTRESOURCE(IDI_ORBIT) );

	bool bSuccess = __super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxBlockViewCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::RenderMode:
		{
			AcGsView* pView = GetGsView();
			if( pView )
			{
			#if (_ARXTARGET >= 20)
				pView->setVisualStyle( AcGiVisualStyle( RenderModeToVisualStyle( pProp->GetLongValue() ) ) );
			#else
				pView->setMode( (AcGsView::RenderMode)pProp->GetLongValue() );
			#endif
			}
		}
		break;
	case Prop::BlockName:
		DisplayBlock( pProp->GetStringValue() );
		break;
	}
	return !bFailed;
}

DROPEFFECT CArxBlockViewCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwDropEffect = __super::OnBeginDrag( point, SourceData );
	AcDbDatabase* pDb = (mpSourceDb? mpSourceDb : acdbCurDwg());
	if( !pDb )
		return dwDropEffect;
	CString sBlockName = mpTemplate->GetStringProperty( Prop::BlockName );
	if (sBlockName.IsEmpty())
	{
		if (mpSourceDb)
			sBlockName = ACDB_MODEL_SPACE;
		else
			return dwDropEffect;
	}
	CStringA sTextA( sBlockName );
	SIZE_T cchText = sTextA.GetLength() + 1;
	HGLOBAL hData = GlobalAlloc( GHND, cchText );
	if( !hData )
		return dwDropEffect;
	lstrcpynA( (char*)GlobalLock( hData ), sTextA, cchText );
	GlobalUnlock( hData );
	SourceData.CacheGlobalData( CF_TEXT, hData );
	AcDbBlockTable* pBlockTable = NULL;
	if( Acad::eOk != pDb->getBlockTable( pBlockTable, AcDb::kForRead ) )
		return dwDropEffect;
	AcDbObjectId idBlock;
	Acad::ErrorStatus es = pBlockTable->getAt( sBlockName, idBlock );
	pBlockTable->close();
	if( es != Acad::eOk )
		return dwDropEffect;
	UINT CF_DclBlockRecId = CAcadBlockInsertDropTarget::GetAcadBlockClipboardFormat();
	HGLOBAL hDclBlockRecIdPtr = GlobalAlloc( GHND, sizeof(AcDbObjectId) );
	if( !hDclBlockRecIdPtr )
		return dwDropEffect;
	*(AcDbObjectId*)GlobalLock( hDclBlockRecIdPtr ) = idBlock;
	GlobalUnlock( hDclBlockRecIdPtr );
	SourceData.CacheGlobalData( CF_DclBlockRecId, hDclBlockRecIdPtr );
	return DROPEFFECT_COPY;
}

void CArxBlockViewCtrl::Clear()
{
	clearAll();
	if( mpSourceDb )
	{
		delete mpSourceDb;
		mpSourceDb = NULL;
		mbPreloaded = false;
	}
}

void CArxBlockViewCtrl::PaintUI( CDC* pdc /*= NULL*/ )
{
	if( mbOrbiting || mpTemplate->GetBooleanProperty( Prop::ShowOrbitCircles ) )
		DrawOrbitCircles( pdc );
}

void CArxBlockViewCtrl::AddUIDrawable( AcGsModel* pModel, AcGsView* pView )
{
	pView->add( &mOrbitGadget, pModel );
	mOrbitGadget.setGsView( pView );
}
#if (_ARXTARGET >= 20)
AcGiVisualStyle::Type CArxBlockViewCtrl::GetVisualStyle()
{
	return RenderModeToVisualStyle( mpTemplate->GetLongProperty( Prop::RenderMode ) );
}
#else
AcGsView::RenderMode CArxBlockViewCtrl::GetRenderMode()
{
	return (AcGsView::RenderMode)mpTemplate->GetLongProperty( Prop::RenderMode );
}
#endif

void CArxBlockViewCtrl::DrawOrbitCircles(CDC* pDC /*= NULL*/ )
{
	if( mbPanning )
		return;
		
	if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) != 1 )
		return;

	CDC* pdc = pDC? pDC : GetDC();
		
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

	if( pDC == NULL )
		ReleaseDC( pdc );
}

void CArxBlockViewCtrl::DrawOrbitQuadCircle(CDC *pdc, int nX, int nY)
{
	int sX = nX - (nOrbitQuadCircleDia / 2);
	int sY = nY - (nOrbitQuadCircleDia / 2);
	int eX = nX + (nOrbitQuadCircleDia / 2);
	int eY = nY + (nOrbitQuadCircleDia / 2);
	pdc->MoveTo(sX, sY);
	pdc->Arc(sX, sY, eX, eY, sX, nY, eX, nY);		
	pdc->Arc(sX, sY, eX, eY, eX, nY, sX, nY);		
}

bool CArxBlockViewCtrl::GetDwgSize( AcDbExtents& ext )
{
	AcDbDatabase* pDb = (mpSourceDb? mpSourceDb : acdbCurDwg());
	if( !pDb )
		return false;
	AcGePoint3d ptMin = pDb->extmin();
	AcGePoint3d ptMax = pDb->extmax();
	ext.set( ptMin, ptMax );
	return true;
}

bool CArxBlockViewCtrl::GetBlockSize( LPCTSTR pszBlockName, AcDbExtents& ext )
{
	if( !pszBlockName || !*pszBlockName )
		return false;
	AcDbDatabase* pDb = ((mbPreloaded && mpSourceDb)? mpSourceDb : acdbCurDwg());
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

void CArxBlockViewCtrl::RefreshBlock()
{
	if( !m_hWnd )
		return;

	AcGsView* pView = GetGsView();
	if( !pView )
		return;

	AcGePoint3d target = pView->target();
	AcGePoint3d camera = pView->position();
	AcGeVector3d upVector = pView->upVector();
	double fieldHeight = pView->fieldHeight();
	double fieldWidth = pView->fieldWidth();

	AcDbDatabase* pDb = (mpSourceDb? mpSourceDb : acdbCurDwg());
	if( !pDb )
		return;
 
	AcDbBlockTable* pTab = NULL;
	Acad::ErrorStatus es = pDb->getBlockTable( pTab, AcDb::kForRead );
	if( es != Acad::eOk )
		return;
	CString sBlockName = mpTemplate->GetStringProperty( Prop::BlockName );
	if (sBlockName.IsEmpty() && mpSourceDb)
		sBlockName = ACDB_MODEL_SPACE;
	AcDbBlockTableRecord* pRec = NULL;
	es = pTab->getAt( sBlockName, pRec, AcDb::kForRead );
	pTab->close();	
	if( es != Acad::eOk )
		return;
	UpdateModel( pRec );
	pRec->close();

	OnNeedRepaint( true );
}

bool CArxBlockViewCtrl::PreLoadDwg( LPCTSTR pszFilename )
{
	Clear();
	if( !pszFilename || !*pszFilename )
		return false;

	CString sPath = theWorkspace.FindFile( pszFilename ); 
	if( sPath.IsEmpty() )
	{				
		CString sMsg;
		sMsg.Format( theWorkspace.LoadResourceString( IDS_DWGNOTLOADING ), pszFilename );
		theWorkspace.DisplayAlert( sMsg );
		return false;
	}
	mpSourceDb = new AcDbDatabase(false, true);
	Acad::ErrorStatus es = mpSourceDb->readDwgFile( sPath, _SH_DENYNO, false );
	mpSourceDb->closeInput( true );
	if( es != Acad::eOk )
	{
		CString sMsg;
		sMsg.Format( theWorkspace.LoadResourceString( IDS_DWGNOTLOADING ), pszFilename );
		theWorkspace.DisplayAlert( sMsg );
		delete mpSourceDb;
		mpSourceDb = NULL;
		return false;
	}
	mbPreloaded = true;
	return true;			
}

bool CArxBlockViewCtrl::DisplayDwg( LPCTSTR pszFilename )
{
	return DisplayDwg( pszFilename, 1.0, true, 1, AcGeVector3d::kZAxis );
}

bool CArxBlockViewCtrl::DisplayDwg( LPCTSTR pszFilename, double dZoomFactor, bool bZoomExtents,
																		int nScaleType, const AcGeVector3d& vecViewDir )
{
	Clear();
	if( !pszFilename || !*pszFilename )
		return false;

	CString sPath = theWorkspace.FindFile( pszFilename ); 
	if( sPath.IsEmpty() )
	{				
		CString sMsg;
		sMsg.Format( theWorkspace.LoadResourceString( IDS_DWGNOTLOADING ), pszFilename );
		theWorkspace.DisplayAlert( sMsg );
		return false;
	}

	mpSourceDb = new AcDbDatabase(false, true);
	Acad::ErrorStatus es = mpSourceDb->readDwgFile( sPath, _SH_DENYNO, false );
	mpSourceDb->closeInput( true );
	if( es != Acad::eOk )
	{
		CString sMsg;
		sMsg.Format( theWorkspace.LoadResourceString( IDS_DWGNOTLOADING ), pszFilename );
		theWorkspace.DisplayAlert( sMsg );
		delete mpSourceDb;
		mpSourceDb = NULL;
		return false;
	}
			
	AcDbBlockTable* pBlockTable = NULL;
	es = mpSourceDb->getBlockTable( pBlockTable, AcDb::kForRead );
	if( es != Acad::eOk )    
	{
		delete mpSourceDb;
		mpSourceDb = NULL;
		return false;
	}

	AcDbBlockTableRecord* pModelSpace = NULL;
	es = pBlockTable->getAt( ACDB_MODEL_SPACE, pModelSpace, AcDb::kForRead );
	pBlockTable->close();
	if( es != Acad::eOk )
	{
		delete mpSourceDb;
		mpSourceDb = NULL;
		return false;
	}

	DisplayBTR( pModelSpace, dZoomFactor, bZoomExtents, nScaleType, vecViewDir );
	pModelSpace->close();

	return true; 
}

bool CArxBlockViewCtrl::DisplayBlock( LPCTSTR pszBlockName )
{
	return DisplayBlock( pszBlockName, 1.0, true, 1, AcGeVector3d::kZAxis );
}

bool CArxBlockViewCtrl::DisplayBlock( LPCTSTR pszBlockName, double dZoomFactor, bool bZoomExtents,
																			int nScaleType, const AcGeVector3d& vecViewDir )
{
	if( !pszBlockName || !*pszBlockName )
	{
		Clear();
		return false;
	}
	AcDbDatabase* pDb = ((mbPreloaded && mpSourceDb)? mpSourceDb : acdbCurDwg());
	if( !pDb )
		return false;
		
	AcDbBlockTable* pBlockTable = NULL;
	Acad::ErrorStatus es = pDb->getBlockTable( pBlockTable, AcDb::kForRead );
	if( es != Acad::eOk )    
	{
		delete mpSourceDb;
		mpSourceDb = NULL;
		return false;
	}

	AcDbBlockTableRecord* pBTR = NULL;
	es = pBlockTable->getAt( pszBlockName, pBTR, AcDb::kForRead );
	pBlockTable->close();
	if( es != Acad::eOk )
	{
		delete mpSourceDb;
		mpSourceDb = NULL;
		return false;
	}

	mpTemplate->SetStringProperty( Prop::BlockName, pszBlockName );
	DisplayBTR( pBTR, dZoomFactor, bZoomExtents, nScaleType, vecViewDir );
	pBTR->close();

	return true;
}

void CArxBlockViewCtrl::StartUIDrag()
{
	SetCapture();
	if( mbPanning )
		::SetCursor( mhPanCursor );
	else if( mbZooming )
		::SetCursor( mhZoomCursor );
	else if( mbOrbiting )
		::SetCursor( mhOrbitCursor );
}

void CArxBlockViewCtrl::EndUIDrag()
{
	::SetCursor( mhArrowCursor );
	ReleaseCapture();
}

bool CArxBlockViewCtrl::CanShowHighlight() const
{
	return !(mbPanning || mbZooming || mbOrbiting);
}

BEGIN_MESSAGE_MAP(CArxBlockViewCtrl, CArxGsViewCtrl)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxBlockViewCtrl message handlers

BOOL CArxBlockViewCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( !GetGsView() || mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 0 )
		return TRUE;

	if( zDelta < 0 )
	{
		Zoom( 0.9 );
		mdScale *= 0.9;// mirror our own scale factor
	}
	else
	{
		Zoom( 1.1 );
		mdScale *= 1.1;// mirror our own scale factor
	}
	return TRUE;
}

void CArxBlockViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	SetFocus();
	if( GetGsView() )
	{
		switch( mpTemplate->GetLongProperty( Prop::InterfaceMode ) )
		{
		case 3: //zoom
			mbZooming = true;
			mbPanning = false;
			mbOrbiting = false;
			StartUIDrag();
			mStartPt = point;					
			mdPreviousZoomScale = 1.0;
			break;
		case 2: //pan
			mbZooming = false;
			mbPanning = true;
			mbOrbiting = false;
			StartUIDrag();
			mStartPt = point;					
			break;
		case 1: //orbit
			mbZooming = false;
			mbPanning = false;
			mbOrbiting = true;
			StartUIDrag();
			mStartPt = point;					
			break;
		}
	}
}

void CArxBlockViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//end orbit and pan
	mbOrbiting = false;
	mbZooming = false;
	mbPanning = false;
		
	OnNeedRepaint( true );
	EndUIDrag();
	__super::OnLButtonUp( nFlags, point );
}

void CArxBlockViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnMButtonDown( nFlags, point );
	
	if( GetGsView() && mpTemplate->GetLongProperty( Prop::InterfaceMode ) != 0 )
	{
		mbPanning = true;
		StartUIDrag();
		mStartPt = point;
	}
}

void CArxBlockViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnMButtonUp( nFlags, point );

	if( !mbPanning )
		return;
	EndUIDrag();
	mbPanning = false;
}

void CArxBlockViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove( nFlags, point );
	
	if( mpTemplate->GetLongProperty( Prop::InterfaceMode ) == 0 )
		return;

	AcGsView* pView = GetGsView();
	if (pView)
	{
		if (mbZooming)
		{
			CRect rcThis;
			GetClientRect(&rcThis);
			double zDelta = point.y - mStartPt.y;
			
			if (zDelta<0)
			{
				if (mdPreviousZoomScale != 1.0)
				{
					double dZoomScale = 1.0 - mdPreviousZoomScale;
					dZoomScale = 1.0 - dZoomScale;
					pView->zoom(dZoomScale);			
				}
				zDelta = zDelta / rcThis.Height();
				mdPreviousZoomScale = 1.0 - zDelta;
				pView->zoom(mdPreviousZoomScale);
			}
			else if (zDelta>0)
			{
				if (mdPreviousZoomScale != 1.0)
				{
					double dZoomScale = 1.0 - mdPreviousZoomScale;
					dZoomScale = 1.0 - dZoomScale;
					pView->zoom(dZoomScale);			
				}
				zDelta = zDelta / rcThis.Height();
				mdPreviousZoomScale = 1.0 - zDelta;
				pView->zoom(mdPreviousZoomScale);
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
			pan_vec.transformBy(pView->viewingMatrix() * pView->worldToDeviceMatrix().inverse());
			pView->dolly(pan_vec);
			RedrawWindow();
			mStartPt = point;
		}
		if (mbOrbiting)
		{
			const double Half_Pi =  1.570796326795;

			AcGsDCRect view_rect;
			pView->getViewport (view_rect);

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

			pView->roll( roll_angle);               // 1: roll camera to make up vector coincident with rotation vector
			pView->orbit( 0.0f,  orbit_y_angle);     // 2: orbit along y to make up vector parallel with rotation vector
			pView->orbit( rotation_angle, 0.0f);     // 3: orbit along x by rotation angle
			pView->orbit( 0.0f, -orbit_y_angle);     // 4: orbit along y by the negation of 2
			pView->roll(-roll_angle);               // 5: roll camera by the negation of 1
			
			RedrawWindow();
			mStartPt = point;
		}
	}
}
