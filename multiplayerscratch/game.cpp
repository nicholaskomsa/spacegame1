#include "gamemode.h"

#include "game.h"

#include "gamemode_splash.h"
#include "gamemode_play.h"




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

	addAlphaKeys();
	addNumberKeys();

	addKey(SDLK_BACKSPACE);
	addKey(SDLK_RETURN);



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

	mGraphics.setup("Networking Tests");

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
	mGameModes.push(std::make_shared< GameMode_Splash >(this));
	do {
		mGameModes.top()->setup();
		mGameModes.top()->run();
		std::shared_ptr<GameMode> nextMode = mGameModes.top()->shutdown();
		mGameModes.pop();

		if (nextMode) mGameModes.push(nextMode);
		else mPlaying = false;
	} while (mPlaying);
}


void Game::go1() {
	mPlaying = true;

	int lag = 0;
	std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();

	long longestTime = 0;
	long frameCount = 0;
	long tickCount = 0;
	long fpsElapsedTime = 0;

	while (mPlaying) {

		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

		std::chrono::milliseconds elapsedTime(std::chrono::duration_cast<std::chrono::milliseconds>(startTime - endTime));
		endTime = startTime;

		lag += static_cast<int>(elapsedTime.count());

		if (elapsedTime.count() == 0) { //the sim loop will not be run if lag was not increased
			Sleep(5);
		}

		const int fps = 60;
		const int lengthOfFrame = 1000 / fps;

		//MSG msg;
		//while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}

		//game-sim loop, is independent of rendering
		std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::steady_clock::now();
		while (lag >= lengthOfFrame) {

			mSDLEventPoller.poll();

			CefDoMessageLoopWork();

			//finish tick
			lag -= lengthOfFrame;
			tickCount++;	//keep track of how many ticks weve done this second
		}

		//render the frame
		mGraphics.getRoot()->renderOneFrame();
		frameCount++;	//keep trrack of how many frames weve done this second

						//handle per-second stats
		fpsElapsedTime += static_cast<long>(elapsedTime.count());
		if (fpsElapsedTime >= 1000) {

			std::stringstream sstr;
			sstr << frameCount << ", "
				<< tickCount;


			mCefProcess->getCefAppBrowser()->getBrowser()->writeJavascript("setStatus", sstr.str(), mCefProcess->getCefAppBrowser()->getGuiSheetManager()->getActiveSheet()->url);
			fpsElapsedTime = 0;
			frameCount = 0;
			tickCount = 0;
		}
	}
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
