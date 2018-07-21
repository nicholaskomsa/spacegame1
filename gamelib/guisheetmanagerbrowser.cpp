#include "guisheetmanagerbrowser.h"
#include "guisheetmanagermessages.h"


GuiSheetManagerBrowser::GuiSheetManagerBrowser()
{}

//called by browser
void GuiSheetManagerBrowser::sendGuiToRenderer(GuiSheet* pSheet, CefRefPtr<CefBrowser> browser1) {

	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(GuiSheetManagerMessages::msgSetBindings);
	CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();

	int index = 0;
	for (GuiSheet::BrowserBindingsMap::iterator it = pSheet->bindings.begin(); it != pSheet->bindings.end(); it++) {
		msgArgs->SetString(index++, it->first);
	}
	//the last string is the url to set
	msgArgs->SetString(index, pSheet->url);
	browser1->SendProcessMessage(PID_RENDERER, msg);

}

//called in browser, received a message from renderer
void GuiSheetManagerBrowser::processGuiMessage(CefRefPtr<CefProcessMessage> msg) {

	std::string messageID = msg->GetName();
	CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();

	GuiArgList guiArgList;

	for (std::size_t i = 0; i < msgArgs->GetSize(); i++) {
		guiArgList.push(msgArgs->GetString(i));
	}

	call(mActiveSheet, messageID, guiArgList);
}


void GuiSheetManagerBrowser::setMediaPath(std::string path) {
	mMediaPath = path;
}
void GuiSheetManagerBrowser::setBrowser(CefRefPtr<CefBrowser> browser1) {
	browser = browser1;
}

void GuiSheetManagerBrowser::destroy() {
	browser = NULL;
}

void GuiSheetManagerBrowser::call(std::string sheetName, std::string method, GuiArgList guiArgList) {


	GuiSheet* sheet = NULL;
	if (sheets.find(sheetName) != sheets.end()) sheet = sheets[sheetName];
	else
		OutputDebugStringA("SHEET NOT FOUND IN CALL");
;

	auto function = sheet->bindings.find(method);

	if (function != sheet->bindings.end()) {
		//	sheet->bindings[method](guiArgList);
		function->second(guiArgList);
	}
	else
		OutputDebugStringA("BINDING NOT FOUND IN CALL");
}

void GuiSheetManagerBrowser::addSheet(std::string sheetName, GuiSheet* pSheet) {
	
	pSheet->url = "cefgui://app/" + mMediaPath + "/" + pSheet->url;
	sheets[sheetName] = pSheet;
}
GuiSheet* GuiSheetManagerBrowser::getActiveSheet() {
	GuiSheetMap::iterator it = sheets.find(mActiveSheet);
	if (it == sheets.end()) return nullptr;
	return sheets[mActiveSheet];

}
GuiSheet* GuiSheetManagerBrowser::getSheet(std::string sheetName) {
	GuiSheetMap::iterator it = sheets.find(sheetName);
	if (it == sheets.end()) return nullptr;
	return sheets[sheetName];
}
void GuiSheetManagerBrowser::removeSheet(std::string sheetName) {
	GuiSheetMap::iterator it = sheets.find(sheetName);
	if (it != sheets.end())
		sheets.erase(it);
}

void GuiSheetManagerBrowser::setSheet(std::string sheetName) {
	if (sheets.find(sheetName) != sheets.end()) {
		mActiveSheet = sheetName;
		sendGuiToRenderer(sheets[mActiveSheet], browser);
	}
}