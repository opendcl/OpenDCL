// ArxGsViewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CArxGsViewCtrl window

class CArxGsViewCtrl : public CStatic, public CDialogControl
{
protected:
	CAcadColorService mColorService;
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

private:
	AcGsDevice* mpDevice;
	AcGsView* mpView;
	AcGsModel* mpModel;
	AcGsModel* mpGhostModel;
	bool mbLDblClick;
	bool mbRDblClick;
	COLORREF mclrHighlight;
	bool mbHighlighted;

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

protected:
	AcGsView* GetGsView() { return mpView; }
	virtual bool CanShowHighlight() const { return true; }
	virtual void PaintUI( CDC* pdc = NULL ) {}
	virtual void AddUIDrawable( AcGsModel* pModel, AcGsView* pView ) {}
	virtual AcGsView::RenderMode GetRenderMode() { return AcGsView::k2DOptimized; }
	void init(HMODULE hRes, bool bCreateModel = true);
	void erasePreview();
	bool GetActiveViewPortInfo( ads_real &height, ads_real &width, AcGePoint3d &target, AcGeVector3d &viewDir, ads_real &viewTwist, bool getViewCenter );
	void clearAll();
	bool UpdateModel( AcGiDrawable* pDrawable );

public:
	void SetHighlight( const COLORREF& clrHighlight );
	void RemoveHighlight();	
	void DisplayTheBlock( AcDbBlockTableRecord *pRec,
												double dZoomFactor, 
												bool   bZoomExtents,
												int	   nScaleType,		
												double dVectorX, 
												double dVectorY, 
												double dVectorZ,
												double dCameraX, 
												double dCameraY, 
												double dCameraZ );
	void Zoom( double dZfactor );

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
