// Geometry.h : header file
//

#pragma once

#include "Resource.h"
#include "GeometryImage.h"
#include "PropertyObject.h"
#include "DclControlTemplate.h"

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CGeometry dialog

class CGeometry : public CPropertyPage
{
	TDclControlPtr mpDclControl;
	CGeometryImage	m_ExampleImage;
	TPropertyPtr m_pUseTopFromBottom;
	TPropertyPtr m_pUseBottomFromBottom;
	TPropertyPtr m_pUseLeftFromRight;
	TPropertyPtr m_pUseRightFromRight;

protected:
friend class CGeometryImage;
	CComboBox	m_Right;
	CComboBox	m_Bottom;
	CComboBox	m_Top;
	CComboBox	m_Left;

// Dialog Data
	enum { IDD = IDD_GEOMETRY };

// Construction
public:
	CGeometry( TDclControlPtr pDclControl );
	~CGeometry();

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
