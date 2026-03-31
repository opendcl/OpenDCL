// FolderComboBox.h : header file
//

#pragma once

#include "FolderTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderComboBox window

class CFolderComboBox : public CComboBox
{
	WndTheme mWndTheme;
	bool mbHandleComboMessages;
	int mnIconWidth;
	CFolderTreeCtrl	mPopupCtrl;
	bool mbDropLock;
	enum uiState { fNormal = 0, fHot = 1, fPressed = 2, fDisabled = 4, fOpen = 8, };
	class UIState
	{
		bool hot : 1;
		bool pressed : 1;
		bool disabled : 1;
		bool open : 1;
	public:
		UIState() : hot(0), pressed(0), disabled(0), open(0) {}
		UIState( uiState flags )
			: hot((flags & fHot) != 0)
			, pressed((flags & fPressed) != 0)
			, disabled((flags & fDisabled) != 0)
			, open((flags & fOpen) != 0) 
			{}
		UIState& operator =( uiState flags )
			{
				hot = ((flags & fHot) != 0);
				pressed = ((flags &fPressed) != 0);
				disabled = ((flags & fDisabled) != 0);
				open = ((flags & fOpen) != 0);
				return *this;
			}
		uiState asFlags() const
		{
			return (uiState)((hot? fHot : 0) | (pressed? fPressed : 0) | (disabled? fDisabled : 0) | (open? fOpen : 0));
		}
		operator uiState() const { return asFlags(); }
		bool isNormal() const { return (asFlags() == fNormal); }
		void setNormal() { *this = fNormal; }
		bool isHot() const { return hot; }
		void setHot() { hot = true; }
		void setCold() { hot = false; }
		bool isPressed() const { return pressed; }
		void setPressed() { pressed = true; }
		void setUnpressed() { pressed = false; }
		bool isDisabled() const { return disabled; }
		void setDisabled() { disabled = true; }
		void setEnabled() { disabled = false; }
		bool isOpen() const { return open; }
		void setOpen() { open = true; }
		void setClosed() { open = false; }
	} mfState;
	bool mbPreVistaBehavior;

public:
	CFolderComboBox();
	virtual ~CFolderComboBox();

public:
	bool Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwStyle, UINT nID );

protected:
	virtual const WndTheme& GetFolderComboBoxTheme() const { return mWndTheme; }
	void OpenTheme();

// Implementation
public:
	CString GetSelectedPath();
	bool SelectPath( LPCTSTR pszPath );
	void AddPath( LPCTSTR pszPath );

// CComboBox overrides
	int AddString(LPCTSTR lpszString);
	int DeleteString(UINT nIndex);
	int Dir(UINT attr, LPCTSTR lpszWildCard);
	int FindString(int nStartAfter, LPCTSTR lpszString) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int GetCount() const;
	int GetCurSel() const;
	BOOL GetDroppedState();
	int GetLBText(int nIndex, LPTSTR lpszText) const;
	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;
	int GetTopIndex() const;
	int InsertString(int nIndex, LPCTSTR lpszString);
	void ResetContent();
	int SelectString(int nStartAfter, LPCTSTR lpszString);
	int SetCurSel(int nSelect);
	int SetTopIndex(int nIndex);
	void ShowDropDown(BOOL bShowIt = TRUE);

protected:
	HTREEITEM FromIndex( int idxItem, HTREEITEM htiStartAt, int& ctSearched ) const;
	HTREEITEM FromIndex( int idxItem ) const;
	int FromHandle( HTREEITEM htiItem, HTREEITEM htiStartAt, int& ctSearched ) const;
	int FromHandle( HTREEITEM htiItem ) const;
	int FindString( const CString& sSearch, HTREEITEM htiStartAt, bool bExactMatch = true ) const;
	
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg LRESULT OnCbAddString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbDeleteString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbDir( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbFindString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbFindStringExact( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbGetCount( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbGetCurSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbGetDroppedState( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbGetLBText( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbGetLBTextLen( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbGetTopIndex( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbInsertString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbSelectString( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbSetCurSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbSetTopIndex( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCbShowDropDown( WPARAM wParam, LPARAM lParam );
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg LRESULT OnThemechanged(WPARAM wParam, LPARAM lParam);
};
