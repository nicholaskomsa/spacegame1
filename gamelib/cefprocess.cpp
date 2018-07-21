#include "cefprocess.h"

#include "include/cef_sandbox_win.h"


#include "cefapprenderer.h"
#include "cefappother.h"



// Determine the process type based on command-line arguments.
CefProcess::ProcessType CefProcess::GetProcessType(CefRefPtr<CefCommandLine> command_line) {
	const std::string processTypeID{ "type" };
	const std::string rendererProcessID{ "renderer" };

	// The command-line flag won't be specified for the browser process.
	if (!command_line->HasSwitch(processTypeID.c_str()))
		return BrowserProcess;

	const std::string& process_type = command_line->GetSwitchValue(processTypeID.c_str());
	if (process_type == rendererProcessID)
		return RendererProcess;

	return OtherProcess;
}

CefRefPtr<CefAppBrowser> CefProcess::getCefAppBrowser() {
	return dynamic_cast<CefAppBrowser*>(mApp.get());
}
CefProcess::~CefProcess() {
}


void CefProcess::determineProcessType() {

	CefRefPtr<CefCommandLine> command_line_;
	// Keep a representation of the original command-line.
	command_line_ = CefCommandLine::CreateCommandLine();
	command_line_->InitFromString(::GetCommandLineW());

	mProcessType = GetProcessType(command_line_);

	switch (mProcessType) {

	case ProcessType::BrowserProcess:
		mApp = new CefAppBrowser();
		break;

	case ProcessType::RendererProcess:
	case ProcessType::ZygoteProcess:
		mApp = new CefAppRenderer();
		break;

	case ProcessType::OtherProcess:
		mApp = new CefAppOther();
		break;

	default:
		LogManager::get("cefgui") << "Default Switch Reached in determine process type" << Log::end;

	}
}

void CefProcess::doBranching() {

	LogManager::add("cefgui", "cefgui.log", std::ios::out);

	determineProcessType();

	void* sandbox_info = NULL;

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(::GetModuleHandle(NULL));


	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, mApp, sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		exit(exit_code);
	}
	//if exit_code is -1 then we are in the originating process so continue

	CefSettings settings;

	settings.windowless_rendering_enabled = true;
	settings.no_sandbox = true;

	bool result = CefInitialize(main_args, settings, mApp, sandbox_info);
}

