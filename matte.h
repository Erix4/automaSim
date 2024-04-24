#ifndef MATTE_H
#define MATTE_H

#include "rendob.h"

enum material {wood, steel};

//class RendOb;
class Matte : public RendOb{
    private:
        material materials[MATTE_AREA];
    public:
        Matte(material startMaterial);//raw material constructor
        Matte(Matte& original);//copy constructor
        Matte();//default constructor
        material getCoord(int x, int y);
        void setCoord(int x, int y, material material);
        void render(SDL_Renderer* rend);
};

#endif