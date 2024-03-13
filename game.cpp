//handle game initialization and loop


//using list
#include <list>
#include "game.h"

void render(SDL_Renderer* rend, std::vector<RendOb*> objs, SDL_Point camPos){
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);//clear screen(backbuffer)
    SDL_RenderClear(rend);
    //
    for(unsigned int i = 0; i < objs.size(); i++){
        //printf("rendering %d\n", i);
        objs[i]->render(rend, camPos);
    }
    //
    SDL_RenderPresent(rend);//load backbuffer onto screen
}

int handleEvents(SDL_Event e, bool &mouseUp){
    mouseUp = false;
    while( SDL_PollEvent( &e ) ){
        switch (e.type){
            case SDL_QUIT:
                printf("quit registered\n");
                return 1;
                break;
            case SDL_KEYDOWN://key press registered
                switch (e.key.keysym.scancode){
                    case 'f':
                        printf("f pressed\n");
                        //
                        break;
                    default:
                        //printf("other key pressed\n");
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                mouseUp = true;
                break;
            default:
                break;
        }
    }
    //
    return 0;
}

void handleKeys(SDL_Point &camPos, SDL_Point fieldSize, bool &updateVis){
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    //
    if(keystates[SDL_SCANCODE_RIGHT]){
        camPos.x += CAM_SPEED;
        updateVis = true;
    }else if(keystates[SDL_SCANCODE_LEFT]){
        camPos.x -= CAM_SPEED;
        updateVis = true;
    }
    //
    if(keystates[SDL_SCANCODE_DOWN]){
        camPos.y += CAM_SPEED;
        updateVis = true;
    }else if(keystates[SDL_SCANCODE_UP]){
        camPos.y -= CAM_SPEED;
        updateVis = true;
    }
    //
    if(camPos.x < 0) camPos.x = 0;
    if(camPos.y < 0) camPos.y = 0;
    if(camPos.x > ((fieldSize.x - SCREEN_CELL_WIDTH) << 4)) camPos.x = ((fieldSize.x - SCREEN_CELL_WIDTH) << 4);
    if(camPos.y > ((fieldSize.y - SCREEN_CELL_HEIGHT) << 4)) camPos.y = ((fieldSize.y - SCREEN_CELL_HEIGHT) << 4);
}

bool handleMouseMain(RendOb* startButton){
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    //
    //printf("mouse at %d, %d\n", mx,my);
    //
    if(collisionDet(mx, my, 0, 0, 
                    startButton->position.x * CELL_SIZE, 
                    startButton->position.y * CELL_SIZE, 
                    startButton->size.x * CELL_SIZE, 
                    startButton->size.y * CELL_SIZE)){
        startButton->color = hex2sdl("#dbdbdb");
        return true;
    }
    startButton->color = hex2sdl("#ffffff");
    return false;
}

void gameLoop(SDL_Renderer* rend, int screenWidth, int screenHeight){//function called once
    printf("starting game loop\n");
    //
    short unsigned int gameMode = 0;
    //
    std::vector<RendOb*> menuObjects;
    std::vector<RendOb*> gameObjects;
    //
    SDL_Point fieldSize = {30, 25};
    SDL_Point camPos = {((fieldSize.x << 4) - SCREEN_WIDTH / SUBCELL_SIZE) >> 1,
                        ((fieldSize.y << 4) - SCREEN_HEIGHT / SUBCELL_SIZE) >> 1}; //position always describes top left corner
    SDL_Point menuPos = {0,0};
    printf("camPos: %d, %d\n", camPos.x, camPos.y);
    //
    SDL_Color white = {255, 255, 255};
    SDL_Color lightGray = hex2sdl("#dbdbdb");
    SDL_Color gray = hex2sdl("#878787");
    SDL_Color black = {0,0,0};
    //
    RendOb *startButton = new RendOb(2, SCREEN_CELL_HEIGHT / 2 - 2, SCREEN_CELL_WIDTH - 4, 4, white);
    menuObjects.push_back(startButton);
    gameObjects.push_back(new Checkerboard(fieldSize, white, lightGray, gray));
    gameObjects.push_back(new RendOb(10, 10, 2, 5, gray));
    //
    int curTick = 0;
    bool mouseUp = false;
    //
    SDL_Event e;
    int quit = 0;
    while( quit == 0 ){
        quit = handleEvents(e, mouseUp);
        //
        switch(gameMode){
            case 0://title screen
                if(handleMouseMain(startButton) && mouseUp){
                    gameMode = 1;
                    printf("changing to gamemode 1\n");
                }
                //
                render(rend, menuObjects, menuPos);
                //
                break;
            default://main game
                bool updateVis = false;
                handleKeys(camPos, fieldSize, updateVis);
                //
                if(updateVis){
                    //printf("camPos: %d, %d\n", camPos.x, camPos.y);
                    for(unsigned int i = 0; i < gameObjects.size(); i++){
                        gameObjects[i]->updateVisibilty(camPos);
                    }
                }
                //
                for(unsigned int i = 0; i < gameObjects.size(); i++){
                    gameObjects[i]->updateAnim();
                }
                //
                if(curTick % 4 == 0){
                    for(unsigned int i = 0; i < gameObjects.size(); i++){
                        gameObjects[i]->updateAction();
                    }
                    //
                    curTick = 0;
                }
                //
                render(rend, gameObjects, camPos);
                curTick++;
                //
                break;
        }
    }
}