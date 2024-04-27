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
#include "SDL2a/SDL_ttf.h"

//Screen dimension constants
int const SCREEN_WIDTH = 750;//640;
int const SCREEN_HEIGHT = 550;//480;

//game constants
//int PX_CELL_SIZE;// = 32;//starting, default cell size

//int SCREEN_CELL_WIDTH;// = SCREEN_WIDTH / PX_CELL_SIZE;
//int SCREEN_CELL_HEIGHT;// = SCREEN_HEIGHT / PX_CELL_SIZE;

int const CAM_SPEED = 8;//speed of camera movement

int const MAX_PUTS = 4;//max number of inputs or outputs in machines
int const MAX_CONFIGS = 4;//max number of configurations for a machine

int const MATTE_SIZE = 4;
int const MATTE_AREA = MATTE_SIZE * MATTE_SIZE;

int const MAX_ELEMENTS = 20;//max number of elements in a popup

SDL_Color hex2sdl(std::string input);

SDL_Color const SDLColor_WHITE = {255,255,255,255};
SDL_Color const SDLColor_BLACK = {0,0,0,255};
SDL_Color const SDLColor_CLEAR = {0,0,0,0};
SDL_Color const SDLColor_LIGHT_PURPLE = hex2sdl("d8d4f2");
SDL_Color const SDLColor_RED_PURPLE = hex2sdl("c4b2bc");
SDL_Color const SDLColor_LIGHT_GRAY = hex2sdl("dbdbdb");


class RendOb;
class MouseState{
    public:
        MouseState();
        bool busy;//mouse is carrying, dragging, or clicking something
        RendOb* carrying;
        int mx; int my;
        int last_mx; int last_my;
        bool mouseDown;
        bool mouseUpEvent;
        float pinchDist;
};

class camState{
    public:
        int camx;
        int camy;
        int subcellSize;//size of c
        int cellSize;//
        void setSubcellSize(int sub){subcellSize = sub; cellSize = (sub << 4);};
};

bool collisionDet(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

#endif