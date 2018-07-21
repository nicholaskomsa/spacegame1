
#include "cefv8handler.h"

#include "log.h"





MyV8Handler::MyV8Handler(CefRefPtr<CefBrowser> browser, GuiSheetManagerRenderer* manager)
	:mBrowser(browser)
	, mGuiSheetManagerRenderer(manager)
{}
MyV8Handler::~MyV8Handler() {
	//LogManager::get("renderer") << "v8 destruct" << Log::end;
}

bool MyV8Handler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) {

	//LogManager::get("renderer") << "send js to browser " << name.ToString() << " " << Log::end;
	return mGuiSheetManagerRenderer->sendJavascriptToBrowser(mBrowser, name, arguments);
}
