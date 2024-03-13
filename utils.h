#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL_ttf.h"
#include <string>
#include "game.h"

class RendOb;
class mouseState{
    public:
        bool busy;
        RendOb* carrying;
        int mx; int my;
        bool mouseDown;
        bool mouseUpEvent;
};

SDL_Color hex2sdl(std::string input);

bool collisionDet(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

#endif