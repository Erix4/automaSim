#include "matte.h"

Matte::Matte(material fillMaterial){
    for(int i = 0; i < MATTE_AREA; i++){
        materials[i] = fillMaterial;
    }
}

Matte::Matte(Matte& original){
    for(int i = 0; i < MATTE_SIZE; i++){
        for(int j = 0; j < MATTE_SIZE; j++){
            setCoord(i, j, original.getCoord(i, j));//copy every material over
        }
    }
}

void Matte::render(SDL_Renderer* rend){
    for(int i = 0; i < MATTE_AREA; i++){
        SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
        SDL_Rect curRect = {position.x, position.y, size.x, size.y};
        SDL_RenderFillRect(rend, &curRect);
    }
}

material Matte::getCoord(int x, int y){
    return materials[y*MATTE_SIZE+x];
}

void Matte::setCoord(int x, int y, material newmatt){
    materials[y*MATTE_SIZE+x] = newmatt;
}