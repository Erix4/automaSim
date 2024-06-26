#ifndef POPUP_H
#define POPUP_H

#include "matte.h"

class Machine;
class Popup : public RendOb{//abstract class
    protected:
        RendOb* popupElements[MAX_ELEMENTS];//passes static campos (0,0) into elements
        int numElements;
        bool popupState;//whether popup is current enabled
    public:
        Popup() : Popup(0,0,100,80,SDLColor_LIGHT_GRAY,2){}
        Popup(int x_pos, int y_pos, int x_size, int y_size, SDL_Color color,int renderType);
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void update(MouseState*);
        bool getPopupState();
};

class ShopPopup : public Popup{
    private:
        SDL_Point collapsedSize;
        //
        SDL_Point shopCam;
        //
        std::function<void(int)> newMachineFunc;
    public:
        ShopPopup() : ShopPopup(200, 50, 10, nullptr){}
        ShopPopup(int pxHeight, int buttonPxWidth, int buttonPxHeight, std::function<void(int)> newMachineFunc);
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void update(MouseState*);
        void addMachineButton(int type, SDL_Texture *texture);//enum passed as int
};

#endif