#include <guisheet.h>


#include "gamemode.h"
#include "game.h"
#include "gamemode_test.h"

#include "test_playground.h"


void GameMode_Test::handleInput() {

	static bool jumping = false;
	static int keyDownA = 0;
	static int keyDownD = 0;
	static int keyDownW = 0;
	static int keyDownSpace = 0;

	static bool doubleJump = false;
	

	if (!keyDownW && mGame->isKeyDown(SDLK_w)) {
		keyDownW = SDLK_w;
		if (!mTestPlayground->isPlayerJumping()) {
			mTestPlayground->playerJump(false);
			doubleJump = false;
		
		}else {
			if (!doubleJump) {
				mTestPlayground->playerJump(true);
				doubleJump = true;
			}
		}
	}
	else if( keyDownW && !mGame->isKeyDown(SDLK_w)) {
		keyDownW = 0;
	}

	if (!keyDownA && mGame->isKeyDown(SDLK_a)) {
		keyDownA = SDLK_a;
		
		mTestPlayground->playerLeft();

	}else if(keyDownA && !mGame->isKeyDown(SDLK_a)) {
		keyDownA = 0;
	}

	if (!keyDownD && mGame->isKeyDown(SDLK_d)) {
		keyDownD = SDLK_d;
		
		mTestPlayground->playerRight();

	}else if(keyDownD && !mGame->isKeyDown(SDLK_d)) {
		keyDownD = 0;
	}

	if (!keyDownA && !keyDownD) {
		mTestPlayground->playerStopX();
	}


	if (!keyDownSpace && mGame->isKeyDown(SDLK_SPACE)) {
		keyDownSpace = SDLK_SPACE;

		mTestPlayground->playerShootGun();

	}
	else if (keyDownSpace && !mGame->isKeyDown(SDLK_SPACE)) {
		keyDownSpace = 0;
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


void GameMode_Test::step( std::chrono::milliseconds elapsedTime) {

	if (mGame->isKeyDown(SDLK_F1)) mGame->getCamera()->setPolySolid();
	if (mGame->isKeyDown(SDLK_F2)) mGame->getCamera()->setPolyWireFrame();
	if (mGame->isKeyDown(SDLK_F3)) mGame->getCamera()->setPolyPoints();

	handleInput();

	mTestPlayground->step(elapsedTime);
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


