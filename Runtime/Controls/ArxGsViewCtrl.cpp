// ArxGsViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGsViewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGsViewCtrl

CArxGsViewCtrl::CArxGsViewCtrl( TDclControlPtr pTemplate,
																CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mColorService( long(-19), long(-22) )
, mArxServices( this )
, mDragDropService( this )
, mpGsReactor( NULL )
, mbHighlighted( false )
, mbLDblClick( false )
, mbRDblClick( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxGsViewCtrl::~CArxGsViewCtrl()
{
	clearAll();
}

bool CArxGsViewCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

void CArxGsViewCtrl::SetHighlight( const COLORREF& clrHighlight )
{
	mbHighlighted = true;
	mclrHighlight = clrHighlight;		
	Invalidate();
}

void CArxGsViewCtrl::RemoveHighlight()
{
	mbHighlighted = false;
	Invalidate();
}

void CArxGsViewCtrl::clearAll()
{
	delete mpGsReactor;
	mpGsReactor = NULL;
	mbHighlighted = false;
}

bool CArxGsViewCtrl::UpdateModel( AcGiDrawable* pDrawable )
{
	if( !mpGsReactor )
		return false;
	mpGsReactor->setDrawable( pDrawable );
	return true;
}

void CArxGsViewCtrl::Zoom( double dZfactor )
{
	if( mpGsReactor )
		mpGsReactor->zoom( dZfactor );
	Invalidate();
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
	AcDbDatabase* pDb = pRec->database();
	if( !pDb )
		pDb = acdbCurDwg();
	if( !mpGsReactor || mpGsReactor->database() != pDb )
	{
		delete mpGsReactor;
		mpGsReactor = new GsViewReactor( this, pDb );
	}
	AcDbExtents extents;
	AcGePoint3d extMax;
	AcGePoint3d extMin;

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
			if( sContents.IsEmpty() )
				continue;
		}

		// add this entity to the total extents
		extents.addExt (temp);

		// of course we must close it
		pEntity->close();
	}

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
	AcGsView *pView = GetGsView();	
	AcGePoint3d eye = boxCenter + vCameraViewDir;

	if (yMax == 0.0)
		yMax = 0.00000000002;
	
	if (xMax == 0.0)
		xMax = 0.00000000002;

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
	pView->setMode( GetRenderMode() );

	if( mpGsReactor )
		mpGsReactor->setDrawable( pRec );

	Invalidate();
	PaintUI();
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

	AcGsView* pView = GetGsView();
	if( pView )
	{
		//update the gs view
		pView->invalidate(); 
		pView->update();
	}
	PaintUI( &dc );
	if( mbHighlighted && CanShowHighlight() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		rcClient.DeflateRect( 2, 2 );
		
		CPen Pen( PS_SOLID, 1, mclrHighlight );
		CPen* pOldPen = dc.SelectObject( &Pen );

		dc.MoveTo( 1, 1 );
		dc.LineTo( rcClient.Width(), 1 );		
		dc.LineTo( rcClient.Width(), rcClient.Height() );		
		dc.LineTo( 1, rcClient.Height() );		
		dc.LineTo( 1, 1 );		

		dc.SelectObject( pOldPen );			
		if( GetFocus() == this )
			dc.DrawFocusRect( rcClient );
	}
}

void CArxGsViewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize( nType, cx, cy );
	if( mpGsReactor ) 
	{
		CRect rect;
		GetClientRect( &rect);
		mpGsReactor->onSize( rect.Width(), rect.Height() );
	}
}

void CArxGsViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	if( !GetGsView() )
		return;
	
	if( (nFlags & MK_LBUTTON) != 0 && mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
		if( dwDropEffect != DROPEFFECT_NONE )
			return;
	}
	__super::OnLButtonDown( nFlags, point );
}

void CArxGsViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnLButtonUp( nFlags, point );

	if( mbLDblClick)
	{
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());	
		mbLDblClick = false;
	}
	else
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());	
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxGsViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnMButtonDown( nFlags, point );
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxGsViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnMButtonUp( nFlags, point );
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxGsViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove( nFlags, point );
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

__UINT_LRESULT CArxGsViewCtrl::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

void CArxGsViewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus( pOldWnd );
	Invalidate();
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxGsViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);

	mbLDblClick = true;

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxGsViewCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnRButtonDblClk(nFlags, point);

	mbRDblClick = true;

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxGsViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( mbRDblClick )
	{
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventRightDblClick), IsAsyncEvents());	
		mbRDblClick = false;
	}
	else
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventRightClick), IsAsyncEvents());	
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnRButtonUp(nFlags, point);
}

void CArxGsViewCtrl::OnDestroy() 
{
	clearAll();
	__super::OnDestroy();
}

void CArxGsViewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	Invalidate();
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
