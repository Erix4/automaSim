#include "rendob.h"

RendOb::RendOb(int x_pos, int y_pos, int x_size, int y_size, SDL_Color color){
    position = {x_pos, y_pos};//default starting position
    size = {x_size, y_size};
    //
    visible = true;//currently in view of camera
    //
    this->color = color;
    usingImage = false;
}

RendOb::RendOb(int x_pos, int y_pos, int x_size, int y_size, SDL_Texture *texture){
    position = {x_pos, y_pos};//default starting position
    size = {x_size, y_size};
    //
    visible = true;//currently in view of camera
    //
    this->texture = texture;
    usingImage = true;
}

void RendOb::render(SDL_Renderer* rend, SDL_Point camPos){
    if(!visible) return;
    SDL_Rect curRect = {((position.x * PX_CELL_SIZE) - camPos.x),
                        ((position.y * PX_CELL_SIZE) - camPos.y),
                        size.x * PX_CELL_SIZE,
                        size.y * PX_CELL_SIZE};
    if(usingImage){
        SDL_RenderCopy(rend, texture, NULL, &curRect);
        //
        return;
    }
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(rend, &curRect);
}
void RendOb::updateVisibilty(SDL_Point camPos){
    //bottom right corner of RendOb is to bottom right of top left corner, and vice versa
    //one cell of leeway for offset
    visible = (((position.x + size.x + 1) * PX_CELL_SIZE) > camPos.x &&
            (position.x * PX_CELL_SIZE) < camPos.x + SCREEN_WIDTH &&
            ((position.y + size.y + 1) * PX_CELL_SIZE) >camPos.y &&
            (position.y * PX_CELL_SIZE) < camPos.y + SCREEN_HEIGHT);
}

void RendOb::gridShift(int x, int y){
    position.x += x;
    position.y += y;
}

void RendOb::setPosition(int x, int y){
    position.x = x;
    position.y = y;
}

RendOb::~RendOb(){
    if(usingImage){
        SDL_DestroyTexture(texture);
    }
}

Text::Text(){//default constructor
    this->text = "Incomplete";
    //
    //fontFile = "/assets/fonts/Dosis-VariableFont_wght.ttf";
    //font = TTF_OpenFont("/assets/fonts/Dosis-VariableFont_wght.ttf", 24);
}

/*Text::Text(const char *text, int x_pos, int y_pos, int x_size, int y_size, SDL_Color color) :
    RendOb(x_pos, y_pos, x_size, y_size, color){
    //
    this->text = text;
    fontFile = "/assets/fonts/Dosis-VariableFont_wght.ttf";
    font = TTF_OpenFont(fontFile, 24);
}
//
Text::Text(const char *text, int x_pos, int y_pos, int x_size, int y_size, SDL_Color color, const char *file, int fontSize) :
    RendOb(x_pos, y_pos, x_size, y_size, color){
    //
    this->text = text;
    fontFile = file;
    font = TTF_OpenFont(fontFile, fontSize);
}
//
void Text::render(SDL_Renderer* rend, SDL_Point camPos){
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color); 
    //
    SDL_Texture* message = SDL_CreateTextureFromSurface(rend, surfaceMessage);//convert to texture
    //
    SDL_Rect curRect = {position.x, position.y, size.x, size.y};//textbox
    //
    SDL_RenderCopy(rend, message, NULL, &curRect);
    //
    SDL_FreeSurface(surfaceMessage);// Free surface and texture
    SDL_DestroyTexture(message);
}
//
void Text::setFont(const char *file, int fontSize){
    fontFile = file;
    font = TTF_OpenFont(fontFile, fontSize);
}
//
void Text::setFontSize(int fontSize){
    font = TTF_OpenFont(fontFile, fontSize);
}*/

Button::Button(int x_pos = 0, int y_pos = 0, int x_size = 5, int y_size = 5,
                SDL_Color defaultColor = {255,255,255,255},
                SDL_Color hoverColor = {150,150,150,255},
                SDL_Color clickColor = {0,0,0,255},
                std::function<void()> actFunc = [](){}){//default constructor
    position = {x_pos, y_pos};
    size = {x_size, y_size};
    //
    visible = true;//currently in view of camera
    activationFunc = actFunc;//function called when button pressed
    //
    this->color = defaultColor;
    this->defaultColor = defaultColor;
    this->hoverColor = hoverColor;
    this->clickColor = clickColor;
    usingImage = false;
    //
    lastCamPos = {0,0};
}

Button::Button(int x_pos, int y_pos, int x_size, int y_size,
                SDL_Texture *defaultTexture,
                SDL_Texture *hoverTexture,
                SDL_Texture *clickTexture,
                std::function<void()> actFunc = [](){}){//default constructor
    position = {x_pos, y_pos};
    size = {x_size, y_size};
    //
    visible = true;//currently in view of camera
    activationFunc = actFunc;//function called when button pressed
    //
    this->color = defaultColor;
    this->texture = defaultTexture;
    this->defaultTexture = defaultTexture;
    this->hoverTexture = hoverTexture;
    this->clickTexture = clickTexture;
    usingImage = true;
    //
    lastCamPos = {0,0};
}

void Button::updateAction(MouseState* mouseState){
    if(clicked){
        if(!mouseState->mouseDown){//unlicked
            clicked = false;
            mouseState->busy = false;
        }
        return;
    }
    if(collisionDet(mouseState->mx, mouseState->my, 0, 0, 
                    position.x * PX_CELL_SIZE - lastCamPos.x, 
                    position.y * PX_CELL_SIZE - lastCamPos.y, 
                    size.x * PX_CELL_SIZE, 
                    size.y * PX_CELL_SIZE)){//mouse over button
        if(!mouseState->busy && mouseState->mouseDown){//mouse clicked and not clicking other thing
            clicked = true;
            mouseState->busy = true;
            color = clickColor;
            activationFunc();
            return;
        }
        color = hoverColor;//not clicked
        return;
    }
    color = defaultColor;//button not interacted with
}

void Button::setActivationFunc(std::function<void()> func){
    activationFunc = func;
}

void ButtonStatic::render(SDL_Renderer* rend, SDL_Point camPos){
    if(!visible) return;
    SDL_Rect curRect = {(position.x),
                        (position.y),
                        size.x,
                        size.y};
    //
    if(usingImage){
        SDL_RenderCopy(rend, texture, NULL, &curRect);
        //
        return;
    }
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(rend, &curRect);
    //
    lastCamPos = camPos;
}

void ButtonStatic::updateAction(MouseState* mouseState){
    if(clicked){
        if(!mouseState->mouseDown){//unclicked
            clicked = false;
            mouseState->busy = false;
        }
        return;
    }
    if(collisionDet(mouseState->mx, mouseState->my, 0, 0, 
                    position.x, 
                    position.y, 
                    size.x, 
                    size.y)){//mouse over button
        if(!mouseState->busy && mouseState->mouseDown){//mouse clicked and not clicking other thing
            clicked = true;
            mouseState->busy = true;
            color = clickColor;
            texture = clickTexture;
            activationFunc();
            return;
        }
        color = hoverColor;//not clicked
        texture = hoverTexture;
        return;
    }
    color = defaultColor;//button not interacted with
    texture = defaultTexture;
}

void ButtonPx::render(SDL_Renderer* rend, SDL_Point camPos){
    if(!visible) return;
    SDL_Rect curRect = {(position.x - camPos.x),
                        (position.y - camPos.y),
                        size.x,
                        size.y};
    //
    if(usingImage){
        SDL_RenderCopy(rend, texture, NULL, &curRect);
        //
        return;
    }
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(rend, &curRect);
}

void ButtonPx::updateAction(MouseState* mouseState){
    if(clicked){
        if(!mouseState->mouseDown){//unclicked
            clicked = false;
            mouseState->busy = false;
        }
        return;
    }
    if(collisionDet(mouseState->mx, mouseState->my, 0, 0, 
                    position.x - lastCamPos.x, 
                    position.y - lastCamPos.y, 
                    size.x, 
                    size.y)){//mouse over button
        printf("mouse over\n");
        if(!mouseState->busy && mouseState->mouseDown){//mouse clicked and not clicking other thing
            printf("clicked");
            clicked = true;
            mouseState->busy = true;
            color = clickColor;
            texture = clickTexture;
            activationFunc();
            return;
        }
        color = hoverColor;//not clicked
        texture = hoverTexture;
        return;
    }
    color = defaultColor;//button not interacted with
    texture = defaultTexture;
}

Checkerboard::Checkerboard(SDL_Point fieldSize, SDL_Color color1, SDL_Color color2){
    position = {0, 0};//default starting position
    size = {fieldSize.x, fieldSize.y};
    printf("set to size to (%d, %d)\n", size.x, size.y);
    //
    visible = true;//currently in view of camera
    //
    this->color = color1;
    this->color2 = color2;
}

void Checkerboard::render(SDL_Renderer* rend, SDL_Point camPos){
    SDL_Rect curRect;
    for(int i = 0; i < size.x; i++){
        for(int j = 0; j < size.y; j++){
            //printf("rendering on (%d, %d) with %d\n", i, j, PX_CELL_SIZE);
            if((i+j) % 2 == 0){
                SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
                curRect = {(position.x + i) * PX_CELL_SIZE - camPos.x,
                            (position.y + j) * PX_CELL_SIZE - camPos.y,
                            PX_CELL_SIZE,
                            PX_CELL_SIZE};
                SDL_RenderFillRect(rend, &curRect);
            }else{
                SDL_SetRenderDrawColor(rend, color2.r, color2.g, color2.b, color2.a);
                curRect = {(position.x + i) * PX_CELL_SIZE - camPos.x,
                            (position.y + j) * PX_CELL_SIZE - camPos.y,
                            PX_CELL_SIZE,
                            PX_CELL_SIZE};
                SDL_RenderFillRect(rend, &curRect);
            }
        }
    }
}

void Checkerboard::gridShift(int x, int y){
    size.x += x;
    size.y += y;
    //
    RendOb::gridShift(x, y);
}
