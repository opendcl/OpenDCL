////////////////////////////////////////////////////////////////////////
// DirDialog.h: interface for the CDirDialog class.
//////////////////////////////////////////////////////////////////////

#pragma once


class CDirDialog
{
public:
	CDirDialog();
	virtual ~CDirDialog();

	BOOL DoBrowse(CWnd *pwndParent = NULL);

	CString m_strWindowTitle;
	CString m_strPath;
	CString m_strInitDir;
	CString m_strSelDir;
	CString m_strTitle;
	int m_iImageIndex;
	BOOL m_bStatus;

private:
	virtual BOOL SelChanged(LPCSTR lpcsSelection, CString& csStatusText) { return TRUE; };
	static int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};
