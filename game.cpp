//handle game initialization and loop


//using list
#include <list>
#include "game.h"

int PX_CELL_SIZE;
int SCREEN_CELL_WIDTH;
int SCREEN_CELL_HEIGHT;

Game::Game(SDL_Renderer* rend){
    gameMode = 0;
    //
    PX_CELL_SIZE = 32;
    SCREEN_CELL_HEIGHT = SCREEN_HEIGHT / PX_CELL_SIZE;
    SCREEN_CELL_WIDTH = SCREEN_WIDTH / PX_CELL_SIZE;
    //
    fieldSize = {20, 15};//field size is in cells, (0,0) represents top left of grid
    fieldPos = {((fieldSize.x * PX_CELL_SIZE) - SCREEN_WIDTH) >> 1,//(field pixel width - screen pixel width) / 2
                ((fieldSize.y * PX_CELL_SIZE) - SCREEN_HEIGHT) >> 1}; //position always describes top left corner
    //
    mouseState = new MouseState();
    updateVis = false;
    //
    bkgColor = SDLColor_DARK_BLUE;
    //
    pickupSound = Mix_LoadWAV("assets/sounds/pickUp.wav");
    placeSound = Mix_LoadWAV("assets/sounds/place.wav");
    placeFailSound = Mix_LoadWAV("assets/sounds/placeFail.wav");
    //
    this->rend = rend;
    //
    loadTextures();
}

void Game::render(std::vector<RendOb*> objs[], SDL_Point camPos){
    SDL_SetRenderDrawColor(rend, bkgColor.r, bkgColor.g, bkgColor.b, 255);//clear screen(backbuffer)
    SDL_RenderClear(rend);
    //
    for(int j = 0; j < LAYER_NUM; j++){
        for(int i = 0; i < (int)objs[j].size(); i++){
        objs[j][i]->render(rend, camPos);
    }
    }
    //
    SDL_RenderPresent(rend);//load backbuffer onto screen
}

int Game::handleEvents(){
    mouseState->mouseUpEvent = false;
    mouseState->pinchDist = 0;
    mouseState->scrollX = 0;
    //
    SDL_Event e;
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

void Game::handleKeys(){
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    //
    //KEYS
    if(keystates[SDL_SCANCODE_RIGHT]){
        fieldPos.x += CAM_SPEED;
        updateVis = true;
    }else if(keystates[SDL_SCANCODE_LEFT]){
        fieldPos.x -= CAM_SPEED;
        updateVis = true;
    }
    //
    if(keystates[SDL_SCANCODE_DOWN]){
        fieldPos.y += CAM_SPEED;
        updateVis = true;
    }else if(keystates[SDL_SCANCODE_UP]){
        fieldPos.y -= CAM_SPEED;
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

bool Game::handleMouseTitle(){
    mouseState->last_mx = mouseState->mx;
    mouseState->last_my = mouseState->my;
    auto state = SDL_GetMouseState(&(mouseState->mx), &(mouseState->my));
    //
    mouseState->mouseDown = (state == 1);
    //
    return false;
}

void Game::startGame(){
    gameMode = 1;
    mouseState->busy = 0;
}

//NOT IMPLEMENTED YET
void machineClicked(Machine* machine, int speed, int progress){
    //
}

void Game::placeMachine(std::vector<RendOb*> gameObjects[], Machine *placedMachine){
    for(int i = 0; i < (int)gameObjects[2].size(); i++){
        if(gameObjects[2][i] == (RendOb*)placedMachine){
            gameObjects[2].erase(gameObjects[2].begin() + i);//remove ith item from 2nd layer of gameobjects
            break;
        }
    }
    //
    mouseState->busy = 0;
    //
    SDL_Point collisionPos;
    SDL_Point carryPos = placedMachine->getPlacingCellPos();
    int ret = validateCarryPos(carryPos, collisionPos, gameObjects);
    printf("returned %d from pos (%d, %d) and collision (%d, %d)\n", ret, carryPos.x, carryPos.y, collisionPos.x, collisionPos.y);
    if(ret == 0){//placed in invalid area, delete
        //delete placedMachine; //TODO: memory leak?
        Mix_PlayChannel( -1, placeFailSound, 0 );
        return;
    }
    //
    placedMachine->setPosition(carryPos.x, carryPos.y);
    //
    gameObjects[0].push_back(placedMachine);
    //
    Mix_PlayChannel( -1, placeSound, 0 );
}

void Game::newMachine(int type, std::vector<RendOb*> gameObjects[]){
    printf("making new machine\n");
    RendOb* newMach = new Machine(mouseState->mx - PX_CELL_SIZE, mouseState->my - PX_CELL_SIZE, (machineType)type, imageTextures[type], std::bind(&Game::placeMachine, this, gameObjects, std::placeholders::_1));
    //
    Mix_PlayChannel( -1, pickupSound, 0 );
    //
    gameObjects[2].push_back(newMach);
    //
    mouseState->busy = 1;
    mouseState->carrying = newMach;
}

void Game::loadTextures(){
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    imageTextures[menuStartButton] = IMG_LoadTexture(rend, "assets/images/menuStartButton.png");
    imageTextures[menuStartButtonHover] = IMG_LoadTexture(rend, "assets/images/menuStartButtonHover.png");
    imageTextures[menuStartButtonClick] = IMG_LoadTexture(rend, "assets/images/menuStartButtonClick.png");
    imageTextures[menuContinueButton] = IMG_LoadTexture(rend, "assets/images/menuContinueButton.png");
    imageTextures[menuContinueButtonHover] = IMG_LoadTexture(rend, "assets/images/menuContinueButtonHover.png");
    imageTextures[menuContinueButtonClick] = IMG_LoadTexture(rend, "assets/images/menuContinueButtonClick.png");
    //
    imageTextures[shifterMachine] = IMG_LoadTexture(rend, "assets/images/shifterMachine.png");
    imageTextures[smelterMachine] = IMG_LoadTexture(rend, "assets/images/smelterMachine.png");
    imageTextures[sawMachine] = IMG_LoadTexture(rend, "assets/images/sawMachine.png");
    imageTextures[rotaterMachine] = IMG_LoadTexture(rend, "assets/images/rotaterMachine.png");
    imageTextures[pressMachine] = IMG_LoadTexture(rend, "assets/images/pressMachine.png");
    imageTextures[recycleMachine] = IMG_LoadTexture(rend, "assets/images/recycleMachine.png");
    //
    //SDL_SetTextureScaleMode(textures[menuStartButton], SDL_ScaleModeBest);
}

int Game::validateCarryPos(SDL_Point &carryPos, SDL_Point &collisionPos, std::vector<RendOb*> objs[]){
    if(carryPos.x < 0){
        carryPos.x = 0;
    }else if(carryPos.x + 2 > fieldSize.x){
        carryPos.x = fieldSize.x - 2;
    }
    if(carryPos.y < 0){
        carryPos.y = 0;
    }else if(carryPos.y + 2 > fieldSize.y){
        carryPos.y = fieldSize.y - 2;
    }
    //
    /*SDL_Point pxPos = mouseState->carrying->getPosition();
    SDL_Point modPos = {pxPos.x % PX_CELL_SIZE, pxPos.y % PX_CELL_SIZE};
    SDL_Point leanPos;//position machine is leaning towards (to connect to)
    //|x-0.5| > |y-0.5| -> x is closer to grid line
    if(std::abs(modPos.x - (PX_CELL_SIZE >> 1)) > std::abs(modPos.y - (PX_CELL_SIZE >> 1))){//leaning vertically
        if(modPos.y > (PX_CELL_SIZE >> 1)){//going up (negative y)
            leanPos = {carryPos.x, carryPos.y - 1};
        }else{
            leanPos = {carryPos.x, carryPos.y + 1};
        }
    }else{//leaning horizontally
        if(modPos.x > (PX_CELL_SIZE >> 2)){//going left (negative x)
            leanPos = {carryPos.x - 1, carryPos.y};
        }else{
            leanPos = {carryPos.x + 1, carryPos.y};
        }
    }*/
    //
    bool foundConnection = false;
    for(int i = 0; i < (int)objs[0].size(); i++){
        if(dynamic_cast<Machine*>(objs[0][i]) == nullptr) continue;//only check machines
        collisionPos = objs[0][i]->getPosition();
        if(collisionDet(carryPos.x, carryPos.y, 2, 2,
                    collisionPos.x, collisionPos.y, 2, 2)){
            return 0;//placement in collision
        }
        //
        /*if(collisionDet(leanPos.x, leanPos.y, 2, 2,
                    collisionPos.x, collisionPos.y, 2, 2)){
            foundConnection = true;//no collision, leaning towards connection
            leanMachine = (Machine*)objs[0][i];
        }*/
        //
    }
    //
    //if(foundConnection) return 1;
    return 2;
}

void Game::gameLoop(){//function called once
    printf("starting game loop\n");
    //
    std::vector<RendOb*> menuObjects[LAYER_NUM];
    std::vector<RendOb*> gameObjects[LAYER_NUM];
    std::vector<Machine*> inputMachines;
    //
    std::stack<Machine*> machineStack;
    //
    SDL_Point menuPos = {0,0};
	//
	SDL_Color lightbrown = hex2sdl("a29587");
	SDL_Color lightlightbrown = hex2sdl("ada092");
    //
    RendOb *startButton = new Button(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 150, 500, 125, imageTextures[menuStartButton], imageTextures[menuStartButtonHover], imageTextures[menuStartButtonClick], 2, std::bind(&Game::startGame, this));
    RendOb *continueButton = new Button(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 + 25, 500, 125, imageTextures[menuContinueButton], imageTextures[menuContinueButtonHover], imageTextures[menuContinueButtonClick], 2, std::bind(&Game::startGame, this));
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
    //printf("cur volume: %d\n", Mix_VolumeChunk(testChunk, -1));
    //
    RendOb *highlight = new RendOb(0,0,2,2,SDLColor_CLEAR_BLUE,0);
    RendOb *connectionHighlight = new RendOb(0,0,2,2,SDLColor_CLEAR_GREEN,0);
    bool newHighlight = false;
    highlight->setVisibility(false);
    connectionHighlight->setVisibility(false);
    gameObjects[1].push_back(highlight);
    gameObjects[1].push_back(connectionHighlight);
    //
    ShopPopup *shop = new ShopPopup(200, 50, 30, std::bind(&Game::newMachine, this, std::placeholders::_1, gameObjects));
    gameObjects[1].push_back(shop);
    shop->addMachineButton(shifter, imageTextures[shifterMachine]);
    shop->addMachineButton(smelter, imageTextures[smelterMachine]);
    shop->addMachineButton(saw, imageTextures[sawMachine]);
    shop->addMachineButton(rotater, imageTextures[rotaterMachine]);
    shop->addMachineButton(press, imageTextures[pressMachine]);
    //
    int curTick = 0;
    //
    int quit = 0;
    while( quit == 0 ){
        quit = handleEvents();
        //
        switch(gameMode){
            case 0://title screen
                handleMouseTitle();
                //
				for(int i = 0; i < (int)menuObjects[0].size(); i++){
					menuObjects[0][i]->update(mouseState);
				}
				//
                render(menuObjects, menuPos);
                //
                if(gameMode == 1){
                    printf("last, busy: %d\n", mouseState->busy);
                }
                //
                break;
            default://main game
                bool updateVis = false;
                handleKeys();
                //
                if(updateVis){//only update visible on camera move or zoom
                    for(int j = 0; j < LAYER_NUM; j++){
                        for(int i = 0; i < (int)gameObjects[j].size(); i++){
                            gameObjects[j][i]->updateVisibilty(fieldPos);
                        }
                    }
                }
                //
                for(int j = 0; j < LAYER_NUM; j++){
                    for(int i = 0; i < (int)gameObjects[j].size(); i++){
                        gameObjects[j][i]->update(mouseState);//update state for all objects
                    }
                }
                //
                for(int i = 0; i < (int)inputMachines.size(); i++){
                    inputMachines[i]->stepThroughMachine(machineStack);
                }
                //
                if(mouseState->busy == 0 && mouseState->mouseDown){//clicking on field and nothing else
                    mouseState->busy = 3;
                }
                //
                if(mouseState->busy == 0 && newHighlight){
                    newHighlight = false;
                    highlight->setVisibility(false);
                    connectionHighlight->setVisibility(false);
                }
                //
                if(mouseState->busy == 3){//currently moving field
                    fieldPos.x += mouseState->last_mx - mouseState->mx;
                    fieldPos.y += mouseState->last_my - mouseState->my;
                    updateVis = true;
                }else if(mouseState->busy == 1){//currently carrying machine
                    if(newHighlight){
                        highlight->setVisibility(true);
                        SDL_Point carryingPos = ((Machine*)mouseState->carrying)->getPlacingCellPos();//look out for seg faults!
                        //
                        SDL_Point collisionPos;
                        int ret = validateCarryPos(carryingPos, collisionPos, gameObjects);
                        connectionHighlight->setVisibility(false);
                        if(ret == 0){//in collision
                            highlight->setColor(SDLColor_CLEAR_RED);
                        }else if(ret == 1){//found connection
                            connectionHighlight->setVisibility(true);
                            connectionHighlight->setPosition(leanMachine->getPosition().x, leanMachine->getPosition().y);
                            highlight->setColor(SDLColor_CLEAR_BLUE);
                            //
                        }else{//no collision
                            highlight->setColor(SDLColor_CLEAR_BLUE);
                            //
                        }
                        //
                        highlight->setPosition(carryingPos.x, carryingPos.y);
                    }else{
                        newHighlight = true;
                    }
                }
                //
                if(mouseState->pinchDist != 0){//this could be made smoother
                    float growthFactor = (mouseState->pinchDist * 20) + 1;
                    //printf("pinchDist: %f\n", mouseState->pinchDist);
                    PX_CELL_SIZE = ((float)PX_CELL_SIZE * growthFactor);
                    fieldPos.x += (mouseState->mx + fieldPos.x) * (growthFactor - 1);//adjust camera to zoom around mouse
                    fieldPos.y += (mouseState->my + fieldPos.y) * (growthFactor - 1);
                    //printf("calc cell size: %d\n", PX_CELL_SIZE);
                    updateVis = true;
                }
                //
                if(updateVis){
                    if(fieldPos.x < -(SCREEN_WIDTH >> 1)) fieldPos.x = -(SCREEN_WIDTH >> 1);
                    if(fieldPos.y < -(SCREEN_HEIGHT >> 1)) fieldPos.y = -(SCREEN_HEIGHT >> 1);
                    if(fieldPos.x > (fieldSize.x * PX_CELL_SIZE) - (SCREEN_WIDTH >> 1)) fieldPos.x = ((fieldSize.x * PX_CELL_SIZE) - (SCREEN_WIDTH >> 1));
                    if(fieldPos.y > (fieldSize.y * PX_CELL_SIZE) - (SCREEN_HEIGHT >> 1)) fieldPos.y = ((fieldSize.y * PX_CELL_SIZE) - (SCREEN_HEIGHT >> 1));
                    //
                    if(PX_CELL_SIZE < (SCREEN_HEIGHT >> 6)) PX_CELL_SIZE = (SCREEN_HEIGHT >> 6);
                    if(PX_CELL_SIZE > (SCREEN_HEIGHT >> 2)) PX_CELL_SIZE = SCREEN_HEIGHT >> 2;
                    SCREEN_CELL_HEIGHT = SCREEN_HEIGHT / PX_CELL_SIZE;
                    SCREEN_CELL_WIDTH = SCREEN_WIDTH / PX_CELL_SIZE;
                }
                //
                render(gameObjects, fieldPos);
                //
                break;
        }
    }
}