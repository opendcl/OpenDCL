// ModelessDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "BaseDlg.h"
#include "ArxDialogObject.h"

#if (_MFC_VER < 0x0800)
#define __LRESULT UINT
#else
#define __LRESULT LRESULT
#endif


#ifndef WM_ACAD_MFC_BASE
#define WM_ACAD_MFC_BASE        (1000)
#endif

#ifndef WM_ACAD_KEEPFOCUS
#define WM_ACAD_KEEPFOCUS       (WM_ACAD_MFC_BASE + 1)
#endif


class CModelessDialogX : public CArxDialogObject
{
	friend class CModelessDlg;
	CModelessDlg* mpOwner;
protected:
	CModelessDialogX( CModelessDlg& Owner, CDclFormObject* pDclForm );
	~CModelessDialogX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const;
	virtual HWND GetHWnd() const;
	virtual bool CreateModeless( UINT nID ) const;
	virtual void CloseDialog(int nStatus);
	virtual bool SetMinMaxSize( const CSize& min, const CSize& max );
};


/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

class CModelessDlg : public CBaseDlg
{
	CWnd* mpParent;
	CModelessDialogX mDialogX;
	bool mbResizable;
	bool mbTrackingMouse;
	bool mbInMenuLoop;
	HWND mhwndKeyboardFocus;

	bool				m_bAboutToClose;
	bool m_bAsModal;
	
	enum { IDD = IDD_RESIZEABLE };

// Construction	
public:
	CModelessDlg( CDclFormObject* pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	~CModelessDlg();

public:
	virtual CControlPane& GetControlPane() { return mDialogX.GetControlPane(); }
	virtual const CDialogObject& GetDialogObject() const { return mDialogX; }
	virtual CDialogObject& GetDialogObject() { return mDialogX; }

	bool Create( UINT nTemplateID );

public:
	bool IsResizable() const { return mbResizable; }

	void SizeDialog ();
	void SetTitleBarIcon(int nPictureID);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
	virtual void OnCancel();

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT onAcadKeepFocus(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg __LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
