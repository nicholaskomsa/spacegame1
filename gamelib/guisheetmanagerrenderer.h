#include "guisheet.h"
//
#include <include/cef_app.h>

#include <unordered_map>

#pragma once


class GuiSheetManagerRenderer {



public:
	GuiSheetManagerRenderer();

	class RBinding {};
	typedef std::unordered_map< std::string, RBinding > RendererBindingsMap;
	RendererBindingsMap rbindings;

	
	void setRendererGui(CefRefPtr<CefProcessMessage> msg, CefRefPtr<CefBrowser> browser1);
	
	void generateV8Bindings(CefRefPtr<CefV8Value> object, CefRefPtr<CefV8Handler> handler);
	
	bool sendJavascriptToBrowser(CefRefPtr<CefBrowser> browser1, std::string javascriptID, const CefV8ValueList& args);

};
