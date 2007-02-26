// ArxGraphicButtonCtrl.h : header file
//
// ARX specific functionality for GraphicButton control

#pragma once

#include "GraphicButtonCtrl.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGraphicButtonCtrl window

class CArxGraphicButtonCtrl : public CGraphicButtonCtrl
{
	COleOdcDropTarget mDropTarget;	
	CString msOnClickedEvent;

public:
	bool			m_bInvokeWithSendString;

public:
	CArxGraphicButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxGraphicButtonCtrl();

// DialogControl Interface
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );

public:
	void SetDragnDrop(BOOL bRegister);
	void SetOnClickedEvent( LPCTSTR pszLispFunctionName ) { msOnClickedEvent = pszLispFunctionName; }

protected:
	afx_msg void OnClicked();	
	afx_msg void OnDoubleclicked();	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP();
};
