// ToolboxPane.h : header file
//

#pragma once

#include "PtrTypes.h"

class CStudioDialogObject;


/////////////////////////////////////////////////////////////////////////////
// CToolboxPane window

class CToolboxPane : public CDialog
{
	bool mbToolSelected; // true = tool selected (cross cursor), false = arrow cursor selected
	CStudioDialogObject* mpActiveDlgObject;
	CToolBarCtrl mToolbar;
	int mnSelectedCtrl;

	//ActiveX control
	CLSID mAxClsid;
	CString msAxFileName;
	CString msAxLicenseKey;

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
	void UpdateToolUI();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
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
