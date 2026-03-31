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
	LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) override;
	void PreSubclassWindow() override;
	BOOL PreTranslateMessage( MSG* pMsg ) override;
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnEnHscroll();
	afx_msg void OnEnVscroll();
};
