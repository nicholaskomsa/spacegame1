
#include <Windows.h>

#include <include/cef_client.h>
#include <include/wrapper/cef_helpers.h>

#include "cefrenderhandlerogre.h"

#include "guisheetmanagerbrowser.h"

#include "sdlinput.h"

#include "sdlkeycodeswindows.h"

#pragma once


class CefClientBrowser  : public CefClient, public CefLifeSpanHandler {
protected:

	GuiSheetManagerBrowser mGuiSheetManagerBrowser;
	int mCurrentKeyModifiers{ 0 };

	CefRefPtr<CefRenderHandlerOgre> mRenderHandlerOgre;
	CefRefPtr<CefBrowser> mBrowser;
	int mBrowserID;
	int mBrowserCount;
	bool mIsClosing;



public:
	CefClientBrowser(Graphics* graphics, std::string mediaPath ) {

		mRenderHandlerOgre = new CefRenderHandlerOgre(graphics->getSceneManager());
		mGuiSheetManagerBrowser.setMediaPath(mediaPath);
	}


	virtual ~CefClientBrowser() {}

	// CefClient methods.
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }

	CefRefPtr<CefRenderHandler> GetRenderHandler() {
		return mRenderHandlerOgre;
	}

	GuiSheetManagerBrowser* getGuiSheetManager() {
		return &mGuiSheetManagerBrowser;
	}



	// CefLifeSpanHandler methods.
	void OnAfterCreated(CefRefPtr<CefBrowser> browser) {
		// Must be executed on the UI thread.
		CEF_REQUIRE_UI_THREAD();

		if (!mBrowser.get()) {
			// Keep a reference to the main browser.
			mBrowser = browser;
			mBrowserID = browser->GetIdentifier();
			

			mGuiSheetManagerBrowser.setBrowser(mBrowser);
		}

		// Keep track of how many browsers currently exist.
		mBrowserCount++;
	}
	bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE { 
		
		return true;
	}
	void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE { 

		mGuiSheetManagerBrowser.destroy();
		mBrowser = NULL;
		mRenderHandlerOgre = NULL;
	};

	// Member accessors.
	void writeJavascript(std::string functionName, std::string functionString, std::string url) {
		std::stringstream sstr;
		sstr << functionName << "(" << functionString << ");";
		mBrowser->GetMainFrame()->ExecuteJavaScript(sstr.str(), url, 0);
	}

	//CefRefPtr<CefBrowser> getBrowser() { return mBrowser; }
	void destroy() {
		mBrowser->GetHost()->CloseBrowser(true);
		mRenderHandlerOgre->clear();
	}

	bool isClosing() { return mIsClosing; }
	void setBrowser(CefRefPtr< CefBrowser> browser) {
		mBrowser = browser;
	}
	void resize(int width, int height) {

		mRenderHandlerOgre->resize(width, height);
		if( mBrowser && mBrowser->GetHost() ) 
			mBrowser->GetHost()->WasResized();
		
		//CefDoMessageLoopWork();
	}
	bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) {

		mGuiSheetManagerBrowser.processGuiMessage(message);

		return true;
	}



	void keyPressed(const SDL_KeyboardEvent* evt ) {
		if (mBrowser == NULL) return;

		GuiSheet* sheet = mGuiSheetManagerBrowser.getActiveSheet();
		if (sheet) sheet->keyPressed(evt);
		
	
		CefKeyEvent event;
		mCurrentKeyModifiers = getKeyboardModifiers(evt->keysym.mod);
		event.modifiers = mCurrentKeyModifiers;

		int keyCode  = getWindowsKeyCode(evt->keysym);


		BYTE VkCode = LOBYTE(VkKeyScanA(keyCode));
		UINT scanCode = MapVirtualKey(VkCode, MAPVK_VK_TO_VSC);
		event.native_key_code = (scanCode << 16) |  // key scan code
			1;                  // key repeat count
		event.windows_key_code = VkCode;

		event.type = KEYEVENT_RAWKEYDOWN;
		mBrowser->GetHost()->SendKeyEvent(event);

		event.windows_key_code = keyCode;
		event.type = KEYEVENT_CHAR;
		mBrowser->GetHost()->SendKeyEvent(event);

		event.windows_key_code = VkCode;
		// bits 30 and 31 should be always 1 for WM_KEYUP
		//event.native_key_code |= 0xC0000000;

	//	event.type = KEYEVENT_KEYDOWN;
	//	mBrowser->GetHost()->SendKeyEvent(event);
	}

	void keyReleased(const SDL_KeyboardEvent* evt) {

		if (mBrowser == NULL) return;

		GuiSheet* sheet = mGuiSheetManagerBrowser.getActiveSheet();
		if (sheet) sheet->keyReleased( evt );
		
		CefKeyEvent event;
		mCurrentKeyModifiers = getKeyboardModifiers(evt->keysym.mod);
		event.modifiers = mCurrentKeyModifiers;

		int keyCode = getWindowsKeyCode(evt->keysym);

		BYTE VkCode = LOBYTE(VkKeyScanA(keyCode));

		event.windows_key_code = VkCode;
		 //bits 30 and 31 should be always 1 for WM_KEYUP
		event.native_key_code |= 0xC0000000;

		event.type = KEYEVENT_KEYUP;
		////mBrowser->GetHost()->SendKeyEvent(event);
	}

	void mousePressed(const SDL_MouseButtonEvent* evt) {
		if (mBrowser == NULL) return;

		GuiSheet* sheet = mGuiSheetManagerBrowser.getActiveSheet();
		if (sheet) sheet->mousePressed(evt);
		

		CefMouseEvent mouse_event;
		mouse_event.x = evt->x;
		mouse_event.y = evt->y;
		mouse_event.modifiers = mCurrentKeyModifiers;

		Uint8 id = evt->button;

		CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
		switch (id) {
		case SDL_BUTTON_RIGHT:
			btnType = MBT_RIGHT;
			break;
		case SDL_BUTTON_MIDDLE:
			btnType = MBT_MIDDLE;
			break;
		}

		mBrowser->GetHost()->SendMouseClickEvent(mouse_event, btnType, false, 1);
	}
	void mouseReleased(const SDL_MouseButtonEvent* evt) {
		if (mBrowser == NULL) return;

		GuiSheet* sheet = mGuiSheetManagerBrowser.getActiveSheet();
		if (sheet) sheet->mouseReleased(evt);

		CefMouseEvent mouse_event;
		mouse_event.x = evt->x;
		mouse_event.y = evt->y;
		mouse_event.modifiers = mCurrentKeyModifiers;

		Uint8 id = evt->button;

		CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
		switch (id) {
		case SDL_BUTTON_RIGHT:
			btnType = MBT_RIGHT;
			break;
		case SDL_BUTTON_MIDDLE:
			btnType = MBT_MIDDLE;
			break;
		}
		
		mBrowser->GetHost()->SendMouseClickEvent(mouse_event, btnType, true, 1);
	}

	void mouseMoved(const SDL_MouseMotionEvent* evt) {
		if ( mBrowser == NULL) return;

		GuiSheet* sheet = mGuiSheetManagerBrowser.getActiveSheet();
		if (sheet) sheet->mouseMoved(evt);
		
		CefMouseEvent mouse_event;
		mouse_event.x = evt->x;
		mouse_event.y = evt->y;
		mouse_event.modifiers = mCurrentKeyModifiers;

		mBrowser->GetHost()->SendMouseMoveEvent(mouse_event, false);
	}

	void mouseScrolled(const SDL_MouseWheelEvent* evt) {

	
		CefMouseEvent mouse_event;
		mouse_event.x = evt->x;
		mouse_event.y = evt->y;
		mouse_event.modifiers = mCurrentKeyModifiers;
	
		mBrowser->GetHost()->SendMouseWheelEvent(mouse_event, evt->x, evt->y );
	}


	IMPLEMENT_REFCOUNTING(CefClientBrowser);
};
