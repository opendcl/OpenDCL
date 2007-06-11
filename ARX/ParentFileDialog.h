// ParentFileDialog.h : header file
//

#pragma once

#include "ParentDlg.h"
#include "ArxDialogObject.h"

class CFontCollection;
class CDclControlObject;
class CDclFormObject;


class CFileDialogX : public CArxDialogObject
{
	friend class CParentFileDialog;
	CParentFileDialog* mpOwner;
	CParentDlg* mpParent;
protected:
	CFileDialogX( CParentFileDialog& Owner, CDclFormObject* pDclForm, CParentDlg* pParent );
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
	CFileDialogX mDialogX;
	//CParentDlg mParentDlg;
	CString msTitle;
	CString msFilterList;
	FileDialogParams* mpParams;

public:
	bool				m_bInvokeWithSendString;

	CRect m_rcThis;
	CRect m_rcParent;

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
	~CParentFileDialog();
protected:
	void InitializeFromParams( CDclFormObject* pSourceForm, DialogParams* pParams = NULL );

	//Attributes
public:
	//const CParentDlg& GetParentDlg() const { return mParentDlg; }
	//CParentDlg& GetParentDlg() { return mParentDlg; }
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }

	void CloseNow();
	void CtrlModifyStyle(int nCtrl);

protected:	
	DECLARE_MESSAGE_MAP()

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
};
