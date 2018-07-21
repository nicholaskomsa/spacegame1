#include <stack>


#include "graphics.h"
#include "sdlinput.h"
#include "cefprocess.h"
#include "physics.h"

#include "camera.h"



#pragma once


class GameMode;


class Game : public Ogre::FrameListener, public SDLWindowEventHandler, public SDLKeyEventHandler, public SDLMouseEventHandler {

	Graphics mGraphics;
	CefProcess* mCefProcess;
	SDLEventPoller mSDLEventPoller;

	Camera mCamera;

	bool mPlaying{ false };

	GuiSheet mDefaultGuiSheet;

	std::stack< std::shared_ptr< GameMode > > mGameModes;


	void createScene();
	void setupKeys();
	void setupGui();
public:

	void setup(CefProcess* cefProcess);
	void shutdown();

	void go();
	void go1();


	Camera* getCamera();
	Graphics* getGraphics();

	void exit();
	bool playing();
	void pollInput();
	void doCefWork();
	void render();
	void setGuiSheet(std::string sheetName, GuiSheet* sheet);
	CefRefPtr<CefAppBrowser> getGui();

	//////////////////////////SDL EVENTS

	void quit(SDL_Event* event);
	void windowResized(SDL_Event* event);
	void resizeGui(int width, int height);

	virtual void keyDown(const Key& k);
	virtual void keyUp(const Key& k);
	virtual void mouseMotion(const SDL_MouseMotionEvent& mouseMotion);
	virtual void mouseDown(const Button& button);
	virtual void mouseUp(const Button& button);
	virtual void mouseWheel(const SDL_MouseWheelEvent& mouseWheel);
};


