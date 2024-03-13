#ifndef MATTE_H
#define MATTE_H

#include "utils.h"

class Matte : public RendOb{
    private:
        short int materials[16];
    public:
        int getCoord(int x, int y);
        void setCoord(int x, int y, short int material);
};

#endif