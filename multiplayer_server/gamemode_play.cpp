#include <guisheet.h>


#include "gamemode.h"

#include "game.h"

#include "gamemode_play.h"




void GameMode_Play::onGuiLoad(GuiArgList args) {

	mNetworkServer = std::make_shared<NetworkServer>();

	mNetworkServer->setOnDataReceived(this, &GameMode_Play::onDataReceived);

	mNetworkServer->startup(1);
}

void GameMode_Play::onGuiSendTextMessage(GuiArgList args) {
	mNetworkServer->sendMessageToClients(args[0].toString());
}

void GameMode_Play::onDataReceived(RakNet::Packet* p) {




	std::string msg = Network::compileStringFromData(p->data, p->length);

	writeTextOutput(msg);
}


void GameMode_Play::writeTextOutput(std::string text) {
	std::stringstream sstr;
	sstr << "'" << text << "<br>'";

	mGame->getGui()->getBrowser()->writeJavascript("writeTextOutput", sstr.str(), url);
}




void GameMode_Play::handleInput() {


	if (mGame->isKeyDown(SDLK_F1)) mGame->getCamera()->setPolySolid();
	if (mGame->isKeyDown(SDLK_F2)) mGame->getCamera()->setPolyWireFrame();
	if (mGame->isKeyDown(SDLK_F3)) mGame->getCamera()->setPolyPoints();
}

GameMode_Play::GameMode_Play(Game* game)
	: GameMode(game)
	, GuiSheet("networking_server.htm")
{
	GuiSheet::bind("onLoad", this, &GameMode_Play::onGuiLoad);
	GuiSheet::bind("onSendTextMessage", this, &GameMode_Play::onGuiSendTextMessage);


}

void GameMode_Play::setup() {
	mGame->setGuiSheet("play", this);


}

void GameMode_Play::step() {

	handleInput();



	if (mNetworkServer) mNetworkServer->processMessages();
}


std::shared_ptr<GameMode> GameMode_Play::shutdown() {

	if (!mGame->playing())
		return nullptr;

	return nullptr;
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
