

#include "include/cef_base.h"
#include "include/cef_command_line.h"

#include "cefappbrowser.h"



#pragma once



class CefProcess {
protected:

	enum ProcessType {
		BrowserProcess,
		RendererProcess,
		ZygoteProcess,
		OtherProcess,
	};

	CefRefPtr< CefApp > mApp;
	ProcessType mProcessType;

	// Determine the process type based on command-line arguments.
	static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

public:
	~CefProcess();

	CefRefPtr<CefAppBrowser> getCefAppBrowser();


	void determineProcessType();

	void doBranching();

};
