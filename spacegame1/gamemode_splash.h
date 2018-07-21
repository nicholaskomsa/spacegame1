#include <memory>
#include <sdlinput.h>


#pragma once

class Game;
class GameMode;
class GuiSheet;
class GuiArgList;


class GameMode_Splash : public GameMode, public GuiSheet {
	bool mStartGame{ false };
	bool mStartCredits{ false };

	void onGuiStart(GuiArgList args);
	void onGuiCredits(GuiArgList args);

	void onGuiExit(GuiArgList args);
public:
	GameMode_Splash(Game* game);
	virtual ~GameMode_Splash();

	void setup();
	void step();
	std::shared_ptr<GameMode> shutdown();
	void keyPressed(const SDL_KeyboardEvent* evt);
};

