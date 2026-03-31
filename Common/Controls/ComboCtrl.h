// ComboCtrl.h : header file
//

#pragma once

#include "AcadColorService.h"

class CInputFilter;

#ifndef _TComboBase
	#define _TComboBase CComboBox
#endif //_TComboBase


/////////////////////////////////////////////////////////////////////////////
// CComboCtrl window

class CComboCtrl : public _TComboBase
{
	CAcadColorService mColorService;
	bool mbAutoComplete;

public:
	CComboCtrl();
	CComboCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID );
	virtual ~CComboCtrl();

public:
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual CInputFilter* GetInputFilter() { return NULL; }

public:
	bool Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID );

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
