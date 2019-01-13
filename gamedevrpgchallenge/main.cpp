#include <windows.h>

#include <exception.h>

#include "game.h"


INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT) {

	CefProcess	cefProcess;
	cefProcess.doBranching();

	try {

		Game game;
		game.setup(&cefProcess);
		game.go();
		game.shutdown();

	}
	catch (Ogre::Exception& e) {
		OutputDebugStringA(e.getFullDescription().c_str());
		MessageBoxEx(NULL, e.getFullDescription().c_str(), "Exception", MB_OK, 0);

	}
	catch (Exception& e) {
		e.writeDebugString();
		MessageBoxEx(NULL, e.message.c_str(), "Exception", MB_OK, 0);
	}
	catch (...) {
		OutputDebugStringA("unhandled Exception!");
		MessageBoxEx(NULL, "Unhandled Exception", "Exception", MB_OK, 0);
	}

	CefDoMessageLoopWork();
	CefShutdown();

	return 0;
}