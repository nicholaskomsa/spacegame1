#include <guisheet.h>


#include "gamemode.h"
#include "game.h"
#include "gamemode_test.h"

#include "test_playground.h"


void GameMode_Test::handleInput() {

	static bool jumping = false;
	static int keyDownA = 0;
	static int keyDownD = 0;

	if (mTestPlayground->isNotFalling() && mGame->isKeyDown(SDLK_w)) {

		mTestPlayground->playerJump();
	}

	if (!keyDownA && mGame->isKeyDown(SDLK_a)) {
		keyDownA = SDLK_a;
		mTestPlayground->playerRight();

	}else if(keyDownA && !mGame->isKeyDown(SDLK_a)) {
		keyDownA = 0;
		mTestPlayground->playerStopX();
	}

	if (!keyDownD && mGame->isKeyDown(SDLK_d)) {
		keyDownD = SDLK_d;
		mTestPlayground->playerLeft();

	}else if(keyDownD && !mGame->isKeyDown(SDLK_d)) {
		keyDownD = 0;
		mTestPlayground->playerStopX();
	}
}


GameMode_Test::GameMode_Test(Game* game)
	: GameMode(game)
	, GuiSheet("statusbar.html")
{}
GameMode_Test::~GameMode_Test() {}

void GameMode_Test::setup() {
	mGame->setGuiSheet("statusbar", this);

	mTestPlayground = std::make_unique<Test_Playground>(mGame);
	mTestPlayground->setup();

}


void GameMode_Test::step() {

	if (mGame->isKeyDown(SDLK_F1)) mGame->getCamera()->setPolySolid();
	if (mGame->isKeyDown(SDLK_F2)) mGame->getCamera()->setPolyWireFrame();
	if (mGame->isKeyDown(SDLK_F3)) mGame->getCamera()->setPolyPoints();

	handleInput();

	mTestPlayground->step();
}

std::shared_ptr<GameMode> GameMode_Test::shutdown() {

	mTestPlayground->shutdown();
	return nullptr;
}

void GameMode_Test::keyPressed(const SDL_KeyboardEvent* evt) {
	switch (evt->keysym.sym) {
	case SDLK_ESCAPE:
		mGame->exit();
		break;
	}
}


