#include <guisheet.h>


#include "gamemode.h"

#include "game.h"

#include "gamemode_play.h"


#include "gamemode_splash.h"


void GameMode_Play::handleInput() {


	if (mGame->isKeyDown(SDLK_F1)) mGame->getCamera()->setPolySolid();
	if (mGame->isKeyDown(SDLK_F2)) mGame->getCamera()->setPolyWireFrame();
	if (mGame->isKeyDown(SDLK_F3)) mGame->getCamera()->setPolyPoints();
}

GameMode_Play::GameMode_Play(Game* game)
	: GameMode(game)
	, GuiSheet("missilecommand_play.htm")
{

}

void GameMode_Play::setup() {
	mGame->setGuiSheet("play", this);

	//start playing some music

}

void GameMode_Play::step() {

	handleInput();

}


std::shared_ptr<GameMode> GameMode_Play::shutdown() {

	if (!mGame->playing())
		return nullptr;

	return std::make_shared<GameMode_Splash>(mGame);
}

void GameMode_Play::keyPressed(const SDL_KeyboardEvent* evt) {
	switch (evt->keysym.sym) {
	case SDLK_ESCAPE:
		mPlayMode = false;
		break;
	}
}
void GameMode_Play::keyReleased(const SDL_KeyboardEvent* evt) {

}
void GameMode_Play::mousePressed(const SDL_MouseButtonEvent* evt) {

}
void GameMode_Play::mouseReleased(const SDL_MouseButtonEvent* evt) {


}
void GameMode_Play::mouseMoved(const SDL_MouseMotionEvent* evt) {


}
