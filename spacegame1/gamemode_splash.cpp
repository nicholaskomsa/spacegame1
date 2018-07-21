#include <guisheet.h>


#include "gamemode.h"
#include "game.h"
#include "gamemode_splash.h"

#include "gamemode_play.h"


void GameMode_Splash::onGuiCredits(GuiArgList args) {
	mPlayMode = false;
	mStartCredits = true;
}

void GameMode_Splash::onGuiStart(GuiArgList args) {
	mPlayMode = false;
	mStartGame = true;
}

void GameMode_Splash::onGuiExit(GuiArgList args) {
	mGame->exit();
}

GameMode_Splash::GameMode_Splash(Game* game)
	: GameMode(game)
	, GuiSheet("missilecommand_splash.htm")
{
	GuiSheet::bind("onStart", this, &GameMode_Splash::onGuiStart);
	GuiSheet::bind("onCredits", this, &GameMode_Splash::onGuiCredits);
}
GameMode_Splash::~GameMode_Splash() {}

void GameMode_Splash::setup() {
	mGame->setGuiSheet("splash", this);

}

void GameMode_Splash::step() {

	if (mGame->isKeyDown(SDLK_F1)) mGame->getCamera()->setPolySolid();
	if (mGame->isKeyDown(SDLK_F2)) mGame->getCamera()->setPolyWireFrame();
	if (mGame->isKeyDown(SDLK_F3)) mGame->getCamera()->setPolyPoints();

}

std::shared_ptr<GameMode> GameMode_Splash::shutdown() {
	if (!mGame->playing())
		return nullptr;
	if (mStartGame)
		return std::make_shared<GameMode_Play>(mGame);

	return nullptr;
}

void GameMode_Splash::keyPressed(const SDL_KeyboardEvent* evt) {
	switch (evt->keysym.sym) {
	case SDLK_ESCAPE:
		mGame->exit();
		break;
	}
}


