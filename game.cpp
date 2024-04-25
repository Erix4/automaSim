//handle game initialization and loop


//using list
#include <list>
#include "game.h"

int PX_CELL_SIZE;
int SCREEN_CELL_WIDTH;
int SCREEN_CELL_HEIGHT;

void render(SDL_Renderer* rend, std::vector<RendOb*> objs, SDL_Point camPos, SDL_Color bkgColor){
    SDL_SetRenderDrawColor(rend, bkgColor.r, bkgColor.g, bkgColor.b, 255);//clear screen(backbuffer)
    SDL_RenderClear(rend);
    //
    for(unsigned int i = 0; i < objs.size(); i++){
        //printf("rendering %d\n", i);
        objs[i]->render(rend, camPos);
    }
    //
    SDL_RenderPresent(rend);//load backbuffer onto screen
}

int handleEvents(SDL_Event e, MouseState* mouseState){
    mouseState->mouseUpEvent = false;
    mouseState->pinchDist = 0;
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
            case SDL_MOUSEWHEEL://TODO: this
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
    if(!mouseState->mouseDown && mouseState->busy){
        mouseState->busy = false;
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
}

void machineClicked(Machine* machine, int speed, int progress){
    //
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
    std::vector<RendOb*> menuObjects;
    std::vector<RendOb*> gameObjects;
    std::vector<Machine*> inputMachines;
    //
    std::stack<Machine*> machineStack;
    //
    SDL_Point fieldSize = {20, 15};//field size is in cells, (0,0) represents top left of grid
    SDL_Point camPos = {((fieldSize.x * PX_CELL_SIZE) - SCREEN_WIDTH) >> 1,//(field pixel width - screen pixel width) / 2
                        ((fieldSize.y * PX_CELL_SIZE) - SCREEN_HEIGHT) >> 1}; //position always describes top left corner
    SDL_Point menuPos = {0,0};
    printf("camPos: %d, %d\n", camPos.x, camPos.y);
    //
    SDL_Color lightGray = hex2sdl("#dbdbdb");
    SDL_Color gray = hex2sdl("#878787");
	SDL_Color pureblack = {0,0,0};
	//
	SDL_Color lightpurple = hex2sdl("d8d4f2");
	SDL_Color redpurple = hex2sdl("c4b2bc");
	SDL_Color lightbrown = hex2sdl("a29587");
	SDL_Color lightlightbrown = hex2sdl("ada092");
	SDL_Color brown = hex2sdl("846c5b");
	SDL_Color black = hex2sdl("332e3c");
    //
    RendOb *startButton = new Button(2, SCREEN_CELL_HEIGHT / 2 - 5, SCREEN_CELL_WIDTH - 4, 4, lightpurple, redpurple, black, std::bind(startGame, &gameMode, mouseState));
    RendOb *resumeButton = new Button(2, SCREEN_CELL_HEIGHT / 2 + 1, SCREEN_CELL_WIDTH - 4, 4, lightpurple, redpurple, black, [](){});
    //Text *startText = new Text("Test", 0, 0, 300, 300, SDL_WHITE);
    menuObjects.push_back(startButton);
    menuObjects.push_back(resumeButton);
    //menuObjects.push_back(startText);
	//menuObjects
	//
    //TTF_Font* font;
    //TTF_Init();
    //
    gameObjects.push_back(new Checkerboard(fieldSize, lightlightbrown, lightbrown));
    gameObjects.push_back(new RendOb(10, 10, 2, 5, brown));
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
				for(unsigned int i = 0; i < menuObjects.size(); i++){
					menuObjects[i]->updateAction(mouseState);
				}
				//
                render(rend, menuObjects, menuPos, pureblack);
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
                        gameObjects[i]->updateAction(mouseState);
                    }
                    //
                    curTick = 0;
                }
                //
                for(unsigned int i = 0; i < inputMachines.size(); i++){
                    inputMachines[i]->stepThroughMachine(machineStack);
                }
                //
                if(!mouseState->busy && mouseState->mouseDown){
                    camPos.x += mouseState->last_mx - mouseState->mx;
                    camPos.y += mouseState->last_my - mouseState->my;
                    updateVis = true;
                }
                //
                if(mouseState->pinchDist != 0){//this could be made smoother
                    printf("pinchDist: %f\n", mouseState->pinchDist);
                    if(mouseState->pinchDist > 0){
                        PX_CELL_SIZE = ((float)PX_CELL_SIZE * ((mouseState->pinchDist * 20) + 1));
                    }else{
                        PX_CELL_SIZE = ((float)PX_CELL_SIZE * ((mouseState->pinchDist * 20) - 1));
                    }
                    printf("new cell size: %d\n", PX_CELL_SIZE);
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
                render(rend, gameObjects, camPos, gray);
                curTick++;
                //
                break;
        }
    }
}