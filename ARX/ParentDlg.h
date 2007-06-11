// ParentDlg.h : header file
//

#pragma once

#include "Resource.h"

class CDialogControl;
class CFontCollection;
class CDclFormObject;
class CDclControlObject;
class CParentFileDialog;

extern const UINT WM_FILEDLG_GETFILENAME;
extern const UINT WM_FILEDLG_GETFILETITLE;
extern const UINT WM_FILEDLG_GETFILEEXT;
extern const UINT WM_FILEDLG_GETFILEPATH;
extern const UINT WM_FILEDLG_GETFOLDERPATH;
extern const UINT WM_FILEDLG_GETFOLDERNAME;
extern const UINT WM_FILEDLG_GETSELECTEDFILECOUNT;
extern const UINT WM_FILEDLG_GETSELECTEDFILES;


/////////////////////////////////////////////////////////////////////////////
// CParentDlg dialog

class CParentDlg : public CCommonDialog
{
public:
	CParentFileDialog* mpFileDlg;

	BOOL m_bShowGrip;	
	BOOL m_bInitDone;			// if all internal vars initialized
	POINT m_ptMinTrackSize;		// min tracking size
	SIZE m_szGripSize;			// set at construction time	
	CRect m_rcGripRect;			// current pos of grip

	enum { IDD = IDD_CUSTOM_FILE_DIALOG };

// Construction
public:
	CParentDlg( CWnd* pParent, CParentFileDialog* pFileDlg );
	~CParentDlg();

public:
	void UpdateGripPos();
	void ShowSizeGrip(BOOL bShow);

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	UINT OnNcHitTest(CPoint point);
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void PostNcDestroy();

	//custom file dialog messages
	afx_msg LRESULT OnGetFileName( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFileTitle( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFileExt( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFilePath( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFolderPath( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFolderName( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetSelectedFiles( WPARAM wParam, LPARAM lParam );

protected:
	DECLARE_MESSAGE_MAP()
};
