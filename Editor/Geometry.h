// Geometry.h : header file
//

#pragma once

#include "Resource.h"
#include "GeometryImage.h"
#include "Splitter.h"
#include "PropertyObject.h"

class CDclControlObject;
class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CGeometry dialog

class CGeometry : public CPropertyPage
{
	DECLARE_DYNCREATE(CGeometry)

// Construction
public:
	CGeometry();
	~CGeometry();

// Dialog Data
	//{{AFX_DATA(CGeometry)
	enum { IDD = IDD_GEOMETRY };
	CSplitter	m_TopSplitter;
	CSplitter	m_RightSplitter;
	CSplitter	m_LeftSplitter;
	CSplitter	m_BottomSplitter;
	CComboBox	m_Right;
	CComboBox	m_Bottom;
	CComboBox	m_Top;
	CComboBox	m_Left;
	CGeometryImage	m_ExampleImage;
	//}}AFX_DATA
	RefCountedPtr< CPropertyObject > m_pUseTopFromBottom;
	RefCountedPtr< CPropertyObject > m_pUseBottomFromBottom;
	RefCountedPtr< CPropertyObject > m_pUseLeftFromRight;
	RefCountedPtr< CPropertyObject > m_pUseRightFromRight;

	CDclControlObject *m_pControl;
	CDclFormObject  *m_pDclForm;

	void ShowSplitter(CString sName);
	void ShowAllSplitters();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeometry)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGeometry)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLeft();
	afx_msg void OnSelchangeTop();
	afx_msg void OnSelchangeBottom();
	afx_msg void OnSelchangeRight();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
