// ButtonCtrl.h : header file
//
// Base functionality shared by all types and styles of button controls

#pragma once

#include "XPStyleButtonST.h"
#include "DialogControl.h"
#include "AcadColorService.h"
#include "ThemeHelperST.h"

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
	void SetBackgroundColor( long nAcadColor ) override
	{
		__super::SetBackgroundColor( nAcadColor );
		ResetButtonBackgroundColor();
	}
	void SetBackgroundColor( COLORREF color ) override
	{
		__super::SetBackgroundColor( color );
		ResetButtonBackgroundColor();
	}
	void SetForegroundColor( long nAcadColor ) override
	{
		__super::SetForegroundColor( nAcadColor );
		ResetButtonForegroundColor();
	}
	void SetForegroundColor( COLORREF color ) override
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
	CThemeHelperST mThemeHelper;

public:
	CButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CButtonCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }

protected:
	DWORD OnDrawBackground(CDC* pDC, CRect* pRect) override;
	virtual bool IsUsingPresetGraphic() const { return mbUsingPresetGraphic; }
	virtual void UpdateButtonGraphic() {}
	void SetResourceIcon(UINT idIcon);

protected:
	DECLARE_MESSAGE_MAP();

protected:
	void PreSubclassWindow() override;
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
