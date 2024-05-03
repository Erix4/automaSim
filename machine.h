#ifndef MACHINE_H
#define MACHINE_H

#include "popup.h"
//#include "rendob.h"
//#include "utils.h"

enum machineType {shifter, smelter, saw, rotater, press, recycle, belt};

/*
NOTES:
 - for now, machine automatically activates with max speed
 - when machines are manually activated, matte will be wasted whenever a machine outputs without
    a ready input in the corresponding output machine
 - for now, mattes are overridden in the process function
 - the process function will return a bool indicating whether it was activated succesfully or
    not, but for now it will return lots of falses because it's constantly called
*/
class Machine : public RendOb{
    protected:
        Machine* inputMachs[MAX_PUTS];
        Machine* outputMachs[MAX_PUTS];
        Matte* inputMattes[MAX_PUTS];
        Matte* outputMattes[MAX_PUTS];
        int inputIdxs[MAX_PUTS];
        //
        int configs[MAX_CONFIGS];//configurations for any given machine
        //
        machineType type;
        //std::function<void((std::vector<Matte*>)&, (std::vector<Matte*>)&)> process;
        std::function<void(Matte*)> wasteMatte;
        std::function<void(Machine*)> machinePlaced;//machine placed
        //
        bool visited;
        //
        int placingStage;//stage of being placed by mouse: 0-placed, 1-initial click, 2-first release, 3-second click, 4-second release
        SDL_Point placingCellPos;
        //
        int processTimer;
        int machineSpeed;
        //
        virtual bool process();
        //
        Button* selectionButton;//button which can be pressed to bring up machine popup
    public:
        Machine() : Machine(0, 0, shifter){}//default values for machine
        Machine(int x_pos, int y_pos, machineType type);//machine with no texture
        Machine(int x_pos, int y_pos, machineType type, SDL_Texture *texture, std::function<void(Machine*)> machinePlaced);//machine with texture
        //
        std::optional<Matte*> getOutput(int outputId = 0);
        void stepThroughMachine(std::stack<Machine*>& machineStack);
        void connectMachine(bool input, int idx, Machine* mach);
        //
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void update(MouseState *MouseState);
        //
        SDL_Point getPlacingCellPos();
        void setPlacingCellPos(SDL_Point);
        int getPlacingStage();
        machineType getType();
};

/*Belt is slightly different because it needs a resizeable array of materials on the belt*/
class Belt : public Machine{
    private:
        std::vector<Matte*> movingMattes;//vector so belt can be resized
        int beltStart;
        int beltEnd;
        int direction;//0-up,1-right,2-down,3-left
        //
        bool process();
    public:
        int beltSize;
        Belt() : Belt(0, 0, nullptr, nullptr){}
        Belt(int x_pos, int y_pos, SDL_Texture *texture, std::function<void(Machine*)> machinePlaced);
        //
        void render(SDL_Renderer* rend, SDL_Point camPos);
        void update(MouseState *MouseState);
        //
        void setDirection(int direction);
        int getDirection();
        void setUpBelt(int beltSize);
};

#endif