// ColorsPage.h : header file
//

#pragma once

#include "Resource.h"
#include "PropertyObject.h"
#include "FilteredEditCtrl.h"
#include "ColorPickerCB.h"
#include "ColorListBox.h"
#include "ColorPatch.h"
#include "DclControlTemplate.h"


/////////////////////////////////////////////////////////////////////////////
// CColorsPage dialog

class CColorsPage : public CPropertyPage
{
	Prop::Id midProp;
	TDclControlPtr mpControl;
	CString msTitle;
	TPropertyPtr mpColor;
	
// Construction
public:
	CColorsPage(Prop::Id idProp, TDclControlPtr pControl);
	~CColorsPage();

// Dialog Data
	enum { IDD = IDD_BACKCOLORS };
	CColorPickerCB	m_ColorCB;
	CFilteredEditCtrl	m_Edit;
	CColorListBox	m_SystemColors;
	CColorPatch		m_Color;

	void DisplayColor();

// Overrides
public:
	BOOL OnApply() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	afx_msg void OnSelchangeSystemcolorlist();
	afx_msg void OnSelchangeColorcombo();
	afx_msg void OnChangeEdit();
	afx_msg void OnTruebtn();
	afx_msg void OnPaint();
};
