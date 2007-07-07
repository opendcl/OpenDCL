// GeometryImage.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CGeometryImage window

class CGeometryImage : public CStatic
{
// Construction
public:
	CGeometryImage();

// Attributes
public:
	CBitmap *m_pBitmap;
// Operations
public:

	void DrawDimensions(HDC hdc);
	void DrawArrow(HDC hdc, int x,int Y, int Direction);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeometryImage)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGeometryImage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGeometryImage)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
