#ifndef GAME_HEADER
#define GAME_HEADER

#include "machine.h" //game -> machine -> popup -> matte -> rendob -> utils

enum textureTypes{shifterMachine, smelterMachine, sawMachine, rotaterMachine, pressMachine, recycleMachine, beltShopMachine,
                menuStartButton, menuStartButtonHover, menuStartButtonClick,
                menuContinueButton, menuContinueButtonHover, menuContinueButtonClick,
};

//game.cpp

class Game{
    private:
        int gameMode;
        //
        SDL_Point fieldSize;
        SDL_Point fieldPos;//field position
        MouseState *mouseState;
        bool updateVis;//whether to update object visibility
        //
        SDL_Color bkgColor;
        //
        Mix_Chunk *placeSound;
        Mix_Chunk *pickupSound;
        Mix_Chunk *placeFailSound;
        //
        SDL_Renderer *rend;
        SDL_Texture *imageTextures[20];
        //
        Machine *leanMachine;//machine that will be connected to
        //
        int handleEvents();
        void handleKeys();
        bool handleMouseTitle();
        void loadTextures();
        int validateCarryPos(SDL_Point &carryPos, SDL_Point &collisionPos, std::vector<RendOb*> objs[]);//validates carrypos and returns machine in collision, if applicable
        void render(std::vector<RendOb*> objs[], SDL_Point camPos);
    public:
        Game(SDL_Renderer* rend);
        void gameLoop();
        void startGame();
        void placeMachine(std::vector<RendOb*> objs[], Machine *placedMachine);
        void newMachine(int type, std::vector<RendOb*> gameObjects[]);
};

#endif