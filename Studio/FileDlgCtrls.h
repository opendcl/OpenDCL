// FileDlgCtrls.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CFileDlgCtrls window

class CFileDlgCtrls : public CStatic
{
	CSize mszBitmap;

// Construction
public:
	CFileDlgCtrls();
	virtual ~CFileDlgCtrls();

protected:
	bool Initialize();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL) override;
public:
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff) override;
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
