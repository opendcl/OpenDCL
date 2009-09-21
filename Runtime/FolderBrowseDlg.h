////////////////////////////////////////////////////////////////////////
// FolderBrowseDlg.h: interface for the CFolderBrowseDlg class.
//////////////////////////////////////////////////////////////////////

#pragma once


class CFolderBrowseDlg
{
	CString msCaption;
	CString msInitialFolder;
	DWORD mdwFlags;
	CString msRootFolder;
	CString msSelectedFolder;

public:
	CFolderBrowseDlg( LPCTSTR pszCaption, LPCTSTR pszInitialFolder = NULL, LPCTSTR pszRootFolder = NULL,
							DWORD dwFlags = BIF_RETURNONLYFSDIRS );
	virtual ~CFolderBrowseDlg();

	BOOL DoBrowse(CWnd *pwndParent = NULL);
	const CString& GetSelectedFolder() const { return msSelectedFolder; }

private:
	virtual BOOL SelChanged(LPCTSTR pszSelection, CString& csStatusText) { return TRUE; };
	static int __stdcall CFolderBrowseDlg::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};
