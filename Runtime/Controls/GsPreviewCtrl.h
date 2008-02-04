// GsPreviewCtrl.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "OleOdcDropTarget.h"
#include "AcadColorTable.h"

class CAcadDocReactor;
class CAcadBlockReactor;
class CPropertyObject;

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


// Simple lighweight drawable to do orbit glyph
//
class OrbitGadget : public AcGiDrawable
{
    AcGsNode* m_pNode;
    AcGsView* m_pAcGsView;
public:	

	OrbitGadget():m_pNode(NULL) {}
    void setGsView(AcGsView* pView) {m_pAcGsView=pView;}

	virtual Adesk::UInt32   setAttributes   (AcGiDrawableTraits * traits)
    {
        traits->setTrueColor (AcCmEntityColor(0,200,0));
        return kDrawableNone;
    }
    virtual Adesk::Boolean  worldDraw       (AcGiWorldDraw * wd)
    {
        return Adesk::kFalse;
    }
    virtual void            viewportDraw    (AcGiViewportDraw * pViewportDraw)
    {
        AcGsDCRect view_rect;
        m_pAcGsView->getViewport (view_rect);

        int nViewportX = (view_rect.m_max.x - view_rect.m_min.x) + 1;
        int nViewportY = (view_rect.m_max.y - view_rect.m_min.y) + 1;
        AcGeMatrix3d scale = m_pAcGsView->viewingMatrix() * m_pAcGsView->worldToDeviceMatrix().inverse();
        AcGeVector3d viewDir = m_pAcGsView->target()-m_pAcGsView->position();
        const double radius  = min (nViewportX, nViewportY) * 0.4f*scale.scale();

		// I removed this line because it was causing problems with loaded drawings.
        //pViewportDraw->geometry().circle(m_pAcGsView->target(),radius,viewDir);
        return;
    }
    virtual Adesk::Boolean  isPersistent    (void) const
    { 
        return Adesk::kFalse;
    };                 
    virtual AcDbObjectId    id              (void) const
    {
        return AcDbObjectId::kNull;
    }
    virtual void            setGsNode       (AcGsNode * gsnode)
    {
        m_pNode = gsnode;
    }
    virtual AcGsNode*       gsNode          (void) const
    {
        return m_pNode;
    }
};

/////////////////////////////////////////////////////////////////////////////
// CGsPreviewCtrl window

class CGsPreviewCtrl : public CStatic
{
	class CDocReactor : public AcApDocManagerReactor
	{
		CGsPreviewCtrl* mpPreviewCtrl;
	public:
		CDocReactor( CGsPreviewCtrl* pPreviewCtrl )
			: mpPreviewCtrl( pPreviewCtrl )
			{ acDocManager->addReactor( this ); }
		~CDocReactor()
			{ acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ if( mpPreviewCtrl ) mpPreviewCtrl->UpdateBlock(); }
		void documentDestroyed(const char* filename)
			{ if( mpPreviewCtrl ) mpPreviewCtrl->UpdateBlock(); }
	} mDocReactor;

public:
	bool				m_bBeingLDblClicked;
	bool				m_bBeingRDblClicked;
	AcDbDatabase		*m_pLoadedDwg;
	CString				m_BlockName;
	CString				m_FileName;
	bool				m_bSelectedRect;
	COLORREF			m_HighlightColor;
	double				m_dPreviousZoomScale;
	CAcadBlockReactor	*m_pBlockReactor;	
	AcDbBlockTableRecord *m_pRec;

	AcDbObjectId		m_Line[4];
	AcDbObjectId		m_HatchId;
	CString				m_sHatchPattern;

	CStdioFile			*m_pLog;
	
public:
	void UpdateBlock();
	void ResizeHatch();

	void SetAcadColor(long nColor);
	bool GetActiveViewPortInfo (ads_real &height, ads_real &width, AcGePoint3d &target, AcGeVector3d &viewDir, ads_real &viewTwist, bool getViewCenter);
	void SetHighLight(int nColorIndex);
	void RemoveHighLight();	
	void DoHighLight(CDC *pdc);
	void Refresh(CDC *pdc);
	AcDbBlockTableRecord *m_pHatchBlock;
	BOOL DisplayHatchPattern(CString sPattern);
	BOOL DisplayExternalBlock(
		CString sBlockName, 
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ);
	void DisplayTheBlock(
		AcDbBlockTableRecord *pRec,
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,		
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ);
	bool LoadPreviewDwg(CString sFileName);
	bool LoadPreviewDwg(
		CString sFileName, 
		double dZoomFactor, 
		bool   bZoomExtents,
		int	   nScaleType,
		double dVectorX, 
		double dVectorY, 
		double dVectorZ,
		double dCameraX, 
		double dCameraY, 
		double dCameraZ);
	void DrawOrbitCircles(CDC* pDC = NULL);
    void DrawOrbitQuadCircle(CDC *pdc, int nX, int nY);
	void AllowOrbiting(bool bOrbiting);
	void Zoom(double dZfactor);
	void SetRenderMode();
	void SetDragnDrop(BOOL bRegister);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGsPreviewCtrl)
	public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	//}}AFX_VIRTUAL


public:
  CGsPreviewCtrl()
	: CStatic()
	, mDocReactor( this )
	, mpDevice( NULL )
	, mpView( NULL )
	, mpGhostModel( NULL )
	, mpModel( NULL )
	, mbModelCreated( false )
	, mbZooming( false )
	, mbPanning( false )
	, mbOrbiting( false )
	{
		// No tooltip created
		m_ToolTip.m_hWnd = NULL;
		m_bSelectedRect = false;
		m_pRenderMode = NULL;
		m_pAllowCircles = NULL;
		m_pInterfaceMode = NULL;
		m_dPreviousZoomScale = 1.0;
		m_bBeingLDblClicked = false;
		m_bBeingRDblClicked = false;
		m_FileName = CString();
		m_pBlockReactor = NULL;	
		m_pStaticBrush = new CBrush();			
		m_pStaticBrush->CreateSolidBrush(GetRGBColor(-22));
		m_pLoadedDwg = NULL;
		mhSavedCursor = NULL;
		mhZoomCursor = NULL;
		mhPanCursor = NULL;
		mhCrossCursor = NULL;
		mhOrbitCursor = NULL;
		mhArrowCursor = NULL;
    m_Line[0] = NULL;
    m_Line[1] = NULL;
    m_Line[2] = NULL;
    m_Line[3] = NULL;
		m_pLog = NULL;
	}

	CBrush *m_pStaticBrush;		
	COLORREF m_BackColor;	
	
	CPPToolTip m_ToolTip;
	
	virtual ~CGsPreviewCtrl() 
	{ 
		erasePreview(); 
		clearAll();
		if (m_pBlockReactor != NULL)
		{
			delete m_pBlockReactor;
			m_pBlockReactor = NULL;
		}
		if (m_pStaticBrush)
			delete m_pStaticBrush;
	
		if (mhSavedCursor != NULL)
			DeleteObject(mhSavedCursor);
		
		if (mhZoomCursor != NULL)
			DeleteObject(mhZoomCursor);
		
		if (mhPanCursor != NULL)
			DeleteObject(mhPanCursor);
		
		if (mhCrossCursor != NULL)
			DeleteObject(mhCrossCursor);
		
		if (mhOrbitCursor != NULL)
			DeleteObject(mhOrbitCursor);
		
		if (mhArrowCursor != NULL)
			DeleteObject(mhArrowCursor);		
	}

	void init(HMODULE hRes, bool bCreateModel = true);
	void erasePreview();
	void clearAll();
	void ClearScreenArea();
	bool PreLoadDwg(CString sFileName);
	bool GetBlockSize( LPCTSTR pszBlockName, AcDbExtents& ext );
	bool GetDwgSize( AcDbExtents& ext );
	BOOL DisplayBlock(CString sBlockName);
	BOOL DisplayBlock(CString sBlockName, 
						double dZoomFactor,
						bool   bZoomExtents,
						int	   nScaleType,
						double dVectorX, 
						double dVectorY, 
						double dVectorZ,
						double dCameraX,
						double dCameraY,
						double dCameraZ);

    AcGsView*   view()      { return mpView; }
    AcGsDevice* device()    { return mpDevice; }
    AcGsModel*  model()     { return mpModel; }
    void setModel(AcGsModel* pModel);
	double mdScale,mdExtentsScale;
	// Generated message map functions
protected:
	//{{AFX_MSG(CGsPreviewCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	HCURSOR				mhSavedCursor;
	HCURSOR				mhZoomCursor;
	HCURSOR				mhPanCursor;
    HCURSOR				mhCrossCursor;
    HCURSOR				mhOrbitCursor;
	HCURSOR				mhArrowCursor;
    AcGsDevice          *mpDevice;
    AcGsView            *mpView;
    AcGsModel           *mpModel;
    AcGsModel           *mpGhostModel;
    OrbitGadget         mOrbitGadget;
    bool				mbModelCreated;
    bool				mbZooming;
    bool				mbPanning;
    bool				mbOrbiting;
    CPoint				mStartPt;
public:
	TDclControlPtr m_ArxControl;
	TPropertyPtr m_pInterfaceMode;
	TPropertyPtr m_pAllowCircles;
	TPropertyPtr m_pRenderMode;
	CRect				m_rcFocus;
};
