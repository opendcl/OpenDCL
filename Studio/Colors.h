// Colors.h : header file
//

#pragma once

#include "Resource.h"
#include "PropertyObject.h"
#include "FilteredEditCtrl.h"
#include "ColorPickerCB.h"
#include "ColorListBox.h"
#include "ColorPatch.h"

class CDclControlObject;
class COpenDCLView;

/////////////////////////////////////////////////////////////////////////////
// CColors dialog

class CColors : public CPropertyPage
{
	Prop::Id midProp;
	CDclControlObject* mpControl;
	COpenDCLView* mpView;
	CString msTitle;
	TPropertyPtr mpColor;
	
// Construction
public:
	CColors(Prop::Id idProp, CDclControlObject* pControl, COpenDCLView* pView);
	~CColors();

// Dialog Data
	//{{AFX_DATA(CColors)
	enum { IDD = IDD_BACKCOLORS };
	CColorPickerCB	m_ColorCB;
	CFilteredEditCtrl	m_Edit;
	CColorListBox	m_SystemColors;
	CColorPatch		m_Color;
	//}}AFX_DATA

	void DisplayColor();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColors)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColors)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSystemcolorlist();
	afx_msg void OnSelchangeColorcombo();
	afx_msg void OnChangeEdit();
	afx_msg void OnKillfocusEdit();
	afx_msg void OnTruebtn();
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
