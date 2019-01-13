#include <memory>


#pragma once


class Game;


class GameMode {
protected:
	Game * mGame;
	bool mPlayMode{ true };
public:
	GameMode(Game* game);

	virtual ~GameMode();

	virtual void setup() = 0;
	void run();
	virtual std::shared_ptr<GameMode> shutdown() = 0;

	virtual void step() = 0;
};