#include "popup.h"

StaticPopup::StaticPopup(int x_pos, int y_pos, int x_size, int y_size, SDL_Color color) : RendOb(x_pos, y_pos, x_size, y_size, color){
    popupState = false;
    numElements = 0;
}

void StaticPopup::render(SDL_Renderer* rend, SDL_Point camPos){
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

void StaticPopup::update(MouseState *mouseState){
    for(int i = 0; i <numElements; i++){
        popupElements[i]->update(mouseState);
    }
}

bool StaticPopup::getPopupState(){
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
    ButtonStatic *collapseButton = new ButtonStatic(0, SCREEN_HEIGHT - buttonPxHeight - pxHeight,
                                        buttonPxWidth, buttonPxHeight,
                                        SDLColor_LIGHT_PURPLE, SDLColor_RED_PURPLE, SDLColor_BLACK, nullptr);
    collapseButton->setActivationFunc(std::bind(toggleCollapse, &popupState, buttonPxHeight, pxHeight, collapseButton));
    //
    popupElements[0] = collapseButton;
    numElements = 1;
}

void ShopPopup::render(SDL_Renderer* rend, SDL_Point camPos){
    if(popupState){
        StaticPopup::render(rend, shopCam);
    }else{
        popupElements[0]->render(rend, shopCam);
    }
}

void ShopPopup::update(MouseState *mouseState){
    StaticPopup::update(mouseState);//update all popup elements
    //
    shopCam.x += mouseState->scrollX;
    //if(s)
}

void ShopPopup::addMachineButton(int type, SDL_Texture *texture){
    std::function<void()> curMachineFunc = std::bind(newMachineFunc, type);
    RendOb *newMachine = new ButtonPx(0, 0, size.y / 2, size.y / 2, texture, texture, texture, curMachineFunc);
    //
    popupElements[numElements] = newMachine;
    numElements++;
}