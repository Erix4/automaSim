#include "popup.h"

Popup::Popup(int x_pos, int y_pos, int x_size, int y_size, SDL_Color color, int renderType) : RendOb(x_pos, y_pos, x_size, y_size, color, renderType){
    popupState = false;
    numElements = 0;
}

void Popup::render(SDL_Renderer* rend, SDL_Point camPos){
    if(!visible) return;
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    SDL_Rect curRect = {(position.x),
                        (position.y),
                        size.x,
                        size.y};
    SDL_RenderFillRect(rend, &curRect);
    //
    for(int i = 0; i < numElements; i++){
        popupElements[i]->render(rend, camPos);
    }
}

void Popup::update(MouseState *mouseState){
    for(int i = 0; i <numElements; i++){
        popupElements[i]->update(mouseState);
    }
}

bool Popup::getPopupState(){
    return popupState;
}

void toggleCollapse(bool *collapsed, int buttonPxHeight, int pxHeight, Button *collapseButton){
    *collapsed = !*collapsed;
    if(*collapsed){
        collapseButton->setPosition(0, SCREEN_HEIGHT - pxHeight - buttonPxHeight);
    }else{
        collapseButton->setPosition(0, SCREEN_HEIGHT - buttonPxHeight);
    }
}

ShopPopup::ShopPopup(int pxHeight, int buttonPxWidth, int buttonPxHeight, std::function<void(int)> newMachineFunc){
    popupState = true;
    visible = true;
    //
    this->newMachineFunc = newMachineFunc;
    //
    position = {0, SCREEN_HEIGHT - pxHeight};
    size = {SCREEN_WIDTH, pxHeight};
    collapsedSize = {buttonPxWidth, buttonPxHeight};
    //
    shopCam = {-pxHeight / 8, -position.y - (pxHeight / 4)};//scroll position in pixels
    //
    color = SDLColor_ORANGE;
    //
    Button *collapseButton = new Button(0, SCREEN_HEIGHT - buttonPxHeight - pxHeight,
                                        buttonPxWidth, buttonPxHeight,
                                        SDLColor_LIGHT_PURPLE, SDLColor_RED_PURPLE, SDLColor_BLACK, 2, nullptr);
    collapseButton->setActivationFunc(std::bind(toggleCollapse, &popupState, buttonPxHeight, pxHeight, collapseButton));
    //
    popupElements[0] = collapseButton;
    numElements = 1;
}

void ShopPopup::render(SDL_Renderer* rend, SDL_Point camPos){
    if(popupState){
        Popup::render(rend, shopCam);
    }else{
        popupElements[0]->render(rend, shopCam);
    }
}

void ShopPopup::update(MouseState *mouseState){
    Popup::update(mouseState);//update all popup elements
    //
    shopCam.x += mouseState->scrollX;
    //if(s)
}

void ShopPopup::addMachineButton(int type, SDL_Texture *texture){
    std::function<void()> curMachineFunc = std::bind(newMachineFunc, type);
    RendOb *newMachine = new Button((numElements - 1) * size.y * 3/4, 0, size.y / 2, size.y / 2, texture, texture, texture, 1, curMachineFunc);
    //
    popupElements[numElements] = newMachine;
    numElements++;
}