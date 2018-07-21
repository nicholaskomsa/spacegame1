#include <map>
#include <algorithm>
#include <functional>


#include "guiarglist.h"
#include "sdlinput.h"


#pragma once



class GuiSheet {
public:
	std::string url;
public:
	GuiSheet();
	virtual ~GuiSheet();

	//used by browser
	//typedef void (GuiSheet::*Callback)(GuiArgList);
	typedef std::function<void(GuiArgList)> Callback;
	typedef std::map< std::string, Callback > BrowserBindingsMap;
	BrowserBindingsMap bindings;

	GuiSheet(std::string url1);

	template<typename DERIVED >
	void bind(std::string name, DERIVED* derived, void(DERIVED::*derivedCallback)(GuiArgList)) {

		bindings[name] = std::bind(derivedCallback, derived, std::placeholders::_1);
	}


	virtual void keyPressed(const SDL_KeyboardEvent* evt ) {}
	virtual void keyReleased(const SDL_KeyboardEvent* evt ){}
	virtual void mousePressed(const SDL_MouseButtonEvent* evt ) {}
	virtual void mouseReleased(const SDL_MouseButtonEvent* evt) {}

	virtual void mouseMoved(const SDL_MouseMotionEvent* evt) {}

};
