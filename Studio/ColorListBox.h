// ColorListBox.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CColorListBox window

class CColorListBox : public CListBox
{
// Construction
public:
	CColorListBox();

// Attributes
private:
   bool        m_bInitialized;                  // Control Initialized?
   CString     m_sColorName;                    // Name Of Selected Color
   CStringArray m_arsText;
private:

   void        Initialize( void );                 // Initialize Control/Colors

public:
   COLORREF    GetSelectedColorValue( void );      // Get Selected Color Value
   CString     GetSelectedColorName( void );       // Get Selected Color Name

   void        SetSelectedColorValue( COLORREF crClr );// Set Selected Color Value
   void        SetSelectedColorName( LPCTSTR cpColor ); // Set Selected Color Name

   bool        RemoveColor( LPCTSTR cpColor );       // Remove Color From List
   bool        RemoveColor( COLORREF crClr );         // Remove Color From List
   
   int         AddColor( CString cpNam, COLORREF crClr );// Insert A New Color

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorListBox)
public:
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
protected:
	void PreSubclassWindow() override;
	//}}AFX_VIRTUAL
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

// Implementation
public:
	virtual ~CColorListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
