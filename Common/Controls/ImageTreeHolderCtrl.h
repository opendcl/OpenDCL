// ImageTreeHolderCtrl.h : header file
//

#pragma once

class CImageTreeCtrl;


/////////////////////////////////////////////////////////////////////////////
// CImageTreeHolderCtrl window

class CImageTreeHolderCtrl : public CStatic
{
	CImageTreeCtrl* mpImageTreeCtrl;

	// Construction
public:
	CImageTreeHolderCtrl( CImageTreeCtrl* pImageTreeCtrl, const CRect& rcWnd, CWnd* pParentWnd );
	virtual ~CImageTreeHolderCtrl();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
