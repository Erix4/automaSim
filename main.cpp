// Handle startup, exit, and window management

/*
Notes for myself:

This isn't finished (duh)

Make sure to add new files to the OBJS in the Makefile
You shouldn't need to touch this file, look in game.cpp and beyond instead
*/


//Using SDL and standard IO
#include "game.h"
#include <list>
#include <ctime>
#include <iostream>

using namespace std;

int main(int argc, char* args[]){
    cout << "Test" << endl;
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    //
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;
    //
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    }
    //
    //Create window
    window = SDL_CreateWindow( "Automa Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ){
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    }
    //
    //Create a renderer
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, render_flags);
    if (!rend){
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    //
    //Get window surface
    screenSurface = SDL_GetWindowSurface( window );
    //
    //Fill the surface black (replaced in rendering loop)
    SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0, 0, 0 ) );
    //
    //Update the surface
    SDL_UpdateWindowSurface( window );
    //
    srand ( time(NULL) );
    //
    /*if (TTF_Init() < 0) {//initialize ttf library
        SDL_Log("Couldn't initialize TTF: %s\n",SDL_GetError());
        SDL_Quit();
        return(2);
    }*/
    //
    printf("calling game loop\n");
    //
    gameLoop(rend, SCREEN_WIDTH, SCREEN_HEIGHT);
    //
    //Destroy window
    SDL_DestroyWindow( window );
    //
    //Quit SDL subsystems
    SDL_Quit();
    //
    return 0;
}
