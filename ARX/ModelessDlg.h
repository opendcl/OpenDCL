// ModelessDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "SnapDlg.h"

class CAcadDocReactor;


#ifndef WM_ACAD_MFC_BASE
#define WM_ACAD_MFC_BASE        (1000)
#endif

#ifndef WM_ACAD_KEEPFOCUS
#define WM_ACAD_KEEPFOCUS       (WM_ACAD_MFC_BASE + 1)
#endif


/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

class CModelessDlg : public CSnapDlg
{
// Construction	
public:
	CModelessDlg(CDclFormObject* pSourceForm, CWnd* pParent = NULL, int nX = -1, int nY = -1);

public:
	void SizeDialog ();
	void SetTitleBarIcon(int nPictureID);
	void SetDclForm(CDclFormObject *pDclFormObject);		
	void CenterDialog();
	void CenterDialog(int nNewWidth, int nNewHeight);
	void ResizeDialog(int nNewWidth, int nNewHeight);
	void AboutToClose();
	bool QueryForClose();
	void FireCloseEvent();
	bool				m_bClosing;
	bool				m_bAboutToClose;
	CAcadDocReactor		*m_pDocToModReactor;
	int m_nX;
	int m_nY;
	bool m_bAsModal;
	
// Dialog Data
	//{{AFX_DATA(CModelessDlg)
	enum { IDD = IDD_RESIZEABLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelessDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg LONG onAcadKeepFocus(UINT, LONG);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
