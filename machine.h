#ifndef MACHINE_H
#define MACHINE_H

#include "popup.h"
//#include "rendob.h"
//#include "utils.h"

enum machineType {shifter, smelter, saw, rotater, press, recycle};

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
        std::function<void(Machine*)> machineClicked;//opens machine popup
        //
        bool visited;
        //
        bool placing;//whether machine is currently being placed by mouse
        //
        int processTimer;
        int machineSpeed;
        //
        virtual bool process();
        void updateAnim();
        void updateAction();
        //
        Button* selectionButton;//button which can be pressed to bring up machine popup
    public:
        Machine() : Machine(0, 0, shifter){}//default values for machine
        Machine(int x_pos, int y_pos, machineType type);//machine with no texture
        Machine(int x_pos, int y_pos, machineType type, SDL_Texture *texture);//machine with texture
        std::optional<Matte*> getOutput(int outputId = 0);
        void stepThroughMachine(std::stack<Machine*>& machineStack);
        void connectMachine(bool input, int idx, Machine* mach);
};

/*Belt is slightly different because it needs a resizeable array of materials on the belt*/
class Belt : public Machine{
    private:
        std::vector<Matte*> movingMattes;//vector so belt can be resized
        int beltStart;
        int beltEnd;
        bool process();
    public:
        int beltSize;
        Belt() : Belt(0, 0, 1, 1){}
        Belt(int x_pos, int y_pos, int x_size, int y_size);
};

#endif