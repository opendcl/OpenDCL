// PictureBox.h : header file
//

#pragma once

class CPictureObject;


/////////////////////////////////////////////////////////////////////////////
// CPictureBox window

class CPictureBox : public CWnd
{
// Construction
public:
	CPictureBox();
	void Refresh(HDC hdc);
	void SetAcadColor(long nColorArg);
	void SetPicture(const CPictureObject* pPict);
	//void SetPictureID(long sPictureID);
	void SetPictureBlank();
	void AutoSize();
	void SetIcon(UINT nId);

// Attributes
public:
	CImageList m_ImageList;
	HICON m_hIcon;
	bool m_bUseMask;
	int m_cxIcon;
	int m_cyIcon;
	BOOL m_AutoSize;
	int m_BorderStyle;
	COLORREF m_BkColor;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureBox)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPictureBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPictureBox)
	afx_msg void OnSysColorChange();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//HICON m_hIconOut;
	LPPICTURE m_pPictureHolder;
	CString m_ClassName;
	int m_PictureID;
};
