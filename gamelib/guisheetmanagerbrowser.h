#pragma once
#include "guisheet.h"
//
#include <include/cef_app.h>

#include <unordered_map>

#pragma once



class GuiSheetManagerBrowser {
protected:
	std::string mMediaPath;
public:
	GuiSheetManagerBrowser();

	//called by browser
	void sendGuiToRenderer(GuiSheet* pSheet, CefRefPtr<CefBrowser> browser1);

	//called in browser, received a message from renderer
	void processGuiMessage(CefRefPtr<CefProcessMessage> msg);


public:
	typedef std::map< std::string, GuiSheet* > GuiSheetMap;
	GuiSheetMap sheets;
	CefRefPtr<CefBrowser> browser;

	std::string mActiveSheet;


	void setMediaPath(std::string path);

	void setBrowser(CefRefPtr<CefBrowser> browser1);
	void destroy();
	void call(std::string sheetName, std::string method, GuiArgList guiArgList);

	void addSheet(std::string sheetName, GuiSheet* pSheet);
	GuiSheet* getSheet(std::string sheetName);
	GuiSheet* getActiveSheet();
	void removeSheet(std::string sheetName);
	void setSheet(std::string sheetName);
};
