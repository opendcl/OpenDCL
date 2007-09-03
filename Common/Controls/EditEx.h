// EditEx.h : header file
//

#pragma once

#include "AcadColorService.h"

class CInputFilter;

#ifndef _TEditBase
	#ifdef __ACADTARGET
		#define _TEditBase CAcUiEdit
	#else
		#define _TEditBase CEdit
	#endif //__ACADTARGET
#endif //_TEditBase


/////////////////////////////////////////////////////////////////////////////
// CEditEx window

class CEditEx : public _TEditBase
{
	CAcadColorService mColorService;

public:
	CEditEx();
	CEditEx( CWnd* pParentWnd, const CRect& rectWnd, UINT nID );
	virtual ~CEditEx();

public:
	virtual bool Create( CWnd* pParentWnd, const CRect& rectWnd, UINT nID );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual CInputFilter* GetInputFilter() { return NULL; }

public:

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
};
