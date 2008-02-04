// ColorPickerCB.cpp
//
// (c) 1998 James R. Twine
//
// Based On Code That Was Found On www.codeguru.com, And Was 
// © 1997 Baldvin Hansson
//
//	Create A ComboBox (Drop List) And Set The
//	"Owner Draw Fixed" And "Has Strings" Styles/Checkboxes, And
//	Map This Class To It.  See?  Simple!
//

#pragma once


class CColorPickerCB : public CComboBox
{
// Construction
public:
	CColorPickerCB();

// Attributes
private:
	bool			m_bInitialized;							// Control Initialized?
	CString			m_sColorName;							// Name Of Selected Color

private:

	void			Initialize( void );						// Initialize Control/Colors

public:
	COLORREF		GetSelectedColorValue( void );			// Get Selected Color Value
	CString			GetSelectedColorName( void );			// Get Selected Color Name

	void			SetSelectedColorValue( COLORREF crClr );// Set Selected Color Value
	void			SetSelectedColorName( LPCTSTR cpColor );	// Set Selected Color Name

	bool			RemoveColor( LPCTSTR cpColor );			// Remove Color From List
	bool			RemoveColor( COLORREF crClr );			// Remove Color From List
	
	int				AddColor( LPCTSTR cpNam, COLORREF crClr );// Insert A New Color

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerCB)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual	~CColorPickerCB();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPickerCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
