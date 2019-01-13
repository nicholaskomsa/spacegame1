#include <guisheet.h>


#include "gamemode.h"
#include "game.h"
#include "gamemode_splash.h"

#include "gamemode_play.h"




void GameMode_Splash::onGuiStart(GuiArgList args) {
	mPlayMode = false;
	mStartGame = true;
}

void GameMode_Splash::onGuiLoad(GuiArgList args) {
	//mNetwork->setup();

}

void GameMode_Splash::onGuiExit(GuiArgList args) {
	mGame->exit();
}

void GameMode_Splash::onGuiSendTextMessage(GuiArgList args) {
	mNetwork->sendMessageToClients( args[0].toString() );

}
void GameMode_Splash::onGuiSelectServer(GuiArgList args) {
	int maxConnections = args[0];

	mNetwork = std::make_shared<NetworkServer>();
	mNetwork->setOnDataReceived(this, &GameMode_Splash::onDataReceived);

	std::shared_ptr<NetworkServer> server = std::dynamic_pointer_cast<NetworkServer>(mNetwork);
	server->startup(maxConnections);
}
void GameMode_Splash::onGuiSelectClient(GuiArgList args) {
	int clientPort = args[0];

	mNetwork = std::make_shared<NetworkClient>();
	mNetwork->setOnDataReceived(this, &GameMode_Splash::onDataReceived);

	std::shared_ptr<NetworkClient> client = std::dynamic_pointer_cast<NetworkClient>(mNetwork);
	client->connect("127.0.0.1", "nick", 1,1234U, clientPort);
}


void GameMode_Splash::onDataReceived(RakNet::Packet* p) {

	std::string msg = Network::compileStringFromData(p->data, p->length);

	writeTextOutput(msg);
}

GameMode_Splash::GameMode_Splash(Game* game)
	: GameMode(game)
	, GuiSheet("networking.htm")
{
	GuiSheet::bind("onStart", this, &GameMode_Splash::onGuiStart);
	GuiSheet::bind("onLoad", this, &GameMode_Splash::onGuiLoad);
	GuiSheet::bind("onSendTextMessage", this, &GameMode_Splash::onGuiSendTextMessage);

	GuiSheet::bind("onSelectServer", this, &GameMode_Splash::onGuiSelectServer);
	GuiSheet::bind("onSelectClient", this, &GameMode_Splash::onGuiSelectClient);


}
GameMode_Splash::~GameMode_Splash() {}

void GameMode_Splash::setup() {
	mGame->setGuiSheet("splash", this);

}

void GameMode_Splash::step() {

	if (mGame->isKeyDown(SDLK_F1)) mGame->getCamera()->setPolySolid();
	if (mGame->isKeyDown(SDLK_F2)) mGame->getCamera()->setPolyWireFrame();
	if (mGame->isKeyDown(SDLK_F3)) mGame->getCamera()->setPolyPoints();

	if(mNetwork) mNetwork->processMessages();

}

std::shared_ptr<GameMode> GameMode_Splash::shutdown() {

	if (mNetwork) {
		mNetwork->shutdown();
		mNetwork.reset();
	}

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

void GameMode_Splash::writeTextOutput(std::string text) {
	std::stringstream sstr;
	sstr << "'" << text << "<br>'";

	mGame->getGui()->getBrowser()->writeJavascript("writeTextOutput", sstr.str(), url);
}
