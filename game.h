#ifndef GAME_HEADER
#define GAME_HEADER

//Screen dimension constants
int const SCREEN_WIDTH = 750;//640;
int const SCREEN_HEIGHT = 550;//480;

//game constants
int const CELL_SIZE = 32;
int const SUBCELL_SIZE = CELL_SIZE / 16;

int const SCREEN_CELL_WIDTH = SCREEN_WIDTH / CELL_SIZE;
int const SCREEN_CELL_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

int const CAM_SPEED = 8;//speed of camera movement

#include <stdio.h>
#include "SDL2/SDL.h"
#include <vector>

//game.cpp
void gameLoop(SDL_Renderer* rend, int screenWidth, int screenHeight);


//utils.cpp
#include "utils.h"
#include "rendob.h"
//#include "matte.h"
//#include "machine.h"

#endif