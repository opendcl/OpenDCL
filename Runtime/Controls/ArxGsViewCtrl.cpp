// ArxGsViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGsViewCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "Resource.h"
#include <math.h>


/////////////////////////////////////////////////////////////////////////////
// CArxGsViewCtrl

CArxGsViewCtrl::CArxGsViewCtrl( TDclControlPtr pTemplate,
																CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mColorService( long(-19), long(-22) )
, mArxServices( this )
, mDragDropService( this )
, mBlockInsertDropTarget( this )
, mpGsReactor( NULL )
, mclrHighlight( CAcadColorService::GetTransparentColor() )
, mLBState( up )
, mRBState( up )
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

bool CArxGsViewCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	if( !__super::OnApplyBackgroundColor( pProp ) )
		return false;
	AcGsDevice* pGsDevice = GetGsDevice();
	if( pGsDevice )
	{
		AcGsColor color = pGsDevice->getBackgroundColor();
		COLORREF aColor = GetRGBColor( pProp->GetLongValue() );
		color.m_red = GetRValue( aColor );
		color.m_green = GetGValue( aColor );
		color.m_blue = GetBValue( aColor );
		pGsDevice->setBackgroundColor( color );
	}
	return true;
}

void CArxGsViewCtrl::SetHighlight( const COLORREF& clrHighlight )
{
	mclrHighlight = clrHighlight;		
	OnNeedRepaint( false );
}

void CArxGsViewCtrl::RemoveHighlight()
{
	mclrHighlight = CAcadColorService::GetTransparentColor();
	OnNeedRepaint( false );
}

void CArxGsViewCtrl::clearAll()
{
	delete mpGsReactor;
	mpGsReactor = NULL;
	RemoveHighlight();
}

bool CArxGsViewCtrl::UpdateModel( AcGiDrawable* pDrawable )
{
	if( !mpGsReactor )
		return false;
	mpGsReactor->setDrawable( pDrawable );
	return true;
}

void CArxGsViewCtrl::Zoom( double dZoomFactor )
{
	if( mpGsReactor )
		mpGsReactor->zoom( dZoomFactor );
	OnNeedRepaint( false );
}

void CArxGsViewCtrl::DisplayBTR( AcDbBlockTableRecord* pBTR, double dZoomFactor, 
																 bool bZoomExtents, int nScaleType,
																 const AcGeVector3d& vecViewDir )
{
	AcDbDatabase* pDb = pBTR->database();
	if( !pDb )
		pDb = acdbCurDwg();
	if( !mpGsReactor || mpGsReactor->database() != pDb )
	{
		delete mpGsReactor;
		mpGsReactor = new GsViewReactor( this, pDb );
	}
	AcDbExtents extBTR;
	AcDbBlockTableRecordIterator* pBlockIterator = NULL;
	if( Acad::eOk != pBTR->newIterator( pBlockIterator ) )
		return;
	for( ; !pBlockIterator->done(); pBlockIterator->step() )
	{
		AcDbEntity* pEntity = NULL;  
		if( Acad::eOk != pBlockIterator->getEntity( pEntity, AcDb::kForRead ) )
			continue;
		AcDbExtents extEnt;
		pEntity->getGeomExtents( extEnt ); //get the extents of the entity
		
		// sometimes mtext objects can be present and empty
		// if they are they wreck the extents calculation
		// so we must ignore empty MText entities.
		if( pEntity->isA() == AcDbMText::desc() )
		{
			ACHAR* pszMText = ((AcDbMText*)pEntity)->contents();
			// if the Mext is empty, don't include it's extents.
			if( !pszMText || !*pszMText )
				continue;
		}
		extBTR.addExt( extEnt ); //add this entity to the total extents
		pEntity->close();
	}
	delete pBlockIterator;

	// Calculate the extents in WCS
	AcGePoint3d extMax = extBTR.maxPoint();
	AcGePoint3d extMin = extBTR.minPoint();

	// get the view port information - see parameter list
	ads_real height = 0.0, width = 0.0, viewTwist = 0.0;
	AcGePoint3d targetView(0,0,0);
	
	// we are only interested in the directions of the view, not the sizes, so we normalise. 
	AcGeVector3d vecView = vecViewDir.normal();
	//**********************************************
	// Our view coordinate space consists of z direction 
	// get a perp vector off the z direction
	// Make sure its normalised
	AcGeVector3d viewXDir = vecView.perpVector().normal();
	
	// correct the x angle by applying the twist
	viewXDir = viewXDir.rotateBy (viewTwist, -vecView);
	
	
	// now we can work out y, this is of course perp to the x and z directions. No need to normalise this, 
	// as we know that x and z are of unit length, and perpendicular, so their cross product must be on unit length
	AcGeVector3d viewYDir = vecView.crossProduct (viewXDir);

	AcGeLineSeg3d calcLine(extMax, extMin);
	AcGePoint3d boxCenter = calcLine.midPoint();
	//**********************************************
	// create a transform from WCS to View space
	// this represents the transformation from WCS to the view space. (Actually not quite since 
	// we are keeping the fixed point as the center of the box for convenience )
	AcGeMatrix3d viewMat;
	viewMat = AcGeMatrix3d::alignCoordSys (boxCenter, AcGeVector3d::kXAxis, AcGeVector3d::kYAxis, AcGeVector3d::kZAxis,  
												boxCenter, viewXDir, viewYDir, vecView).inverse();

	AcDbExtents extView = extBTR;
	// transforms the extents in WCS->view space
	extView.transformBy (viewMat);
	//**********************************************
	// get the extents of the AutoCAD view
	double xMax = fabs(extView.maxPoint ().x - extView.minPoint ().x);
	double yMax = fabs(extView.maxPoint ().y - extView.minPoint ().y);

	//**********************************************
	// setup the view
	AcGsView* pView = GetGsView();	
	assert( pView != NULL );
	if( !pView )
		return;
	AcGePoint3d eye = boxCenter + vecView;

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
		AcGsView::RenderMode eMode;
		try
		{		
			if ((rb.resval.rint==0) && (nVPNum == 1))
			{
				// SHADEMODE is not available in non mspace layout viewports
				eMode = AcGsView::k2DOptimized;
			}
			else
			{
				AcGsView * pCurView  = acgsGetGsView (nVPNum, false);
				if( pCurView )
				{
					//eMode  = (pCurView)? pCurView->mode(): AcGsView::k2DOptimized;
					eMode = AcGsView::kFlatShaded;
					pCurView->setMode(eMode);
				}
			}
			pView->setMode(eMode);
			pView->setView(eye, boxCenter, viewYDir, xMax , yMax );
		}
		catch(...)
		{
			Trace(_T("* CArxGsViewCtrl::DisplayBlock() exception caught"));
			assert(false);
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
		mpGsReactor->setDrawable( pBTR );

	OnNeedRepaint( true );
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
	return HandleEraseBkgnd( pDC );
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
	CRect rcClient;
	GetClientRect( &rcClient );
	if( CanShowHighlight() )
	{
		rcClient.DeflateRect( 1, 1 );
		if( !CAcadColorService::IsTransparentColor( mclrHighlight ) )
		{
			CPen Pen( PS_SOLID, 1, mclrHighlight );
			CPen* pOldPen = dc.SelectObject( &Pen );
			dc.MoveTo( rcClient.TopLeft() );
			dc.LineTo( rcClient.right, rcClient.top );		
			dc.LineTo( rcClient.right, rcClient.bottom );		
			dc.LineTo( rcClient.left, rcClient.bottom );		
			dc.LineTo( rcClient.left, rcClient.top );		
			dc.SelectObject( pOldPen );			
		}
	}
	rcClient.DeflateRect( 2, 2 );
	if( CanShowFocus() && GetFocus() == this )
		dc.DrawFocusRect( &rcClient );
}

void CArxGsViewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize( nType, cx, cy );
	if( mpGsReactor ) 
	{
		CRect rect;
		GetClientRect( &rect );
		mpGsReactor->onSize( rect.Width(), rect.Height() );
	}
}

void CArxGsViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	mLBState = down;
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) );
	if( !GetGsView() )
		return;
	
	if( (nFlags & MK_LBUTTON) != 0 && mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
		PostMessage( WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y) );	
		if( dwDropEffect != DROPEFFECT_NONE )
			return;
	}
	__super::OnLButtonDown( nFlags, point );
}

void CArxGsViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnLButtonUp( nFlags, point );

	if( mLBState == dblclk )
		GetArxServices()->HandleEvent( Prop::EventDblClicked );	
	else if( mLBState == down )
		GetArxServices()->HandleEvent( Prop::EventClicked );	
	mLBState = up;
	OnNeedRepaint( false );
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 1, nFlags, point.x, point.y ) );
}

void CArxGsViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnMButtonDown( nFlags, point );
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) );
}

void CArxGsViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnMButtonUp( nFlags, point );
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 4, nFlags, point.x, point.y ) );
}

void CArxGsViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove( nFlags, point );
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
}

__UINT_LRESULT CArxGsViewCtrl::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

void CArxGsViewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus( pOldWnd );
	OnNeedRepaint( false );
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxGsViewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	OnNeedRepaint( true );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxGsViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);
	mLBState = dblclk;
	GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 1, nFlags, point.x, point.y ) );
}

void CArxGsViewCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnRButtonDblClk(nFlags, point);
	mRBState = dblclk;
	GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 2, nFlags, point.x, point.y ) );
}

void CArxGsViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( mRBState == dblclk )
		GetArxServices()->HandleEvent( Prop::EventRightDblClick );	
	else if( mRBState == down )
		GetArxServices()->HandleEvent( Prop::EventRightClick );	
	mRBState = up;
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonUp(nFlags, point);
}

void CArxGsViewCtrl::OnDestroy() 
{
	clearAll();
	__super::OnDestroy();
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
	GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxGsViewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	mRBState = down;
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonDown(nFlags, point);
}
