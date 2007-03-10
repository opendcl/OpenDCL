#if !defined(AFX_SPREADSHEETHDR_H__94283EF9_B264_41F6_AF6A_E0235DF21B05__INCLUDED_)
#define AFX_SPREADSHEETHDR_H__94283EF9_B264_41F6_AF6A_E0235DF21B05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpreadSheetHdr.h : header file
//

class CSpreadSheet;

/////////////////////////////////////////////////////////////////////////////
// CSpreadSheetHdr window

class CSpreadSheetHdr : public CHeaderCtrl
{
// Construction
public:
	CSpreadSheetHdr();

// Attributes
public:
	CSpreadSheet *m_pParent;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpreadSheetHdr)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpreadSheetHdr();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpreadSheetHdr)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPREADSHEETHDR_H__94283EF9_B264_41F6_AF6A_E0235DF21B05__INCLUDED_)
