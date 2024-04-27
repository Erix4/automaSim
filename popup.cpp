#include "popup.h"

StaticPopup::StaticPopup(){}

void StaticPopup::render(SDL_Renderer* rend, SDL_Point camPos){
    if(!visible) return;
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    SDL_Rect curRect = {(position.x),
                        (position.y),
                        size.x,
                        size.y};
    SDL_RenderFillRect(rend, &curRect);
}

void toggleCollapse(bool &collapsed, int buttonPxHeight, int pxHeight, Button *collapseButton){
    collapsed = !collapsed;
    if(collapsed){
        collapseButton->setPosition(0, SCREEN_HEIGHT - buttonPxHeight);
    }else{
        collapseButton->setPosition(0, SCREEN_HEIGHT - pxHeight);
    }
}

ShopPopup::ShopPopup(int pxHeight, int buttonPxWidth, int buttonPxHeight){
    collapsed = true;
    //
    position = {0, SCREEN_HEIGHT - pxHeight};
    size = {SCREEN_WIDTH, pxHeight};
    collapsedSize = {buttonPxWidth, buttonPxHeight};
    //
    color = SDLColor_LIGHT_GRAY;
    //
    ButtonStatic *collapseButton = new ButtonStatic(0, SCREEN_HEIGHT - buttonPxHeight,
                                        buttonPxWidth, buttonPxHeight,
                                        SDLColor_LIGHT_PURPLE, SDLColor_RED_PURPLE, SDLColor_BLACK, nullptr);
    collapseButton->setActivationFunc(std::bind(toggleCollapse, collapsed, buttonPxHeight, pxHeight, collapseButton));
    //
    popupElements[0] = collapseButton;
}

void ShopPopup::render(SDL_Renderer* rend, SDL_Point camPos){
    popupElements[0]->render(rend, camPos);
    if(!collapsed){
        StaticPopup::render(rend, camPos);
    }
}

void ShopPopup::updateAction(MouseState* mouseState){
    popupElements[0]->updateAction(mouseState);
}