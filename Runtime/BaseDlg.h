// BaseDlg.h : header file
//
// Modified for ODCL, based on gripper-enabled dialog class (originally named CBaseDlg)
// By Mike Scanlon, 8/26/98
// email: mrs3691@megahertz.njit.edu

#pragma once

#include "ControlPane.h"
#include "ArxDialogObject.h"

class CFontCollection;

#if (_MFC_VER < 0x0800)
#define __LRESULT UINT
#else
#define __LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

class CBaseDlg : public CDialog
{
	TDclFormPtr mpSourceForm;
	int mnInitialX;
	int mnInitialY;
	bool mbHasTitleBar;
	int mnMinWidth;
	int mnMinHeight;
	int mnMaxWidth;
	int mnMaxHeight;
	int mnNCWidth; //width of non-client window area
	int mnNCHeight; //height of non-client window area
	bool mbShowGrip;

	BOOL m_sizing;
	CSize				m_szGripSize;
	CRect				m_rcGripRect;

// Construction
public:
	CBaseDlg( TDclFormPtr pSourceForm, UINT idd, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CBaseDlg();

public:
	virtual CControlPane& GetControlPane() = 0;
	virtual const CDialogObject& GetDialogObject() const = 0;
	virtual CDialogObject& GetDialogObject() = 0;
	virtual void SetMinMaxSize( const CSize& szMin, const CSize& szMax );

protected:
	int GetNCWidth() const { return mnNCWidth; }
	int GetNCHeight() const { return mnNCHeight; }
	void SavePosition();
	CRect ReadPosition() const;
	void UpdateGripPos();
	void SetTitleBarIcon( int nPictureID );

protected:
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
  afx_msg __LRESULT OnNcHitTest(CPoint point);
	afx_msg void PostNcDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};
