// ToolBox.h : header file
//

#pragma once

#define ID_TOOLBAR 14
#define nDeToolTipLength 79
#define nDeButtonCount 35
#define nDeBitmapSize 21
#define nDeActiveXButton 28

#define nCommandIDLimit 0xf000


/////////////////////////////////////////////////////////////////////////////
// CToolBox window

class CToolBox : public CDialog
{
// Construction
public:
	CToolBox();
	
// Attributes
public:
	CToolBarCtrl	m_ToolBoxButtons; // project tree child control
	TBBUTTON		*m_pTBButtons;
	CView			*m_pActiveView;
	int				m_nSelectedCtrl;
// Operations
public:
	void ResetToPointer();
	void AddTheButtons();
	int nButtonIndex;
	void AddButton(UINT);
	void AddTheIcons();
	CString NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void SetActiveView(CView *pView);
	void EnableToolboxTabs(bool bEnabled);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBox)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolBox();

	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	// Generated message map functions
protected:
	//{{AFX_MSG(CToolBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolboxPointer();
	afx_msg void OnToolboxAngleslider();
	afx_msg void OnToolboxBlocklist();
	afx_msg void OnToolboxBlockview();
	afx_msg void OnToolboxCheckbox();
	afx_msg void OnToolboxCombobox();
	afx_msg void OnToolboxDwgpreview();
	afx_msg void OnToolboxFrame();
	afx_msg void OnToolboxGraphicbutton();
	afx_msg void OnToolboxHtml();
	afx_msg void OnToolboxLabel();
	afx_msg void OnToolboxListbox();
	afx_msg void OnToolboxListview();
	afx_msg void OnToolboxMonthpicker();
	afx_msg void OnToolboxOption();
	afx_msg void OnToolboxPicturebox();
	afx_msg void OnToolboxProgressbar();
	afx_msg void OnToolboxRectangle();
	afx_msg void OnToolboxScrollbar();
	afx_msg void OnToolboxSlider();
	afx_msg void OnToolboxSlideview();
	afx_msg void OnToolboxSpinbutton();
	afx_msg void OnToolboxTabs();
	afx_msg void OnToolboxTextbox();
	afx_msg void OnToolboxTextbutton();
	afx_msg void OnToolboxTree();
	afx_msg void OnToolboxUrllink();
	afx_msg void OnToolboxOptionList();
	virtual BOOL OnInitDialog();
	afx_msg void OnToolboxActivex();
	afx_msg void OnToolboxDwglist();
	afx_msg void OnToolboxAnimate();
	afx_msg void OnToolboxImagecombobox();
	afx_msg void OnToolboxGrid();
	afx_msg void OnToolboxSlitter();
	afx_msg void OnToolboxHatch();
	//}}AFX_MSG
	BOOL OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	void OnToolBarClicked( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );

	DECLARE_MESSAGE_MAP()
};
