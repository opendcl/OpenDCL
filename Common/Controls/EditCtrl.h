// EditCtrl.h : header file
//

#pragma once

#include "AcadColorService.h"

class CInputFilter;

#ifndef _TEditBase
	#ifdef _ACADTARGET
		#define _TEditBase CAcUiEdit
	#else
		#define _TEditBase CEdit
	#endif //_ACADTARGET
#endif //_TEditBase


/////////////////////////////////////////////////////////////////////////////
// CEditCtrl window

class CEditCtrl : public _TEditBase
{
	CAcadColorService mColorService;

public:
	CEditCtrl();
	CEditCtrl( CWnd* pParentWnd, const CRect& rectWnd, UINT nID,
						 DWORD dwStyle = GetDefaultWndStyle(), CInputFilter* pFilter = NULL );
	virtual ~CEditCtrl();

public:
	virtual bool Create( CWnd* pParentWnd, const CRect& rectWnd, UINT nID,
											 DWORD dwStyle = GetDefaultWndStyle(), CInputFilter* pFilter = NULL );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual CInputFilter* GetInputFilter() { return NULL; }

public:
	static DWORD GetDefaultWndStyle() { return (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL); }

protected:
	void GetRawWindowText( CString& sText );

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
