#include "cefapprenderer.h"



#include "cefv8handler.h"


// CefApp methods.
CefRefPtr<CefRenderProcessHandler> CefAppRenderer::GetRenderProcessHandler(){
	return this;
}


// CefRenderProcessHandler methods.

void CefAppRenderer::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {};
void CefAppRenderer::OnWebKitInitialized() {};
void CefAppRenderer::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {

	LogManager::get("renderer") << "On browser created: " << browser->GetIdentifier() << Log::end;

};
void CefAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {



	LogManager::get("renderer") << "On browser destroyed: " << browser->GetIdentifier() << Log::end;


};
//CefRefPtr<CefLoadHandler> GetLoadHandler();
bool CefAppRenderer::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	NavigationType navigation_type,
	bool is_redirect) {

	LogManager::get("renderer") << "On before nav: " << browser->GetIdentifier() << Log::end;

	return false; // false ; this routine did not handle the event itself
}

void CefAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {



	LogManager::get("renderer") << "On Context Created browser: " << browser->GetIdentifier() << ", " << context->GetBrowser()->GetIdentifier() << Log::end;

	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// Create an instance of my CefV8Handler object.
	CefRefPtr<CefV8Handler> handler = new MyV8Handler(browser, &mGuiSheetManagerRenderer);


	mGuiSheetManagerRenderer.generateV8Bindings(object, handler);

}

void CefAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {

	LogManager::get("renderer") << "OnContextReleased id: " << browser->GetIdentifier() << Log::end;


};

void CefAppRenderer::OnUncaughtException(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context,
	CefRefPtr<CefV8Exception> exception,
	CefRefPtr<CefV8StackTrace> stackTrace) {};

void CefAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefDOMNode> node) {};

bool CefAppRenderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {

	LogManager::get("renderer") << "setRendererGui browser: " << browser->GetIdentifier() << Log::end;
	mGuiSheetManagerRenderer.setRendererGui(message, browser);

	return true;
};

CefAppRenderer::CefAppRenderer() {
	LogManager::add("renderer", "renderer.log");
}
