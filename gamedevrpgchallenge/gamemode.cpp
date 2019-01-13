
#include "gamemode.h"

#include "game.h"


GameMode::GameMode(Game* game)
	:mGame(game)
{}

GameMode::~GameMode() {}



void GameMode::run() {

	std::chrono::nanoseconds lag(0);
	std::chrono::time_point<std::chrono::high_resolution_clock> endTime = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::high_resolution_clock> fpsStartTime = endTime;

	int frameCount = 0;
	int tickCount = 0;

	const int fps = 60;
	const std::chrono::nanoseconds lengthOfFrame(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)) / fps);

	do {


		//do status stuff
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime2 = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds fpsElapsedTime(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime2 - fpsStartTime));

		if (fpsElapsedTime >= std::chrono::seconds(1)) {

			std::stringstream sstr;
			sstr << frameCount << ", "
				<< tickCount;

			mGame->getGui()->getBrowser()->writeJavascript("setStatus", sstr.str(), mGame->getGui()->getGuiSheetManager()->getActiveSheet()->url);

			fpsStartTime = std::chrono::high_resolution_clock::now();
			frameCount = 0;
			tickCount = 0;
		}

		std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

		std::chrono::nanoseconds elapsedTime(std::chrono::duration_cast<std::chrono::nanoseconds>(startTime - endTime));
		endTime = startTime;

		lag += elapsedTime;


		//game-sim loop, is independent of rendering
		while (lag >= lengthOfFrame) {

			//update the player and ship
			mGame->pollInput();
			mGame->doCefWork();

			step();

			//finish tick
			lag -= lengthOfFrame;

			tickCount++;	//keep track of how many ticks weve done this second
		}


		//mGame->pollInput();
		//mGame->doCefWork();
		//render the frame
		mGame->render();
		frameCount++;	//keep trrack of how many frames weve done this second

	} while (mGame->playing() && mPlayMode);

}