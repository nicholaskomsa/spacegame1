#include <memory>
#include <sdlinput.h>
#include "network.h"


#pragma once

class Game;
class GameMode;
class GuiSheet;
class GuiArgList;


class GameMode_Play : public GameMode, public GuiSheet {

	std::shared_ptr<NetworkServer> mNetworkServer;

	void onDataReceived(RakNet::Packet*);

	void handleInput();

	void writeTextOutput(std::string text);

	void onGuiLoad(GuiArgList args);
	void onGuiSendTextMessage(GuiArgList args);


public:
	GameMode_Play(Game* game);

	void step();

	//gamemode
	void setup();
	std::shared_ptr<GameMode> shutdown();

	//guisheet
	void keyPressed(const SDL_KeyboardEvent* evt);
	void keyReleased(const SDL_KeyboardEvent* evt);
	void mousePressed(const SDL_MouseButtonEvent* evt);
	void mouseReleased(const SDL_MouseButtonEvent* evt);
	void mouseMoved(const SDL_MouseMotionEvent* evt);
};
