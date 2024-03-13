#include "rendob.h"

RendOb::RendOb(int x_pos = 0, int y_pos = 0, int x_size = 5, int y_size = 5, SDL_Color color = {255,255,255,255}){//default constructor
    position = {x_pos, y_pos};//default starting position
    size = {x_size, y_size};
    offsetx = 0; offsety = 0;
    //
    visible = true;//currently in view of camera
    //
    this->color = color;
}

void RendOb::render(SDL_Renderer* rend, SDL_Point camPos){
    if(!visible) return;
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    SDL_Rect curRect = {((position.x << 4) - camPos.x + offsetx) * SUBCELL_SIZE,
                        ((position.y << 4) - camPos.y + offsety) * SUBCELL_SIZE,
                        size.x * CELL_SIZE,
                        size.y * CELL_SIZE};
    SDL_RenderFillRect(rend, &curRect);
}
void RendOb::updateVisibilty(SDL_Point camPos){
    //bottom right corner of RendOb is to bottom right of top left corner, and vice versa
    //one cell of leeway for offset
    visible = (position.x + size.x > 16 * camPos.x - 1 &&
        position.x < SCREEN_CELL_WIDTH &&
        position.y + size.y > 16 * camPos.y - 1 &&
        position.y < SCREEN_CELL_HEIGHT);
}

void RendOb::gridShift(int x, int y){
    position.x += x;
    position.y += y;
}

/*Text::Text(){//default constructor
    this->text = "Incomplete";
    //
    fontFile = "/assets/fonts/NotoSansJP-Regular.ttf";
    //font = TTF_OpenFont(fontFile, 24);
}

Text::Text(const char *text, int x_pos, int y_pos, int x_size, int y_size, SDL_Color color) :
    RendOb(x_pos, y_pos, x_size, y_size, color){
    //
    this->text = text;
    fontFile = "/assets/fonts/NotoSansJP-Regular.ttf";
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
void Text::render(SDL_Renderer* rend){
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
                std::function<void> actFunc = {}){//default constructor
    position = {x_pos, y_pos};//default starting position
    size = {x_size, y_size};
    offsetx = 0; offsety = 0;
    //
    visible = true;//currently in view of camera
    activationFunc = actFunc;//function called when button pressed
    //
    this->color = defaultColor;
    this->defaultColor = defaultColor;
    this->hoverColor = hoverColor;
    this->clickColor = clickColor;
}

void Button::updateAction(mouseState){
    if(clicked && mouseState.mouseUpEvent){
        //unclicked
    }
    if(collisionDet(mx, my, 0, 0, 
                    position.x * CELL_SIZE, 
                    position.y * CELL_SIZE, 
                    size.x * CELL_SIZE, 
                    size.y * CELL_SIZE)){
        if(!mouseState.busy && mouseState.mouseDown){
            activationFunc();
            clicked = true;
            mouseState.busy = true;
            color = clickColor;
        }else{
            color = hoverColor;
        }
    }else if(!clicked){
        color = defaultColor;
    }
}

Checkerboard::Checkerboard(SDL_Point fieldSize, SDL_Color color1, SDL_Color color2, SDL_Color color3){
    position = {0, 0};//default starting position
    size = {fieldSize.x, fieldSize.y};
    offsetx = 0; offsety = 0;
    //
    visible = true;//currently in view of camera
    //
    this->color = color1;
    this->color2 = color2;
    this->color3 = color3;
}

void Checkerboard::render(SDL_Renderer* rend, SDL_Point camPos){
    SDL_Point startCell = {0,0};
    SDL_SetRenderDrawColor(rend, color3.r, color3.g, color3.b, color3.a);
    if(camPos.x < 0){
        startCell.x = -camPos.x;
        SDL_Rect curRect = {0, 0, startCell.x * SUBCELL_SIZE, SCREEN_HEIGHT};
        SDL_RenderFillRect(rend, &curRect);
        //
        curRect = {(startCell.x + (size.x << 4)) * SUBCELL_SIZE, 0, (startCell.x + 10) * SUBCELL_SIZE, SCREEN_HEIGHT};
        SDL_RenderFillRect(rend, &curRect);
    }
    if(camPos.y < 0){
        startCell.y = -camPos.y;
        SDL_Rect curRect = {0, 0, SCREEN_WIDTH, startCell.y * SUBCELL_SIZE};
        SDL_RenderFillRect(rend, &curRect);
        //
        curRect = {0, (startCell.y + (size.y << 4)) * SUBCELL_SIZE, SCREEN_WIDTH, (startCell.y + 1) * SUBCELL_SIZE};
        SDL_RenderFillRect(rend, &curRect);
    }
    //
    //printf("start x: %d, size x: %d, screen width: %d\n", startCell.x, (size.x << 4), (SCREEN_CELL_WIDTH << 4));
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    for(int i = startCell.x; i < (size.x << 4) && i - camPos.x < (SCREEN_CELL_WIDTH << 4) + 1; i++){
        for(int j = startCell.y; j < (size.y << 4) && j - camPos.y < (SCREEN_CELL_HEIGHT << 4) + 1; j++){
            SDL_Rect curRect = {((i * 2 + (j % 2)) * 16 - camPos.x) * SUBCELL_SIZE,
                                (j * 16 - camPos.y) * SUBCELL_SIZE,
                                CELL_SIZE,
                                CELL_SIZE};
            SDL_RenderFillRect(rend, &curRect);
        }
    }
    SDL_SetRenderDrawColor(rend, color2.r, color2.g, color2.b, color2.a);//TODO: fix
    for(int i = startCell.x; i < (size.x << 4) && i - camPos.x < (SCREEN_CELL_WIDTH << 4) + 1; i++){
        for(int j = startCell.y; j < (size.y << 4) && j - camPos.y < (SCREEN_CELL_HEIGHT << 4) + 1; j++){
            SDL_Rect curRect = {((i * 2 + (j + 1 % 2)) * 16 - camPos.x) * SUBCELL_SIZE,
                                (j * 16 - camPos.y) * SUBCELL_SIZE,
                                CELL_SIZE,
                                CELL_SIZE};
            SDL_RenderFillRect(rend, &curRect);
        }
    }
}

void Checkerboard::gridShift(int x, int y){
    size.x += x;
    size.y += y;
    //
    RendOb::gridShift(x, y);
}
