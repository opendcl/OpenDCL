// ParentFileDialog.h : header file
//

#pragma once

#include "ParentDlg.h"

class CFontCollection;
class CDclControlObject;
class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CParentFileDialog dialog

class CParentFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CParentFileDialog)

protected:
	CDclFormObject* mpSourceForm;
	CControlPane mControlPane;
	CDclControlObject* mpControl;
	CParentDlg mParentDlg;

public:
	bool				m_bInvokeWithSendString;
	CFontCollection		*m_pFontCollection;	
	CString				m_sProjectName;
	CString				m_sDialogName;
	CDclControlObject	*m_pFileDlgProps;
	CStringArray		*m_pStrList;
	CString				m_sFileName;
	CString				m_sPathName;
	CString				m_sFileTitle;
	CString				m_sFolderPath;

	CRect m_rcThis;
	CRect m_rcParent;
	CWnd  *m_pParent;

public:
	CParentFileDialog(
		CDclFormObject* pSourceForm,
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }
	const CDclControlObject* GetControl() const { return mpControl; }
	CDclControlObject* GetControl() { return mpControl; }
	const CParentDlg& GetParentDlg() const { return mParentDlg; }
	CParentDlg& GetParentDlg() { return mParentDlg; }


	void SetDclForm(CDclFormObject *pDclFormObject);
	BOOL ReadListViewNames();
	void CloseNow();

// Dialog Data
	enum { IDD = IDD_CUSTOM_FILE_DIALOG };

	void StoreFileList();
	void CtrlModifyStyle(int nCtrl);

protected:	
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void OnSelectionChanged();

	afx_msg void OnHelp();
	virtual BOOL OnFileNameOK();	
	virtual void OnFolderChange();
	virtual void OnTypeChange();
	//{{AFX_MSG(CParentFileDialog)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
