#ifndef POPUP_H
#define POPUP_H

#include "matte.h"

class StaticPopup : public RendOb{
    private:
        RendOb popupElements[MAX_ELEMENTS];//passes static campos (0,0) into elements
        bool popupState;//whether popup is current enabled
    public:
        StaticPopup();
        void render(SDL_Renderer* rend, SDL_Point camPos);
};

class FloatingPopup : public StaticPopup{
    public:
        void render(SDL_Renderer* rend, SDL_Point camPos);
};

#endif