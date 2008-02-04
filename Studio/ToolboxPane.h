// ToolboxPane.h : header file
//

#pragma once

#include "PtrTypes.h"

class CStudioDialogObject;


#define ID_TOOLBAR 14
#define nDeToolTipLength 79
#define nDeButtonCount 35
#define nDeBitmapSize 21
#define nDeActiveXButton 28

#define nCommandIDLimit 0xf000


/////////////////////////////////////////////////////////////////////////////
// CToolboxPane window

class CToolboxPane : public CDialog
{
	bool mbToolSelected; // true = tool selected (cross cursor), false = arrow cursor selected
	CStudioDialogObject* mpActiveDlgObject;

protected:
	CToolBarCtrl	m_ToolBoxButtons;
	TBBUTTON		*m_pTBButtons;
	int				m_nSelectedCtrl;
	CLSID m_clsid;
	CString m_sLicenseKey;
	CString m_ActiveXFileName;
	int nButtonIndex;

// Construction
public:
	CToolboxPane();
	virtual ~CToolboxPane();

// Attributes
public:
	bool IsPointer() const { return !mbToolSelected; }
	ControlType GetSelectedTool() const;
	bool GetActiveXControlInfo( CLSID& clsid, CString& sLicenseKey, CString& sFilename ) const;

// Operations
public:
	void ResetToPointer();
	void ActivateDlgObject( CStudioDialogObject* pDlgObject );
	void ActivateDclControl( TDclControlPtr pDclControl );

protected:
	void UpdateTabStripToolUI();
	void AddTheButtons();
	void AddButton(UINT);
	void AddTheIcons();
	CString GetTooltipText( UINT nID );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolboxPane)
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	BOOL OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
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
	afx_msg void OnToolboxActivex();
	afx_msg void OnToolboxDwglist();
	afx_msg void OnToolboxAnimate();
	afx_msg void OnToolboxImagecombobox();
	afx_msg void OnToolboxGrid();
	afx_msg void OnToolboxSlitter();
	afx_msg void OnToolboxHatch();
};
