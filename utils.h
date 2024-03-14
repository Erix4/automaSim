#ifndef UTILS_H
#define UTILS_H

//cpp libraries
#include <stdio.h>
#include <vector>
#include <string>
#include <functional>

//imported libraries
#include "SDL2/SDL.h"
//#include "SDL_ttf.h"

//other headers
#include "game.h"


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