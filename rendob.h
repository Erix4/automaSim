#ifndef RENDOB_H
#define RENDOB_H

#include "utils.h"

extern int PX_CELL_SIZE;
extern int SCREEN_CELL_WIDTH;
extern int SCREEN_CELL_HEIGHT;

class MouseState;
class RendOb{
    public:
        RendOb(int, int, int, int, SDL_Color);
        RendOb() : RendOb(0,0,0,0,SDLColor_WHITE){}//default constructor
        RendOb(int, int, int, int, SDL_Texture *texture);//default constructor
        ~RendOb();
        virtual void render(SDL_Renderer* rend, SDL_Point camPos);//draw the object onto the screen (default is just a rectangle)
        virtual void update(MouseState*){}//update object state
        void updateVisibilty(SDL_Point camPos);//update visibility based on camera position
        virtual void gridShift(int x, int y);//cells or rows were added to the grid, so a shift is necessary
        void setPosition(int x, int y);
    protected:
        bool visible;
        bool usingImage;
        SDL_Texture *texture;
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
        Button() : Button(0,0,5,5,SDLColor_BLACK,{150,150,150,255},SDLColor_CLEAR, nullptr){}
        Button(int, int, int, int, SDL_Color, SDL_Color, SDL_Color, std::function<void()>);
        Button(int, int, int, int, SDL_Texture*, SDL_Texture*, SDL_Texture*, std::function<void()>);
        //void render(SDL_Renderer* rend, SDL_Point camPos);
        virtual void update(MouseState*);
        void setActivationFunc(std::function<void()> func);
        void render(SDL_Renderer* rend, SDL_Point camPos);
    protected:
        SDL_Color defaultColor;
        SDL_Color hoverColor;
        SDL_Color clickColor;
        std::function<void()> activationFunc;
        bool clicked;
        //
        SDL_Texture *defaultTexture;
        SDL_Texture *hoverTexture;
        SDL_Texture *clickTexture;
        //
        SDL_Point lastCamPos;
};

class ButtonStatic : public Button{
    public:
        using Button::Button;
        void update(MouseState*);
        void render(SDL_Renderer* rend, SDL_Point camPos);
};

class ButtonPx : public Button{
    public:
        using Button::Button;
        void update(MouseState*);
        void render(SDL_Renderer* rend, SDL_Point camPos);
};

class Checkerboard : public RendOb{
    public:
        Checkerboard(SDL_Point fieldSize, SDL_Color color1, SDL_Color color2);
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void gridShift(int x, int y);//also changes checkerboard size
    protected:
        SDL_Color color2;
};

#endif