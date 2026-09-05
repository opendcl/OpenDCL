// HtmlBrowser.cpp : implementation file
//

#include "StdAfx.h"
#include "HtmlBrowser.h"

#include <memory>
#include <vector>

#include <shlobj.h>
#include <shlwapi.h>

#include "../../Library/WebView2/include/WebView2.h"

#if (_MSC_VER <= 1400)
#define OLECMDID_OPTICAL_ZOOM ((OLECMDID)63)
#endif

// WRL requires NTDDI_VISTA+; BRX/GRX/legacy ARX still compile this TU with
// WINVER 0x0500/0x0501. Homegrown COM callbacks keep the host SDK macros intact.
template<typename Interface, typename Arg1, typename Arg2, typename Fn>
class CWv2Handler : public Interface
{
	LONG m_refs;
	Fn m_fn;
public:
	explicit CWv2Handler(const Fn& fn) : m_refs(1), m_fn(fn) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
	{
		if (!ppv)
			return E_POINTER;
		if (riid == IID_IUnknown || riid == __uuidof(Interface))
		{
			*ppv = static_cast<Interface*>(this);
			AddRef();
			return S_OK;
		}
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return (ULONG)InterlockedIncrement(&m_refs);
	}
	ULONG STDMETHODCALLTYPE Release()
	{
		const ULONG n = (ULONG)InterlockedDecrement(&m_refs);
		if (n == 0)
			delete this;
		return n;
	}
	HRESULT STDMETHODCALLTYPE Invoke(Arg1 a, Arg2 b)
	{
		return m_fn(a, b);
	}
};

template<typename Interface, typename Arg1, typename Arg2, typename Fn>
CComPtr<Interface> Wv2Handler(const Fn& fn)
{
	CComPtr<Interface> p;
	p.Attach(new CWv2Handler<Interface, Arg1, Arg2, Fn>(fn));
	return p;
}

#define WM_OPENDCL_WV2_ENV    (WM_APP + 210)
#define WM_OPENDCL_WV2_CTRL   (WM_APP + 211)
#define WM_OPENDCL_WV2_NAV    (WM_APP + 212)
#define WM_OPENDCL_WV2_SCRIPT (WM_APP + 213)

/////////////////////////////////////////////////////////////////////////////
// IE child: CHtmlView hosted under the outer CWnd. Never shares an HWND with WebView2.

class CIeHtmlView : public CHtmlView
{
	CHtmlBrowser* m_owner;
public:
	explicit CIeHtmlView(CHtmlBrowser* owner) : m_owner(owner) {}

	IWebBrowser2* App() const { return m_pBrowserApp; }

	void OnStatusTextChange(LPCTSTR) override {}
	HRESULT OnUpdateUI() override
	{
		return m_owner ? m_owner->OnUpdateUI() : CHtmlView::OnUpdateUI();
	}
	HRESULT OnGetHostInfo(DOCHOSTUIINFO* pInfo) override
	{
		HRESULT hr = CHtmlView::OnGetHostInfo(pInfo);
		if (FAILED(hr))
			return hr;
		return m_owner ? m_owner->OnGetHostInfo(pInfo) : S_OK;
	}
	void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName,
		CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel) override
	{
		if (m_owner)
			m_owner->OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
	}
	void OnNavigateComplete2(LPCTSTR strURL) override
	{
		if (m_owner)
			m_owner->OnNavigateComplete2(strURL);
		CHtmlView::OnNavigateComplete2(strURL);
	}
	void OnDocumentComplete(LPCTSTR lpszURL) override
	{
		if (m_owner)
			m_owner->OnDocumentComplete(lpszURL);
		CHtmlView::OnDocumentComplete(lpszURL);
	}
	void OnCommandStateChange(long nCommand, BOOL bEnable) override
	{
		if (m_owner)
			m_owner->OnCommandStateChange(nCommand, bEnable);
		CHtmlView::OnCommandStateChange(nCommand, bEnable);
	}
	void PostNcDestroy() override { CWnd::PostNcDestroy(); }
	void OnDestroy() { CWnd::OnDestroy(); }

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CIeHtmlView, CHtmlView)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WebView2 state

struct CHtmlBrowser::Wv2
{
	enum Kind { None, Starting, Ready, Failed };
	enum OpKind { OpNavigate, OpHtml, OpStop, OpRefresh, OpBack, OpForward, OpHome, OpSearch, OpZoom, OpReplace };
	struct Op { OpKind kind; CString a; CString b; long n; };

	Kind kind;
	bool offline;
	bool busy;
	bool silent;
	long zoomPercent;
	CString locationUrl;
	CString locationName;
	CString homeUrl;
	CString scriptResult;
	bool scriptDone;
	std::vector<Op> queue;
	CComPtr<ICoreWebView2Environment> env;
	CComPtr<ICoreWebView2Controller> controller;
	CComPtr<ICoreWebView2> webview;
	EventRegistrationToken tokNavStarting;
	EventRegistrationToken tokNavCompleted;
	EventRegistrationToken tokNewWindow;
	CWnd host;
	HMODULE loader;

	Wv2()
		: kind(None), offline(false), busy(false), silent(false)
		, zoomPercent(100), scriptDone(false), loader(NULL)
	{
		tokNavStarting.value = 0;
		tokNavCompleted.value = 0;
		tokNewWindow.value = 0;
	}

	void Close()
	{
		if (webview)
		{
			if (tokNavStarting.value)
				webview->remove_NavigationStarting(tokNavStarting);
			if (tokNavCompleted.value)
				webview->remove_NavigationCompleted(tokNavCompleted);
			if (tokNewWindow.value)
				webview->remove_NewWindowRequested(tokNewWindow);
		}
		tokNavStarting.value = 0;
		tokNavCompleted.value = 0;
		tokNewWindow.value = 0;
		if (controller)
		{
			controller->Close();
			controller.Release();
		}
		webview.Release();
		env.Release();
		if (host.GetSafeHwnd())
			host.DestroyWindow();
		kind = None;
		busy = false;
	}

	void Enqueue(OpKind k, const CString& a = CString(), const CString& b = CString(), long n = 0)
	{
		Op op;
		op.kind = k;
		op.a = a;
		op.b = b;
		op.n = n;
		queue.push_back(op);
	}
};

// WebView2 string getters return CoTaskMemAlloc LPWSTR, not BSTR.
static CString Wv2TakeString(LPWSTR s)
{
	CString out(s);
	if (s)
		CoTaskMemFree(s);
	return out;
}

static CString JsonUnquote(LPCWSTR json)
{
	if (!json || !json[0] || wcscmp(json, L"null") == 0)
		return CString();
	CString s(json);
	if (s.GetLength() >= 2 && s[0] == L'"' && s[s.GetLength() - 1] == L'"')
		s = s.Mid(1, s.GetLength() - 2);
	CString out;
	out.Preallocate(s.GetLength());
	for (int i = 0; i < s.GetLength(); ++i)
	{
		if (s[i] == L'\\' && i + 1 < s.GetLength())
		{
			const WCHAR c = s[++i];
			switch (c)
			{
			case L'"': out += L'"'; break;
			case L'\\': out += L'\\'; break;
			case L'/': out += L'/'; break;
			case L'n': out += L'\n'; break;
			case L'r': out += L'\r'; break;
			case L't': out += L'\t'; break;
			default: out += c; break;
			}
		}
		else
			out += s[i];
	}
	return out;
}

static CString JsEscape(const CString& s)
{
	CString out;
	out.Preallocate(s.GetLength());
	for (int i = 0; i < s.GetLength(); ++i)
	{
		const WCHAR c = s[i];
		if (c == L'\\' || c == L'\'' || c == L'"')
		{
			out += L'\\';
			out += c;
		}
		else if (c == L'\n')
			out += L"\\n";
		else if (c == L'\r')
			out += L"\\r";
		else
			out += c;
	}
	return out;
}

static bool PumpUntil(bool& done, DWORD timeoutMs)
{
	const DWORD start = GetTickCount();
	while (!done)
	{
		if (GetTickCount() - start >= timeoutMs)
			return false;
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (done)
			return true;
		Sleep(10);
	}
	return true;
}

static HMODULE LoadWebView2Loader()
{
	HMODULE self = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCTSTR)&LoadWebView2Loader, &self);
	TCHAR dir[MAX_PATH] = {};
	if (self)
		GetModuleFileName(self, dir, MAX_PATH);
	PathRemoveFileSpec(dir);

	CString path;
	path.Format(_T("%s\\WebView2Loader.dll"), dir);
	HMODULE loader = LoadLibrary(path);
	if (loader)
		return loader;
#ifdef _WIN64
	path.Format(_T("%s\\WebView2\\x64\\WebView2Loader.dll"), dir);
#else
	path.Format(_T("%s\\WebView2\\x86\\WebView2Loader.dll"), dir);
#endif
	loader = LoadLibrary(path);
	if (loader)
		return loader;
	return LoadLibrary(_T("WebView2Loader.dll"));
}

static CString WebView2UserDataFolder()
{
	TCHAR appdata[MAX_PATH] = {};
	if (FAILED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata)))
		GetTempPath(MAX_PATH, appdata);
	CString folder;
	folder.Format(_T("%s\\OpenDCL\\WebView2\\%u"), appdata, GetCurrentProcessId());
	SHCreateDirectoryEx(NULL, folder, NULL);
	return folder;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlBrowser

CHtmlBrowser::CHtmlBrowser()
	: mbSubclassedControl(true)
	, m_ie(NULL)
	, m_wv2(new Wv2())
{
}

CHtmlBrowser::~CHtmlBrowser()
{
	if (m_wv2)
	{
		m_wv2->Close();
		delete m_wv2;
		m_wv2 = NULL;
	}
	if (m_ie)
	{
		if (m_ie->GetSafeHwnd())
			m_ie->DestroyWindow();
		delete m_ie;
		m_ie = NULL;
	}
}

bool CHtmlBrowser::UsingWebView2() const
{
	return m_wv2 && m_wv2->kind == Wv2::Ready && m_wv2->webview;
}

bool CHtmlBrowser::UsingInternetExplorer() const
{
	return m_ie != NULL && m_ie->GetSafeHwnd() != NULL && IeApp() != NULL;
}

IWebBrowser2* CHtmlBrowser::IeApp() const
{
	return m_ie ? m_ie->App() : NULL;
}

HRESULT CHtmlBrowser::OnGetHostInfo(DOCHOSTUIINFO* /*pInfo*/)
{
	return S_OK;
}

void CHtmlBrowser::CreateInternetExplorerChild()
{
	if (m_ie && m_ie->GetSafeHwnd())
		return;
	if (m_wv2 && m_wv2->host.GetSafeHwnd())
		m_wv2->host.ShowWindow(SW_HIDE);

	AfxEnableControlContainer();
	if (!m_ie)
		m_ie = new CIeHtmlView(this);

	CRect rc;
	GetClientRect(&rc);
	if (rc.IsRectEmpty())
		rc.SetRect(0, 0, 320, 240);
	if (!m_ie->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rc, this, AFX_IDW_PANE_FIRST))
	{
		delete m_ie;
		m_ie = NULL;
	}
}

bool CHtmlBrowser::StartWebView2()
{
	if (!m_wv2)
		m_wv2 = new Wv2();
	m_wv2->loader = LoadWebView2Loader();
	if (!m_wv2->loader)
		return false;

	typedef HRESULT (STDAPICALLTYPE *FnCreateEnv)(
		PCWSTR, PCWSTR, ICoreWebView2EnvironmentOptions*,
		ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*);
	FnCreateEnv createEnv = (FnCreateEnv)GetProcAddress(
		m_wv2->loader, "CreateCoreWebView2EnvironmentWithOptions");
	if (!createEnv)
		return false;

	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() < 8 || rc.Height() < 8)
		rc.SetRect(0, 0, 320, 240);
	if (!m_wv2->host.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rc, this, 0x5756))
		return false;

	const CString userData = WebView2UserDataFolder();
	const HWND hwnd = m_hWnd;
	m_wv2->kind = Wv2::Starting;
	m_wv2->busy = true;

	const HRESULT hr = createEnv(
		nullptr, userData, nullptr,
		Wv2Handler<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler, HRESULT, ICoreWebView2Environment*>(
			[hwnd](HRESULT errorCode, ICoreWebView2Environment* env) -> HRESULT {
				if (env)
					env->AddRef();
				if (IsWindow(hwnd))
					::PostMessage(hwnd, WM_OPENDCL_WV2_ENV, (WPARAM)errorCode, (LPARAM)env);
				else if (env)
					env->Release();
				return S_OK;
			}));
	if (FAILED(hr))
	{
		m_wv2->Close();
		return false;
	}
	return true;
}

void CHtmlBrowser::FallbackToInternetExplorer()
{
	if (m_wv2)
	{
		m_wv2->kind = Wv2::Failed;
		m_wv2->Close();
	}
	CreateInternetExplorerChild();
	DrainQueue();
}

void CHtmlBrowser::DrainQueue()
{
	if (!m_wv2)
		return;
	std::vector<Wv2::Op> ops;
	ops.swap(m_wv2->queue);
	for (size_t i = 0; i < ops.size(); ++i)
	{
		const Wv2::Op& op = ops[i];
		switch (op.kind)
		{
		case Wv2::OpNavigate: Navigate2(op.a); break;
		case Wv2::OpHtml: LoadHtmlCode(op.a); break;
		case Wv2::OpStop: Stop(); break;
		case Wv2::OpRefresh: Refresh(); break;
		case Wv2::OpBack: GoBack(); break;
		case Wv2::OpForward: GoForward(); break;
		case Wv2::OpHome: GoHome(); break;
		case Wv2::OpSearch: GoSearch(); break;
		case Wv2::OpZoom: SetOpticalZoom(op.n); break;
		case Wv2::OpReplace: ReplaceText(op.a, op.b); break;
		}
	}
}

void CHtmlBrowser::LayoutChildren()
{
	CRect rc;
	GetClientRect(&rc);
	if (m_wv2 && m_wv2->host.GetSafeHwnd())
	{
		m_wv2->host.MoveWindow(rc);
		if (m_wv2->controller)
		{
			RECT bounds = { 0, 0, rc.Width(), rc.Height() };
			if (bounds.right < 8)
				bounds.right = 320;
			if (bounds.bottom < 8)
				bounds.bottom = 240;
			m_wv2->controller->put_Bounds(bounds);
		}
	}
	if (m_ie && m_ie->GetSafeHwnd())
		m_ie->MoveWindow(rc);
}

CString CHtmlBrowser::GetFullName() const
{
	if (UsingWebView2() && m_wv2->env)
	{
		LPWSTR ver = NULL;
		if (SUCCEEDED(m_wv2->env->get_BrowserVersionString(&ver)) && ver)
			return Wv2TakeString(ver);
		return CString();
	}
	IWebBrowser2* app = IeApp();
	if (!app)
		return CString();
	CComBSTR bstr;
	app->get_FullName(&bstr);
	return CString(bstr);
}

CString CHtmlBrowser::GetType() const
{
	if (UsingWebView2())
		return _T("WebView2");
	IWebBrowser2* app = IeApp();
	if (!app)
		return CString();
	CComBSTR bstr;
	app->get_Type(&bstr);
	return CString(bstr);
}

CString CHtmlBrowser::GetLocationName() const
{
	if (m_wv2 && (m_wv2->kind == Wv2::Ready || m_wv2->kind == Wv2::Starting))
		return m_wv2->locationName;
	IWebBrowser2* app = IeApp();
	if (!app)
		return CString();
	CComBSTR bstr;
	app->get_LocationName(&bstr);
	return CString(bstr);
}

CString CHtmlBrowser::GetLocationURL() const
{
	if (m_wv2 && (m_wv2->kind == Wv2::Ready || m_wv2->kind == Wv2::Starting))
		return m_wv2->locationUrl;
	IWebBrowser2* app = IeApp();
	if (!app)
		return CString();
	CComBSTR bstr;
	app->get_LocationURL(&bstr);
	return CString(bstr);
}

void CHtmlBrowser::Navigate( LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
														 LPCTSTR lpszTargetFrameName /* = NULL */ ,
														 LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
														 DWORD dwPostDataLen /* = 0 */)
{
	Navigate2(lpszURL, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen);
}

void CHtmlBrowser::Navigate2( LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
															LPCTSTR lpszTargetFrameName /* = NULL */,
															LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
															DWORD dwPostDataLen /* = 0 */)
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpNavigate, lpszURL);
		m_wv2->busy = true;
		return;
	}
	if (UsingWebView2())
	{
		m_wv2->busy = true;
		if (m_wv2->homeUrl.IsEmpty())
			m_wv2->homeUrl = lpszURL;
		m_wv2->webview->Navigate(CComBSTR(lpszURL));
		return;
	}
	if (m_ie)
		m_ie->Navigate2(lpszURL, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen);
}

void CHtmlBrowser::Stop()
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpStop);
		return;
	}
	if (UsingWebView2())
	{
		m_wv2->webview->Stop();
		m_wv2->busy = false;
		return;
	}
	if (m_ie)
		m_ie->Stop();
}

void CHtmlBrowser::Refresh()
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpRefresh);
		return;
	}
	if (UsingWebView2())
	{
		m_wv2->busy = true;
		m_wv2->webview->Reload();
		return;
	}
	if (m_ie)
		m_ie->Refresh();
}

void CHtmlBrowser::GoBack()
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpBack);
		return;
	}
	if (UsingWebView2())
	{
		BOOL can = FALSE;
		m_wv2->webview->get_CanGoBack(&can);
		if (can)
		{
			m_wv2->busy = true;
			m_wv2->webview->GoBack();
		}
		return;
	}
	if (m_ie)
		m_ie->GoBack();
}

void CHtmlBrowser::GoForward()
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpForward);
		return;
	}
	if (UsingWebView2())
	{
		BOOL can = FALSE;
		m_wv2->webview->get_CanGoForward(&can);
		if (can)
		{
			m_wv2->busy = true;
			m_wv2->webview->GoForward();
		}
		return;
	}
	if (m_ie)
		m_ie->GoForward();
}

void CHtmlBrowser::GoHome()
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpHome);
		return;
	}
	if (UsingWebView2())
	{
		Navigate2(m_wv2->homeUrl.IsEmpty() ? _T("about:blank") : m_wv2->homeUrl);
		return;
	}
	if (m_ie)
		m_ie->GoHome();
}

void CHtmlBrowser::GoSearch()
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpSearch);
		return;
	}
	if (UsingWebView2())
		return;
	if (m_ie)
		m_ie->GoSearch();
}

BOOL CHtmlBrowser::GetBusy() const
{
	if (m_wv2 && (m_wv2->kind == Wv2::Starting || m_wv2->kind == Wv2::Ready))
		return m_wv2->busy ? TRUE : FALSE;
	return (m_ie && m_ie->GetBusy()) ? TRUE : FALSE;
}

BOOL CHtmlBrowser::GetOffline() const
{
	if (m_wv2 && (m_wv2->kind == Wv2::Starting || m_wv2->kind == Wv2::Ready))
		return m_wv2->offline ? TRUE : FALSE;
	return (m_ie && m_ie->GetOffline()) ? TRUE : FALSE;
}

void CHtmlBrowser::SetOffline( BOOL bOffline )
{
	if (m_wv2)
		m_wv2->offline = bOffline ? true : false;
	if (m_ie)
		m_ie->SetOffline(bOffline);
}

void CHtmlBrowser::SetSilent( BOOL bSilent )
{
	if (m_wv2)
		m_wv2->silent = bSilent ? true : false;
	if (UsingWebView2() && m_wv2->webview)
	{
		CComPtr<ICoreWebView2Settings> settings;
		if (SUCCEEDED(m_wv2->webview->get_Settings(&settings)) && settings)
			settings->put_AreDefaultScriptDialogsEnabled(bSilent ? FALSE : TRUE);
		return;
	}
	if (m_ie)
		m_ie->SetSilent(bSilent);
}

BOOL CHtmlBrowser::LoadFromResource( LPCTSTR lpszResource )
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	CString strResourceURL;
	TCHAR szModule[_MAX_PATH] = {};
	if (!GetModuleFileName(hInstance, szModule, _MAX_PATH))
		return FALSE;
	strResourceURL.Format(_T("res://%s/%s"), szModule, lpszResource);
	Navigate(strResourceURL, 0, 0, 0);
	return TRUE;
}

BOOL CHtmlBrowser::LoadFromResource( UINT nRes )
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	CString strResourceURL;
	TCHAR szModule[_MAX_PATH] = {};
	if (!GetModuleFileName(hInstance, szModule, _MAX_PATH))
		return FALSE;
	strResourceURL.Format(_T("res://%s/%d"), szModule, nRes);
	Navigate(strResourceURL, 0, 0, 0);
	return TRUE;
}

void CHtmlBrowser::OnBeforeNavigate2( LPCTSTR lpszURL,
																			DWORD /*nFlags*/,
																			LPCTSTR /*lpszTargetFrameName*/,
																			CByteArray& /*baPostedData*/,
																			LPCTSTR /*lpszHeaders*/,
																			BOOL* pbCancel )
{
	static const TCHAR APP_PROTOCOL[] = _T("app:");
	const int cch = lstrlen(APP_PROTOCOL);
	if (cch > 0 && CString(lpszURL).Left(cch).CompareNoCase(APP_PROTOCOL) == 0)
	{
		OnAppCmd(lpszURL + cch);
		if (pbCancel)
			*pbCancel = TRUE;
	}
}

void CHtmlBrowser::LoadHtmlCode( const CString& sHtmlCode )
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpHtml, sHtmlCode);
		m_wv2->busy = true;
		return;
	}
	if (UsingWebView2())
	{
		m_wv2->busy = true;
		m_wv2->webview->NavigateToString(CComBSTR(sHtmlCode));
		return;
	}

	IWebBrowser2* app = IeApp();
	if (!app)
		return;

	int cchHtml = sHtmlCode.GetLength() + 1;
	HGLOBAL hHTMLText = GlobalAlloc(GPTR, sizeof(CHAR) * cchHtml);
	if (!hHTMLText)
		return;
	lstrcpynA((CHAR*)hHTMLText, CStringA(sHtmlCode), cchHtml);
	CComPtr<IStream> pStream;
	if (SUCCEEDED(CreateStreamOnHGlobal(hHTMLText, TRUE, &pStream)))
		LoadWebBrowserFromStream(app, pStream);
}

HRESULT CHtmlBrowser::LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream)
{
	CComPtr<IDispatch> pHtmlDoc;
	HRESULT hr = pWebBrowser->get_Document(&pHtmlDoc);
	if (!pHtmlDoc)
	{
		Navigate2(_T("about:blank"));
		if (SUCCEEDED(hr))
			hr = pWebBrowser->get_Document(&pHtmlDoc);
	}
	if (SUCCEEDED(hr) && pHtmlDoc)
	{
		CComPtr<IPersistStreamInit> pPersistStreamInit;
		hr = pHtmlDoc->QueryInterface(IID_IPersistStreamInit, (void**)&pPersistStreamInit);
		if (SUCCEEDED(hr))
		{
			hr = pPersistStreamInit->InitNew();
			if (SUCCEEDED(hr))
				hr = pPersistStreamInit->Load(pStream);
		}
	}
	return hr;
}

void CHtmlBrowser::ReplaceText( LPCTSTR pszOldText, LPCTSTR pszNewText )
{
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpReplace, pszOldText, pszNewText);
		return;
	}
	if (UsingWebView2())
	{
		CString script;
		script.Format(
			_T("(function(){var b=document.body;if(!b)return '0';")
			_T("var o='%s';var n='%s';b.innerHTML=b.innerHTML.split(o).join(n);return '1';})()"),
			(LPCTSTR)JsEscape(pszOldText), (LPCTSTR)JsEscape(pszNewText));
		const HWND hwnd = m_hWnd;
		m_wv2->webview->ExecuteScript(CComBSTR(script),
			Wv2Handler<ICoreWebView2ExecuteScriptCompletedHandler, HRESULT, LPCWSTR>(
				[hwnd](HRESULT, LPCWSTR json) -> HRESULT {
					BSTR b = json ? SysAllocString(json) : NULL;
					if (IsWindow(hwnd))
						::PostMessage(hwnd, WM_OPENDCL_WV2_SCRIPT, 0, (LPARAM)b);
					else if (b)
						SysFreeString(b);
					return S_OK;
				}));
		return;
	}

	if (!m_ie)
		return;
	LPDISPATCH lpDispatch = m_ie->GetHtmlDocument();
	if (!lpDispatch)
		return;
	CComPtr<IHTMLDocument2> pHtmlDocument;
	lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDocument);
	lpDispatch->Release();
	if (!pHtmlDocument)
		return;
	CComPtr<IHTMLElement> pBodyElm;
	pHtmlDocument->get_body(&pBodyElm);
	if (!pBodyElm)
		return;
	CComPtr<IHTMLBodyElement> pBody;
	pBodyElm->QueryInterface(&pBody);
	if (!pBody)
		return;
	CComPtr<IHTMLTxtRange> pTxtRange;
	pBody->createTextRange(&pTxtRange);
	if (!pTxtRange)
		return;
	CComBSTR bsSearch(lstrlen(pszOldText) + 1, pszOldText);
	VARIANT_BOOL bFound;
	long lFlags = 4;
	while ((pTxtRange->findText(bsSearch, 0, lFlags, &bFound), bFound == VARIANT_TRUE))
	{
		pTxtRange->pasteHTML(CComBSTR(pszNewText));
		pTxtRange.Release();
		pBody->createTextRange(&pTxtRange);
		if (!pTxtRange)
			break;
	}
}

CString CHtmlBrowser::GetHtmlText()
{
	if (UsingWebView2())
	{
		m_wv2->scriptDone = false;
		m_wv2->scriptResult.Empty();
		const HWND hwnd = m_hWnd;
		m_wv2->webview->ExecuteScript(
			L"document.body?document.body.innerHTML:''",
			Wv2Handler<ICoreWebView2ExecuteScriptCompletedHandler, HRESULT, LPCWSTR>(
				[hwnd](HRESULT, LPCWSTR json) -> HRESULT {
					BSTR b = json ? SysAllocString(json) : NULL;
					if (IsWindow(hwnd))
						::PostMessage(hwnd, WM_OPENDCL_WV2_SCRIPT, 0, (LPARAM)b);
					else if (b)
						SysFreeString(b);
					return S_OK;
				}));
		PumpUntil(m_wv2->scriptDone, 4000);
		return m_wv2->scriptResult;
	}

	if (!m_ie)
		return CString();
	LPDISPATCH lpDispatch = m_ie->GetHtmlDocument();
	if (!lpDispatch)
		return CString();
	CComPtr<IHTMLDocument2> pHtmlDocument;
	lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDocument);
	lpDispatch->Release();
	if (!pHtmlDocument)
		return CString();
	CComPtr<IHTMLElement> pBodyElm;
	pHtmlDocument->get_body(&pBodyElm);
	if (!pBodyElm)
		return CString();
	CComPtr<IHTMLBodyElement> pBody;
	pBodyElm->QueryInterface(IID_IHTMLBodyElement, (void**)&pBody);
	if (!pBody)
		return CString();
	CComPtr<IHTMLTxtRange> pTxtRange;
	pBody->createTextRange(&pTxtRange);
	if (!pTxtRange)
		return CString();
	CComBSTR html;
	pTxtRange->get_htmlText(&html);
	return (LPCWSTR)html;
}

HRESULT CHtmlBrowser::SetOpticalZoom( long nZoomPercentage )
{
	if (m_wv2)
		m_wv2->zoomPercent = nZoomPercentage;
	if (m_wv2 && m_wv2->kind == Wv2::Starting)
	{
		m_wv2->Enqueue(Wv2::OpZoom, CString(), CString(), nZoomPercentage);
		return S_OK;
	}
	if (UsingWebView2() && m_wv2->controller)
	{
		const double factor = nZoomPercentage <= 0 ? 1.0 : (double)nZoomPercentage / 100.0;
		return m_wv2->controller->put_ZoomFactor(factor);
	}
	IWebBrowser2* app = IeApp();
	if (!app)
		return E_FAIL;
	return app->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
		COleVariant(nZoomPercentage, VT_I4), NULL);
}

BEGIN_MESSAGE_MAP(CHtmlBrowser, CWnd)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
	ON_MESSAGE(WM_OPENDCL_WV2_ENV, &CHtmlBrowser::OnWebView2Environment)
	ON_MESSAGE(WM_OPENDCL_WV2_CTRL, &CHtmlBrowser::OnWebView2Controller)
	ON_MESSAGE(WM_OPENDCL_WV2_NAV, &CHtmlBrowser::OnWebView2Nav)
	ON_MESSAGE(WM_OPENDCL_WV2_SCRIPT, &CHtmlBrowser::OnWebView2Script)
END_MESSAGE_MAP()

void CHtmlBrowser::PostNcDestroy()
{
	CWnd::PostNcDestroy();
}

void CHtmlBrowser::OnDestroy()
{
	if (m_wv2)
	{
		m_wv2->kind = Wv2::Failed;
		m_wv2->queue.clear();
		m_wv2->Close();
	}
	if (m_ie && m_ie->GetSafeHwnd())
		m_ie->DestroyWindow();
	CWnd::OnDestroy();
}

void CHtmlBrowser::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	LayoutChildren();
}

void CHtmlBrowser::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
	if (!mbSubclassedControl)
		return;
	if (!StartWebView2())
		CreateInternetExplorerChild();
}

BOOL CHtmlBrowser::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* /*pContext*/)
{
	mbSubclassedControl = false;
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID))
		return FALSE;
	if (!StartWebView2())
		CreateInternetExplorerChild();
	return TRUE;
}

LRESULT CHtmlBrowser::OnWebView2Environment(WPARAM wParam, LPARAM lParam)
{
	CComPtr<ICoreWebView2Environment> env;
	env.Attach((ICoreWebView2Environment*)lParam);
	if (!m_wv2 || m_wv2->kind != Wv2::Starting)
		return 0;
	if (FAILED((HRESULT)wParam) || !env || !m_wv2->host.GetSafeHwnd())
	{
		FallbackToInternetExplorer();
		return 0;
	}
	m_wv2->env = env;
	const HWND hwnd = m_hWnd;
	const HRESULT hr = env->CreateCoreWebView2Controller(
		m_wv2->host.GetSafeHwnd(),
		Wv2Handler<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler, HRESULT, ICoreWebView2Controller*>(
			[hwnd](HRESULT errorCode, ICoreWebView2Controller* controller) -> HRESULT {
				if (controller)
					controller->AddRef();
				if (IsWindow(hwnd))
					::PostMessage(hwnd, WM_OPENDCL_WV2_CTRL, (WPARAM)errorCode, (LPARAM)controller);
				else if (controller)
					controller->Release();
				return S_OK;
			}));
	if (FAILED(hr))
		FallbackToInternetExplorer();
	return 0;
}

LRESULT CHtmlBrowser::OnWebView2Controller(WPARAM wParam, LPARAM lParam)
{
	CComPtr<ICoreWebView2Controller> controller;
	controller.Attach((ICoreWebView2Controller*)lParam);
	if (!m_wv2 || m_wv2->kind != Wv2::Starting)
	{
		if (controller)
			controller->Close();
		return 0;
	}
	if (FAILED((HRESULT)wParam) || !controller)
	{
		FallbackToInternetExplorer();
		return 0;
	}

	m_wv2->controller = controller;
	if (FAILED(controller->get_CoreWebView2(&m_wv2->webview)) || !m_wv2->webview)
	{
		FallbackToInternetExplorer();
		return 0;
	}

	CComPtr<ICoreWebView2Settings> settings;
	if (SUCCEEDED(m_wv2->webview->get_Settings(&settings)) && settings)
	{
		settings->put_IsStatusBarEnabled(FALSE);
		settings->put_AreDevToolsEnabled(FALSE);
		settings->put_AreDefaultScriptDialogsEnabled(m_wv2->silent ? FALSE : TRUE);
	}

	LayoutChildren();
	controller->put_IsVisible(TRUE);
	if (m_wv2->zoomPercent > 0)
		controller->put_ZoomFactor((double)m_wv2->zoomPercent / 100.0);

	const HWND hwnd = m_hWnd;
	m_wv2->webview->add_NavigationStarting(
		Wv2Handler<ICoreWebView2NavigationStartingEventHandler, ICoreWebView2*, ICoreWebView2NavigationStartingEventArgs*>(
			[hwnd](ICoreWebView2*, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
				if (args)
				{
					LPWSTR uri = NULL;
					args->get_Uri(&uri);
					if (uri && _wcsnicmp(uri, L"app:", 4) == 0)
						args->put_Cancel(TRUE);
					if (uri)
						CoTaskMemFree(uri);
				}
				::PostMessage(hwnd, WM_OPENDCL_WV2_NAV, 1, 0);
				return S_OK;
			}), &m_wv2->tokNavStarting);

	m_wv2->webview->add_NavigationCompleted(
		Wv2Handler<ICoreWebView2NavigationCompletedEventHandler, ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs*>(
			[hwnd](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs*) -> HRESULT {
				LPWSTR src = NULL;
				if (sender)
					sender->get_Source(&src);
				if (!::PostMessage(hwnd, WM_OPENDCL_WV2_NAV, 0, (LPARAM)src) && src)
					CoTaskMemFree(src);
				if (sender)
				{
					LPWSTR title = NULL;
					sender->get_DocumentTitle(&title);
					if (title && !::PostMessage(hwnd, WM_OPENDCL_WV2_NAV, 2, (LPARAM)title))
						CoTaskMemFree(title);
				}
				return S_OK;
			}), &m_wv2->tokNavCompleted);

	m_wv2->webview->add_NewWindowRequested(
		Wv2Handler<ICoreWebView2NewWindowRequestedEventHandler, ICoreWebView2*, ICoreWebView2NewWindowRequestedEventArgs*>(
			[](ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) -> HRESULT {
				if (!args)
					return S_OK;
				args->put_Handled(TRUE);
				LPWSTR uri = NULL;
				args->get_Uri(&uri);
				if (sender && uri)
					sender->Navigate(uri);
				if (uri)
					CoTaskMemFree(uri);
				return S_OK;
			}), &m_wv2->tokNewWindow);

	m_wv2->kind = Wv2::Ready;
	m_wv2->busy = false;
	DrainQueue();
	return 0;
}

LRESULT CHtmlBrowser::OnWebView2Nav(WPARAM wParam, LPARAM lParam)
{
	if (!m_wv2)
		return 0;
	if (wParam == 1)
	{
		m_wv2->busy = true;
		return 0;
	}
	if (wParam == 2)
	{
		m_wv2->locationName = Wv2TakeString((LPWSTR)lParam);
		return 0;
	}
	m_wv2->locationUrl = Wv2TakeString((LPWSTR)lParam);
	m_wv2->busy = false;
	if (UsingWebView2())
	{
		const CString url = m_wv2->locationUrl;
		OnNavigateComplete2(url);
		OnDocumentComplete(url);
		BOOL canBack = FALSE;
		BOOL canFwd = FALSE;
		m_wv2->webview->get_CanGoBack(&canBack);
		m_wv2->webview->get_CanGoForward(&canFwd);
		OnCommandStateChange(CSC_NAVIGATEBACK, canBack);
		OnCommandStateChange(CSC_NAVIGATEFORWARD, canFwd);
	}
	return 0;
}

LRESULT CHtmlBrowser::OnWebView2Script(WPARAM, LPARAM lParam)
{
	CComBSTR json;
	json.Attach((BSTR)lParam);
	if (m_wv2)
	{
		m_wv2->scriptResult = JsonUnquote(json);
		m_wv2->scriptDone = true;
	}
	return 0;
}

BOOL CHtmlBrowser::PreTranslateMessage(MSG* pMsg)
{
	IWebBrowser2* app = IeApp();
	if (app)
	{
		CComQIPtr<IOleInPlaceActiveObject> spInPlace(app);
		if (spInPlace && spInPlace->TranslateAccelerator(pMsg) == S_OK)
			return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg);
}

int CHtmlBrowser::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
