#include <memory>
#include <sdlinput.h>
#include "network.h"

#pragma once

class Game;
class GameMode;
class GuiSheet;
class GuiArgList;










class GameMode_Splash : public GameMode, public GuiSheet {

	std::shared_ptr<Network> mNetwork;



	bool mStartGame{ false };

	void onGuiSelectServer(GuiArgList args);
	void onGuiSelectClient(GuiArgList args);

	void onGuiStart(GuiArgList args);
	void onGuiLoad(GuiArgList args);

	void onGuiExit(GuiArgList args);

	void onGuiSendTextMessage(GuiArgList args);


	void onDataReceived(RakNet::Packet*);
public:
	GameMode_Splash(Game* game);
	virtual ~GameMode_Splash();

	void setup();
	void step();
	std::shared_ptr<GameMode> shutdown();
	void keyPressed(const SDL_KeyboardEvent* evt);

	void writeTextOutput(std::string text);
};

