// ArxBlockViewCtrl.h : header file
//

#pragma once

#include "ArxGsViewCtrl.h"

#if (_ARXTARGET == 18)
#define setAttributes subSetAttributes
#define worldDraw subWorldDraw
#define viewportDraw subViewportDraw
#endif


#ifdef USE_ORBIT_GADGET
// Simple lighweight drawable to do orbit glyph
//
class OrbitGadget : public AcGiDrawable
{
	AcGsView* m_pAcGsView;
#if (_ARXTARGET < 20)
	AcGsNode* m_pNode;
#endif

public:	
	OrbitGadget()
		: m_pAcGsView(NULL)
	#if (_ARXTARGET < 20)
		, m_pNode(NULL)
	#endif
		{}

	void setGsView(AcGsView* pView)
	{
		m_pAcGsView = pView;
	}

	virtual Adesk::UInt32   subSetAttributes   (AcGiDrawableTraits * traits)
		{
				traits->setTrueColor (AcCmEntityColor(0,200,0));
				return kDrawableNone;
		}
		virtual Adesk::Boolean  subWorldDraw       (AcGiWorldDraw * wd)
		{
				return Adesk::kFalse;
		}
		virtual void            subViewportDraw    (AcGiViewportDraw * pViewportDraw)
		{
				AcGsDCRect view_rect;
				m_pAcGsView->getViewport (view_rect);

				int nViewportX = (view_rect.m_max.x - view_rect.m_min.x) + 1;
				int nViewportY = (view_rect.m_max.y - view_rect.m_min.y) + 1;
				AcGeMatrix3d scale = m_pAcGsView->viewingMatrix() * m_pAcGsView->worldToDeviceMatrix().inverse();
				AcGeVector3d viewDir = m_pAcGsView->target()-m_pAcGsView->position();
				const double radius  = min (nViewportX, nViewportY) * 0.4f*scale.scale();

				pViewportDraw->geometry().circle(m_pAcGsView->target(),radius,viewDir);
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
	#if (_ARXTARGET < 20)
		virtual void            setGsNode       (AcGsNode * gsnode)
		{
				m_pNode = gsnode;
		}
		virtual AcGsNode*       gsNode          (void) const
		{
				return m_pNode;
		}
	#endif
};
#endif //USE_ORBIT_GADGET


/////////////////////////////////////////////////////////////////////////////
// CArxBlockViewCtrl window

class CArxBlockViewCtrl : public CArxGsViewCtrl
{
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxBlockViewCtrl* mpCtrl;
	public:
		CDocReactor( CArxBlockViewCtrl* pCtrl )
			: mpCtrl( pCtrl )
			{ acDocManager->addReactor( this ); }
		~CDocReactor()
			{ acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ mpCtrl->RefreshBlock(); }
		void documentDestroyed(const char* filename)
			{ mpCtrl->RefreshBlock(); }
	} mDocReactor;

	class CEdReactor : public AcEditorReactor 
	{
		CArxBlockViewCtrl* mpCtrl;
	public:
		CEdReactor( CArxBlockViewCtrl* pCtrl )
			: mpCtrl( pCtrl )
			{ acedEditor->addReactor( this ); }
		~CEdReactor()
			{ acedEditor->removeReactor( this ); }

		void endInsert( AcDbDatabase* pTo ) override
			{ if( pTo ) mpCtrl->RefreshBlock(); }
		void endDeepClone( AcDbIdMapping& idMap ) override
			{ mpCtrl->RefreshBlock(); }
		void commandEnded( LPCTSTR cmdStr ) override
			{ if( lstrcmp( cmdStr, _T("PURGE") ) == 0 ) mpCtrl->RefreshBlock(); }
	} mEdReactor;

private:
	AcDbDatabase* mpSourceDb;
	bool mbPreloaded;
	HCURSOR mhSavedCursor;
	HCURSOR mhArrowCursor;
	HCURSOR mhZoomCursor;
	HCURSOR mhPanCursor;
	HCURSOR mhCrossCursor;
	HCURSOR mhOrbitCursor;
#ifdef USE_ORBIT_GADGET
	OrbitGadget mOrbitGadget;
#endif
	bool mbZooming;
	bool mbPanning;
	bool mbOrbiting;
	CPoint mStartPt;
	double mdScale;
	double mdExtentsScale;
	double mdPreviousZoomScale;

public:
	CArxBlockViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxBlockViewCtrl();

// DialogControl Interface
public:
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override; //called to get drag data from this control

protected:
	void StartUIDrag();
	void EndUIDrag();
	virtual bool CanShowHighlight() const;
	virtual void PaintUI( CDC* pdc = NULL );
#ifdef USE_ORBIT_GADGET
	virtual void AddUIDrawable( AcGsModel* pModel, AcGsView* pView );
#endif
#if (_ARXTARGET >= 20 || _GRXTARGET >= 2023 || _ZRXTARGET >= 2025)
	virtual AcGiVisualStyle::Type GetVisualStyle();
#else
	virtual AcGsView::RenderMode GetRenderMode();
#endif

public:
	AcDbDatabase* GetSourceDb() { return mpSourceDb; }
	void Clear();
	bool GetBlockSize( LPCTSTR pszBlockName, AcDbExtents& ext );
	bool GetDwgSize( AcDbExtents& ext );
	void RefreshBlock();
	bool DisplayBlock( LPCTSTR pszBlockName );
	bool DisplayBlock( LPCTSTR pszBlockName, double dZoomFactor, bool bZoomExtents,
										 int nScaleType, const AcGeVector3d& vecViewDir );
	bool DisplayDwg( LPCTSTR pszFilename );
	bool DisplayDwg( LPCTSTR pszFilename, double dZoomFactor, bool bZoomExtents,
									 int nScaleType, const AcGeVector3d& vecViewDir );
	void DrawOrbitCircles( CDC* pDC = NULL );
	void DrawOrbitQuadCircle( CDC *pdc, int nX, int nY );
	bool PreLoadDwg( LPCTSTR pszFilename );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
