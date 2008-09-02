// ArxHtmlCtrl.h : header file
//

#pragma once

#include "HtmlCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxHtmlCtrl window

class CArxHtmlCtrl : public CHtmlCtrl
{
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

public:
	CArxHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxHtmlCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }

protected:
	virtual void OnAppCmd( LPCTSTR lpszWhere ); //override to handle links to "app:mumble...".
	virtual void OnNavigateComplete2( LPCTSTR strURL );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
