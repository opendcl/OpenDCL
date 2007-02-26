// ButtonCtrl.h : header file
//
// Base functionality shared by all types and styles of button controls

#pragma once

#include "XPStyleButtonST.h"
#include "DialogControl.h"

class CDclControlObject;
class CProject;

enum ButtonStyle
{
	ButtonStyle_Raised	  = 0,
	ButtonStyle_Flat		  = 1,
	ButtonStyle_Pick		  = 2,
	ButtonStyle_Select	  = 3,
	ButtonStyle_Filter	  = 4,
	ButtonStyle_NoBorder	= 5,
};


/////////////////////////////////////////////////////////////////////////////
// CButtonCtrl window

class CButtonCtrl : public CXPStyleButtonST, public CDialogControl
{
public:
	CButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CButtonCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );

// These were added to the modified CButtonST class in the original ODCL code
public:
	COLORREF		m_ForeColor;
	COLORREF		m_BackColor;
	int		m_nDirectory;
	
	CBrush			*m_pStaticBrush;	

	//These may have been part of an older CButtonST class
	BOOL		m_bHasBorder;		// Is has a border?

public:
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

protected:
	virtual void PreSubclassWindow();
	afx_msg void PostNcDestroy();

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:
	DECLARE_MESSAGE_MAP();
};
