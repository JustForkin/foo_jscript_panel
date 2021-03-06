#include "stdafx.h"
#include "host.h"
#include "popup_msg.h"
#include "user_message.h"

HostComm::HostComm()
	: m_hwnd(NULL)
	, m_hdc(NULL)
	, m_width(0)
	, m_height(0)
	, m_gr_bmp(NULL)
	, m_suppress_drawing(false)
	, m_paint_pending(false)
	, m_instance_type(KInstanceTypeCUI)
	, m_script_info(get_config_guid())
	, m_panel_tooltip_param_ptr(new panel_tooltip_param)
{
	m_max_size.x = INT_MAX;
	m_max_size.y = INT_MAX;

	m_min_size.x = 0;
	m_min_size.y = 0;
}

HostComm::~HostComm() {}

GUID HostComm::GetGUID()
{
	return get_config_guid();
}

HDC HostComm::GetHDC()
{
	return m_hdc;
}

HWND HostComm::GetHWND()
{
	return m_hwnd;
}

POINT& HostComm::MaxSize()
{
	return m_max_size;
}

POINT& HostComm::MinSize()
{
	return m_min_size;
}

int HostComm::GetHeight()
{
	return m_height;
}

int HostComm::GetWidth()
{
	return m_width;
}

panel_tooltip_param_ptr& HostComm::PanelTooltipParam()
{
	return m_panel_tooltip_param_ptr;
}

t_script_info& HostComm::ScriptInfo()
{
	return m_script_info;
}

t_size HostComm::GetInstanceType()
{
	return m_instance_type;
}

void HostComm::Redraw()
{
	m_paint_pending = false;
	RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void HostComm::RefreshBackground(LPRECT lprcUpdate)
{
	HWND wnd_parent = GetAncestor(m_hwnd, GA_PARENT);

	if (!wnd_parent || IsIconic(core_api::get_main_window()) || !IsWindowVisible(m_hwnd))
		return;

	HDC dc_parent = GetDC(wnd_parent);
	HDC hdc_bk = CreateCompatibleDC(dc_parent);
	POINT pt = { 0, 0 };
	RECT rect_child = { 0, 0, m_width, m_height };
	RECT rect_parent;
	HRGN rgn_child = NULL;

	// HACK: for Tab control
	// Find siblings
	HWND hwnd = NULL;
	while (hwnd = FindWindowEx(wnd_parent, hwnd, NULL, NULL))
	{
		TCHAR buff[64];
		if (hwnd == m_hwnd) continue;
		GetClassName(hwnd, buff, _countof(buff));
		if (_tcsstr(buff, _T("SysTabControl32")))
		{
			wnd_parent = hwnd;
			break;
		}
	}

	if (lprcUpdate)
	{
		HRGN rgn = CreateRectRgnIndirect(lprcUpdate);
		rgn_child = CreateRectRgnIndirect(&rect_child);
		CombineRgn(rgn_child, rgn_child, rgn, RGN_DIFF);
		DeleteRgn(rgn);
	}
	else
	{
		rgn_child = CreateRectRgn(0, 0, 0, 0);
	}

	ClientToScreen(m_hwnd, &pt);
	ScreenToClient(wnd_parent, &pt);

	CopyRect(&rect_parent, &rect_child);
	ClientToScreen(m_hwnd, (LPPOINT)&rect_parent);
	ClientToScreen(m_hwnd, (LPPOINT)&rect_parent + 1);
	ScreenToClient(wnd_parent, (LPPOINT)&rect_parent);
	ScreenToClient(wnd_parent, (LPPOINT)&rect_parent + 1);

	// Force Repaint
	m_suppress_drawing = true;
	SetWindowRgn(m_hwnd, rgn_child, FALSE);
	RedrawWindow(wnd_parent, &rect_parent, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);

	// Background bitmap
	HBITMAP old_bmp = SelectBitmap(hdc_bk, m_gr_bmp_bk);

	// Paint BK
	BitBlt(hdc_bk, rect_child.left, rect_child.top, rect_child.right - rect_child.left, rect_child.bottom - rect_child.top, dc_parent, pt.x, pt.y, SRCCOPY);

	SelectBitmap(hdc_bk, old_bmp);
	DeleteDC(hdc_bk);
	ReleaseDC(wnd_parent, dc_parent);
	DeleteRgn(rgn_child);
	SetWindowRgn(m_hwnd, NULL, FALSE);
	m_suppress_drawing = false;
	m_paint_pending = true;
	RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void HostComm::Repaint()
{
	m_paint_pending = true;
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void HostComm::RepaintRect(int x, int y, int w, int h)
{
	RECT rc;
	rc.left = x;
	rc.top = y;
	rc.right = x + w;
	rc.bottom = y + h;

	m_paint_pending = true;
	InvalidateRect(m_hwnd, &rc, FALSE);
}

ScriptHost::ScriptHost(HostComm* host)
	: m_host(host)
	, m_window(new com_object_impl_t<FbWindow, false>(host))
	, m_gdi(com_object_singleton_t<GdiUtils>::instance())
	, m_fb2k(com_object_singleton_t<FbUtils>::instance())
	, m_utils(com_object_singleton_t<JSUtils>::instance())
	, m_playlistman(com_object_singleton_t<FbPlaylistManager>::instance())
	, m_console(com_object_singleton_t<JSConsole>::instance())
	, m_dwStartTime(0)
	, m_dwRef(1)
	, m_engine_inited(false)
	, m_has_error(false)
	, m_lastSourceContext(0) {}

ScriptHost::~ScriptHost() {}

bool ScriptHost::HasError()
{
	return m_has_error;
}

bool ScriptHost::Ready()
{
	return m_engine_inited && m_script_engine;
}

HRESULT ScriptHost::InitScriptEngine()
{
	const DWORD classContext = CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER;

	static const CLSID jscript9clsid = { 0x16d51579, 0xa30b, 0x4c8b,{ 0xa2, 0x76, 0x0f, 0xf4, 0xdc, 0x41, 0xe7, 0x55 } };
	HRESULT hr = m_script_engine.CreateInstance(jscript9clsid, NULL, classContext);

	if (FAILED(hr))
	{
		return hr;
	}

	IActiveScriptProperty* pActScriProp = NULL;
	m_script_engine->QueryInterface(IID_IActiveScriptProperty, (void**)&pActScriProp);
	VARIANT scriptLangVersion;
	scriptLangVersion.vt = VT_I4;
	scriptLangVersion.lVal = SCRIPTLANGUAGEVERSION_5_8 + 1;
	pActScriProp->SetProperty(SCRIPTPROP_INVOKEVERSIONING, NULL, &scriptLangVersion);
	pActScriProp->Release();
	return hr;
}

HRESULT ScriptHost::Initialize()
{
	Finalize();

	m_has_error = false;

	pfc::stringcvt::string_wide_from_utf8_fast wcode(m_host->get_script_code());
	script_preprocessor preprocessor(wcode.get_ptr());
	preprocessor.process_script_info(m_host->ScriptInfo());

	IActiveScriptParsePtr parser;
	HRESULT hr = InitScriptEngine();

	if (SUCCEEDED(hr)) hr = m_script_engine->SetScriptSite(this);
	if (SUCCEEDED(hr)) hr = m_script_engine->QueryInterface(&parser);
	if (SUCCEEDED(hr)) hr = parser->InitNew();
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"window", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"gdi", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"fb", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"utils", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"plman", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"console", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->SetScriptState(SCRIPTSTATE_CONNECTED);
	if (SUCCEEDED(hr)) hr = m_script_engine->GetScriptDispatch(NULL, &m_script_root);
	if (SUCCEEDED(hr)) hr = ProcessImportedScripts(preprocessor, parser);
	if (SUCCEEDED(hr))
	{
		DWORD source_context;
		GenerateSourceContext("<main>", source_context);
		hr = parser->ParseScriptText(wcode.get_ptr(), NULL, NULL, NULL, source_context, 0, SCRIPTTEXT_HOSTMANAGESSOURCE | SCRIPTTEXT_ISVISIBLE, NULL, NULL);
	}

	if (SUCCEEDED(hr))
	{
		m_engine_inited = true;
	}
	else
	{
		m_engine_inited = false;
		m_has_error = true;
	}

	m_callback_invoker.Init(m_script_root);
	return hr;
}

HRESULT ScriptHost::InvokeCallback(int callbackId, VARIANTARG* argv, UINT argc, VARIANT* ret)
{
	if (HasError()) return E_FAIL;
	if (!Ready()) return E_FAIL;

	HRESULT hr = E_FAIL;

	try
	{
		hr = m_callback_invoker.Invoke(callbackId, argv, argc, ret);
	}
	catch (std::exception& e)
	{
		pfc::print_guid guid(m_host->get_config_guid());
		console::printf(JSP_NAME " (%s): Unhandled C++ Exception: \"%s\", will crash now...", m_host->ScriptInfo().build_info_string().get_ptr(), e.what());
	}
	catch (_com_error& e)
	{
		pfc::print_guid guid(m_host->get_config_guid());
		console::printf(JSP_NAME " (%s): Unhandled COM Error: \"%s\", will crash now...", m_host->ScriptInfo().build_info_string().get_ptr(), pfc::stringcvt::string_utf8_from_wide(e.ErrorMessage()).get_ptr());
	}
	catch (...)
	{
		pfc::print_guid guid(m_host->get_config_guid());
		console::printf(JSP_NAME " (%s): Unhandled Unknown Exception, will crash now...", m_host->ScriptInfo().build_info_string().get_ptr());
	}

	return hr;
}

HRESULT ScriptHost::ProcessImportedScripts(script_preprocessor& preprocessor, IActiveScriptParsePtr& parser)
{
	script_preprocessor::t_script_list scripts;
	preprocessor.process_import(m_host->ScriptInfo(), scripts);
	HRESULT hr = S_OK;

	for (t_size i = 0; i < scripts.get_count(); ++i)
	{
		pfc::string8 path = pfc::stringcvt::string_utf8_from_wide(scripts[i].path.get_ptr());
		DWORD source_context;
		GenerateSourceContext(path, source_context);
		hr = parser->ParseScriptText(scripts[i].code.get_ptr(), NULL, NULL, NULL, source_context, 0, SCRIPTTEXT_HOSTMANAGESSOURCE | SCRIPTTEXT_ISVISIBLE, NULL, NULL);
		if (FAILED(hr)) break;
	}

	return hr;
}

STDMETHODIMP ScriptHost::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

STDMETHODIMP ScriptHost::GetDocVersionString(BSTR* pstr)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::GetItemInfo(LPCOLESTR name, DWORD mask, IUnknown** ppunk, ITypeInfo** ppti)
{
	if (ppti) *ppti = NULL;

	if (ppunk) *ppunk = NULL;

	if (mask & SCRIPTINFO_IUNKNOWN)
	{
		if (!name) return E_INVALIDARG;
		if (!ppunk) return E_POINTER;

		if (wcscmp(name, L"window") == 0)
		{
			(*ppunk) = m_window;
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"gdi") == 0)
		{
			(*ppunk) = m_gdi;
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"fb") == 0)
		{
			(*ppunk) = m_fb2k;
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"utils") == 0)
		{
			(*ppunk) = m_utils;
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"plman") == 0)
		{
			(*ppunk) = m_playlistman;
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"console") == 0)
		{
			(*ppunk) = m_console;
			(*ppunk)->AddRef();
			return S_OK;
		}
	}

	return TYPE_E_ELEMENTNOTFOUND;
}

STDMETHODIMP ScriptHost::GetLCID(LCID* plcid)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::GetWindow(HWND* phwnd)
{
	*phwnd = m_host->GetHWND();
	return S_OK;
}

STDMETHODIMP ScriptHost::OnEnterScript()
{
	m_dwStartTime = pfc::getTickCount();
	return S_OK;
}

STDMETHODIMP ScriptHost::OnLeaveScript()
{
	return S_OK;
}

STDMETHODIMP ScriptHost::OnScriptError(IActiveScriptError* err)
{
	m_has_error = true;

	if (!err) return E_POINTER;

	ReportError(err);
	return S_OK;
}

STDMETHODIMP ScriptHost::OnScriptTerminate(const VARIANT* result, const EXCEPINFO* excep)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::OnStateChange(SCRIPTSTATE state)
{
	return E_NOTIMPL;
}

STDMETHODIMP_(ULONG) ScriptHost::AddRef()
{
	return InterlockedIncrement(&m_dwRef);
}

STDMETHODIMP_(ULONG) ScriptHost::Release()
{
	ULONG n = InterlockedDecrement(&m_dwRef);

	if (n == 0)
	{
		delete this;
	}

	return n;
}

void ScriptHost::Finalize()
{
	InvokeCallback(CallbackIds::on_script_unload);

	if (Ready())
	{
		// Call GC explicitly
		IActiveScriptGarbageCollector* gc = NULL;
		if (SUCCEEDED(m_script_engine->QueryInterface(IID_IActiveScriptGarbageCollector, (void**)&gc)))
		{
			gc->CollectGarbage(SCRIPTGCTYPE_EXHAUSTIVE);
			gc->Release();
		}

		m_script_engine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
		m_script_engine->SetScriptState(SCRIPTSTATE_CLOSED);
		m_script_engine->Close();
		//m_script_engine->InterruptScriptThread(SCRIPTTHREADID_ALL, NULL, 0);
		m_engine_inited = false;
	}

	m_contextToPathMap.remove_all();
	m_callback_invoker.Reset();

	if (m_script_engine)
	{
		m_script_engine.Release();
	}

	if (m_script_root)
	{
		m_script_root.Release();
	}
}

void ScriptHost::GenerateSourceContext(pfc::string8 path, DWORD& source_context)
{
	source_context = m_lastSourceContext++;
	m_contextToPathMap[source_context] = path;
}

void ScriptHost::ReportError(IActiveScriptError* err)
{
	if (!err) return;

	DWORD ctx = 0;
	ULONG line = 0;
	LONG charpos = 0;
	EXCEPINFO excep = { 0 };
	//WCHAR buf[512] = { 0 };
	_bstr_t sourceline;
	_bstr_t name;

	if (FAILED(err->GetSourcePosition(&ctx, &line, &charpos)))
	{
		line = 0;
		charpos = 0;
	}

	if (FAILED(err->GetSourceLineText(sourceline.GetAddress())))
	{
		sourceline = L"<source text only available at compile time>";
	}

	if (FAILED(err->GetExceptionInfo(&excep)))
	{
		return;
	}

	// Do a deferred fill-in if necessary
	if (excep.pfnDeferredFillIn)
	{
		(*excep.pfnDeferredFillIn)(&excep);
	}

	pfc::string_formatter formatter;
	formatter << "Error: " JSP_NAME_VERSION " (" << m_host->ScriptInfo().build_info_string().get_ptr() << ")\n";

	if (excep.bstrSource && excep.bstrDescription)
	{
		formatter << pfc::stringcvt::string_utf8_from_wide(excep.bstrSource) << ":\n";
		formatter << pfc::stringcvt::string_utf8_from_wide(excep.bstrDescription) << "\n";
	}
	else
	{
		pfc::string8_fast errorMessage;

		if (uFormatSystemErrorMessage(errorMessage, excep.scode))
			formatter << errorMessage;
		else
			formatter << "Unknown error code: 0x" << pfc::format_hex_lowercase((unsigned)excep.scode);
	}

	if (m_contextToPathMap.exists(ctx))
	{
		formatter << "File: " << m_contextToPathMap[ctx] << "\n";
	}

	formatter << "Line: " << (t_uint32)(line + 1) << ", Col: " << (t_uint32)(charpos + 1) << "\n";
	formatter << pfc::stringcvt::string_utf8_from_wide(sourceline);
	if (name.length() > 0) formatter << "\nAt: " << name;

	if (excep.bstrSource) SysFreeString(excep.bstrSource);
	if (excep.bstrDescription) SysFreeString(excep.bstrDescription);
	if (excep.bstrHelpFile) SysFreeString(excep.bstrHelpFile);

	FB2K_console_formatter() << formatter;
	popup_msg::g_show(formatter, JSP_NAME);
	MessageBeep(MB_ICONASTERISK);
	SendMessage(m_host->GetHWND(), UWM_SCRIPT_ERROR, 0, 0);
}

void ScriptHost::Stop()
{
	m_engine_inited = false;
	if (m_script_engine) m_script_engine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
}
