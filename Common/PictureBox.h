// PictureBox.h : header file
//

#pragma once

#include "AcadColorService.h"
#include "RefCountedPtr.h"
#include "PictureObject.h"

const COLORREF COLOR_TRANSPARENT = (COLORREF)-1;
const COLORREF COLOR_USEBACKGROUND = (COLORREF)0;


/////////////////////////////////////////////////////////////////////////////
// CPictureBox window

class CPictureBox : public CButton
{
	CAcadColorService mColorService;
	TPicturePtr mpPicture;
protected:
	int				m_cxIcon;
	int				m_cyIcon;
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
	virtual void OnMouseEntered() {}
	void DrawPicture( TPicturePtr, bool bStretchToFit = false );
	void SetPicture( TPicturePtr pPicture );
	void SetPicture( UINT nIconResId );
	void SetPictureBlank();
	void Clear();
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
	bool LoadPictureFile(LPCTSTR szFile, bool bStretch = false);
	void CopyDC();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};
