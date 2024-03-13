#include "matte.h"

Matte::Matte(){
    //
}

void Matte::render(SDL_Renderer* rend){
    for(int i = 0; i < 16; i++){
        SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
        SDL_Rect curRect = {position.x, position.y, size.x, size.y};
        SDL_RenderFillRect(rend, &curRect);
    }
}

int Matte::getCoord(int x, int y){
    return materials[y*4+x];
}

void Matte::setCoord(int x, int y, short int material){
    materials[y*4+x] = material;
}