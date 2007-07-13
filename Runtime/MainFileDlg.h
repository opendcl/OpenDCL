// MainFileDlg.h : header file
//

#pragma once

#include "DialogObject.h"

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CMainFileDlg dialog

class CMainFileDlg : public CCommonDialog
{
	CDclFormObject* mpSourceForm;
	int mnInitialX;
	int mnInitialY;
	int mnMinWidth;
	int mnMinHeight;
	int mnMaxWidth;
	int mnMaxHeight;
	int mnNCWidth; //width of non-client window area
	int mnNCHeight; //height of non-client window area
	bool mbInitialized;

// Construction
public:
	CMainFileDlg( CDclFormObject* pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CMainFileDlg();

public:
	int GetNCWidth() const { return mnNCWidth; }
	int GetNCHeight() const { return mnNCHeight; }
	void SetDialogMinExtents( int nWidth, int nHeight );
	void SetDialogMaxExtents( int nWidth, int nHeight );	
	void SavePosition();
	CRect ReadPosition() const;
	void Initialize();

protected:
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
