#pragma once

#include "ResizableDialog.h"
#include "HtmlBrowser.h"
#include "Resource.h"
#include <map>

class CTreeNode;


// CControlBrowser dialog

class CControlBrowser : public CResizableDialog
{
	TDclControlPtr mpDclControl;
	TPropertyPtr mpInitialProp;
	HTREEITEM mhtiInitialProp;
	CImageList mImageList;
	CTreeCtrl mObjectTree;
	bool mbClosing;
	std::map< CString, CString > mParams;
	class NoNavigateBrowser : public CHtmlBrowser
	{
		CControlBrowser& mBrowser;
		bool mbEnableNavigate;
	public:
		NoNavigateBrowser( CControlBrowser& Browser ) : mBrowser( Browser ), mbEnableNavigate( false ) {}
	protected:
		virtual void OnDocumentComplete(LPCTSTR lpszURL);
		virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
		virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	public:
		void Navigate(LPCTSTR URL, DWORD dwFlags = 0,
									LPCTSTR lpszTargetFrameName = NULL,
									LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
									DWORD dwPostDataLen = 0)
			{
				mbEnableNavigate = true;
				__super::Navigate( URL, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen );
			}
		void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL)
			{
				mbEnableNavigate = true;
				__super::Navigate2( pIDL, dwFlags, lpszTargetFrameName );
			}
		void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
									 LPCTSTR lpszTargetFrameName = NULL,	LPCTSTR lpszHeaders = NULL,
									 LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0)
			{
				mbEnableNavigate = true;
				__super::Navigate2( lpszURL, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen );
			}
		void Navigate2(LPCTSTR lpszURL, DWORD dwFlags, CByteArray& baPostedData,
									 LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL)
			{
				mbEnableNavigate = true;
				__super::Navigate2( lpszURL, dwFlags, baPostedData, lpszTargetFrameName, lpszHeader );
			}
	} mDescription;

// Dialog Data
	enum { IDD = IDD_CONTROLBROWSER };

public:
	CControlBrowser( TDclControlPtr pDclControl, CWnd* pParent = NULL );   // standard constructor
	CControlBrowser( TPropertyPtr pProp, CWnd* pParent = NULL );   // standard constructor
	virtual ~CControlBrowser();

public:
	HTREEITEM InsertItem( HTREEITEM hParent, CTreeNode* pItem );
	void RemoveItem( HTREEITEM hTarget );
	TDclControlPtr GetMainControl() const { return mpDclControl; }
	void SetDescription( LPCTSTR pszDescription, const std::map< CString, CString >& params );

	virtual void OnDocumentLoaded( bool bClickedLink );
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);

protected:
	//bool OnBeginClipboardCopy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBackClicked();
	afx_msg void OnForwardClicked();
};
