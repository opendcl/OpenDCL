// ObjectDCLView.h : interface of the CObjectDCLView class
//

#pragma once

#include "GripRect.h"
#include "UndoActions.h"
#include "SelectedControl.h"
#include "ObjectDCLDoc.h"
#include "Resource.h"


class CFontCollection;
class CDclFormObject;
class CControlHolder;
enum ControlType;
enum PropertyId;


#define IDC_CONTROLHOLDER 220
#define nGripSize 7
#define nBtnHeight 25
#define nBtnWidth 26
#define PACKVERSION(major,minor) MAKELONG(minor,major)


#define nRefreshAllProperties -2

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
#define nDeRoundRangeMin  -179
#define nDeRoundRangeMax   180
#define nDeZeroAngle   90

#define nSizingRectThickness  1
#define nMinGridSpacing  4
#define nInitialGridSpacing  8
#define nControlStartId  100
#define nControlOffset  8

#define nComboStyle12 12
#define nComboDropHeight 300

#define nm100 -100
#define nm99 -99

#define nDeGridSpacing 8
#define nNotSet -1


typedef CList<CSelectedControl*> CSelectedCol;
typedef CList<CUndoActions*> CUndoCol;


class CObjectDCLView : public CView
{
protected: // create from serialization only
	CObjectDCLView();
	DECLARE_DYNCREATE(CObjectDCLView)

public:
	//{{AFX_DATA(CObjectDCLView)
	enum{ IDD = IDD_OBJECTDCL_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	BOOL m_bGripsCreate;
	CGripRect m_GripRect1;
	CGripRect m_GripRect2;
	CGripRect m_GripRect3;
	CGripRect m_GripRect4;
	CGripRect m_GripRect5;
	CGripRect m_GripRect6;
	CGripRect m_GripRect7;
	CGripRect m_GripRect8;
	void HideSizingRect();
	void MoveGripRectsForward();
	CSelectedControl m_SelectedControl;
	CSelectedCol	 m_SelectedList;
	CUndoCol		 m_UndoList;
	CLSID m_clsid;
	CString m_sBaseCode;
	CString m_sLicenseKey;
	CSize m_StartupSize;
	//CObjectDCLDoc* GetDocument() const;	
	bool m_StandardCursorID; // true = cross, false = arrow	
	short m_gridSpacing;
	CString m_ActiveXFileName;
	CString m_DclFormName;
	
public:
	int GetNextControlId();
	CRect GetSplitterRect(int nId);
	void CalcControlOffsetDistances(CDclControlObject *pArxObject, CRect &rcCtrl);
	void MoveThisInPosition();
	void DrawActiveXCtrl(CDclControlObject *pCtrl, CDC* pDC);
	void InsertControl(CRect rcPos, ControlType nCtrlToInsert);
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
	void UpdateControl(CDclControlObject *pArxObject, PropertyId ChangedPropertyID);
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
	void UpdateFontBool(bool bBool, PropertyId nId);
	void UpdateAxFont(CSelectedControl *pSelControl, int nId, bool bBool, int nSize = 0, CString sName = "");
	// tab pane functions
	//void AddTabPanes();
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

public:
	void ResizeParentToFit(BOOL bShrinkOnly);
	void UpdateClientHeight(CDclControlObject* pArxObject, CWnd *pControl);
	void OnMouseButtonDown(UINT nFlags, CPoint point);
	void AdjustOffsets(int cx, int cy);
	CSize GetControlSize(CWnd *pControl, int nControlType);
	CRect CalcResizeRect(int nQuadrant, CPoint point);
	CWnd* AddCWndControl(CDclControlObject* pDclControl, CRect rcPos, bool bForceUpdate);
	void NullDrawRect();
	void PaintBackGround(HDC hdc);
	int RoundToGridPattern(int Value);
	CString FindNextControlName(CString sControlName);
	int GetDclFormIndex();
	bool CheckControlsForSelection(CRect rcSelArea, bool bLookForOne);
	bool CheckControlsForSelection(CPoint point, UINT nFlags, bool bMouseDown = false);
	void ShowGripRects(BOOL bShow, CRect rcCtrl);
	void MoveControl(CSelectedControl *pSelectedControl, CPoint point);
	void HideGrips();
	bool StoreControlsPosition(CSelectedControl *pSelectedControl);
	void ClearSelection(bool bResetZOrder = true);
	bool IsInSelection(CDclControlObject *pArxObject);
	void ShowSelection();
	void ClearChildControls(CControlHolder *pParentControl);
	void ClearUndoList();
	void UpdateGripPos();
	void ClearEventProperties(CDclControlObject *pCtrl);
// control creation operations
public:
	void InsertControl		(CRect rcPos);
	bool CreateChildControl	(CControlHolder *pParent, CDclControlObject *pDclControl, bool bForceUpdate = false);
	void CreateHatch		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateLabel		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateImageComboBox(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateAnimate		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateButton		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateGraphicButton(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateFrame		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateTextBox		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateCheckBox		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateOptionButton	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateComboBox		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateMonthCal		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateListBox		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateDwgDirList	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateScrollBar	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateSlider		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreatePictureBox	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateTabStrip		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateTree			(CControlHolder *pParent, CDclControlObject *pArxObject);
	void Create3DRect		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateProgress		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateSpinButton	(CControlHolder *pParent, CDclControlObject *pArxObject);	
	void CreateStaticURL	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateRoundSlider	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateGrid			(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateBlockView	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateSlideView	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateDwgThumbNail	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateDwgPreview	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateHtmlCtrl		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateListView		(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateBlockList	(CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateOptionList	(CControlHolder *pParent, CDclControlObject *pArxObject);
	bool CreateActiveX		(CControlHolder *pParent, CDclControlObject *pArxObject, bool bForceUpdate);
	void CreateFileDlgCtrls (CControlHolder *pParent, CDclControlObject *pArxObject);
	void CreateSplitter		(CControlHolder *pParent, CDclControlObject *pArxObject);

	void SetupTreeControl	(CTreeCtrl *pControl);
	void AddHiddenProperties(CDclControlObject *pArxObject, short nType, CString Name);
	void AddProperties(CDclControlObject *pDclControl);

public:
	void UpdateProperty(PropertyId nID, CDclControlObject *pArxObject, CControlHolder *pParent);
	void RefreshChildControl(CDclControlObject *pArxObject, PropertyId ChangedPropertyID);
	void ResizeChildControl(CDclControlObject *pArxObject);
	void CheckAutoSizeProp(CDclControlObject *pArxObject, CControlHolder *pParent);
	void ResetImageList(CWnd *pControl, int nID, CDclControlObject *pArxObject);
	void UpdateChildControl(CDclControlObject *pArxObject, CControlHolder *pParent);

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
	//{{AFX_VIRTUAL(CObjectDCLView)
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
	virtual ~CObjectDCLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CObjectDCLView)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
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
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnEditCopy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnEditObjectbrowser();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnToolsSetlispsymbolnames();
	afx_msg void OnToolsClearlispsymbolnames();
	afx_msg void OnUpdateToolsSetlispsymbolnames(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsClearlispsymbolnames(CCmdUI *pCmdUI);
};
