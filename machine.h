#ifndef MACHINE_H
#define MACHINE_H

#include <vector>
#include "matte.h"
#include "utils.h"

class Machine : public RendOb{
    private:
        std::vector<Machine*> inputs;
        std::vector<Machine*> outputs;
        //
        bool visited;
        //
        Matte current[2];
        Matte output[2];
        //
        short int processCycles;
        short int cycleState;
        //
        short int maxOutputs;
        bool outputSpent[2];
        //
        void pushOutput();
        virtual void process() = 0;
    public:
        Matte getOutput(int outputId = 0);
};

class Shifter : public Machine{
    private:
        //
};

#endif