// SnapDlg.h : header file
//
// By Mike Scanlon, 8/26/98
// email: mrs3691@megahertz.njit.edu

#pragma once

#include "ControlPane.h"

class CDclFormObject;
class CDclControlObject;
class CFontCollection;
class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CSnapDlg dialog

class CSnapDlg : public CDialog
{
	CDclFormObject* mpSourceForm;

public:
	HICON				m_hIconAcad;
	
	int					m_nMinWidth;
	int					m_nMinHeight;
	int					m_nMaxWidth;
	int					m_nMaxHeight;
	
	bool				m_bShowGrip;
	CSize				m_szGripSize;
	CRect				m_rcGripRect;

// Construction
public:
	CSnapDlg(CDclFormObject* pSourceForm, UINT idd, CWnd* pParent = NULL);	// standard constructor

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	BOOL m_sizing;
	virtual void SizeDialog() = 0;

public:
	void SetDialogMinExtents(int nWidth, int nHeight);
	void SetDialogMaxExtents(int nWidth, int nHeight);	
	void SaveSize();
	CRect ReadRect();
	CSize ReadSize();
	void UpdateGripPos();

	// Generated message map functions
protected:
	virtual BOOL OnInitDialog();
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

protected:
	DECLARE_MESSAGE_MAP()
};
