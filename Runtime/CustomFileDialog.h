// CustomFileDialog.h : header file
//

#pragma once

#include "MainFileDlg.h"
#include "ArxDialogObject.h"

class CFontCollection;
class CDclControlObject;
class CDclFormObject;


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


class CFileDialogX : public CArxDialogObject
{
	friend class CCustomFileDialog;
	CCustomFileDialog* mpOwner;
	FileDialogParams* mpParams;
protected:
	CFileDialogX( CCustomFileDialog& Owner, CDclFormObject* pDclForm, FileDialogParams* pParams = NULL );
	~CFileDialogX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return false; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return true; }
	virtual HWND GetHWnd() const;
	virtual void CloseDialog(int nStatus);
	virtual INT_PTR DoModal();
	virtual bool Show(bool bShow = true) { return false; }
};


/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog dialog

class CCustomFileDialog : public CFileDialog
{
	CFileDialogX mDialogX;
	CString msTitle;
	CString msFilterList;
	CString msDefaultExtension;
	CString msInitialDirectory;
	CString msResultBuf;
	FileDialogParams* mpParams;
	CMainFileDlg mMainFileDlg;
	CDclControlObject* mpFileDlgCtrl;

public:
	CCustomFileDialog( CDclFormObject* pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	~CCustomFileDialog();

	//Attributes
public:
	CControlPane& GetControlPane() { return mDialogX.GetControlPane(); }
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }
	CMainFileDlg& GetMainDialog() { return mMainFileDlg; }

	void GetResults();	
	void CloseNow();
	void CtrlModifyStyle(int nCtrl);

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
