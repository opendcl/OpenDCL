// CModalVDcl.h : header file
//

#pragma once

#include "SnapDlg.h"
#include "Resource.h"

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CModalVDcl dialog

class CModalVDcl : public CSnapDlg
{
// Construction
public:
	CModalVDcl(CDclFormObject* pSourceForm, CWnd* pParent = NULL, int nX = -1, int nY = -1);   // standard constructor
	~CModalVDcl();

public:
	void SizeDialog();
	void CloseDialog();
	void SetTitleBarIcon(int nPictureID);
	void SetDclForm(CDclFormObject *pDclFormObject);
	void CenterDialog();
	void CenterDialog(int nNewWidth, int nNewHeight);
	void ResizeDialog(int nNewWidth, int nNewHeight);
	bool QueryForClose();
	
	// this back ground CStatic is used to hide some controls
	// that the system somehow added in on very infrequent occations
	//CStatic				m_BackGround;
	int m_nX;
	int m_nY;
// Dialog Data
	//{{AFX_DATA(CModalVDcl)
	enum { IDD = IDD_MODALDIALOG };
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModalVDcl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModalVDcl)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	afx_msg BOOL OnNotify_ToolTipText(UINT  id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};
