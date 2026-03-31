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
	HRESULT SetOpticalZoom( long nZoomPercentage ); //set optical zoom percentage

protected:
	virtual void OnAppCmd( LPCTSTR lpszWhere ) {} //override to handle links to "app:mumble...".
	void OnStatusTextChange(LPCTSTR lpszText) override {} //eat 'em, else they mess up the AutoCAD status bar
	// override to trap "app:" pseudo protocol
	void OnBeforeNavigate2( LPCTSTR lpszURL,
													DWORD nFlags,
													LPCTSTR lpszTargetFrameName,
													CByteArray& baPostedData,
													LPCTSTR lpszHeaders,
													BOOL* pbCancel ) override;
	HRESULT OnUpdateUI() override;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg void PostNcDestroy() override;
	void PreSubclassWindow() override;
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
};

 // Microsoft Systems Journal -- January 2000
 // If this code works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 // Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT too.
 //
