// HtmlBrowser.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CHtmlBrowser window
// Outer CWnd. Edge WebView2 is parented to a plain child CWnd. IE WebBrowser
// is a separate child CHtmlView created only if WebView2 is unavailable.

class CIeHtmlView;

class CHtmlBrowser : public CWnd
{
	bool mbSubclassedControl;
	CIeHtmlView* m_ie;
	struct Wv2;
	Wv2* m_wv2;

// Construction
public:
	CHtmlBrowser();
	virtual ~CHtmlBrowser();

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
	void Navigate2( LPCTSTR lpszURL, DWORD dwFlags = 0,
									LPCTSTR lpszTargetFrameName = NULL,
									LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
									DWORD dwPostDataLen = 0 );
	void Stop();
	void Refresh();
	void GoBack();
	void GoForward();
	void GoHome();
	void GoSearch();
	BOOL GetBusy() const;
	BOOL GetOffline() const;
	void SetOffline( BOOL bOffline );
	void SetSilent( BOOL bSilent );
	BOOL LoadFromResource( LPCTSTR lpszResource );
	BOOL LoadFromResource( UINT nRes );
	HRESULT LoadWebBrowserFromStream( IWebBrowser* pWebBrowser, IStream* pStream );
	void LoadHtmlCode( const CString& sHtmlCode );
	CString GetHtmlText();
	HRESULT SetOpticalZoom( long nZoomPercentage );

protected:
	virtual void OnAppCmd( LPCTSTR lpszWhere ) {}
	virtual void OnStatusTextChange(LPCTSTR) {}
	virtual void OnBeforeNavigate2( LPCTSTR lpszURL,
													DWORD nFlags,
													LPCTSTR lpszTargetFrameName,
													CByteArray& baPostedData,
													LPCTSTR lpszHeaders,
													BOOL* pbCancel );
	virtual HRESULT OnUpdateUI() { return S_OK; }
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO* pInfo);
	virtual void OnNavigateComplete2(LPCTSTR) {}
	virtual void OnDocumentComplete(LPCTSTR) {}
	virtual void OnCommandStateChange(long /*nCommand*/, BOOL /*bEnable*/) {}

	bool UsingWebView2() const;
	bool UsingInternetExplorer() const;
	IWebBrowser2* IeApp() const;
	void CreateInternetExplorerChild();
	bool StartWebView2();
	void FallbackToInternetExplorer();
	void DrainQueue();
	void LayoutChildren();

	friend class CIeHtmlView;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg LRESULT OnWebView2Environment(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebView2Controller(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebView2Nav(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebView2Script(WPARAM wParam, LPARAM lParam);
	void PostNcDestroy() override;
	void PreSubclassWindow() override;
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
};
