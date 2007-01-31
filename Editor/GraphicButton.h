// GraphicButton.h : header file
//

#pragma once

#include "ColorButton.h"

class CPictureObject;


/////////////////////////////////////////////////////////////////////////////
// CGraphicButton window

class CGraphicButton : public CColorButton
{
// Construction
public:
	CGraphicButton();
	void SetPictureID(int sPictureID);
	void SetPictureBlank();
	void SetDefaultPicture(int nNewButtonStyle);
	CPictureObject* m_pPicture;
// Attributes
public:
	int m_cxIcon;
	int m_cyIcon;
	BOOL m_AutoSize;

// Operations
public:
	void Refresh(CDC *pdc);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphicButton)
	public:
	virtual BOOL Create(int ButtonStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphicButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphicButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_ButtonStyle;
	void DrawTheIcon(CDC* pDC, RECT* rcItem, CRect *captionRect);
	CString m_ClassName;
	int	 m_PictureID;	
	HICON m_hIconOut;
	HICON m_hIconPic;
	HBITMAP m_hBmpPic;
};
