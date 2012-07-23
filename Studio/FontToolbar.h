// FontToolBar.h : header file
//

#pragma once

#include "Resource.h"
#include "FontCombo.h"
#include "PtrTypes.h"
#include "FontSettings.h"
#include <vector>

class CDclControlObject;
class CStudioFrame;


/////////////////////////////////////////////////////////////////////////////
// CFontToolbar dialog

class CFontToolbar : public CDialogBar
{
	std::vector< TDclControlPtr > mControls;
	CFontCombo m_FontNames;
	CComboBox m_FontSizes;
	CToolBarCtrl m_Buttons;
	TBBUTTON* mpTBButtons;
	FontSettings mSettings;
	CStudioFrame* mpStudioFrame;
	CFont mFont;

// Construction
public:
	CFontToolbar( CStudioFrame* pStudioFrame );   // standard constructor

	void OnActivateDclControl( TDclControlPtr pDclControl );
	void SetFont( const FontSettings& FS );
	void AddFont( const FontSettings& FS );
	void ClearFont();

protected:
	void AddTheButtons();
	CString GetTooltipText( UINT nID );
	
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
	afx_msg void OnFontName();
	afx_msg void OnFontSize();
	afx_msg void OnFontBold();
	afx_msg void OnFontItalic();
	afx_msg void OnFontUnderline();
	afx_msg void OnFontScaled();
	BOOL OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );

	DECLARE_MESSAGE_MAP()
};
