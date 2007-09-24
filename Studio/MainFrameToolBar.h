// MainFrameToolBar.h : header file
//

#pragma once

#include "Resource.h"
#include "FontCombo.h"
#include "FontSizes.h"
#include "PtrTypes.h"

class CDclControlObject;

#define nDeMainButtonCount 14
#define nDeBtnSizeH 16
#define nDeBtnSizeW 15
#define nDeTTTLen 79


/////////////////////////////////////////////////////////////////////////////
// CMainFrameToolBar dialog

class CMainFrameToolBar : public CDialogBar
{
// Construction
public:
	CMainFrameToolBar();   // standard constructor

// Dialog Data
	enum { IDD = IDR_MAINFRAME };

	CFontCombo		m_FontNames;
	CFontSizes		m_FontSizes;
	long      m_lFontSize;
	short			m_nFontWeight;
	short			m_nFontCharset;
	bool			m_bBold;
	bool			m_bUnderline;
	bool			m_bItalic;
	bool			m_bScaled;
	CToolBarCtrl	m_Buttons;
	TBBUTTON		*m_pTBButtons;
	CFont			m_font;
	CView			*m_pActiveView;

	void SetFontToolBar(TDclControlPtr pCtrl);
	void AddFontToToolBar(TDclControlPtr pCtrl);
	void SetActiveView(CView *pView);
	void Setup();
	void AddTheButtons();
	CString NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	
// Overrides
	// ClassWizard generated virtual function overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
protected:
	// Generated message map functions
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFontBold();
	afx_msg void OnFontItalic();
	afx_msg void OnFontUnderline();
	afx_msg void OnFontScaled();
	BOOL OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );

	DECLARE_MESSAGE_MAP()
};
