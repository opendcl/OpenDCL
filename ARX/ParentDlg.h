// ParentDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "ArxDialogObject.h"

class CDialogControl;
class CFontCollection;
class CDclControlObject;

extern const UINT WM_FILEDLG_GETFILENAME;
extern const UINT WM_FILEDLG_GETFILETITLE;
extern const UINT WM_FILEDLG_GETFILEEXT;
extern const UINT WM_FILEDLG_GETFILEPATH;
extern const UINT WM_FILEDLG_GETFOLDERPATH;
extern const UINT WM_FILEDLG_GETFOLDERNAME;
extern const UINT WM_FILEDLG_GETSELECTEDFILECOUNT;
extern const UINT WM_FILEDLG_GETSELECTEDFILES;


class CFileDialogX : public CArxDialogObject
{
	friend class CParentDlg;
	CParentDlg* mpOwner;
protected:
	CFileDialogX( CParentDlg& Owner, CDclFormObject* pDclForm );
	~CFileDialogX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return false; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return true; }
	virtual HWND GetHWnd() const;
	virtual void CloseDialog(int nStatus) const;
	virtual INT_PTR DoModal();
	virtual bool Show(bool bShow = true) { return false; }
};


/////////////////////////////////////////////////////////////////////////////
// CParentDlg dialog

class CParentDlg : public CCommonDialog
{
	CFileDialogX mDialogX;

public:
	CWnd				*m_pMainChild;

	BOOL m_bShowGrip;	
	BOOL m_bInitDone;			// if all internal vars initialized
	POINT m_ptMinTrackSize;		// min tracking size
	SIZE m_szGripSize;			// set at construction time	
	CRect m_rcGripRect;			// current pos of grip

	enum { IDD = IDD_CUSTOM_FILE_DIALOG };

// Construction
public:
	CParentDlg( CDclFormObject* pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	~CParentDlg();

public:
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }

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
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

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
