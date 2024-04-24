#ifndef RENDOB_H
#define RENDOB_H

#include "utils.h"

class MouseState;
class RendOb{
    public:
        RendOb(int, int, int, int, SDL_Color);
        RendOb();//default constructor
        virtual void render(SDL_Renderer* rend, SDL_Point camPos);//draw the object onto the screen (default is just a rectangle)
        virtual void updateAnim(){}//update animation of object (every tick)
        virtual void updateAction(MouseState*){}//update object state (every four? ticks)
        void updateVisibilty(SDL_Point camPos);//update visibility based on camera position
        virtual void gridShift(int x, int y);//cells or rows were added to the grid, so a shift is necessary
    protected:
        short int offsetx, offsety;
        bool visible;
        SDL_Point position;
        SDL_Point size;
        SDL_Color color;
};

class Text : public RendOb{
    public:
        Text();
        //
        //Text(const char *text, int x_pos, int y_pos, int x_size, int y_size, SDL_Color color);
        //
        //Text(const char *text, int x_pos, int y_pos, int x_size, int y_size, SDL_Color color, const char *file, int fontSize = 24);
        //
        //virtual void render(SDL_Renderer* rend, SDL_Point camPos);
        //
        //void setFont(const char *file, int fontSize=24);
        //
        //void setFontSize(int fontSize);
        //
    protected:
        const char *text;
        TTF_Font* font;
        const char *fontFile;
};

class Button : public RendOb{
    public:
        Button(int, int, int, int, SDL_Color, SDL_Color, SDL_Color, std::function<void()>);
        //void render(SDL_Renderer* rend, SDL_Point camPos);
        void updateAction(MouseState*);
    protected:
        SDL_Color defaultColor;
        SDL_Color hoverColor;
        SDL_Color clickColor;
        std::function<void()> activationFunc;
        bool clicked;
};

class Checkerboard : public RendOb{
    public:
        Checkerboard(SDL_Point fieldSize, SDL_Color color1, SDL_Color color2, SDL_Color color3);
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void gridShift(int x, int y);//also changes checkerboard size
    protected:
        SDL_Color color2;
        SDL_Color borderColor;
};

#endif