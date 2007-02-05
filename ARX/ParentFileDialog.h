// ParentFileDialog.h : header file
//

#pragma once

#include "ParentDlg.h"

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


/////////////////////////////////////////////////////////////////////////////
// CParentFileDialog dialog

class CParentFileDialog : public CFileDialog
{
	CParentDlg mParentDlg;
	CString msTitle;
	CString msFilterList;
	FileDialogParams* mpParams;

public:
	bool				m_bInvokeWithSendString;

	CRect m_rcThis;
	CRect m_rcParent;

	enum { IDD = IDD_CUSTOM_FILE_DIALOG };

public:
	CParentFileDialog( CDclFormObject* pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	CParentFileDialog(
		CDclFormObject* pSourceForm,
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
protected:
	void InitializeFromParams( CDclFormObject* pSourceForm, DialogParams* pParams = NULL );

	//Attributes
public:
	const CParentDlg& GetParentDlg() const { return mParentDlg; }
	CParentDlg& GetParentDlg() { return mParentDlg; }
	CDialogObject& GetDialogObject() { return mParentDlg.GetDialogObject(); }
	const CDialogObject& GetDialogObject() const { return mParentDlg.GetDialogObject(); }

	void CloseNow();
	void CtrlModifyStyle(int nCtrl);

protected:	
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void OnSelectionChanged();

	afx_msg void OnHelp();
	virtual BOOL OnFileNameOK();	
	virtual void OnFolderChange();
	virtual void OnTypeChange();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
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
