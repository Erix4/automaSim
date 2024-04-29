#ifndef GAME_HEADER
#define GAME_HEADER

#include "machine.h" //game -> machine -> popup -> matte -> rendob -> utils

enum textureTypes{shifterMachine,
                menuStartButton, menuStartButtonHover, menuStartButtonClick,
                menuContinueButton, menuContinueButtonHover, menuContinueButtonClick,
};

//game.cpp
void gameLoop(SDL_Renderer* rend, int screenWidth, int screenHeight);

#endif