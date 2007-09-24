// HtmlCtrl.h : header file
//

#pragma once

#include "PPToolTip.h"
#include <afxhtml.h>
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl html view


class CHtmlCtrl : public CHtmlView {
public:
    CHtmlCtrl() {
		// No tooltip created
		m_ToolTip.m_hWnd = NULL;
		}
    ~CHtmlCtrl() { }
// memory leak fix methods
public:
	
	void ReplaceText(CString sOldText, CString sNewText);
	CPPToolTip m_ToolTip;
	
	CString GetFullName() const;
    CString GetType() const;
    CString GetLocationName() const;
    CString GetLocationURL() const;

    void Navigate(LPCTSTR lpszURL, DWORD dwFlags = 0 ,
                  LPCTSTR lpszTargetFrameName = NULL ,
                  LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,

                  DWORD dwPostDataLen = 0);
	BOOL LoadFromResource(LPCTSTR lpszResource);
    BOOL LoadFromResource(UINT nRes);

// normal methods
public:
	TDclControlPtr m_ArxControl;
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
    // Normally, CHtmlView destroys itself in PostNcDestroy,
    // but we don't want to do that for a control since a control
    // is usually implemented as a stack object in a dialog.
    //
    virtual void PostNcDestroy() {  }
 
    // overrides to bypass MFC doc/view frame dependencies
    afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
    virtual void OnNavigateComplete2(LPCTSTR strURL);
    // override to trap "app:" pseudo protocol
    virtual void OnBeforeNavigate2( LPCTSTR lpszURL,
       DWORD nFlags,
       LPCTSTR lpszTargetFrameName,
       CByteArray& baPostedData,
       LPCTSTR lpszHeaders,
       BOOL* pbCancel );
 
    // override to handle links to "app:mumble...". lpszWhere will be "mumble"
    virtual void OnAppCmd(LPCTSTR lpszWhere);
	virtual BOOL PreTranslateMessage(MSG* pMsg);	

	HRESULT LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream);
	void LoadHtmlCode(CString sHtmlCode);
	CString GetHtmlText();

public:
	
#ifdef _DEBUG
	virtual void AssertValid() const
	{
	}
	virtual void Dump(CDumpContext& dc) const
	{
	}
#endif

    DECLARE_MESSAGE_MAP();
    DECLARE_DYNAMIC(CHtmlCtrl)
 };

 // Microsoft Systems Journal -- January 2000
 // If this code works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 // Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT too.
 //
