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
protected:
	CDclFormObject* mpSourceForm;
	CControlPane mControlPane;
	CDclControlObject* mpControl;

public:
	HICON				m_hIconAcad;
	CString				m_sProjectName;
	CString				m_sDialogName;
	CList<CArxDialogControl*>			m_ControlCol;
	CFontCollection		*m_pFontCollection;
	
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

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }
	const CDclControlObject* GetControl() const { return mpControl; }
	CDclControlObject* GetControl() { return mpControl; }

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
	DECLARE_MESSAGE_MAP()
};
