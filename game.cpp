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
                break;
            case SDL_MOUSEWHEEL:
                mouseState->pinchDist = (float)e.wheel.y / 100;
                //
                mouseState->scrollX = e.wheel.x * 10;
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
    //printf("mouse state: d? (%d) busy (%d) pos (%d, %d) uv (%d)\n", mouseState->mouseDown, mouseState->busy, mouseState->mx, mouseState->my, updateVis);
}

void Game::handleClick(std::vector<RendOb*> objs[]){
    if(!mouseState->mouseDown && mouseState->busy > 1){
        mouseState->busy = 0;
    }
    //
    if(mouseState->busy == 0 && mouseState->mouseDown){//clicking on field and nothing else
        mouseState->busy = 3;
    }
    //
    if(mouseState->busy == 3){//currently moving field
        fieldPos.x += mouseState->last_mx - mouseState->mx;
        fieldPos.y += mouseState->last_my - mouseState->my;
        updateVis = true;
    }else if(mouseState->busy == 1){//currently carrying machine
        Machine *curMachine = (Machine*)mouseState->carrying;
        //
        SDL_Point pxCarryingPos;
        SDL_Point carryingPos;
        //
        if(curMachine->getType() == belt){
            pxCarryingPos.x = mouseState->mx - (PX_CELL_SIZE / 2);//put carried machine in center of mouse
            pxCarryingPos.y = mouseState->my - (PX_CELL_SIZE / 2);
        }else{
            pxCarryingPos.x = mouseState->mx - PX_CELL_SIZE;//put carried machine in center of mouse
            pxCarryingPos.y = mouseState->my - PX_CELL_SIZE;
        }
        //
        int curPlacingStage = curMachine->getPlacingStage();//get placing stage
        //
        carryingPos.x = (pxCarryingPos.x + fieldPos.x + (PX_CELL_SIZE / 2)) / PX_CELL_SIZE;//round mouse pos to cell
        carryingPos.y = (pxCarryingPos.y + fieldPos.y + (PX_CELL_SIZE / 2)) / PX_CELL_SIZE;
        //
        int ret;
        Machine *collisionMachine;
        //
        if(curPlacingStage < 4){
            curMachine->setPosition(pxCarryingPos.x, pxCarryingPos.y);//set to mouse pos no matter what
            //
            ret = validateCarryPos(carryingPos, collisionMachine, objs);//move mouse pos into field & check for collision
            curMachine->setPlacingCellPos(carryingPos);//store validated carrypos in machine
        }else{//belt looking for 2nd location, set size
            SDL_Point machinePos = curMachine->getPosition();
            if(std::abs(carryingPos.x - machinePos.x) > std::abs(carryingPos.y - machinePos.y)){//more x than y movement
                //printf("more x movement bc (%d-%d) = %d > %d\n", carryingPos.x, machinePos.x, std::abs(carryingPos.x - machinePos.x), std::abs(carryingPos.y - machinePos.y));
                highlight->setSize({std::abs(carryingPos.x - machinePos.x) + 1, 1});
                if(carryingPos.x < machinePos.x){
                    carryingPos = {machinePos.x - highlight->getSize().x + 1, machinePos.y};
                }else{
                    carryingPos = machinePos;
                }
            }else{
                highlight->setSize({1, std::abs(carryingPos.y - machinePos.y) + 1});
                if(carryingPos.y < machinePos.y){
                    carryingPos = {machinePos.x, machinePos.y - highlight->getSize().y + 1};
                }else{
                    carryingPos = machinePos;
                }
            }
            //
            ret = validateCarryPos(carryingPos, collisionMachine, objs);//move mouse pos into field & check for collision
        }
        //
        if(curMachine->getType() == belt){
            ((Belt*)curMachine)->beltSize = ret - 1;//tell belt it's about to be destroyed if it so
            //
            for(int i = 0; i < 4; i++){
                if(i < numConnectedMachines){
                    connectionHighlights[i]->setSize(connectedMachines[i]->getSize());
                    connectionHighlights[i]->setPosition(connectedMachines[i]->getPosition().x, connectedMachines[i]->getPosition().y);
                    connectionHighlights[i]->setVisibility(true);
                }else{
                    connectionHighlights[i]->setVisibility(false);
                }
            }
        }
        //
        if(ret == 0){//in collision
            highlight->setColor(SDLColor_CLEAR_RED);
        }else{//no collision
            highlight->setColor(SDLColor_CLEAR_BLUE);
        }
        //
        highlight->setPosition(carryingPos.x, carryingPos.y);
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
    printf("entered place machine\n");
    for(int i = 0; i < (int)gameObjects[2].size(); i++){
        if(gameObjects[2][i] == (RendOb*)placedMachine){
            gameObjects[2].erase(gameObjects[2].begin() + i);//remove ith item from 2nd layer of gameobjects
            break;
        }
    }
    //
    mouseState->busy = 0;
    highlight->setVisibility(false);
    numConnectedMachines = 0;
    //
    Machine *collisionMachine;
    SDL_Point carryPos = highlight->getPosition();
    int ret = validateCarryPos(carryPos, collisionMachine, gameObjects);
    printf("returned %d from pos (%d, %d)\n", ret, carryPos.x, carryPos.y);
    if(ret == 0){//placed in invalid area, delete
        //delete placedMachine; //TODO: memory leak?
        Mix_PlayChannel( -1, placeFailSound, 0 );
        return;
    }
    //
    printf("checking (%d, %d) vs (%d, %d) and size (%d, %d)\n", placedMachine->getPosition().x, placedMachine->getPosition().y, highlight->getPosition().x, highlight->getPosition().y, highlight->getSize().x, highlight->getSize().y);
    if(placedMachine->getType() == belt){
        if(highlight->getPosition().x == placedMachine->getPosition().x && highlight->getPosition().y == placedMachine->getPosition().y){//going right or down
            if(highlight->getSize().x > highlight->getSize().y){//going right
                ((Belt*)placedMachine)->setDirection(1);
                ((Belt*)placedMachine)->setUpBelt(highlight->getSize().x);
            }else{//going down
                printf("going down, setting to %d\n", highlight->getSize().y);
                ((Belt*)placedMachine)->setDirection(2);
                ((Belt*)placedMachine)->setUpBelt(highlight->getSize().y);
            }
        }else if(highlight->getPosition().x < placedMachine->getPosition().x){//going left
            ((Belt*)placedMachine)->setDirection(3);
            ((Belt*)placedMachine)->setUpBelt(placedMachine->getPosition().x - highlight->getPosition().x);
        }else{//going up
            ((Belt*)placedMachine)->setDirection(0);
            ((Belt*)placedMachine)->setUpBelt(placedMachine->getPosition().y - highlight->getPosition().y);
        }
    }
    //
    placedMachine->setPosition(highlight->getPosition().x, highlight->getPosition().y);
    placedMachine->setSize(highlight->getSize());
    //
    gameObjects[0].push_back(placedMachine);
    //
    Mix_PlayChannel( -1, placeSound, 0 );
}

void Game::newMachine(int type, std::vector<RendOb*> gameObjects[]){
    printf("making new machine\n");
    //
    RendOb* newMach;
    if(type == belt){
        newMach = new Belt(mouseState->mx - PX_CELL_SIZE, mouseState->my - PX_CELL_SIZE, imageTextures[type], std::bind(&Game::placeMachine, this, gameObjects, std::placeholders::_1));
        highlight->setSize({1,1});
    }else{
        newMach = new Machine(mouseState->mx - PX_CELL_SIZE, mouseState->my - PX_CELL_SIZE, (machineType)type, imageTextures[type], std::bind(&Game::placeMachine, this, gameObjects, std::placeholders::_1));
        highlight->setSize({2,2});
    }
    //
    Mix_PlayChannel(-1, pickupSound, 0);
    //
    gameObjects[2].push_back(newMach);
    //
    mouseState->busy = 1;
    mouseState->carrying = newMach;
    //
    highlight->setVisibility(true);
    //
    newMach->render(rend, fieldPos);//set carrying pos
    SDL_Point carryingPos = ((Machine*)newMach)->getPlacingCellPos();
    highlight->setPosition(carryingPos.x, carryingPos.y);
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
    imageTextures[beltShopMachine] = IMG_LoadTexture(rend, "assets/images/beltShopMachine.png");
    //
    //SDL_SetTextureScaleMode(textures[menuStartButton], SDL_ScaleModeBest);
}

int Game::validateCarryPos(SDL_Point &carryPos, Machine *collisionMachine, std::vector<RendOb*> objs[]){
    Machine *carryingMachine = (Machine*)mouseState->carrying;//look out for seg faults!
    //
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
    if(carryingMachine->getType() != belt){
        for(int i = 0; i < (int)objs[0].size(); i++){
            if(dynamic_cast<Machine*>(objs[0][i]) == nullptr) continue;//only check machines
            collisionMachine = (Machine*)objs[0][i];
            SDL_Point collisionPos = collisionMachine->getPosition();
            SDL_Point collisionSize = collisionMachine->getSize();//can vary for belts
            if(collisionDet(carryPos.x, carryPos.y, 2, 2,
                        collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
                return 0;//placement in collision
            }
        }
        return 2;
    }
    //
    bool foundConnection = false;
    numConnectedMachines = 0;
    for(int i = 0; i < (int)objs[0].size(); i++){
        if(dynamic_cast<Machine*>(objs[0][i]) == nullptr) continue;//only check machines
        collisionMachine = (Machine*)objs[0][i];
        SDL_Point collisionPos = collisionMachine->getPosition();
        SDL_Point collisionSize = collisionMachine->getSize();
        if(collisionDet(carryPos.x, carryPos.y, highlight->getSize().x, highlight->getSize().y,
                    collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
            return 0;//placement in collision
        }
        //
        if(carryingMachine->getType() != belt && collisionMachine->getType() != belt) continue;
        //
        //TODO: machines aren't connecting to belts
        //
        if(carryingMachine->getPlacingStage() < 4){//placing initial position, check on all sides
            if(collisionDet(carryPos.x - 1, carryPos.y, 1, 1,
                    collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
                connectedMachines[numConnectedMachines] = collisionMachine;
                numConnectedMachines++;
                foundConnection = true;
                continue;
            }
            //
            if(collisionDet(carryPos.x, carryPos.y - 1, 1, 1,
                    collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
                connectedMachines[numConnectedMachines] = collisionMachine;
                numConnectedMachines++;
                foundConnection = true;
                continue;
            }
            //
            if(collisionDet(carryPos.x + 1, carryPos.y, 1, 1,
                    collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
                connectedMachines[numConnectedMachines] = collisionMachine;
                numConnectedMachines++;
                foundConnection = true;
                continue;
            }
            //
            if(collisionDet(carryPos.x, carryPos.y + 1, 1, 1,
                    collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
                connectedMachines[numConnectedMachines] = collisionMachine;
                numConnectedMachines++;
                foundConnection = true;
                continue;
            }
        }else{//must be belt in second stage
            //TODO: allow highlighting of machine fr/ first & second stages
            SDL_Point testPoint = carryPos;
            switch(((Belt*)carryingMachine)->getDirection()){
                case 0://up
                    testPoint.y -= 1;
                    break;
                case 1://right
                    testPoint.x += 1;
                    break;
                case 2://down
                    testPoint.y += 1;
                    break;
                default://left
                    testPoint.x -= 1;
            }
            if(collisionDet(carryPos.x - 1, carryPos.y, carryingMachine->getSize().x, carryingMachine->getSize().y,
                    collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y)){
                connectedMachines[0] = collisionMachine;
                numConnectedMachines = 1;
                foundConnection = true;
                continue;
            }
        }
    }
    //
    if(foundConnection) return 1;
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
    highlight = new RendOb(0,0,2,2,SDLColor_CLEAR_BLUE,0);
    highlight->setVisibility(false);
    gameObjects[1].push_back(highlight);
    for(int i = 0; i < 4; i++){
        connectionHighlights[i] = new RendOb(0,0,2,2,SDLColor_CLEAR_GREEN,0);
        connectionHighlights[i]->setVisibility(false);
        gameObjects[1].push_back(connectionHighlights[i]);
    }
    //
    ShopPopup *shop = new ShopPopup(200, 50, 30, std::bind(&Game::newMachine, this, std::placeholders::_1, gameObjects));
    gameObjects[1].push_back(shop);
    shop->addMachineButton(belt, imageTextures[beltShopMachine]);
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
                handleClick(gameObjects);//wait until after update to allow button collision checking
                //
                render(gameObjects, fieldPos);
                //
                break;
        }
    }
}