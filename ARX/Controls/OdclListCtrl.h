// OdclListCtrl.h : header file
//

#pragma once

#include "PPTooltip.h"
#include "OleOdcDropTarget.h"

class CControlPane;
class CAcadDocReactor;
class CAcadBlockReactor;


/////////////////////////////////////////////////////////////////////////////
// CLVEdit 

class CLVEdit : public CEdit
{
// Construction
public:
	CLVEdit() :m_x(0),m_y(0) {}

// Attributes
public:
	int m_x;
	int m_y;
	CRect m_rcOldPos;
	CWnd *m_pParent;
	
// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLVEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLVEdit() {};

	// Generated message map functions
protected:
	//{{AFX_MSG(CLVEdit)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);	
	afx_msg void OnMove(int x, int y);	
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnSetfocus();	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// OdclListCtrl window

class OdclListCtrl : public CListCtrl
{
// Construction
public:
	OdclListCtrl();

// Attributes
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
	// Needed to make this control an OLE data SOURCE (see OnLButtonDown)
    COleDataSource m_COleDataSource;
    
	bool m_bInvokeWithSendString;
	CDclControlObject	*m_ArxControl;
	CAcadDocReactor		*m_pDocToModReactor;
	CAcadBlockReactor	*m_pBlockReactor;	
	CImageList			*m_pBlockImageList;
	CControlPane		*m_pParentCtrlPane;
	CString				m_sProjectName;
	CString				m_sDialogName;
	int					m_nEditSubItem;
	CLVEdit				m_LVEdit;
	CImageList			m_DefaultImageList;
	bool				m_bEditCells;

	// BlockList Attributes
	AcDbDatabase		*m_pLoadedDwg;
	CString				m_FileName;
	
	
// Operations
public:
	bool LoadDwg(CString sFileName);
	void SetAcadColor(long nColor);
	void RefreshBlockList();
	int extractPreview(const AcDbBlockTableRecord* pBTR);
	void SetDragnDrop(BOOL bRegister);
	BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1);
	bool SortNumericItems( int nCol, BOOL bAscending,int low = 0, int high = -1);
	int GetItemImage(int nRow, int nCol);
	void SetItemImage( int nRow, int nCol, int nImage);
	void SetItemTextImage(int nRow, int nCol, CString sText, int nImage);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OdclListCtrl)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~OdclListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(OdclListCtrl)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnOdstatechanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
