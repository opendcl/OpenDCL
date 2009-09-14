// ArxGsViewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "Workspace.h"
#include "AcadColorService.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadBlockInsertDropTarget.h"


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
	CAcadBlockInsertDropTarget mBlockInsertDropTarget;
	CArxDragDropService mDragDropService;

	class GsViewReactor : public AcGsReactor
	{
		AcDbDatabase* mpDb;
		CArxGsViewCtrl* mpCtrl;
		AcGsManager* mpManager;
		AcGsClassFactory* mpFactory;
		AcGsDevice* mpDevice;
		AcGsModel* mpModel;
		AcGsModel* mpGhostModel;
		AcGsView* mpView;
	public:
		GsViewReactor( CArxGsViewCtrl* pCtrl, AcDbDatabase* pDb )
			: mpDb( pDb )
			, mpCtrl( pCtrl )
			, mpManager( NULL )
			, mpDevice( NULL )
			, mpModel( NULL )
			, mpGhostModel( NULL )
			, mpView( NULL )
			{
				assert( mpCtrl != NULL );
				if( pDb )
				{
					mpManager = acgsGetGsManager();
					assert( mpManager != NULL );
					if( !mpManager )
						return;
					mpFactory = mpManager->getGSClassFactory();
					assert( mpFactory != NULL );
					if( !mpFactory )
						return;
					mpFactory->addReactor( this );
					//a device with standard autocad color palette
					mpDevice = mpManager->createAutoCADDevice( mpCtrl->m_hWnd );
					TPropertyPtr pAcadColor = mpCtrl->GetTemplate()->GetPropertyObject(Prop::BackgroundColor);
					if( pAcadColor )
					{
						AcGsColor color = mpDevice->getBackgroundColor();
						COLORREF aColor = GetRGBColor( pAcadColor->GetLongValue() );
						//AcGsColor color;
						color.m_red = GetRValue( aColor );
						color.m_green = GetGValue( aColor );
						color.m_blue = GetBValue( aColor );
						mpDevice->setBackgroundColor( color );
					}	
				      
					CRect rect;
					mpCtrl->GetClientRect( &rect);
					mpDevice->onSize( rect.Width(), rect.Height() );   
					//a simple view
					mpView = mpFactory->createView();
					mpModel = mpManager->createAutoCADModel(); //a model with open/close protocol

					//another model without open/close for the orbit gadget
					mpGhostModel = mpFactory->createModel( AcGsModel::kDirect, 0, 0, 0 );
					mpCtrl->AddUIDrawable( mpGhostModel, mpView );
					mpDevice->add( mpView );
					mpView->setView( AcGePoint3d(), AcGePoint3d( 0, 0, -1 ), AcGeVector3d( 0, 1, 0 ), 0.01, 0.01 );
				}
			}
		~GsViewReactor()
			{
				clear();
			}
	protected:
		void clear()
			{
				if( mpDb )
				{
					if( mpManager )
					{
						if( mpFactory )
						{
							mpFactory->removeReactor( this );
							if( mpView )
							{
								mpFactory->deleteView( mpView );
								mpView = NULL;
							}
							if( mpGhostModel )
							{
								mpFactory->deleteModel( mpGhostModel );
								mpGhostModel = NULL;
							}
							mpFactory = NULL;
						}
						if( mpModel )
						{
							mpManager->destroyAutoCADModel( mpModel );
							mpModel = NULL;
						}
						if( mpDevice )
						{
							mpManager->destroyAutoCADDevice( mpDevice );
							mpDevice = NULL;
						}
						mpManager = NULL;
					}
				}
				mpDb = NULL;
				mpCtrl->Invalidate();
			}
	public:
		AcDbDatabase* database() const { return mpDb; }
		void setDrawable( AcGiDrawable* pDrawable )
			{
				if( mpView )
				{
					mpView->eraseAll();
					if( pDrawable )
						mpView->add( pDrawable, mpModel );
				}
			}
		void zoom( double dblZoomFactor )
			{
				if( mpView )
					mpView->zoom( dblZoomFactor );
			}
		void onSize( int cx, int cy )
			{
				if( mpDevice )
					mpDevice->onSize( cx, cy );
			}
		AcGsView* GetGsView() const { return mpView; }
		AcGsModel* GetGsModel() const { return mpModel; }
		AcGsDevice* GetGsDevice() const { return mpDevice; }
	protected:
		virtual void gsToBeUnloaded( AcGsClassFactory* pClassFactory )
			{
				assert( mpFactory == pClassFactory );
				clear();
			}
	};
	GsViewReactor* mpGsReactor;

private:
	enum btnstate { up = 0, down = 1, dblclk = 2, };
	btnstate mLBState;
	btnstate mRBState;
	COLORREF mclrHighlight;
	bool mbHighlighted;

public:
	CArxGsViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxGsViewCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual COleDropTarget* GetDropOnAcadTarget() { return &mBlockInsertDropTarget; }
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyBackgroundColor( TPropertyPtr pProp );

protected:
	AcGsView* GetGsView() { return (mpGsReactor? mpGsReactor->GetGsView() : NULL); }
	AcGsModel* GetGsModel() { return (mpGsReactor? mpGsReactor->GetGsModel() : NULL); }
	AcGsDevice* GetGsDevice() { return (mpGsReactor? mpGsReactor->GetGsDevice() : NULL); }
	virtual bool CanShowHighlight() const { return true; }
	virtual void PaintUI( CDC* pdc = NULL ) {}
	virtual void AddUIDrawable( AcGsModel* pModel, AcGsView* pView ) {}
	virtual AcGsView::RenderMode GetRenderMode() { return AcGsView::k2DOptimized; }
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
