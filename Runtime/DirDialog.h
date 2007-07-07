////////////////////////////////////////////////////////////////////////
// DirDialog.h: interface for the CDirDialog class.
//////////////////////////////////////////////////////////////////////

#pragma once


class CDirDialog
{
	CString msCaption;
	CString msInitialFolder;
	DWORD mdwFlags;
	CString msRootFolder;
	CString msSelectedFolder;

public:
	CDirDialog( LPCTSTR pszCaption, LPCTSTR pszInitialFolder = NULL, LPCTSTR pszRootFolder = NULL,
							DWORD dwFlags = BIF_RETURNONLYFSDIRS );
	virtual ~CDirDialog();

	BOOL DoBrowse(CWnd *pwndParent = NULL);
	const CString& GetSelectedFolder() const { return msSelectedFolder; }

private:
	virtual BOOL SelChanged(LPCTSTR pszSelection, CString& csStatusText) { return TRUE; };
	static int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};
