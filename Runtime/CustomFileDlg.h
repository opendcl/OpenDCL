// CustomFileDlg.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "MainFileDlg.h"


struct FileDialogParams
{
	FileDialogParams( BOOL bOpen, LPCTSTR pszExt, LPCTSTR pszFile, DWORD flags, LPCTSTR pszFilter )
		: bOpenFileDialog( bOpen )
		, sDefaultExtension( pszExt )
		, sFilename( pszFile )
		, dwFlags( flags )
		, sFilterList( pszFilter )
		{}
	/*in*/ BOOL bOpenFileDialog;
	/*in*/ CString sDefaultExtension;
	/*in-out*/ CString sFilename;
	/*in*/ DWORD dwFlags;
	/*in*/ CString sFilterList;
	/*out*/ CStringArray rsFilenames;
};


// Shim base class that works correctly with old or new CFileDialog constructor
class CNonVistaFileDialog : public CFileDialog
{
public:
	CNonVistaFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = 0UL, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
};


/////////////////////////////////////////////////////////////////////////////
// CCustomFileDlg dialog

class CCustomFileDlg : public CNonVistaFileDialog, public CArxDialogObject
{
	CString msTitle;
	CString msFilterList;
	CString msDefaultExtension;
	CString msInitialDirectory;
	CString msResultBuf;
	FileDialogParams* mpParams;
	CMainFileDlg mMainFileDlg;
	TDclControlPtr mpFileDlgCtrl;
	CPoint mptInitial;
	CSize msizeInitial;
	int mnRightBorder;
	int mnBottomBorder;

public:
	CCustomFileDlg( TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	~CCustomFileDlg();

// Attributes
public:
	CMainFileDlg& GetMainDialog() { return mMainFileDlg; }

// Implementation
protected:
	void CtrlModifyStyle(int nCtrl);

protected:
friend class CMainFileDlg;
	void SavePosition();
	void ReadPosition(POINT& ptTopLeft, SIZE& size) const;
	void OnInitializationComplete();

// CDialogObject overrides
public:
	FormType GetType() const override { return FrmFileDlg; }
	CWnd* GetTopLevelWnd() override { return &mMainFileDlg; }
	bool IsModeless() const override { return false; }
	bool IsDockable() const override { return false; }
	bool IsResizable() const override { return true; }
	void CloseDialog(int nStatus) override;
	INT_PTR DoModal() override;
	bool Show(bool bShow = true) override { return false; }
protected:
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	bool OnApplyResizable( TPropertyPtr pProp ) override; //Prop::AllowResizing
	bool IsAsyncEvents() const override { return false; }

protected:	
	DECLARE_MESSAGE_MAP()

	//custom file dialog messages
	afx_msg LRESULT OnGetFileName( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFileTitle( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFileExt( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFilePath( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFolderPath( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFolderName( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetSelectedFiles( WPARAM wParam, LPARAM lParam );

	afx_msg void OnHelp();
	BOOL OnFileNameOK() override;
	void OnFileNameChange() override;
	void OnFolderChange() override;
	void OnTypeChange() override;
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL OnInitDialog() override;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
