// ComboExCtrl.h : header file
//

#pragma once

#include "AcadColorService.h"

class CInputFilter;

#ifndef _TComboExBase
	#define _TComboExBase CComboBoxEx
#endif //_TComboBase


/////////////////////////////////////////////////////////////////////////////
// CComboCtrl window

class CComboExCtrl : public _TComboExBase
{
	CAcadColorService mColorService;
	bool mbAutoComplete;

public:
	CComboExCtrl();
	CComboExCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID );
	virtual ~CComboExCtrl();

public:
	virtual bool Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual CInputFilter* GetInputFilter() { return NULL; }

public:

protected:
	void GetRawWindowText( CString& sText );

protected:
	DECLARE_MESSAGE_MAP();

protected:
	LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) override;
	void PreSubclassWindow() override;
	BOOL PreTranslateMessage( MSG* pMsg ) override;
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
	afx_msg void OnEditchange();
};
