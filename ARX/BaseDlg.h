// BaseDlg.h : header file
//
// Modified for ODCL, based on gripper-enabled dialog class (originally named CBaseDlg)
// By Mike Scanlon, 8/26/98
// email: mrs3691@megahertz.njit.edu

#pragma once

#include "ControlPane.h"
#include "ArxDialogObject.h"

class CDclFormObject;
class CDclControlObject;
class CFontCollection;
class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

class CBaseDlg : public CDialog
{
	CDclFormObject* mpSourceForm;
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
	CBaseDlg( CDclFormObject* pSourceForm, UINT idd, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CBaseDlg();

public:
	virtual CControlPane& GetControlPane() = 0;
	int GetNCWidth() const { return mnNCWidth; }
	int GetNCHeight() const { return mnNCHeight; }
	void SetDialogMinExtents( int nWidth, int nHeight );
	void SetDialogMaxExtents( int nWidth, int nHeight );	
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
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
#if (_ACADTARGET == 16)
  afx_msg UINT OnNcHitTest(CPoint point);
#else
  afx_msg LRESULT OnNcHitTest(CPoint point);
#endif
	afx_msg void PostNcDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};
