#include "gamemode.h"

#include "game.h"

#include "gamemode_test.h"




void Game::createScene() {
	mGraphics.getSceneManager()->setAmbientLight(Ogre::ColourValue(1, 1, 1, 1));

	mCamera.setPosition(Ogre::Vector3(0, 0, 150));
	mCamera.lookAt(Ogre::Vector3(0, 0, 0));
}
void Game::setupKeys() {
	addKey(SDLK_ESCAPE, std::chrono::milliseconds(1000));
	addKey(SDLK_F1);
	addKey(SDLK_F2);
	addKey(SDLK_F3);


	addKey(SDLK_q, std::chrono::milliseconds(200));
	addKey(SDLK_w, std::chrono::milliseconds(200));
	addKey(SDLK_e, std::chrono::milliseconds(200));
	addKey(SDLK_a, std::chrono::milliseconds(200));
	addKey(SDLK_s, std::chrono::milliseconds(200));
	addKey(SDLK_d, std::chrono::milliseconds(200));
	addKey(SDLK_z, std::chrono::milliseconds(200));
	addKey(SDLK_x, std::chrono::milliseconds(200));
	addKey(SDLK_c, std::chrono::milliseconds(200));
	
	addKey(SDLK_SPACE);

	addButton(SDL_BUTTON_LEFT);
	addButton(SDL_BUTTON_RIGHT);
}
void Game::setupGui() {
	mDefaultGuiSheet = GuiSheet("statusbar.html");
	mCefProcess->getCefAppBrowser()->createBrowser(&mGraphics, "main", &mDefaultGuiSheet);

	resizeGui(mGraphics.getRenderWindow()->getWidth(), mGraphics.getRenderWindow()->getHeight());
}

void Game::setup(CefProcess* cefProcess) {
	mCefProcess = cefProcess;

	mGraphics.setup("Space Box");
	mGraphics.setFrameListener(this);

	mCamera.setup(&mGraphics);

	mSDLEventPoller.setup();
	mSDLEventPoller.addEventHandler("keyHandler", static_cast<SDLKeyEventHandler*>(this));
	mSDLEventPoller.addEventHandler("mouseHandler", static_cast<SDLMouseEventHandler*>(this));
	mSDLEventPoller.addEventHandler("windowHandler", static_cast<SDLWindowEventHandler*>(this));

	setupGui();

	setupKeys();

	createScene();
}

void Game::shutdown() {

	mCefProcess->getCefAppBrowser()->destroyBrowser();

	mSDLEventPoller.shutdown();
	mCamera.shutdown();
	mGraphics.shutdown();

}

void Game::go() {
	mPlaying = true;
	//initial game mode is splash
	mGameModes.push(std::make_shared< GameMode_Test >(this));
	do {
		mGameModes.top()->setup();
		mGameModes.top()->run();
		std::shared_ptr<GameMode> nextMode = mGameModes.top()->shutdown();
		mGameModes.pop();

		if (nextMode) mGameModes.push(nextMode);
		else mPlaying = false;
	} while (mPlaying);
}

Camera* Game::getCamera() {

	return &mCamera;
}

Graphics* Game::getGraphics() {
	return &mGraphics;
}
void Game::exit() {
	mPlaying = false;
}
bool Game::playing() {
	return mPlaying;
}
void Game::pollInput() {
	mSDLEventPoller.poll();
}
void Game::doCefWork() {
	CefDoMessageLoopWork();
}
void Game::render() {
	mGraphics.getRoot()->renderOneFrame();
}
void Game::setGuiSheet(std::string sheetName, GuiSheet* sheet) {
	mCefProcess->getCefAppBrowser()->getGuiSheetManager()->addSheet(sheetName, sheet);
	mCefProcess->getCefAppBrowser()->getGuiSheetManager()->setSheet(sheetName);
}
CefRefPtr<CefAppBrowser> Game::getGui() { return mCefProcess->getCefAppBrowser(); }

//////////////////////////SDL EVENTS

void Game::quit(SDL_Event* event) {
	mPlaying = false;
}

void Game::windowResized(SDL_Event* event) {

	int width = event->window.data1;
	int height = event->window.data2;

	mGraphics.getRenderWindow()->resize(width, height);
	mGraphics.getRenderWindow()->windowMovedOrResized();

	resizeGui(width, height);
}

void Game::resizeGui(int width, int height) {

	if (mCefProcess) {
		mCefProcess->getCefAppBrowser()->resize(width, height);
		//mCefProcess->getCefAppBrowser()->resize(300, 300);
		//mCefProcess->getCefAppBrowser()->resize(200, 300);	
		//mCefProcess->getCefAppBrowser()->resize(100, 300);
		//mCefProcess->getCefAppBrowser()->resize(1920, 1080);
	}
}




void Game::keyDown(const Key& k) {
	mCefProcess->getCefAppBrowser()->getBrowser()->keyPressed(&k.getEvent());
}
void Game::keyUp(const Key& k) {
	mCefProcess->getCefAppBrowser()->getBrowser()->keyReleased(&k.getEvent());
}
void Game::mouseMotion(const SDL_MouseMotionEvent& mouseMotion) {
	mCefProcess->getCefAppBrowser()->getBrowser()->mouseMoved(&mouseMotion);
}
void Game::mouseDown(const Button& button) {
	mCefProcess->getCefAppBrowser()->getBrowser()->mousePressed(&button.getEvent());
}
void Game::mouseUp(const Button& button) {
	mCefProcess->getCefAppBrowser()->getBrowser()->mouseReleased(&button.getEvent());
}
void Game::mouseWheel(const SDL_MouseWheelEvent& mouseWheel) {
	mCefProcess->getCefAppBrowser()->getBrowser()->mouseScrolled(&mouseWheel);
}
