
#include "cefappbrowser.h"



#include "graphics.h"
#include "cefresourcehandler.h"

CefAppBrowser::CefAppBrowser() {}

CefAppBrowser::~CefAppBrowser() {
}
// CefApp methods.
void CefAppBrowser::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line){

	command_line->AppendSwitch("--disable-extensions");

};

CefRefPtr<CefBrowserProcessHandler> CefAppBrowser::GetBrowserProcessHandler(){
	return this;
}




void CefAppBrowser::resize(int width, int height) {
	mClientBrowser->resize(width, height);
}
void CefAppBrowser::createBrowser( Graphics* graphics, std::string sheetName, GuiSheet* sheet ) {

	if (!mInitialized) return;

	CefWindowInfo windowInfo;
	CefBrowserSettings browserSettings;

	std::size_t windowHandle = 0;
	graphics->getRenderWindow()->getCustomAttribute("WINDOW", &windowHandle);

	std::string scheme = "cefgui";
	std::string domain = "app";

	CefRegisterSchemeHandlerFactory(scheme, domain, new ClientSchemeHandlerFactory());


	windowInfo.SetAsWindowless((HWND)windowHandle);
	browserSettings.background_color = CefColorSetARGB(0, 0, 0, 0);	//make transparent

																	//get the cefgui media path
	std::string mediaPath;
	std::ifstream fin("cefguipath.cfg", std::ios::in);
	std::getline(fin, mediaPath);
	fin.close();

	mClientBrowser = new CefClientBrowser( graphics, mediaPath );

	mClientBrowser->getGuiSheetManager()->addSheet(sheetName, sheet);

	//this method requires a url string passed in so i just use the scheme/domain
	bool r = CefBrowserHost::CreateBrowser(windowInfo, mClientBrowser.get(), "cefgui://app/", browserSettings, NULL);
	/**/
	CefDoMessageLoopWork(); //create the browser:: onaftercreated called

	mClientBrowser->getGuiSheetManager()->setSheet(sheetName);

}
GuiSheetManagerBrowser* CefAppBrowser::getGuiSheetManager() {
	return mClientBrowser->getGuiSheetManager();

}
CefRefPtr<CefClientBrowser> CefAppBrowser::getBrowser() {
	return mClientBrowser;
}

void CefAppBrowser::destroyBrowser() {

	mClientBrowser->destroy();

	CefClearSchemeHandlerFactories();
}
//CefBrowserProcessHandler methods
void CefAppBrowser::OnContextInitialized() {
	mInitialized = true;
}