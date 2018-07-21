#include <map>
#include <string>
#include <algorithm>
#include <chrono>

#include <SDL.h>

#include <SDL_syswm.h>

#include "log.h"

#pragma once

class SDLEventPoller {

public:
	class SDLEventHandler {
	protected:
		std::string mName;

	public:
		SDLEventHandler(std::string name)
			:mName(name)
		{}

		virtual ~SDLEventHandler() {}

		virtual void handleEvent(SDL_Event* event) {}

		std::string getName() { return mName; }
	};


	std::map< std::string, SDLEventHandler* > mSDLEventHandlers;

public:

	void setup() {
		if (!SDL_WasInit(SDL_INIT_EVENTS)) {
			SDL_InitSubSystem(SDL_INIT_EVENTS);
		}
		LogManager::add("EventManager", "EventManager.log", std::ios::out );

	}
	void shutdown() {
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
	}

	void poll() {

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			std::for_each(mSDLEventHandlers.begin(), mSDLEventHandlers.end(), [&](auto eventHandler) {
				eventHandler.second->handleEvent(&event);
			});
		}
	}

	void addEventHandler(std::string name, SDLEventHandler* eventHandler) {

		mSDLEventHandlers[name] = eventHandler;

	}
};


class SDLWindowEventHandler : public SDLEventPoller::SDLEventHandler {

public:
	
	SDLWindowEventHandler(std::string name = "windowEventHandler" )
		: SDLEventPoller::SDLEventHandler(name)
	{}


	void handleEvent(SDL_Event* event) {

		switch (event->type) {
		case SDL_QUIT:
			quit(event);
			return;
		case SDL_WINDOWEVENT:
			switch (event->window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				windowResized(event);
				return;
			case SDL_WINDOWEVENT_CLOSE:
				windowClosed(event);
				return;
			default:
				return;
			}
		}
	}
	virtual void quit(SDL_Event* event) {}//window X button
	virtual void windowResized(SDL_Event* event) {}
	virtual void windowClosed(SDL_Event* event) {}
};

class SDLMouseEventHandler : public SDLEventPoller::SDLEventHandler {
public:

	class Button {
		friend class SDLMouseEventHandler;
		std::chrono::milliseconds mTimeSinceLastClick{ 0 };
		std::chrono::milliseconds mTimeBetweenClicks{ 0 };
		bool mDown{ false };
		Uint8 mButtonID;
		SDL_MouseButtonEvent event;

	public:
		Button() {}
		Button(Uint8 buttonID, std::chrono::milliseconds timeBetweenClicks)
			: mButtonID(buttonID)
			, mTimeBetweenClicks(timeBetweenClicks)
		{}

		Uint8 getButtonID() const { return mButtonID; }
		const SDL_MouseButtonEvent& getEvent() const { return event; }

	};

	std::map< Uint8, Button > mButtons;

public:
	SDLMouseEventHandler(std::string name = "mouseEventHandler")
		: SDLEventPoller::SDLEventHandler(name)
	{}

	virtual ~SDLMouseEventHandler() {}

	void addButton(Uint8 buttonID, std::chrono::milliseconds timeBetweenClicks = std::chrono::milliseconds(0)) {
		//note this will replace a previously defined buttonID
		mButtons[buttonID] = Button(buttonID, timeBetweenClicks);
	}

	void handleEvent(SDL_Event* event) {
		std::chrono::milliseconds curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

		switch (event->type) {
		case SDL_MOUSEWHEEL:
			mouseWheel(event->wheel);
			return;
		case SDL_MOUSEMOTION:
			mouseMotion(event->motion);
			return;
		}

		bool isButtonDown = false;

		switch (event->type) {
		case SDL_MOUSEBUTTONDOWN:
			isButtonDown = true;
			break;
		case SDL_MOUSEBUTTONUP:
			isButtonDown = false;
			break;
		default:
			return;
		}

		Uint8 mouseButtonID = event->button.button;

		auto buttonPair = mButtons.find(mouseButtonID);
		if (buttonPair == mButtons.end()) {
			LogManager::get("EventManager") << "Handle Event Button Not Found: " << mName << ", " << (int)mouseButtonID << Log::end;
			return; //EXCEPT <<  "Handle Event Button Not Found";
		}
		auto& button = buttonPair->second;
		button.event = event->button;

		std::chrono::milliseconds elapsedTime = curTime - button.mTimeSinceLastClick;

		if (elapsedTime >= button.mTimeBetweenClicks && isButtonDown) {
			button.mDown = true;
			button.mTimeSinceLastClick = curTime;
			mouseDown(button);
		}
		else if (!isButtonDown && button.mDown) {
			button.mDown = false;
			mouseUp(button);
		}


	}


	virtual void mouseMotion(const SDL_MouseMotionEvent& mouseMotion) {}
	virtual void mouseWheel(const SDL_MouseWheelEvent& mouseWheel) {}
	virtual void mouseDown(const Button& button) {}
	virtual void mouseUp(const Button& button) {}
};



class SDLKeyEventHandler : public SDLEventPoller::SDLEventHandler {
public:


	class Key {
		friend class SDLKeyEventHandler;
		std::chrono::milliseconds mTimeSinceLastClick{ 0 };
		std::chrono::milliseconds mTimeBetweenClicks{ 0 };
		bool mDown{ false };
		SDL_Keycode mKeyCode;
		std::string mText;
		SDL_KeyboardEvent event;

	public:
		Key() {}
		Key(SDL_Keycode keyCode, std::chrono::milliseconds timeBetweenClicks)
			:mKeyCode(keyCode)
			, mTimeBetweenClicks(timeBetweenClicks)
		{}
		SDL_Keycode getKeyCode() const {
			return mKeyCode;
		}
		std::string getString() const {
			return mText;
		}
		const SDL_KeyboardEvent& getEvent() const { return event; }
	};

	std::map< int, Key > mKeys;

public:
	SDLKeyEventHandler(std::string name = "keyEventHandler")
		: SDLEventPoller::SDLEventHandler(name)
	{}

	virtual ~SDLKeyEventHandler() {}

	void handleEvent(SDL_Event* event) {

		std::chrono::milliseconds curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());


		bool isKeyDown = false;
		switch (event->type) {
		case SDL_KEYDOWN:
			isKeyDown = true;
			break;
		case SDL_KEYUP:
			isKeyDown = false;
			break;
		default:
			return;
		}

		int SDLKeyCode = event->key.keysym.sym;
		
		auto keyPair = mKeys.find(SDLKeyCode);
		if (keyPair == mKeys.end()){
			LogManager::get("EventManager") << "Handle Event Key Not Found: " << mName << ", " << SDLKeyCode << Log::end;
			//EXCEPT << "Handle Event KeyNotFound";
			return;
		}
		auto& key = keyPair->second;
		key.event = event->key;

		std::chrono::milliseconds elapsedTime = curTime - key.mTimeSinceLastClick;

		key.mText = event->text.text;

		if (elapsedTime > key.mTimeBetweenClicks && isKeyDown) {

			keyDown(key);
			key.mTimeSinceLastClick = curTime;
			key.mDown = true;
		}
		else if (!isKeyDown && key.mDown) {
			keyUp(key);
			key.mDown = false;
		}

	}

	void addKey(SDL_Keycode SDLKeyCode, std::chrono::milliseconds timeBetweenClicks  = std::chrono::milliseconds(0) ) {
		//note this will replace a previously defined keyID
		mKeys[SDLKeyCode] = Key(SDLKeyCode, timeBetweenClicks);
	}
	void addNumberKeys(std::chrono::milliseconds timeBetweenClicks = std::chrono::milliseconds(0)) {
		addKey(SDLK_0, timeBetweenClicks);
		addKey(SDLK_1, timeBetweenClicks);
		addKey(SDLK_2, timeBetweenClicks);
		addKey(SDLK_3, timeBetweenClicks);
		addKey(SDLK_4, timeBetweenClicks);
		addKey(SDLK_5, timeBetweenClicks);
		addKey(SDLK_6, timeBetweenClicks);
		addKey(SDLK_7, timeBetweenClicks);
		addKey(SDLK_8, timeBetweenClicks);
		addKey(SDLK_9, timeBetweenClicks);
		addKey(SDLK_PERIOD, timeBetweenClicks);
	}

	virtual void keyUp(const Key& k) {}
	virtual void keyDown(const Key& k) {}

	const Key& getKey(int SDLKeyCode) {
		auto key = mKeys.find(SDLKeyCode);
		if (key == mKeys.end())
			EXCEPT << "key does not exist";

		return key->second;
	}
	bool isKeyDown(int SDLKeyCode) {
		if (getKey(SDLKeyCode).mDown) return true;
		return false;
	}
};
