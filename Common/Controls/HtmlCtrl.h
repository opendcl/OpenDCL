// HtmlCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl window

class CHtmlCtrl : public CHtmlView, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CHtmlCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

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
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	virtual BOOL PreTranslateMessage( MSG* pMsg );	
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

 // Microsoft Systems Journal -- January 2000
 // If this code works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 // Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT too.
 //
