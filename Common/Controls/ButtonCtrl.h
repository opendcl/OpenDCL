// ButtonCtrl.h : header file
//
// Base functionality shared by all types and styles of button controls

#pragma once

#include "XPStyleButtonST.h"
#include "DialogControl.h"
#include "AcadColorService.h"

class CProject;

enum ButtonStyle
{
	ButtonStyle_Raised	  = 0,
	ButtonStyle_Flat		  = 1,
	ButtonStyle_Pick		  = 2,
	ButtonStyle_Select	  = 3,
	ButtonStyle_Filter	  = 4,
	ButtonStyle_NoBorder	= 5,
	ButtonStyle_XPTheme		= 6,
};

class CButtonAcadColorService : public CAcadColorService
{
	CButtonST& mButton;

// Construction
public:
	CButtonAcadColorService( CButtonST& Button ) : mButton( Button )
		{
			ResetButtonForegroundColor();
			ResetButtonBackgroundColor();
		}
	virtual ~CButtonAcadColorService()
		{
		}

protected:
	void ResetButtonForegroundColor() const;
	void ResetButtonBackgroundColor() const;

// Operations
public:
	virtual void SetBackgroundColor( long nAcadColor )
	{
		__super::SetBackgroundColor( nAcadColor );
		ResetButtonBackgroundColor();
	}
	virtual void SetBackgroundColor( COLORREF color )
	{
		__super::SetBackgroundColor( color );
		ResetButtonBackgroundColor();
	}
	virtual void SetForegroundColor( long nAcadColor )
	{
		__super::SetForegroundColor( nAcadColor );
		ResetButtonForegroundColor();
	}
	virtual void SetForegroundColor( COLORREF color )
	{
		__super::SetForegroundColor( color );
		ResetButtonForegroundColor();
	}
};

/////////////////////////////////////////////////////////////////////////////
// CButtonCtrl window

class CButtonCtrl : public CXPStyleButtonST, public CDialogControl
{
	CButtonAcadColorService mColorService;
	bool mbUsingPresetGraphic;

public:
	CButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CButtonCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

protected:
	virtual DWORD OnDrawBackground(CDC* pDC, CRect* pRect);
	virtual bool IsUsingPresetGraphic() const { return mbUsingPresetGraphic; }
	virtual void UpdateButtonGraphic() {}
	void SetResourceIcon(UINT idIcon);

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual void PreSubclassWindow();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
