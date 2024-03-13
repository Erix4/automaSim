#include "utils.h"

SDL_Color hex2sdl(std::string input) {

    if (input[0] == '#')
        input.erase(0, 1);

    unsigned long value = stoul(input, nullptr, 16);

    SDL_Color color;

    color.a = (value >> 24) & 0xff;
    color.r = (value >> 16) & 0xff;
    color.g = (value >> 8) & 0xff;
    color.b = (value >> 0) & 0xff;
    return color;
}

bool collisionDet(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
    return (x1 + w1 > x2 &&
            x1 < x2 + w2 &&
            y1 + h1 > y2 &&
            y1 < y2 + h2);
}
//