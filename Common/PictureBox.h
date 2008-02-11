// PictureBox.h : header file
//

#pragma once

#include "AcadColorService.h"

class CPictureObject;

const COLORREF COLOR_TRANSPARENT = (COLORREF)-1;
const COLORREF COLOR_USEBACKGROUND = (COLORREF)0;


/////////////////////////////////////////////////////////////////////////////
// CPictureBox window

class CPictureBox : public CButton
{
	CAcadColorService mColorService;
	CPictureObject* mpPicture;
protected:
	int				m_cxIcon;
	int				m_cyIcon;
	BOOL			m_bMouseTracking;
	LPPICTURE		m_pPictureHolder;
	HBITMAP			m_hbmMem;
	bool			m_bStretchLoadedPicture;

public:
	CPictureBox( CWnd* pParentWnd, UINT nID, const CRect& rcWnd, UINT nIconResId = -1 );
	CPictureBox();
	virtual ~CPictureBox();

public:
	virtual bool IsAutoSized() { return false; }
	virtual void AutoSize() {}
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	void Refresh(CDC *pdc);
	void SetPicture( CPictureObject* pPicture );
	void SetPictureBlank();
	void Refresh();
	void DrawLine(int sX, int sY, int eX, int eY, COLORREF rgb);
	void DrawPoint(int nX, int nY, COLORREF rgb);
	void DrawArc(int sX, int sY, int eX, int eY, int saX, int saY, int eaX, int eaY, COLORREF rgb);
	void DrawCircle(int sX, int sY, int eX, int eY, COLORREF rgb);
	void DrawFillRect(int sX, int sY, int eX, int eY, COLORREF rgb);
	void DrawHatchRect(int sX, int sY, int eX, int eY, COLORREF rgb, int nHatchPattern);
	int  DrawWrappedText(int sX, int sY, int eX, COLORREF crFore, COLORREF crBack, CString sText, CString sJustification);
	void DrawText(int sX, int sY, COLORREF crFore, COLORREF crBack, CString sText, CString sJustification, int nEnabled);
	void DrawEdge(int sX, int sY, int eX, int eY, int nEdge);
	void DrawFocusRect(int sX, int sY, int eX, int eY);
	void DrawRect(int sX, int sY, int eX, int eY, COLORREF rgb);
	void GetTextExtent(LPCTSTR pszText, CSize& ext);
	void LoadPictureFile(LPCTSTR szFile, bool bStretch = false);
	void CopyDC();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};
