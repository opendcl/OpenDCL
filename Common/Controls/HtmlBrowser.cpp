// HtmlBrowser.cpp : implementation file
//

#include "StdAfx.h"
#include "HtmlBrowser.h" 


/////////////////////////////////////////////////////////////////////////////
// CHtmlBrowser

// begin memory leak fix code
CString CHtmlBrowser::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);
	return retVal;
}

CString CHtmlBrowser::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_Type(&bstr);
	CString retVal(bstr);
	return retVal;
}

CString CHtmlBrowser::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);
	return retVal;
}

CString CHtmlBrowser::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);
	return retVal;
}

void CHtmlBrowser::Navigate( LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
														 LPCTSTR lpszTargetFrameName /* = NULL */ ,
														 LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
														 DWORD dwPostDataLen /* = 0 */)
{
	CComBSTR bstrURL(lpszURL);
	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	m_pBrowserApp->Navigate(bstrURL, COleVariant((long)dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),vPostData, COleVariant(lpszHeaders,VT_BSTR));
}

BOOL CHtmlBrowser::LoadFromResource( LPCTSTR lpszResource )
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

BOOL CHtmlBrowser::LoadFromResource( UINT nRes )
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

void CHtmlBrowser::OnBeforeNavigate2( LPCTSTR lpszURL,
																			DWORD nFlags,
																			LPCTSTR lpszTargetFrameName,
																			CByteArray& baPostedData,
																			LPCTSTR lpszHeaders,
																			BOOL* pbCancel )
{
	static const TCHAR APP_PROTOCOL[] = _T("app:");
	static const size_t ctAppProtocol = sizeof(APP_PROTOCOL) / sizeof(APP_PROTOCOL[0]);
	if( CString( lpszURL ).Left( ctAppProtocol ).CompareNoCase( APP_PROTOCOL ) == 0 )
	{
		OnAppCmd( lpszURL + ctAppProtocol );
		*pbCancel = TRUE;
	}
}

void CHtmlBrowser::LoadHtmlCode( const CString& sHtmlCode )
{
	if( !m_pBrowserApp )
		return;

	CComPtr< IUnknown > pUnkBrowser;
  HRESULT hr = m_pBrowserApp->QueryInterface( IID_IUnknown,  (void**)&pUnkBrowser );
	
	if ( SUCCEEDED(hr) )
  {
		// Create a stream containing the HTML.
		int cchHtml = sHtmlCode.GetLength() + 1;
		HGLOBAL hHTMLText = GlobalAlloc( GPTR, sizeof(CHAR) * cchHtml );
		if ( hHTMLText )
		{
			lstrcpynA( (CHAR*)hHTMLText, CStringA( sHtmlCode ), cchHtml );
			CComPtr< IStream > pStream;
			hr = CreateStreamOnHGlobal( hHTMLText, TRUE, &pStream );
			if ( SUCCEEDED(hr) )
			{
			   // Call the helper function to load the Web Browser from the stream.
			   LoadWebBrowserFromStream(m_pBrowserApp, pStream);
			}
		}
  }
}

// load the html code into memory
HRESULT CHtmlBrowser::LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream)
{
  // Retrieve the document object.
	CComPtr< IDispatch > pHtmlDoc;
  HRESULT hr = pWebBrowser->get_Document( &pHtmlDoc );
	if( !pHtmlDoc )
	{
		Navigate2( _T("about:blank") );
		if( SUCCEEDED(hr) )
			hr = pWebBrowser->get_Document( &pHtmlDoc );
	}
  if( SUCCEEDED(hr) && pHtmlDoc )
  {
		// Query for IPersistStreamInit.
		CComPtr< IPersistStreamInit > pPersistStreamInit;
		hr = pHtmlDoc->QueryInterface( IID_IPersistStreamInit,  (void**)&pPersistStreamInit );
		if ( SUCCEEDED(hr) )
		{
			// Initialize the document.
			hr = pPersistStreamInit->InitNew();
			if ( SUCCEEDED(hr) )
			{
				// Load the contents of the stream.
				hr = pPersistStreamInit->Load( pStream );
			}
		}
  }
	return hr;
}

void CHtmlBrowser::ReplaceText( LPCTSTR pszOldText, LPCTSTR pszNewText )
{
	LPDISPATCH lpDispatch = GetHtmlDocument();
	if( !lpDispatch )
		return;

	CComPtr< IHTMLDocument2 > pHtmlDocument;
	lpDispatch->QueryInterface( IID_IHTMLDocument2, (void**)&pHtmlDocument );
	ASSERT(pHtmlDocument);

	lpDispatch->Release();

	CComPtr< IHTMLElement > pBodyElm;
	pHtmlDocument->get_body( &pBodyElm );
	ASSERT(pBodyElm);
	pHtmlDocument = NULL;
	CComPtr< IHTMLBodyElement > pBody;
	pBodyElm->QueryInterface( &pBody) ;
	ASSERT(pBody);
	pBodyElm = NULL;
	CComPtr< IHTMLTxtRange > pTxtRange;
	pBody->createTextRange( &pTxtRange );
	ASSERT(pTxtRange);

	CComBSTR bsSearch( lstrlen( pszOldText ) + 1, pszOldText );

	VARIANT_BOOL bFound;	
	long lFlags = 4; // search flag that indicates match case
	while( (pTxtRange->findText( bsSearch, 0, lFlags, &bFound ), bFound == VARIANT_TRUE) )
	{
		pTxtRange->pasteHTML( CComBSTR( pszNewText ) );
		pTxtRange = NULL;
		pBody->createTextRange( &pTxtRange );
		ASSERT(pTxtRange);
	}
}

CString CHtmlBrowser::GetHtmlText()
{
	LPDISPATCH lpDispatch = GetHtmlDocument();
	if( !lpDispatch )
		return _T("");

	CComPtr< IHTMLDocument2 > pHtmlDocument;
	lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDocument);
	lpDispatch->Release();
	if( !pHtmlDocument )
		return _T("");

	CComPtr< IHTMLElement > pBodyElm;
	pHtmlDocument->get_body(&pBodyElm);
	if( !pBodyElm )
		return _T("");
	pHtmlDocument = NULL;
	CComPtr< IHTMLBodyElement > pBody;
	pBodyElm->QueryInterface(IID_IHTMLBodyElement,(void**)&pBody);
	if( !pBody )
		return _T("");
	pBodyElm = NULL;
	CComPtr< IHTMLTxtRange > pTxtRange;
	pBody->createTextRange(&pTxtRange);
	if( !pTxtRange )
		return _T("");
	pBody = NULL;

	CComBSTR html;
	HRESULT hr = pTxtRange->get_htmlText(&html);
	return (LPCWSTR)html;
}


BEGIN_MESSAGE_MAP(CHtmlBrowser, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHtmlBrowser message handlers

void CHtmlBrowser::PostNcDestroy() 
{
	CWnd::PostNcDestroy();
}

void CHtmlBrowser::OnDestroy()
{
	CWnd::OnDestroy(); // bypass CView doc/frame stuff
}

void CHtmlBrowser::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	if( !mbSubclassedControl )
		return;

	// assure that control containment is on
	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, NULL,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
		return;

	// cache the dispinterface
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		return;
	}
}

BOOL CHtmlBrowser::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	mbSubclassedControl = false;
	return __super::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CHtmlBrowser::PreTranslateMessage(MSG* pMsg)
{
	// check if the browser control wants to handle the message
	if(m_pBrowserApp != NULL)
	{
		CComQIPtr<IOleInPlaceActiveObject> spInPlace( m_pBrowserApp );
		if( spInPlace && spInPlace->TranslateAccelerator(pMsg) == S_OK )
			return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg); //skip CView
}

int CHtmlBrowser::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{
	return CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
}
