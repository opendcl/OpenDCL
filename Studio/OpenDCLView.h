// OpenDCLView.h : interface of the COpenDCLView class
//

#pragma once

#include "ControlGripper.h"
#include "UndoActions.h"
#include "SelectedControl.h"
#include "OpenDCLDoc.h"
#include "Resource.h"


class CFontCollection;
class CDclFormObject;
class CControlHolder;
enum ControlType;
enum Prop::Id;


#define IDC_CONTROLHOLDER 220
#define nBtnHeight 25
#define nBtnWidth 26
#define PACKVERSION(major,minor) MAKELONG(minor,major)


#define nModalSpace -22
#define nWindowColor -6
#define nButtonFace -16
#define nButtonText -19
#define nBlueColor	  5
#define nDefontSize	  8
#define nDeColWidth	 25
#define nDeDropHeight 100
#define nDeIconSpacing 20
#define nDeIconHeight  32
#define nDeTreeIndent  17
#define nDeLargeChange  5
#define nDeTextLimit  36000
#define nDeTextLimitCB  256
#define nDeMaxVal	    100
#define nDeMinTabWidth   50
#define nDeRenderMode	  5
#define nDeRowHeight	 17
#define nDeRowHeightDwg 120
#define nDeTickFrequency 10
#define nDeGridIDCounter 80
#define nDeMoveBy50		 50
#define nDeStartupSize	300

#define nSizingRectThickness  1
#define nMinGridSpacing  4
#define nInitialGridSpacing  8
#define nControlStartId  100
#define nControlOffset  8

#define nm100 -100
#define nm99 -99


typedef CList<CSelectedControl*> CSelectedCol;
typedef CList<CUndoActions*> CUndoCol;


class COpenDCLView : public CView
{
	CControlGripper mGripper; //this is the one and only "hot" resizing gripper for the active control

protected: // create from serialization only
	COpenDCLView();
	DECLARE_DYNCREATE(COpenDCLView)

// Attributes
public:
	void HideSizingRect();
	CSelectedControl m_SelectedControl;
	CSelectedCol	 m_SelectedList;
	CUndoCol		 m_UndoList;
	CLSID m_clsid;
	CString m_sBaseCode;
	CString m_sLicenseKey;
	CSize m_StartupSize;
	bool m_StandardCursorID; // true = cross, false = arrow	
	short m_gridSpacing;
	CString m_ActiveXFileName;
	CString m_DclFormName;
	
public:
	int GetNextControlId();
	CRect GetSplitterRect(int nId);
	void CalcControlOffsetDistances(CDclControlObject *pArxObject, const CRect &rcCtrl);
	void MoveThisInPosition();
	void DrawActiveXCtrl(CDclControlObject *pCtrl, CDC* pDC);
	CDclControlObject* InsertControl(CRect rcPos, ControlType nCtrlToInsert);
	void PreDestroy();
	void OnGridSpacingChanged();
	void OnControlToInsertChanged();
	void OnDclFormNameChange();
	void ClearControls();
	void CopyControlToClipBoard();
	void DeleteSelectedControls();
	void PasteFromClipBoard();
	void RefreshSelectControl();
	void ZOrdedSelectedControls(short direction);
	short GetSelectedTabClientWidth();
	short GetSelectedTabClientHeight();
	void DisplayControls(CDclFormObject *pDclForm);
	short GetSelectedArxIndex();
	long GetSelectedType();
	void OnLostFocus();
	BOOL CanUndo();
	void UndoAction();
	short ResetDclFormIndex();
	void CheckPictureRefs();
	void SelectControl(short nArxControlIndex);
	void SelectControl(CString sName);
	void CalcAllOffsets();
	void RefreshControlsWithPictures();
	void ZOrderUpdateOfSelCtrl(short ZIndex);
	void UpdateZOrderList();
	void UpdateFont(CString sName);
	void UpdateFontSize(int nSize);
	void UpdateFontBool(bool bBool, Prop::Id nId);
	void UpdateAxFont(CSelectedControl *pSelControl, int nId, bool bBool, int nSize = 0, CString sName = "");
	// tab pane functions
	bool IsTabsEnabled();
	void ResizeChildTabPanes();
	CDclFormObject * AddSingleTabPane(int nIndex);
	void RemoveChildTabPane(CDclFormObject *pDclForm);
	bool CanRemoveChildTabPane(int nIndex);


// attributes
public:
	bool	m_bDrawing;
	CRect	m_rcDrawLast;
	CRect	m_rcDraw;
	CPoint	m_DrawStartPoint;
	CDclFormObject *m_pThisDclForm;
	bool	m_MouseDown;
	bool	m_bMoving;
	int		m_nResizeQuadrant;
	int		m_ControlId;
	bool	m_bShowGrip;
	CRect	m_rcGripRect;
	CSize				m_szGripSize;
	CFontCollection *m_pFontCollection;
	BOOL m_ControlCreated;

// drag, resize and move rectangle related operations
public:
	void SetupDragResize(int nQuadrant);	
	void DragResized(int nQuadrant, CPoint point);
	void CompletedDragResize(int nQuadrant, CPoint point);
	void RemoveDragRect(CSelectedControl *pSelectedControl);
	void AllRemoveDragRects();
	int CheckGripsMouseMoveOver(CPoint point);
	void MoveGripsToTop() { mGripper.MoveToTop(); }

public:
	void ResizeParentToFit(BOOL bShrinkOnly);
	void UpdateClientHeight(CDclControlObject* pArxObject, CWnd *pControl);
	void OnMouseButtonDown(UINT nFlags, CPoint point);
	void AdjustOffsets(int cx, int cy);
	CSize GetControlSize(CWnd *pControl, int nControlType);
	CRect CalcResizeRect(int nQuadrant, CPoint point);
	CControlHolder* AddCWndControl(CDclControlObject* pDclControl, CRect rcPos, bool bForceUpdate);
	void NullDrawRect();
	void PaintBackGround(HDC hdc);
	int RoundToGridPattern(int Value);
	CString FindNextControlName(CString sControlName);
	int GetDclFormIndex();
	bool CheckControlsForSelection(CRect rcSelArea, bool bLookForOne);
	bool CheckControlsForSelection(CPoint point, UINT nFlags, bool bMouseDown = false);
	void MoveControl(CSelectedControl *pSelectedControl, CPoint point);
	bool StoreControlsPosition(CSelectedControl *pSelectedControl);
	void ClearSelection(bool bResetZOrder = true);
	bool IsInSelection(CDclControlObject *pArxObject);
	void ShowSelection();
	void ClearChildControls( CControlHolder* pControlHolder );
	void ClearUndoList();
	void UpdateGripPos();
	void ClearEventProperties(CDclControlObject *pCtrl);
// control creation operations
public:
	void InsertControl		(CRect rcPos);

public:
	void RefreshChildControl(CDclControlObject *pArxObject, Prop::Id ChangedPropertyID);
	void ResizeChildControl(CDclControlObject *pArxObject);
	void UpdateControl(CDclControlObject *pArxObject, Prop::Id ChangedPropertyID);

	void FireControlSelected(CDclControlObject *pControl);
	void FireShowFormGrips(BOOL bVisible);
	void FirePopUpMenuPlease(long cx, long cy);
	void FireTabResized(short nClientHeight);
	void FireMouseDownEvent();
	void FireTabControlDeleted();
	void FireSetUndo();
	void FireControlInserted(CDclControlObject *pControl, long ControlType);
	void FireShowPropertyWizard(CDclControlObject *pControl);

// default font members
public:
	CString m_sDefaultFontName;
	int m_sDefaultFontSize;
	BOOL m_sDefaultFontItalic;
	BOOL m_sDefaultFontUnderLine;
	BOOL m_sDefaultFontBold;

// Overrides	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenDCLView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL
// ActiveX releated operations
public:
	
// Implementation
public:
	virtual ~COpenDCLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
public:
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnEditCopy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnGripStartDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGripDragging(WPARAM wParam, LPARAM lParam);

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);		
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBringtofront();
	afx_msg void OnSendtoback();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnEditObjectbrowser();
	afx_msg void OnToolsSetlispsymbolnames();
	afx_msg void OnToolsClearlispsymbolnames();
	afx_msg void OnUpdateToolsSetlispsymbolnames(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsClearlispsymbolnames(CCmdUI *pCmdUI);
};
