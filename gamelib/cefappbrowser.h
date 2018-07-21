
#include "include/cef_app.h"

#include "cefclientbrowser.h"
#include "guisheet.h"
#include "guisheetmanagerbrowser.h"

#pragma once



// Client app implementation for the browser process.
class CefAppBrowser : public CefApp, public CefBrowserProcessHandler {


	// CefClient implementation.
	CefRefPtr<CefClientBrowser> mClientBrowser;

	bool mInitialized{ false };



public:
	CefAppBrowser();
	virtual ~CefAppBrowser();
	// CefApp methods.
	void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line);

	CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler();


	void resize(int width, int height);
	void createBrowser(Graphics* graphics, std::string sheetName, GuiSheet* sheet );
	GuiSheetManagerBrowser* getGuiSheetManager();
	CefRefPtr<CefClientBrowser> getBrowser();

	void destroyBrowser();
	//CefBrowserProcessHandler methods
	void OnContextInitialized();
private:
	IMPLEMENT_REFCOUNTING(CefAppBrowser);
	DISALLOW_COPY_AND_ASSIGN(CefAppBrowser);
};