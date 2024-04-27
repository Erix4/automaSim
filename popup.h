#ifndef POPUP_H
#define POPUP_H

#include "matte.h"

class StaticPopup : public RendOb{//abstract class
    protected:
        RendOb* popupElements[MAX_ELEMENTS];//passes static campos (0,0) into elements
        bool popupState;//whether popup is current enabled
    public:
        StaticPopup();
        void render(SDL_Renderer* rend, SDL_Point camPos);
};

class ShopPopup : public StaticPopup{
    private:
        bool collapsed;
        SDL_Point collapsedSize;
        //
    public:
        ShopPopup() : ShopPopup(200, 50, 10){}
        ShopPopup(int pxHeight, int buttonPxWidth, int buttonPxHeight);
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void updateAction(MouseState*);
};

class FloatingPopup : public StaticPopup{//abstract class
    public:
        //
};

class HighlightPopup : public FloatingPopup{
    //
};

#endif