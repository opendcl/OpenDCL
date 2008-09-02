// HtmlCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "HtmlCtrl.h" 
#include "ControlPane.h"
#include "Workspace.h"
#include "SharedRes.Local.h"


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl

CHtmlCtrl::CHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CHtmlCtrl::~CHtmlCtrl()
{
}

bool CHtmlCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		SetSilent( FALSE );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CHtmlCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	switch( mpTemplate->GetLongProperty( Prop::Justification ) )
	{
	case 0:
		dwStyle |= SS_LEFT;
		break;
	case 1:
		dwStyle |= SS_CENTER;
		break;
	case 2:
		dwStyle |= SS_RIGHT;
		break;
	}
	return dwStyle;
}

bool CHtmlCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Justification:
		{
			switch( pProp->GetLongValue() )
			{
			case 0:
				ModifyStyle( (SS_CENTER | SS_RIGHT), SS_LEFT, 0 );
				break;
			case 1:
				ModifyStyle( (SS_LEFT | SS_RIGHT), SS_CENTER, 0 );
				break;
			case 2:
				ModifyStyle( (SS_LEFT | SS_CENTER), SS_RIGHT, 0 );
				break;
			}
			OnNeedRepaint();
		}
		break;
	}
	return !bFailed;
}

// begin memory leak fix code
CString CHtmlCtrl::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);

	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString CHtmlCtrl::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_Type(&bstr);

	CString retVal(bstr);

	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString CHtmlCtrl::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);

	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString CHtmlCtrl::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);

	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

void CHtmlCtrl::Navigate( LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
													LPCTSTR lpszTargetFrameName /* = NULL */ ,
													LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
													DWORD dwPostDataLen /* = 0 */)
{
	CString strURL(lpszURL);
	BSTR bstrURL = strURL.AllocSysString();

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	m_pBrowserApp->Navigate(bstrURL, COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),vPostData, COleVariant(lpszHeaders,VT_BSTR));

	SysFreeString(bstrURL); // Added this line to prevent leak.
}

BOOL CHtmlCtrl::LoadFromResource( LPCTSTR lpszResource )
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

BOOL CHtmlCtrl::LoadFromResource( UINT nRes )
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
//end memory leak fix code

void CHtmlCtrl::OnBeforeNavigate2( LPCTSTR lpszURL,
																	 DWORD nFlags,
																	 LPCTSTR lpszTargetFrameName,
																	 CByteArray& baPostedData,
																	 LPCTSTR lpszHeaders,
																	 BOOL* pbCancel )
{
	const TCHAR APP_PROTOCOL[] = _T("app:");
	int len = _tcslen(APP_PROTOCOL);
	if( _tcsnicmp( lpszURL, APP_PROTOCOL, len ) ==0 )
	{
		OnAppCmd( lpszURL + len );
		*pbCancel = TRUE;
	}
}

void CHtmlCtrl::LoadHtmlCode( const CString& sHtmlCode )
{
	HRESULT hr;
	IUnknown* pUnkBrowser = NULL;
	IUnknown* pUnkDisp = NULL;
	IStream* pStream = NULL;
	LPDISPATCH pDisp = NULL;
	HGLOBAL hHTMLText;
	
	// Is this the DocumentComplete event for the top frame window?
  // Check COM identity: compare IUnknown interface pointers.
  hr = m_pBrowserApp->QueryInterface( IID_IUnknown,  (void**)&pUnkBrowser );
	
	if ( SUCCEEDED(hr) )
  {
		// This is the DocumentComplete event for the top frame - page is loaded!
		// Create a stream containing the HTML.
		// Alternatively, this stream may have been passed to us.
		int cchHtml = sHtmlCode.GetLength() + 1;
		hHTMLText = GlobalAlloc( GPTR, sizeof(TCHAR) * cchHtml );
		if ( hHTMLText )
		{
			lstrcpyn( (TCHAR*)hHTMLText, sHtmlCode, cchHtml );
			hr = CreateStreamOnHGlobal( hHTMLText, TRUE, &pStream );
			if ( SUCCEEDED(hr) )
			{
			   // Call the helper function to load the Web Browser from the stream.
			   LoadWebBrowserFromStream(m_pBrowserApp, pStream);
			   pStream->Release();
			}
		}
  }
}

// load the html code into memory
HRESULT CHtmlCtrl::LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream)
{
	HRESULT hr;
	IDispatch* pHtmlDoc = NULL;
	IPersistStreamInit* pPersistStreamInit = NULL;

  // Retrieve the document object.
  hr = pWebBrowser->get_Document( &pHtmlDoc );
  if ( SUCCEEDED(hr) )
  {
		if (pHtmlDoc != NULL)
		{
			// Query for IPersistStreamInit.
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
				pPersistStreamInit->Release();
			}
		}
		else
		{
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_BLANKHTML);
			theWorkspace.DisplayAlert(sLoadString);	
		}
  }
	return hr;
}

void CHtmlCtrl::ReplaceText( LPCTSTR pszOldText, LPCTSTR pszNewText )
{
	// search flag that indicates match case
	long lFlags = 4;

	IHTMLDocument2 *lpHtmlDocument = NULL;
	LPDISPATCH lpDispatch = NULL;
	lpDispatch = GetHtmlDocument();
	
	if (lpDispatch == NULL)
		return;

	lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&lpHtmlDocument);
	ASSERT(lpHtmlDocument);

	lpDispatch->Release();

	IHTMLElement *lpBodyElm;
	IHTMLBodyElement *lpBody;
	IHTMLTxtRange *lpTxtRange;

	lpHtmlDocument->get_body(&lpBodyElm);
	ASSERT(lpBodyElm);
	lpHtmlDocument->Release();
	lpBodyElm->QueryInterface(IID_IHTMLBodyElement,(void**)&lpBody);
	ASSERT(lpBody);
	lpBodyElm->Release();
	lpBody->createTextRange(&lpTxtRange);
	ASSERT(lpTxtRange);
	lpBody->Release();

	CComBSTR html;
	CComBSTR newhtml;
	CComBSTR search( lstrlen( pszOldText ) + 1, pszOldText );

	long t;
	bool bFound;	
	while(lpTxtRange->findText(search,0,lFlags,(VARIANT_BOOL*)&bFound),bFound)
	{
		newhtml.Empty();
		lpTxtRange->get_htmlText(&html);
		newhtml.Append(pszNewText);
		newhtml.AppendBSTR(html);
		lpTxtRange->pasteHTML(newhtml);
				
		lpTxtRange->moveStart((BSTR)CComBSTR(L"Character"),1,&t);
		lpTxtRange->moveEnd((BSTR)CComBSTR(L"Textedit"),1,&t);
	}

	lpTxtRange->Release();
	
}

CString CHtmlCtrl::GetHtmlText()
{
	// search flag that indicates match case
	long lFlags = 4;

	IHTMLDocument2 *lpHtmlDocument = NULL;
	LPDISPATCH lpDispatch = NULL;
	lpDispatch = GetHtmlDocument();
	
	if (lpDispatch == NULL)
		return CString();

	lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&lpHtmlDocument);
	lpDispatch->Release();
  if (lpHtmlDocument == NULL) {
    return CString();
  }

	IHTMLElement *lpBodyElm;
	IHTMLBodyElement *lpBody;
	IHTMLTxtRange *lpTxtRange;

	lpHtmlDocument->get_body(&lpBodyElm);
	lpHtmlDocument->Release();
  if (lpBodyElm == NULL) {
    return CString();
  }
	lpBodyElm->QueryInterface(IID_IHTMLBodyElement,(void**)&lpBody);
	lpBodyElm->Release();
  if (lpBody == NULL) {
    return CString();
  }

  lpBody->createTextRange(&lpTxtRange);
	lpBody->Release();
  if (lpTxtRange == NULL) {
    return CString();
  }

	CComBSTR html;
	
	HRESULT hr = lpTxtRange->get_htmlText(&html);
	lpTxtRange->Release();
	return (LPCWSTR)html;
}


BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl message handlers

BOOL CHtmlCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CHtmlCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	return mColorService.CtlColor( pDC, nCtlColor );
}

void CHtmlCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy(); //CHtmlView::PostNcDestroy calls 'delete this'
}

void CHtmlCtrl::OnDestroy()
{
	CWnd::OnDestroy(); // bypass CView doc/frame stuff
}

int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
{
	return CWnd::OnMouseActivate( pDesktopWnd, nHitTest, msg ); // bypass CView doc/frame stuff
}
