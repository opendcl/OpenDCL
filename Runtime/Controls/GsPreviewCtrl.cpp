// GsPreviewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GsPreviewCtrl.h"
#include "Resource.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "ToolTips.h"
#include "AcadBlockReactor.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "Workspace.h"

const int nOrbitOffset = 20;
const int nOrbitQuadCircleDia = 19;
const TCHAR sModelSpace[] = _T("*Model_Space");
	

/////////////////////////////////////////////////////////////////////////////
// CGsPreviewCtrl

BEGIN_MESSAGE_MAP(CGsPreviewCtrl, CStatic)
	//{{AFX_MSG_MAP(CGsPreviewCtrl)
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
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGsPreviewCtrl message handlers
void CGsPreviewCtrl::OnPaint() 
{
	try
	{
    CPaintDC dc(this); 

	if (m_BlockName.GetLength() > 0 || m_pLoadedDwg != NULL)
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
		try
		{
		CDC* pdc = &dc;
		if (mbOrbiting || 
			(m_pAllowCircles && m_pAllowCircles->GetBooleanValue() == TRUE)
			)
			DrawOrbitCircles(pdc);
		
		DoHighLight(pdc);
		if (GetFocus() == this && !mbOrbiting && !mbPanning && !mbZooming)
		{
			// draw the solid rectangle
			pdc->DrawFocusRect(m_rcFocus);	
		}
		}
		catch(...)
		{
			
		}
	}
	try
	{
	if (m_BlockName.GetLength() == 0)
	{
		CDC* pdc = &dc;		
		Refresh(pdc);

		DoHighLight(pdc);
		if (GetFocus() == this && !mbOrbiting && !mbPanning)
		{
			// draw the solid rectangle
			pdc->DrawFocusRect(m_rcFocus);
		}	
	}

	}
	catch(...)
	{
		
	}
	}
	catch(...)
	{
		
	}
}

void CGsPreviewCtrl::DoHighLight(CDC *pdc)
{
	try
	{
	if (m_bSelectedRect && !mbOrbiting && !mbPanning)
	{
		CRect rcCell;
		GetClientRect (&rcCell);
		CSize szValue(1,1);
		rcCell.DeflateRect(szValue);
		
		CPen *pPen = new CPen(PS_SOLID, 1, m_HighlightColor);
		CPen* pOldPen = pdc->SelectObject(pPen);

		pdc->MoveTo(1, 1);
		pdc->LineTo(rcCell.Width(), 1);		
		pdc->LineTo(rcCell.Width(), rcCell.Height());		
		pdc->LineTo(1, rcCell.Height());		
		pdc->LineTo(1, 1);		
		
		//pdc->Rectangle(rcCell);

		pdc->SelectObject(pOldPen);			
		pPen->DeleteObject();	
	}
	}
	catch(...)
	{
		
		clearAll();
	}

}
void CGsPreviewCtrl::AllowOrbiting(bool bOrbiting)
{
	if (m_pAllowCircles && m_pAllowCircles->GetBooleanValue() == TRUE)
		DrawOrbitCircles();
}

void CGsPreviewCtrl::DrawOrbitCircles(CDC* pDC)
{
	try
	{
	if (mbPanning)
		return;
    
	if (m_pInterfaceMode == NULL)
		return;
	if (m_pInterfaceMode->GetLongValue() != 1)
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
	
	CPen *pPen = new CPen(PS_SOLID, 1, RGB(0,255,0));
	CPen* pOldPen = pdc->SelectObject(pPen);

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
	pPen->DeleteObject();
	delete pPen;

	if (pDC == NULL)
		pdc->Detach();
	
	}
	catch(...)
	{
		
		clearAll();
	}	
}
void CGsPreviewCtrl::DrawOrbitQuadCircle(CDC *pdc, int nX, int nY)
{
	int sX = nX - (nOrbitQuadCircleDia / 2);
	int sY = nY - (nOrbitQuadCircleDia / 2);
	int eX = nX + (nOrbitQuadCircleDia / 2);
	int eY = nY + (nOrbitQuadCircleDia / 2);

	
	pdc->MoveTo(sX, sY);
	pdc->Arc(sX, sY, eX, eY, sX, nY, eX, nY);		
	pdc->Arc(sX, sY, eX, eY, eX, nY, sX, nY);		

}
void CGsPreviewCtrl::ClearScreenArea()
{
	PAINTSTRUCT ps; 
	
	CDC* pdc = BeginPaint(&ps);
    
	Refresh(pdc);

	EndPaint(&ps);
}

void CGsPreviewCtrl::Refresh(CDC *pdc)
{	
	try
	{
	CRect rcThis;
	GetClientRect(&rcThis);

	if (m_pStaticBrush != NULL)
	{
		// draw the Window background for the cell				
		pdc->FillRect(rcThis, m_pStaticBrush);
	}
	else	
	{
		CBrush *pCellBrush = new CBrush;	
		pCellBrush->CreateSysColorBrush(COLOR_BTNFACE);
	
		// draw the Window background for the cell				
		pdc->FillRect(rcThis, pCellBrush);
		// delete the brush
		pCellBrush->DeleteObject();
		delete pCellBrush;
	}
	}
	catch(...)
	{
		
	}
}
// Erase view and delete model
void CGsPreviewCtrl::erasePreview()
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

void CGsPreviewCtrl::setModel(AcGsModel* pModel)
{
    erasePreview();
    mpModel = pModel;
    mbModelCreated =false;
}

void CGsPreviewCtrl::clearAll()
{	
	m_BlockName = CString();
	try
	{
		if (mpView)
		{
			mpView->eraseAll();
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


void CGsPreviewCtrl::init(HMODULE hRes, bool bCreateModel)
{
  //Instantiate view, a device and a model object
  AcGsManager* pManager = acgsGetGsManager();
	assert( pManager != NULL );
	if( !pManager )
		return;
  AcGsClassFactory* pFactory = pManager->getGSClassFactory();
  //a device with standard autocad color palette
  mpDevice = pManager->createAutoCADDevice(m_hWnd);
	RefCountedPtr< CPropertyObject > pAcadColor = m_ArxControl->GetPropertyObject(nAcadColor);
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

void CGsPreviewCtrl::SetAcadColor(long nColor) 
{	
	if (m_pStaticBrush)
		delete m_pStaticBrush;

	bool bTilemode = acdbHostApplicationServices()->workingDatabase()->tilemode(); 

	if (nColor == -22 && !bTilemode)
		nColor = -23;

	else if (nColor == -23 && bTilemode)
		nColor = -22;

	m_ArxControl->SetLongProperty(nAcadColor, nColor);

	m_pStaticBrush = new CBrush();
	m_BackColor = GetRGBColor(nColor);
	m_pStaticBrush->CreateSolidBrush(m_BackColor);	

	if (mpDevice == NULL)
		return;

	AcGsColor color = mpDevice->getBackgroundColor();

	COLORREF aColor = GetRGBColor(nColor);

	color.m_red = GetRValue(aColor);
	color.m_green = GetGValue(aColor);
	color.m_blue = GetBValue(aColor);
	
	mpDevice->setBackgroundColor(color);		
}

void CGsPreviewCtrl::ResizeHatch() 
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
void CGsPreviewCtrl::OnSize(UINT nType, int cx, int cy) 
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
			if (m_pAllowCircles && m_pAllowCircles->GetBooleanValue() == TRUE)
				DrawOrbitCircles();
		}
		catch(...)
		{
		}
		
	}
	else
	{
		CDC *pdc = GetDC();
		Refresh(pdc);
		pdc->Detach();
	}
}

BOOL CGsPreviewCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (m_pInterfaceMode == NULL)
		return TRUE;
	
	if (m_pInterfaceMode->GetLongValue() == 0)
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
			pdc->Detach();
		}
    }
    return TRUE;
}

void CGsPreviewCtrl::Zoom(double dZfactor)
{
    mpView->zoom(dZfactor);
    Invalidate();
	if (GetFocus() == this)
	{
		CDC *pdc = GetDC();
		// draw the solid rectangle
		pdc->DrawFocusRect(m_rcFocus);	
		pdc->Detach();
	}
}
void CGsPreviewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
	if (m_ArxControl->GetBoolProperty(nDragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		if (m_pLoadedDwg == NULL)
			BeginDragnDrop(m_ArxControl, point, m_bInvokeWithSendString);
		return;
	}

	SetFocus();
	if (m_BlockName.GetLength() > 0)
	{
		if (m_pInterfaceMode == NULL)
		{
			mbPanning = true;
			mbOrbiting = false;
			SetCapture();
			//set up the proper cursor
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			//::SetCursor(mhPanCursor);
			//store the start point
			mStartPt = point;		
			return;
		}
	
		// zoom mode
		if (m_pInterfaceMode->GetLongValue() == 3)
		{
			mbZooming = true;
			mbPanning = false;
			mbOrbiting = false;
			SetCapture();
			//set up the proper cursor
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			::SetCursor(mhZoomCursor);
			//store the start point
			mStartPt = point;					
			// initialize the zoom scale factor
			m_dPreviousZoomScale = 1.0;
		}
		
		// pan mode
		if (m_pInterfaceMode->GetLongValue() == 2)
		{
			mbZooming = false;
			mbPanning = true;
			mbOrbiting = false;
			SetCapture();
			//set up the proper cursor
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			::SetCursor(mhPanCursor);
			//store the start point
			mStartPt = point;
		}
		// orbit mode
		if (m_pInterfaceMode->GetLongValue() == 1)
		{
			//start orbit
			mbZooming = false;
			mbPanning = false;
			mbOrbiting = true;
			SetCapture();
			//set up the proper cursor
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			::SetCursor(mhOrbitCursor);
			//store the start point
			mStartPt = point;
			return;
		}
	}
	else
	{
		::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
		::SetCursor(mhArrowCursor);	
	}
}

void CGsPreviewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	SetFocus();

	
    //end orbit and pan
	mbOrbiting = false;
	mbZooming = false;
	mbPanning = false;
		
	if (m_pInterfaceMode)
	{
		// zoom mode
		switch (m_pInterfaceMode->GetLongValue())
		{
		case 2:
			{	
				::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
				::SetCursor(mhPanCursor);
				break;
			}
		case 3:
			{	
				::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
				::SetCursor(mhZoomCursor);
				break;
			}
		default:
			{	
				::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
				::SetCursor(mhArrowCursor);
				break;
			}
		}	
	}

	ShowCursor(TRUE);
	Invalidate();
	ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);

	if (m_bBeingLDblClicked)
	{
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);	
		m_bBeingLDblClicked = false;
	}

	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventClicked), m_bInvokeWithSendString);	
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	if (!mpView || m_BlockName.GetLength() == 0)
	{
		return;
	}

}

void CGsPreviewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
	if (m_pInterfaceMode == NULL)
	{
		//start pan
		mbPanning = true;
		SetCapture();
		//set up the proper cursor
		::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
		//::SetCursor(mhPanCursor);
		//store the start point
		mStartPt = point;
		return;
	}
	if (m_BlockName.GetLength() > 0 && 
		m_pInterfaceMode->GetLongValue() != 0)
	{
		//start pan
		mbPanning = true;
		SetCapture();
		//set up the proper cursor
		::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
		::SetCursor(mhPanCursor);
		//store the start point
		mStartPt = point;
	}
    
	
}

void CGsPreviewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	SetFocus();

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
    //end pan
	ReleaseCapture();
	mbPanning = false;
	//::SetClassLong(m_hWnd,GCL_HCURSOR,(long)mhArrowCursor);
	::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
	::SetCursor(mhArrowCursor);
		
	ShowCursor(TRUE);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	pdc->Detach();	

	
	
}

void CGsPreviewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	try
	{
		if (m_pInterfaceMode == NULL)
		{
			mbOrbiting = false;
			mbPanning = false;
			//::SetClassLong(m_hWnd,GCL_HCURSOR,(long)mhArrowCursor);
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			//::SetCursor(mhArrowCursor);
		
			
			ShowCursor(TRUE);
			//ReleaseCapture();
			return;
		}
		// static
		if (m_pInterfaceMode->GetLongValue() == 0)
		{
			mbOrbiting = false;
			mbPanning = false;
			mbZooming = false;
			
			//::SetClassLong(m_hWnd,GCL_HCURSOR,(long)mhArrowCursor);
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			::SetCursor(mhArrowCursor);
		
			
			ShowCursor(TRUE);
			
			return;
		}
		// zoom mode
		switch (m_pInterfaceMode->GetLongValue())
		{
		case 2:
			{	
				::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
				::SetCursor(mhPanCursor);
				ShowCursor(TRUE);
				break;
			}
		case 3:
			{	
				::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
				::SetCursor(mhZoomCursor);
				ShowCursor(TRUE);
				break;
			}
		default:
			{	
				::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
				::SetCursor(mhArrowCursor);
				ShowCursor(TRUE);
				//ReleaseCapture();		
				break;
			}
		}			
		/*
		if (mbZooming == false && mbPanning == false && mbOrbiting == false)
		{
			::SetClassLong(m_hWnd,GCL_HCURSOR,NULL);
			CStatic::SetCursor(mhArrowCursor);
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

#if (_ACADTARGET == 16)
UINT CGsPreviewCtrl::OnNcHitTest(CPoint point) 
#else
LRESULT CGsPreviewCtrl::OnNcHitTest(CPoint point) 
#endif
{
    return HTCLIENT;
}


void CGsPreviewCtrl::OnSetFocus(CWnd* pOldWnd) 
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
	
	pdc->Detach();

	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
	
}


BOOL CGsPreviewCtrl::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID)
{	
	DWORD dwStyle;
	CRect ArxRect;
	mhArrowCursor = LoadCursor(NULL,IDC_ARROW);
    
	HMODULE hRes = _hdllInstance;
    //Load our special cursors
    mhZoomCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_ZOOM));
	mhPanCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_PAN));
    mhCrossCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_CROSS));
    mhOrbitCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_ORBIT));

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(nTop)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(nLeft)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(nHeight)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(nWidth)->GetLongValue() + ArxRect.left;
	
	dwStyle = WS_CHILD | WS_VISIBLE | SS_LEFT |WS_CLIPSIBLINGS |WS_CLIPCHILDREN;
	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	BOOL RetVal = CStatic::Create(_T(""), dwStyle, ArxRect, pParentWnd, nID);
	
	m_pInterfaceMode = m_ArxControl->GetPropertyObject(nAllowOrbiting);
	m_pAllowCircles = m_ArxControl->GetPropertyObject(nShowOrbitCirlces);
	m_pRenderMode = m_ArxControl->GetPropertyObject(nRenderMode);

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);


	switch (m_ArxControl->GetLongProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

	// create and add the new reactor
	m_pBlockReactor = new CAcadBlockReactor(NULL, this);
	acedEditor->addReactor(m_pBlockReactor);

	return RetVal;
}

int CGsPreviewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
bool CGsPreviewCtrl::PreLoadDwg(CString sFileName)
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
			m_BlockName = CString();
			m_FileName = CString();
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
	}
	catch(...)
	{
	}
	return true;			
		
}


void CGsPreviewCtrl::GetBlockList() 
{
	
	AcDbBlockTable *pBlockTable;

	if (m_pLoadedDwg == NULL)
	{
		if (acdbHostApplicationServices()->workingDatabase() == NULL)
		{
			acedRetInt(-1);
			return;
		}
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
	}
	else
		if (Acad::eOk != m_pLoadedDwg->getSymbolTable(pBlockTable, AcDb::kForRead))
		{
			acedRetInt(-1);
			return;
		}

	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	// Iterate through the block table and disaply the names in the ListCtrl.
	const TCHAR *pName;
	AcDbBlockTableIterator *pBTItr;
	if (pBlockTable->newIterator(pBTItr) == Acad::eOk) 
	{
		int nCount = 0;
		while (!pBTItr->done()) 
		{
			AcDbBlockTableRecord *pRecord;
			if (pBTItr->getRecord(pRecord, AcDb::kForRead) == Acad::eOk)
			{
	
				pRecord->getName(pName);
				CString sBlockName = pName;
				if (sBlockName.Left(6) != "*Model" &&
					sBlockName.Left(6) != "*Paper")
				{		
					if (nCount > 0)
					{
						rbpTail->rbnext = acutNewRb(RTSTR);
						rbpTail = rbpTail->rbnext;
					}
	
					acutNewString(sBlockName, rbpTail->resval.rstring);
					nCount++;
				
				}
				pRecord->close();
			}
			pBTItr->step();
		}
	}
	pBlockTable->close();
	
	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);

}

bool CGsPreviewCtrl::LoadPreviewDwg(CString sFileName)
{
	return LoadPreviewDwg(sFileName, 1.0, true, 1, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
}

bool CGsPreviewCtrl::LoadPreviewDwg(
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
		sFileName.Replace(_T("\\"), _T("/"));
		sFileName.Replace(_T("//"), _T("/"));
		
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
			m_BlockName = CString();
			m_FileName = CString();
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
			m_BlockName = CString();
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
		m_FileName = CString();
		
		if (m_pLoadedDwg==NULL)
		{
			m_BlockName = CString();
			return FALSE;			
		}
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
			m_BlockName = CString();
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
			m_BlockName = CString();
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
void CGsPreviewCtrl::GetDwgSize()
{
	AcGePoint3d extMax;
	AcGePoint3d extMin;
	
	if (m_pLoadedDwg != NULL)
	{
		//get the extension values for the model space block
		extMax = m_pLoadedDwg->extmax();
		extMin = m_pLoadedDwg->extmin();
	}
	else
	{
		acedRetInt(-1);
		return;
	}

	
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTREAL, extMax[0]-extMin[0],
		RTREAL, extMax[1]-extMin[1],
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
	else 
		acedRetNil();	
}
void CGsPreviewCtrl::GetBlockSize(CString sBlockName)
{
	if (sBlockName.GetLength() == 0)
	{		
		acedRetNil();
	    return;
	}

    AcDbBlockTableRecord *pRec;
    AcDbBlockTable *pTab;
    Acad::ErrorStatus es;
    AcDbDatabase* pDb;

	if (m_pLoadedDwg == NULL)
	{
		pDb = acdbHostApplicationServices()->workingDatabase(); 
	}
	else
	{
		pDb = m_pLoadedDwg;
	}
    
	if (pDb==NULL)
	{
		sBlockName = CString();
	    
		acedRetNil();
	    return;
	}
    
	if ((es = pDb->getBlockTable(pTab,AcDb::kForRead)) !=Acad::eOk)    
	{
		sBlockName = CString();
		
		acedRetNil();
	    return;
	}
	
	if (!pTab->has(sBlockName))
    {
        pTab->close();
	    sBlockName = CString();
        
		acedRetNil();
	    return;
    }

    if ((es = pTab->getAt(sBlockName,pRec,AcDb::kForRead)) !=Acad::eOk)
    {
		sBlockName = CString();
        pTab->close();
		acedRetNil();
	    return;
    }
    pTab->close();
	

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
			if (sContents.GetLength() == 0)
				continue;
		}

		// add this entity to the total extents
		extents.addExt (temp);

		// of course we must close it
		pEntity->close();
	}
	
	//!CHANGED! 9-16-04 SRM
	//Table was not closed causing block viewer to not load block
	pRec->close();	

	// Calculate the extents in WCS
	extMax = extents.maxPoint ();  
	extMin = extents.minPoint ();   
	
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTREAL, extMax[0]-extMin[0],
		RTREAL, extMax[1]-extMin[1],
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
	else 
		acedRetNil();	
}
BOOL CGsPreviewCtrl::DisplayBlock(CString sBlockName)
{
	return DisplayBlock(sBlockName, 1.0, true, 1, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
}

BOOL CGsPreviewCtrl::DisplayBlock(
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
// m_pLog->WriteString("\r\nDisplayBlock 1");
	clearAll();
// m_pLog->WriteString("\r\nDisplayBlock 2");	
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
// m_pLog->WriteString("\r\nDisplayBlock 3");	
	m_BlockName = sBlockName;
    
	if (sBlockName.GetLength() == 0)
		return FALSE;
// m_pLog->WriteString("\r\nDisplayBlock 4");	
    AcDbBlockTableRecord *pRec;
    AcDbBlockTable *pTab;
    Acad::ErrorStatus es;
    AcDbDatabase* pDb;
// m_pLog->WriteString("\r\nDisplayBlock 5");	
	if (m_pLoadedDwg != NULL)
	{
		// delete it.
		pDb = m_pLoadedDwg;
	}
	else
		pDb = acdbHostApplicationServices()->workingDatabase(); 
// m_pLog->WriteString("\r\nDisplayBlock 6");

	if (pDb==NULL)
	{
		sBlockName = CString();
		m_BlockName = CString();
        return FALSE;
	}
  // m_pLog->WriteString("\r\nDisplayBlock 7");	  
	if ((es = pDb->getBlockTable(pTab,AcDb::kForRead)) !=Acad::eOk)    
	{
		sBlockName = CString();
		m_BlockName = CString();
		return FALSE;
	}
	// m_pLog->WriteString("\r\nDisplayBlock 8");	
	if (!pTab->has(sBlockName))
    {
        pTab->close();
	    sBlockName = CString();
		m_BlockName = CString();
        return FALSE;
    }
// m_pLog->WriteString("\r\nDisplayBlock 9");	
    if ((es = pTab->getAt(sBlockName,pRec,AcDb::kForWrite)) !=Acad::eOk)
    {
		sBlockName = CString();
		m_BlockName = CString();
        pTab->close();
	    return FALSE;
    }
    pTab->close();	
// m_pLog->WriteString("\r\nDisplayBlock 10");	
/*
	AcGePoint3d vertexPts[4];
    AcDbObjectId lineId, cirId, hatchId;
    AcDbObjectIdArray dbObjIds;
    AcDbLine *line;

    vertexPts[0].set(0.0, 0.0, 0.0);
    vertexPts[1].set(20.0, 0.0, 0.0);
    vertexPts[2].set(20.0, 10.0, 0.0);
    vertexPts[3].set(0.0, 10.0, 0.0);

    for (int i = 0; i < 4; i++) 
	{
        line =  new AcDbLine();
        line->setStartPoint(vertexPts[i]) ;
        line->setEndPoint(vertexPts[(i == 3) ? 0 : i+1]) ;
    
		if (pRec->appendAcDbEntity(line) != Acad::eOk)
		{
			pRec->close();
			return 0;
		}
		line->close();

        dbObjIds.append(lineId);
    }
*/
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
// m_pLog->WriteString("\r\nDisplayBlock 11");	
	return TRUE;
}

BOOL CGsPreviewCtrl::DisplayHatchPattern(CString sPattern)
{
	m_sHatchPattern = sPattern;

	AcDbBlockTable *pBlockTable = NULL;
	AcDbBlockTableRecord *pBlockRecord = NULL;
	
	m_pLoadedDwg = new AcDbDatabase();
	
	if (m_pLoadedDwg == NULL)
		return 0;
	
	m_pLoadedDwg->getSymbolTable(pBlockTable,AcDb::kForRead);

	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockRecord, AcDb::kForWrite);

	pBlockTable->close();


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

	long lClr = m_ArxControl->GetLongProperty(nForeColor);
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
	pHatch->setPatternScale(dblPatternScaleFactor * m_ArxControl->GetPropertyObject(nHatchScale)->GetDoubleValue());
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
	
	Acad::ErrorStatus es = pBlockRecord->appendAcDbEntity(hatchId, pHatch);
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
BOOL CGsPreviewCtrl::DisplayExternalBlock(
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
		sBlockName = CString();
		m_BlockName = CString();
        return FALSE;
	}
    
	if ((es = pDb->getBlockTable(pTab,AcDb::kForRead)) !=Acad::eOk)    
	{
		sBlockName = CString();
		m_BlockName = CString();
		return FALSE;
	}
	
	if (!pTab->has(sBlockName))
    {
        pTab->close();
	    sBlockName = CString();
		m_BlockName = CString();
        return FALSE;
    }

    if ((es = pTab->getAt(sBlockName,pRec,AcDb::kForRead)) !=Acad::eOk)
    {
		sBlockName = CString();
		m_BlockName = CString();
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

void CGsPreviewCtrl::DisplayTheBlock(
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
    init(_hdllInstance,true);
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

	if (m_ArxControl->GetType() == CtlHatch)
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
	SetRenderMode();
	if (m_pLoadedDwg == NULL)
	{
		// remove all objects previously viewed
		pView->eraseAll();
	}

	// add the new object to the viewer
	m_pRec = pRec;
    pView->add(pRec,model()); 
    pRec->close();
	Invalidate();
	if (m_pAllowCircles && m_pAllowCircles->GetBooleanValue() == TRUE)
		DrawOrbitCircles();
}

void CGsPreviewCtrl::UpdateBlock()
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
		if (m_pAllowCircles && m_pAllowCircles->GetBooleanValue() == TRUE)
			DrawOrbitCircles();
	}
	catch(...)
	{
	}
}

//***************************************************************************************
// get the view port information - see parameter list
bool CGsPreviewCtrl::GetActiveViewPortInfo (ads_real &height, ads_real &width, AcGePoint3d &target, AcGeVector3d &vCameraViewDir, ads_real &viewTwist, bool getViewCenter)
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

void CGsPreviewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SetFocus();
			
	CStatic::OnLButtonDblClk(nFlags, point);

	m_bBeingLDblClicked = true;

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	// call methods to invoke the event
	//InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), false);//m_bInvokeWithSendString);	

}


void CGsPreviewCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	SetFocus();
		
	CStatic::OnRButtonDblClk(nFlags, point);

	m_bBeingRDblClicked = true;

	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	// call methods to invoke the event
	///InvokeMethod(m_ArxControl->GetStrProperty(nEventRDblClick), false);//m_bInvokeWithSendString);	
	
}

void CGsPreviewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	SetFocus();

	if (m_bBeingRDblClicked)
	{
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventRDblClick), m_bInvokeWithSendString);	
		m_bBeingRDblClicked = false;
	}
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventRClick), m_bInvokeWithSendString);	
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	
	if (!mpView || m_BlockName.GetLength() == 0)
		return;
	
	
	
	CStatic::OnRButtonUp(nFlags, point);
	
	
}


void CGsPreviewCtrl::SetRenderMode() 
{
	AcGsView* pView = view();	

	if (pView == NULL)
		return;
	if (m_pRenderMode == NULL)
		return;

	/*
	pView->setMode(AcGsView::kNone);
	return;
	*/
	switch(m_pRenderMode->GetLongValue())
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
void CGsPreviewCtrl::OnDestroy() 
{
	try
	{
		if (m_pLoadedDwg != NULL)
		{
			clearAll();
			m_BlockName = CString();
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
	CStatic::OnDestroy();
}

void CGsPreviewCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CStatic::OnKillFocus(pNewWnd);
	
	CDC *pdc = GetDC();
	// draw the solid rectangle
	pdc->DrawFocusRect(m_rcFocus);
	
	pdc->Detach();
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}

void CGsPreviewCtrl::SetHighLight(int nColorIndex)
{
	m_bSelectedRect = true;
	m_HighlightColor = GetRGBColor(nColorIndex);		
	Invalidate();
}

void CGsPreviewCtrl::RemoveHighLight()
{
	m_bSelectedRect = false;
	Invalidate();
}

BOOL CGsPreviewCtrl::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	
	return CStatic::PreTranslateMessage(pMsg);
}

void CGsPreviewCtrl::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = m_ArxControl;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}

HBRUSH CGsPreviewCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//pDC->SetBkMode(TRANSPARENT);	
	pDC->SetBkColor(m_BackColor);		
	pDC->SelectObject(m_pStaticBrush);
	return (HBRUSH)(m_pStaticBrush->GetSafeHandle());	
}


void CGsPreviewCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CStatic::OnMButtonDblClk(nFlags, point);
}

void CGsPreviewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CStatic::OnRButtonDown(nFlags, point);
}
