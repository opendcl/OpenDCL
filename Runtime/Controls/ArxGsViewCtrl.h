// ArxGsViewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"

class CAcadDocReactor;
class CAcadBlockReactor;


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
// CArxGsViewCtrl window

class CArxGsViewCtrl : public CStatic, public CDialogControl
{
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxGsViewCtrl* mpGsViewCtrl;
	public:
		CDocReactor( CArxGsViewCtrl* pGsViewCtrl )
			: mpGsViewCtrl( pGsViewCtrl )
			{ acDocManager->addReactor( this ); }
		~CDocReactor()
			{ acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ if( mpGsViewCtrl ) mpGsViewCtrl->UpdateBlock(); }
		void documentDestroyed(const char* filename)
			{ if( mpGsViewCtrl ) mpGsViewCtrl->UpdateBlock(); }
	} mDocReactor;

protected:
	CAcadColorService mColorService;
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;


private:
	HCURSOR mhSavedCursor;
	HCURSOR mhZoomCursor;
	HCURSOR mhPanCursor;
	HCURSOR mhCrossCursor;
	HCURSOR mhOrbitCursor;
	HCURSOR mhArrowCursor;
	AcGsDevice* mpDevice;
	AcGsView* mpView;
	AcGsModel* mpModel;
	AcGsModel* mpGhostModel;
	OrbitGadget mOrbitGadget;
	bool mbModelCreated;
	bool mbZooming;
	bool mbPanning;
	bool mbOrbiting;
	CPoint mStartPt;

public:
	CRect m_rcFocus;
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


public:
	CArxGsViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxGsViewCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void UpdateBlock();
	void ResizeHatch();

	bool GetActiveViewPortInfo (ads_real &height, ads_real &width, AcGePoint3d &target, AcGeVector3d &viewDir, ads_real &viewTwist, bool getViewCenter);
	void SetHighlight( const COLORREF& clrHighlight );
	void RemoveHighlight();	
	void DoHighLight(CDC *pdc);
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
	void Zoom(double dZfactor);
	void SetRenderMode( long lMode );

	void init(HMODULE hRes, bool bCreateModel = true);
	void erasePreview();
	void clearAll();
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
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
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
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
