// ArxDwgPreviewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDwgPreviewCtrl window

class CArxDwgPreviewCtrl : public CButton, public CDialogControl
{
	CAcadColorService mColorService;
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

private:
	bool				m_bSelectedRect;
	COLORREF			m_HighlightColor;
	CString				m_filename;
	CRect				m_rcFocus;

public:
	CArxDwgPreviewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxDwgPreviewCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void PaintCtrl( CDC *pdc );
	LPCTSTR GetFilename() const { return (m_filename.IsEmpty()? NULL : (LPCTSTR)m_filename); }
	void Clear() { m_filename.Empty(); m_bSelectedRect = false; OnNeedRepaint(); }
	void SetHighlight( const COLORREF& clrHighlight );
	void RemoveHighlight();
	void Refresh( CDC *pdc );
	void LoadDwg( LPCTSTR pszFilename );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();	
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
