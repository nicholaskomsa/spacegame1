#include <memory>
#include <sdlinput.h>


#pragma once

class Game;
class GameMode;
class GuiSheet;
class GuiArgList;


class Test_Playground;

class GameMode_Test : public GameMode, public GuiSheet {
	bool mStartGame{ false };
	bool mStartCredits{ false };

	std::unique_ptr<Test_Playground> mTestPlayground;

	void handleInput();
public:
	GameMode_Test(Game* game);
	virtual ~GameMode_Test();

	void setup();
	void step( std::chrono::milliseconds elapsedTime);
	std::shared_ptr<GameMode> shutdown();
	void keyPressed(const SDL_KeyboardEvent* evt);
};

