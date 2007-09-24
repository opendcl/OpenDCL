// HtmlCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "HtmlCtrl.h" 
#include "DclControlObject.h" 
#include "PropertyObject.h" 
#include "InvokeMethod.h"
#include "Resource.h"
#include "PropertyIds.h"
#include "ToolTips.h"
#include "Workspace.h"

//needed until this control is derived from CDialogObject
#define IsAsyncEvents() (m_ArxControl->GetLongProperty( Prop::EventInvoke ) == 1)


 IMPLEMENT_DYNAMIC(CHtmlCtrl, CHtmlView)
 BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
    ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
    ON_WM_MOUSEACTIVATE()
 END_MESSAGE_MAP()

 //////////////////
 // Create control in same position as an existing static control with
 // the same ID (could be any kind of control, really)
 //
 BOOL CHtmlCtrl::Create(TDclControlPtr pControl, CWnd* pParent, UINT nID)
 {
	m_ArxControl = pControl;	
	CRect ArxRect;   

	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

    // create HTML control (CHtmlView)
    BOOL bReturn = CHtmlView::Create(NULL,                  // class name
       NULL,                             // title
       dwStyle,			 // style
       ArxRect,                          // rectangle
       pParent,                          // parent
       nID,                              // control ID
       NULL);                            // frame/doc context not used
	
	CHtmlView::SetSilent(FALSE);

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	return bReturn;
 }
 
 ////////////////
 // Override to avoid CView stuff that assumes a frame.
 //
 void CHtmlCtrl::OnDestroy()
 {
	  // delete the tool tip text control object
  	m_ToolTip.DelTool(this, 1);
    CWnd::OnDestroy(); // bypass CView doc/frame stuff
 }
 
 ////////////////
 // Override to avoid CView stuff that assumes a frame.
 //
 int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
 {
    // bypass CView doc/frame stuff
    return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, msg);
 }
 
 //////////////////
 // Override navigation handler to pass to "app:" links to virtual handler.
 // Cancels the navigation in the browser, since app: is a pseudo-protocol.
 //
 void CHtmlCtrl::OnBeforeNavigate2( LPCTSTR lpszURL,
    DWORD nFlags,
    LPCTSTR lpszTargetFrameName,
    CByteArray& baPostedData,
    LPCTSTR lpszHeaders,
    BOOL* pbCancel )
 {
   const TCHAR APP_PROTOCOL[] = _T("app:");
    int len = _tcslen(APP_PROTOCOL);
    if (_tcsnicmp(lpszURL, APP_PROTOCOL, len)==0) {
       OnAppCmd(lpszURL + len);
       *pbCancel = TRUE;
    }
 }
 
 void CHtmlCtrl::OnAppCmd(LPCTSTR lpszWhere)
 {
    // default: do nothing
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

void CHtmlCtrl::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
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


BOOL CHtmlCtrl::LoadFromResource(LPCTSTR lpszResource)
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

BOOL CHtmlCtrl::LoadFromResource(UINT nRes)
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

void CHtmlCtrl::OnNavigateComplete2(LPCTSTR strURL) 
{
	// call methods to invoke the event
	InvokeMethodString(m_ArxControl->GetStringProperty(Prop::EventNavigateComplete), strURL, IsAsyncEvents());
	CHtmlView::OnNavigateComplete2(strURL);
}

void CHtmlCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	CHtmlView::OnMouseMove(nFlags, point);
}

BOOL CHtmlCtrl::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CHtmlView::PreTranslateMessage(pMsg);
} // End of PreTranslateMessage

void CHtmlCtrl::LoadHtmlCode(CString sHtmlCode) 
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


void CHtmlCtrl::ReplaceText(CString sOldText, CString sNewText)
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
	CComBSTR search(sOldText.GetLength()+1,(LPCTSTR)sOldText);

	long t;
	bool bFound;	
	while(lpTxtRange->findText(search,0,lFlags,(VARIANT_BOOL*)&bFound),bFound)
	{
		newhtml.Empty();
		lpTxtRange->get_htmlText(&html);
		newhtml.Append((LPCTSTR)sNewText);
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


