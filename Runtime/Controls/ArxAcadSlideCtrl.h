// ArxAcadSlideCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadSld.h"
#include "PPToolTip.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadColorService.h"

class CControlPane;


/////////////////////////////////////////////////////////////////////////////
// CArxAcadSlideCtrl window

class CArxAcadSlideCtrl : public CButton, public CDialogControl
{
	CArxControlServices	mArxServices;
	CAcadColorService mColorService;
	CArxDragDropService mDragDropService;
	class CArxSlide : public CAcadSld
	{
		CDialogControl& mCtrl;
	public:
		CArxSlide( CDialogControl& Ctrl ) : mCtrl( Ctrl ) {}
	protected:
		virtual COLORREF getBackgroundColor() const;
	} mArxSlide;
	bool mbTrackingMouse;
	HBITMAP mhbmLast;
	HBITMAP mhbmSaved;

public:
	bool				m_bSelectedRect;
	COLORREF			m_HighlightColor;

// Construction
public:
	CArxAcadSlideCtrl( CControlPane& Pane, TDclControlPtr pTemplate, UINT nID, bool bCreate = true );
	virtual ~CArxAcadSlideCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyCaption( TPropertyPtr pProp ) { return true; }
	virtual CAcadColorService* GetColorService() { return &mColorService; }

// Implementation
public:
	void DrawLine(int sX, int sY, int eX, int eY, const COLORREF& rgb);
	void DrawFillRect(int sX, int sY, int eX, int eY, const COLORREF& rgb);
	void SetHighlight(const COLORREF& rgb);
	void DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, LPCTSTR pszFilename, LPCTSTR pszSlideName);
	void RemoveHighlight();
	bool SetFileName( LPCTSTR pszFilename, LPCTSTR pszSlide );
	void Clear();
	void Snapshot();
private:
	void SaveDC();
	void PaintControl(CDC *pdc);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnPaint();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void PostNcDestroy();

protected:
	DECLARE_MESSAGE_MAP()
};
