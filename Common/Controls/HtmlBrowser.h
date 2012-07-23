// HtmlBrowser.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CHtmlBrowser window

class CHtmlBrowser : public CHtmlView
{
	bool mbSubclassedControl;

// Construction
public:
	CHtmlBrowser() : mbSubclassedControl( true ) {}
	virtual ~CHtmlBrowser() {}

public:
	void ReplaceText( LPCTSTR pszOldText, LPCTSTR pszNewText );
	CString GetFullName() const;
	CString GetType() const;
	CString GetLocationName() const;
	CString GetLocationURL() const;

	void Navigate( LPCTSTR lpszURL, DWORD dwFlags = 0 ,
								 LPCTSTR lpszTargetFrameName = NULL ,
								 LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
								 DWORD dwPostDataLen = 0 );
	BOOL LoadFromResource( LPCTSTR lpszResource );
	BOOL LoadFromResource( UINT nRes );
	HRESULT LoadWebBrowserFromStream( IWebBrowser* pWebBrowser, IStream* pStream );
	void LoadHtmlCode( const CString& sHtmlCode );
	CString GetHtmlText();

protected:
	virtual void OnStatusTextChange(LPCTSTR lpszText) {} //eat 'em, else they mess up the AutoCAD status bar
	virtual void OnAppCmd( LPCTSTR lpszWhere ) {} //override to handle links to "app:mumble...".
	// override to trap "app:" pseudo protocol
	virtual void OnBeforeNavigate2( LPCTSTR lpszURL,
																	DWORD nFlags,
																	LPCTSTR lpszTargetFrameName,
																	CByteArray& baPostedData,
																	LPCTSTR lpszHeaders,
																	BOOL* pbCancel );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	virtual afx_msg void PostNcDestroy();
	virtual void PreSubclassWindow();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

 // Microsoft Systems Journal -- January 2000
 // If this code works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 // Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT too.
 //
