#include "include/cef_app.h"

#include "guisheetmanagerrenderer.h"

#pragma once



// Client app implementation for the renderer process.
class CefAppRenderer : public CefApp, public CefRenderProcessHandler {

	GuiSheetManagerRenderer mGuiSheetManagerRenderer;

	// CefApp methods.
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler();


	// CefRenderProcessHandler methods.
	
	void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info);
	void OnWebKitInitialized();
	void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
	void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser);

	//CefRefPtr<CefLoadHandler> GetLoadHandler();
	bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect);
	
	void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);

	void OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);

	void OnUncaughtException(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace);

	void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node);

	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);

public:

	CefAppRenderer();


private:
	IMPLEMENT_REFCOUNTING(CefAppRenderer);
	DISALLOW_COPY_AND_ASSIGN(CefAppRenderer);
};
