
#include <include/cef_app.h>


#include "guisheetmanagerrenderer.h"

#pragma once




class MyV8Handler : public CefV8Handler {
public:
	GuiSheetManagerRenderer* mGuiSheetManagerRenderer;
	CefRefPtr<CefBrowser> mBrowser;

	MyV8Handler(CefRefPtr<CefBrowser> browser, GuiSheetManagerRenderer* manager);
	virtual ~MyV8Handler();
	//possibly need to set mBrowser to null on destroy

	bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);
	
	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(MyV8Handler);
};


