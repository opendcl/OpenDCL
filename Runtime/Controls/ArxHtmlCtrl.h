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
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }

protected:
	void OnAppCmd( LPCTSTR lpszWhere ) override; //override to handle links to "app:mumble...".
	void OnNavigateComplete2( LPCTSTR strURL ) override;
	void OnDocumentComplete( LPCTSTR lpszURL ) override;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
