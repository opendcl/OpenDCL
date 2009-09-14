// CustomFileDialog.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "MainFileDlg.h"

class CFontCollection;


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


/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog dialog

class CCustomFileDialog : public CFileDialog, public CArxDialogObject
{
	CString msTitle;
	CString msFilterList;
	CString msDefaultExtension;
	CString msInitialDirectory;
	CString msResultBuf;
	FileDialogParams* mpParams;
	CMainFileDlg mMainFileDlg;
	TDclControlPtr mpFileDlgCtrl;
	int mnInitialX;
	int mnInitialY;
	int mnRightBorder;
	int mnBottomBorder;

public:
	CCustomFileDialog( TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	~CCustomFileDialog();

// Attributes
public:
	CMainFileDlg& GetMainDialog() { return mMainFileDlg; }

// Implementation
protected:
	void CtrlModifyStyle(int nCtrl);

protected:
friend class CMainFileDlg;
	void SavePosition();
	CRect ReadPosition() const;
	void OnInitializationComplete();

// CDialogObject overrides
public:
	virtual FormType GetType() const { return FrmFileDlg; }
	virtual CWnd* GetTopLevelWnd() { return &mMainFileDlg; }
	virtual bool IsModeless() const { return false; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return true; }
	virtual void CloseDialog(int nStatus);
	virtual INT_PTR DoModal();
	virtual bool Show(bool bShow = true) { return false; }
protected:
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual bool OnApplyResizable( TPropertyPtr pProp ); //Prop::AllowResizing
	virtual bool IsAsyncEvents() const { return false; }

protected:	
	DECLARE_MESSAGE_MAP()

protected:	
	afx_msg void OnHelp();
	virtual BOOL OnFileNameOK();
	virtual void OnFileNameChange();	
	virtual void OnFolderChange();
	virtual void OnTypeChange();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	//custom file dialog messages
	afx_msg LRESULT OnGetFileName( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFileTitle( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFileExt( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFilePath( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFolderPath( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFolderName( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetSelectedFiles( WPARAM wParam, LPARAM lParam );
};
