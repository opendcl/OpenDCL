// ArxGsViewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "Workspace.h"
#include "AcadColorService.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadBlockInsertDropTarget.h"

// [Owen Wengerd 2021-12-02]
// Orbit gadget is disabled due to unsolvable crashes on Acad caused by some link
// from the gs model used for the orbit gadget with the document current at the
// time the model was created. This caused Acad to crash if the view control was
// cleared after the connected document had already been closed.
//#define USE_ORBIT_GADGET


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


#if defined(_GRXTARGET) && (_GRXTARGET <= 2022)
#define GsColorConvertor(x) x
#else
class GsColorConvertor
{
	AcGsColor mGsColor;
public:
	GsColorConvertor(COLORREF color)
	{
		mGsColor.m_red = GetRValue(color);
		mGsColor.m_green = GetGValue(color);
		mGsColor.m_blue = GetBValue(color);
	}
	operator const AcGsColor& ()
	{
		return mGsColor;
	}
};
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

	class GsViewManager : public AcDbDatabaseReactor, public AcGsReactor, public AcEditorReactor
	{
		AcDbDatabase* mpDb;
		CArxGsViewCtrl* mpCtrl;
	#if (_ACADTARGET >= 20)
		AcGsGraphicsKernel* mpKernel;
	#else
		AcGsClassFactory* mpFactory;
	#endif
		AcGsDevice* mpDevice;
		AcGsModel* mpModel;
	#ifdef USE_ORBIT_GADGET
		AcGsModel* mpGhostModel;
	#endif
		AcGsView* mpView;
	public:
		GsViewManager( CArxGsViewCtrl* pCtrl, AcDbDatabase* pDb )
			: mpDb( pDb )
			, mpCtrl( pCtrl )
		#if (_ACADTARGET >= 20)
			, mpKernel( NULL )
		#else
			, mpFactory( NULL )
		#endif
			, mpDevice( NULL )
			, mpModel( NULL )
		#ifdef USE_ORBIT_GADGET
			, mpGhostModel( NULL )
		#endif
			, mpView( NULL )
			{
				acedEditor->addReactor(this);
				assert( mpCtrl != NULL );
				if( pDb )
				{
					mpDb->addReactor(this);
					AcGsManager* pManager = acgsGetGsManager();
					assert( pManager != NULL );
					if( !pManager )
						return;
				#if (_ACADTARGET >= 20)
					AcGsKernelDescriptor KernelDesc;
					KernelDesc.addRequirement(AcGsKernelDescriptor::k3DDrawing);
					mpKernel = pManager->acquireGraphicsKernel( KernelDesc );
					assert( mpKernel != NULL );
					if( !mpKernel )
						return;
				#ifndef _BRXTARGET
					mpKernel->addReactor( this );
				#endif
					//a device with standard autocad color palette
					mpDevice = pManager->createAutoCADDevice( *mpKernel, mpCtrl->m_hWnd );
				#else
					mpFactory = pManager->getGSClassFactory();
					assert( mpFactory != NULL );
					if( !mpFactory )
						return;
				#ifndef _BRXTARGET
					mpFactory->addReactor( this );
				#endif
					//a device with standard autocad color palette
					mpDevice = pManager->createAutoCADDevice( mpCtrl->m_hWnd );
				#endif
					TPropertyPtr pAcadColor = mpCtrl->GetTemplate()->GetPropertyObject(Prop::BackgroundColor);
					if( pAcadColor )
						mpDevice->setBackgroundColor( GsColorConvertor( GetRGBColor( pAcadColor->GetLongValue() ) ) );

					CRect rect;
					mpCtrl->GetClientRect( &rect);
					onSize( rect.Width(), rect.Height() );
					//a simple view
				#if (_ACADTARGET >= 20)
					mpView = mpKernel->createView();
					mpModel = pManager->createAutoCADModel( *mpKernel ); //a model with open/close protocol

				#ifdef USE_ORBIT_GADGET
					//another model without open/close for the orbit gadget
					mpGhostModel = mpKernel->createModel( AcGsModel::kDirect, 0, 0, 0 );
				#endif //USE_ORBIT_GADGET
				#else
					mpView = mpFactory->createView();
					mpModel = pManager->createAutoCADModel(); //a model with open/close protocol

				#ifdef USE_ORBIT_GADGET
					//another model without open/close for the orbit gadget
					mpGhostModel = mpFactory->createModel( AcGsModel::kDirect, 0, 0, 0 );
				#endif //USE_ORBIT_GADGET
				#endif
				#ifdef USE_ORBIT_GADGET
					mpCtrl->AddUIDrawable( mpGhostModel, mpView );
				#endif //USE_ORBIT_GADGET
					mpDevice->add( mpView );
					mpView->setView( AcGePoint3d(), AcGePoint3d( 0, 0, -1 ), AcGeVector3d( 0, 1, 0 ), 0.01, 0.01 );
				}
			}
		~GsViewManager()
			{
				clear();
				acedEditor->removeReactor(this);
			}
	protected:
		void clear()
			{
				AcGsManager* pManager = acgsGetGsManager();
				if( pManager )
				{
					if( mpModel )
					{
						pManager->destroyAutoCADModel( mpModel );
						mpModel = NULL;
					}
					if( mpDevice )
					{
						if( mpView )
						{
						#if !defined(_ZRXTARGET) && !(defined(_ARXTARGET) && (_ARXTARGET <= 17))
							mpView->eraseAll(); //causes crash in AutoCAD 2007 and earlier [ORW 2012-07-20] //and ZWCAD+ [ORW 2014-06-20]
						#endif
							mpDevice->erase( mpView );
						}
						pManager->destroyAutoCADDevice( mpDevice );
						mpDevice = NULL;
					}
				}
			#if (_ACADTARGET >= 20)
				if( mpKernel )
				{
				#ifndef _BRXTARGET
					mpKernel->removeReactor( this );
				#endif
					if( mpView )
					{
						mpKernel->deleteView( mpView );
						mpView = NULL;
					}
				#ifdef USE_ORBIT_GADGET
					if( mpGhostModel )
					{
						mpKernel->deleteModel( mpGhostModel );
						mpGhostModel = NULL;
					}
				#endif //USE_ORBIT_GADGET
					mpKernel->delRef();
					mpKernel = NULL;
				}
			#else
				if( mpFactory )
				{
				#ifndef _BRXTARGET
					mpFactory->removeReactor( this );
				#endif
					if( mpView )
					{
						mpFactory->deleteView( mpView );
						mpView = NULL;
					}
				#ifdef USE_ORBIT_GADGET
					if( mpGhostModel )
					{
						mpFactory->deleteModel( mpGhostModel );
						mpGhostModel = NULL;
					}
				#endif //USE_ORBIT_GADGET
					mpFactory = NULL;
				}
			#endif
				if( mpDb )
					mpDb->removeReactor(this);
				mpDb = NULL;
				mpCtrl->OnNeedRepaint( false );
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
				if( cx <= 0 || cy <= 0 )
					return;
				if( mpDevice )
					mpDevice->onSize( cx, cy );
			}
		AcGsView* GetGsView() const { return mpView; }
		AcGsModel* GetGsModel() const { return mpModel; }
		AcGsDevice* GetGsDevice() const { return mpDevice; }
	protected:
		virtual void goodbye( const AcDbDatabase* dwg )
			{
				if( mpDb == dwg )
					clear();
			}
		virtual void viewToBeDestroyed( AcGsView* pView )
			{
				if( mpView == pView )
					clear();
			}
	#if defined(_GRXTARGET) && (_GRXTARGET <= 2022)
		virtual void gsToBeUnloaded( OdGsModule* pModule )
			{
				assert( pModule != NULL );
				clear();
			}
	#elif (_ACADTARGET >= 20)
		virtual void gsToBeUnloaded( AcGsGraphicsKernel* pKernel )
			{
				assert( mpKernel == pKernel );
				clear();
			}
	#else
		virtual void gsToBeUnloaded( AcGsClassFactory* pClassFactory )
			{
				assert( mpFactory == pClassFactory );
				clear();
			}
	#endif
		virtual void quitWillStart() //only reached in Bricscad v14.2.06 and later
			{
			#if defined(_GRXTARGET) || (_ACADTARGET >= 20)
				clear();
			#else
				// The cached pointers are no longer valid when this reactor fires, so ensure
				// they are not used when the control is destroyed.
				mpFactory = NULL;
			#endif
			}
	};
	GsViewManager* mpGsManager;

private:
	enum btnstate { up = 0, down = 1, dblclk = 2, };
	btnstate mLBState;
	btnstate mRBState;
	COLORREF mclrHighlight;

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
	AcGsView* GetGsView() { return (mpGsManager? mpGsManager->GetGsView() : NULL); }
	AcGsModel* GetGsModel() { return (mpGsManager? mpGsManager->GetGsModel() : NULL); }
	AcGsDevice* GetGsDevice() { return (mpGsManager? mpGsManager->GetGsDevice() : NULL); }
	virtual bool CanShowHighlight() const { return true; }
	virtual bool CanShowFocus() const { return false; }
	virtual void PaintUI( CDC* pdc = NULL ) {}
#ifdef USE_ORBIT_GADGET
	virtual void AddUIDrawable( AcGsModel* pModel, AcGsView* pView ) {}
#endif
#if (_ACADTARGET >= 20)
	virtual AcGiVisualStyle::Type GetVisualStyle() { return AcGiVisualStyle::kFlat; }
#else
	virtual AcGsView::RenderMode GetRenderMode() { return AcGsView::k2DOptimized; }
#endif
	void clearAll();
	bool UpdateModel( AcGiDrawable* pDrawable );

public:
	void SetHighlight( const COLORREF& clrHighlight );
	void RemoveHighlight();
	void DisplayBTR( AcDbBlockTableRecord* pBTR, double dZoomFactor,
									 bool bZoomExtents, int nScaleType,
									 const AcGeVector3d& vecViewDir );
	void Zoom( double dZoomFactor );

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
