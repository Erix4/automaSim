#ifndef UTILS_H
#define UTILS_H

//cpp libraries
#include <stdio.h>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <stack>

//imported libraries
#include "SDL2/SDL.h"
//#include "SDL_ttf.h"

//Screen dimension constants
int const SCREEN_WIDTH = 750;//640;
int const SCREEN_HEIGHT = 550;//480;

//game constants
int const CELL_SIZE = 32;
int const SUBCELL_SIZE = CELL_SIZE / 16;

int const SCREEN_CELL_WIDTH = SCREEN_WIDTH / CELL_SIZE;
int const SCREEN_CELL_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

int const CAM_SPEED = 8;//speed of camera movement

int const MAX_PUTS = 4;//max number of inputs or outputs in machines
int const MAX_CONFIGS = 4;//max number of configurations for a machine

int const MATTE_SIZE = 4;
int const MATTE_AREA = MATTE_SIZE * MATTE_SIZE;

SDL_Color const SDL_WHITE = {255,255,255,255};


class RendOb;
class MouseState{
    public:
        MouseState();
        bool busy;
        RendOb* carrying;
        int mx; int my;
        bool mouseDown;
        bool mouseUpEvent;
};

class camState{
    public:
        int camx;
        int camy;
        int subcellSize;//size of c
        int cellSize;//
        void setSubcellSize(int sub){subcellSize = sub; cellSize = (sub << 4);};
};

SDL_Color hex2sdl(std::string input);

bool collisionDet(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

#endif