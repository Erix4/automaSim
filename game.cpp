//handle game initialization and loop


//using list
#include <list>
#include "game.h"

int PX_CELL_SIZE;
int SCREEN_CELL_WIDTH;
int SCREEN_CELL_HEIGHT;

void render(SDL_Renderer* rend, std::vector<RendOb*> objs[], SDL_Point camPos, SDL_Color bkgColor){
    SDL_SetRenderDrawColor(rend, bkgColor.r, bkgColor.g, bkgColor.b, 255);//clear screen(backbuffer)
    SDL_RenderClear(rend);
    //
    for(int j = 0; j < LAYER_NUM; j++){
        for(int i = 0; i < objs[j].size(); i++){
        //printf("rendering %d\n", i);
        objs[j][i]->render(rend, camPos);
    }
    }
    //
    SDL_RenderPresent(rend);//load backbuffer onto screen
}

int handleEvents(SDL_Event e, MouseState* mouseState){
    mouseState->mouseUpEvent = false;
    mouseState->pinchDist = 0;
    mouseState->scrollX = 0;
    //
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
                mouseState->mouseUpEvent = true;
				printf("mouseUp event\n");
                break;
            case SDL_MOUSEWHEEL:
                mouseState->pinchDist = (float)e.wheel.y / 100;
                //
                mouseState->scrollX = e.wheel.x * 5;
                //
                break;
            case SDL_MULTIGESTURE:
                mouseState->pinchDist = e.mgesture.dDist;
                //
                break;
            case SDL_FINGERDOWN://SDL_TouchFingerEvent:
                mouseState->mx = e.tfinger.x * SCREEN_WIDTH;
                mouseState->my = e.tfinger.y * SCREEN_HEIGHT;
            default:
                break;
        }
    }
    //
    return 0;
}

void handleKeys(SDL_Point &camPos, SDL_Point fieldSize, bool &updateVis, MouseState* mouseState){
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    //
    //KEYS
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
    //MOUSE
    mouseState->last_mx = mouseState->mx;
    mouseState->last_my = mouseState->my;
    //
    auto state = SDL_GetMouseState(&(mouseState->mx), &(mouseState->my));
    mouseState->mouseDown = (state == 1);
    //
    if(!mouseState->mouseDown && mouseState->busy > 1){
        mouseState->busy = 0;
    }
}

bool handleMouseTitle(MouseState* mouseState){
    mouseState->last_mx = mouseState->mx;
    mouseState->last_my = mouseState->my;
    auto state = SDL_GetMouseState(&(mouseState->mx), &(mouseState->my));
    //
    mouseState->mouseDown = (state == 1);
    //
    return false;
}

void startGame(short unsigned int* gameMode, MouseState* mouseState){
    *gameMode = 1;
    mouseState->busy = 0;
}

void machineClicked(Machine* machine, int speed, int progress){
    //
}

void placeMachine(std::vector<RendOb*> gameObjects[], MouseState *mouseState, Mix_Chunk *placeSound, Machine *placedMachine){
    for(int i = 0; i < gameObjects[2].size(); i++){
        if(gameObjects[2][i] == (RendOb*)placedMachine){
            gameObjects[2].erase(gameObjects[2].begin() + i);//remove ith item from 2nd layer of gameobjects
            break;
        }
    }
    //
    gameObjects[0].push_back(placedMachine);
    //
    Mix_PlayChannel( -1, placeSound, 0 );
}

void newMachine(int type, std::vector<RendOb*> gameObjects[], MouseState *mouseState, SDL_Texture *textures[], Mix_Chunk *pickupSound, Mix_Chunk *placeSound){
    printf("making new machine\n");
    RendOb* newMach = new Machine(mouseState->mx - PX_CELL_SIZE, mouseState->my - PX_CELL_SIZE, (machineType)type, textures[type], std::bind(placeMachine, gameObjects, mouseState, placeSound, std::placeholders::_1));
    //
    Mix_PlayChannel( -1, pickupSound, 0 );
    //
    gameObjects[2].push_back(newMach);
    //
    mouseState->busy = 1;
    mouseState->carrying = newMach;
}


void loadTextures(SDL_Texture *textures[], SDL_Renderer* rend){
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    textures[menuStartButton] = IMG_LoadTexture(rend, "assets/images/menuStartButton.png");
    textures[menuStartButtonHover] = IMG_LoadTexture(rend, "assets/images/menuStartButtonHover.png");
    textures[menuStartButtonClick] = IMG_LoadTexture(rend, "assets/images/menuStartButtonClick.png");
    textures[menuContinueButton] = IMG_LoadTexture(rend, "assets/images/menuContinueButton.png");
    textures[menuContinueButtonHover] = IMG_LoadTexture(rend, "assets/images/menuContinueButtonHover.png");
    textures[menuContinueButtonClick] = IMG_LoadTexture(rend, "assets/images/menuContinueButtonClick.png");
    //
    textures[shifterMachine] = IMG_LoadTexture(rend, "assets/images/shifterMachine.png");
    textures[smelterMachine] = IMG_LoadTexture(rend, "assets/images/smelterMachine.png");
    textures[sawMachine] = IMG_LoadTexture(rend, "assets/images/sawMachine.png");
    textures[rotaterMachine] = IMG_LoadTexture(rend, "assets/images/rotaterMachine.png");
    textures[pressMachine] = IMG_LoadTexture(rend, "assets/images/pressMachine.png");
    textures[recycleMachine] = IMG_LoadTexture(rend, "assets/images/recycleMachine.png");
    //
    //SDL_SetTextureScaleMode(textures[menuStartButton], SDL_ScaleModeBest);
}

void gameLoop(SDL_Renderer* rend, int screenWidth, int screenHeight){//function called once
    printf("starting game loop\n");
    //
    PX_CELL_SIZE = 32;
    SCREEN_CELL_HEIGHT = SCREEN_HEIGHT / PX_CELL_SIZE;
    SCREEN_CELL_WIDTH = SCREEN_WIDTH / PX_CELL_SIZE;
    //
    //float pixel_frac = 1 / SCREEN_WIDTH;//size of pixel movement in pinch gestures
    //SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");
    //
    short unsigned int gameMode = 0;
    //
    MouseState *mouseState = new MouseState();
    //
    std::vector<RendOb*> menuObjects[LAYER_NUM];
    std::vector<RendOb*> gameObjects[LAYER_NUM];
    std::vector<Machine*> inputMachines;
    //
    std::stack<Machine*> machineStack;
    //
    SDL_Texture *imageTextures[20];//set to random number right now
    loadTextures(imageTextures, rend);
    //
    SDL_Point fieldSize = {20, 15};//field size is in cells, (0,0) represents top left of grid
    SDL_Point camPos = {((fieldSize.x * PX_CELL_SIZE) - SCREEN_WIDTH) >> 1,//(field pixel width - screen pixel width) / 2
                        ((fieldSize.y * PX_CELL_SIZE) - SCREEN_HEIGHT) >> 1}; //position always describes top left corner
    SDL_Point menuPos = {0,0};
    printf("camPos: %d, %d\n", camPos.x, camPos.y);
	//
	SDL_Color lightbrown = hex2sdl("a29587");
	SDL_Color lightlightbrown = hex2sdl("ada092");
    //
    RendOb *startButton = new Button(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 150, 500, 125, imageTextures[menuStartButton], imageTextures[menuStartButtonHover], imageTextures[menuStartButtonClick], 2, std::bind(startGame, &gameMode, mouseState));
    RendOb *continueButton = new Button(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 + 25, 500, 125, imageTextures[menuContinueButton], imageTextures[menuContinueButtonHover], imageTextures[menuContinueButtonClick], 2, std::bind(startGame, &gameMode, mouseState));
    //Text *startText = new Text("Test", 0, 0, 300, 300, SDL_WHITE);
    menuObjects[0].push_back(startButton);
    menuObjects[0].push_back(continueButton);
    //menuObjects.push_back(startText);
	//
    //TTF_Font* font;
    //TTF_Init();
    //
    gameObjects[0].push_back(new Checkerboard(fieldSize, lightlightbrown, lightbrown));
    //
    //
    Mix_Chunk *pickupSound = Mix_LoadWAV("assets/sounds/pickUp.wav");
    Mix_Chunk *placeSound = Mix_LoadWAV("assets/sounds/place.wav");
    //printf("cur volume: %d\n", Mix_VolumeChunk(testChunk, -1));
    //
    ShopPopup *shop = new ShopPopup(200, 50, 30, std::bind(newMachine, std::placeholders::_1, gameObjects, mouseState, imageTextures, pickupSound, placeSound));
    gameObjects[1].push_back(shop);
    shop->addMachineButton(shifter, imageTextures[shifterMachine]);
    shop->addMachineButton(smelter, imageTextures[smelterMachine]);
    shop->addMachineButton(saw, imageTextures[sawMachine]);
    shop->addMachineButton(rotater, imageTextures[rotaterMachine]);
    shop->addMachineButton(press, imageTextures[pressMachine]);
    //
    int curTick = 0;
    //
    SDL_Event e;
    int quit = 0;
    while( quit == 0 ){
        quit = handleEvents(e, mouseState);
        //
        //printf("gamemode is ")
        switch(gameMode){
            case 0://title screen
                handleMouseTitle(mouseState);
                //
				for(unsigned int i = 0; i < menuObjects[0].size(); i++){
					menuObjects[0][i]->update(mouseState);
				}
				//
                render(rend, menuObjects, menuPos, SDLColor_DARK_BLUE);
                //
                if(gameMode == 1){
                    printf("last, busy: %d\n", mouseState->busy);
                }
                //
                break;
            default://main game
                bool updateVis = false;
                handleKeys(camPos, fieldSize, updateVis, mouseState);
                //
                if(updateVis){//only update visible on camera move or zoom
                    for(int j = 0; j < LAYER_NUM; j++){
                        for(int i = 0; i < gameObjects[j].size(); i++){
                            gameObjects[j][i]->updateVisibilty(camPos);
                        }
                    }
                }
                //
                for(int j = 0; j < LAYER_NUM; j++){
                    for(int i = 0; i < gameObjects[j].size(); i++){
                        gameObjects[j][i]->update(mouseState);//update state for all objects
                    }
                }
                //
                for(int i = 0; i < inputMachines.size(); i++){
                    inputMachines[i]->stepThroughMachine(machineStack);
                }
                //
                if(mouseState->busy == 0 && mouseState->mouseDown){//clicking on field and nothing else
                    mouseState->busy = 3;
                }
                //
                if(mouseState->busy == 3){//currently moving field
                    camPos.x += mouseState->last_mx - mouseState->mx;
                    camPos.y += mouseState->last_my - mouseState->my;
                    updateVis = true;
                }else if(mouseState->busy == 1){//currently carrying machine
                    //
                }
                //
                if(mouseState->pinchDist != 0){//this could be made smoother
                    float growthFactor = (mouseState->pinchDist * 20) + 1;
                    //printf("pinchDist: %f\n", mouseState->pinchDist);
                    PX_CELL_SIZE = ((float)PX_CELL_SIZE * growthFactor);
                    camPos.x += (mouseState->mx + camPos.x) * (growthFactor - 1);//adjust camera to zoom around mouse
                    camPos.y += (mouseState->my + camPos.y) * (growthFactor - 1);
                    //printf("calc cell size: %d\n", PX_CELL_SIZE);
                    updateVis = true;
                }
                //
                if(updateVis){
                    if(camPos.x < -(SCREEN_WIDTH >> 1)) camPos.x = -(SCREEN_WIDTH >> 1);
                    if(camPos.y < -(SCREEN_HEIGHT >> 1)) camPos.y = -(SCREEN_HEIGHT >> 1);
                    if(camPos.x > (fieldSize.x * PX_CELL_SIZE) - (SCREEN_WIDTH >> 1)) camPos.x = ((fieldSize.x * PX_CELL_SIZE) - (SCREEN_WIDTH >> 1));
                    if(camPos.y > (fieldSize.y * PX_CELL_SIZE) - (SCREEN_HEIGHT >> 1)) camPos.y = ((fieldSize.y * PX_CELL_SIZE) - (SCREEN_HEIGHT >> 1));
                    //
                    if(PX_CELL_SIZE < (SCREEN_HEIGHT >> 6)) PX_CELL_SIZE = (SCREEN_HEIGHT >> 6);
                    if(PX_CELL_SIZE > (SCREEN_HEIGHT >> 2)) PX_CELL_SIZE = SCREEN_HEIGHT >> 2;
                    SCREEN_CELL_HEIGHT = SCREEN_HEIGHT / PX_CELL_SIZE;
                    SCREEN_CELL_WIDTH = SCREEN_WIDTH / PX_CELL_SIZE;
                }
                //
                render(rend, gameObjects, camPos, SDLColor_DARK_BLUE);//TODO: highlights don't fit into layer thing
                //
                break;
        }
    }
}