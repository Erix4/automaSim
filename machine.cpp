#include "machine.h"

Machine::Machine() : RendOb(0,0,0,0,SDL_WHITE){
    visited = false;
    //
    for(int i = 0; i < MAX_PUTS; i++){
        inputMachs[i] = nullptr;
        outputMachs[i] = nullptr;
        inputMattes[i] = nullptr;
        outputMattes[i] = nullptr;
        inputIdxs[i] = 0;
    }
    //
    processTimer = 0;
    machineSpeed = 128;//starting speed for any machine
    //
    for(int i = 0; i < MAX_CONFIGS; i++){
        configs[i] = 0;
    }
}

Machine::Machine(int x_pos, int y_pos, int x_size, int y_size, machineType type) : RendOb(x_pos, y_pos, x_size, y_size,SDL_WHITE){
    visited = false;
    //
    for(int i = 0; i < MAX_PUTS; i++){
        inputMachs[i] = nullptr;
        outputMachs[i] = nullptr;
        inputMattes[i] = nullptr;
        outputMattes[i] = nullptr;
        inputIdxs[i] = 0;
    }
    //
    processTimer = 0;
    machineSpeed = 128;//starting speed for any machine
    //
    for(int i = 0; i < MAX_CONFIGS; i++){
        configs[i] = 0;
    }
}

void Machine::updateAction(){
    if(processTimer > machineSpeed){
        process();//transform input mattes into output mattes
    }
    //
    visited = false;
}

void Machine::updateAnim(){
    //
}

bool Machine::process(){//default process function
    if(inputMattes[0] == nullptr) return false;//no material to process
    //
    outputMattes[0] = inputMattes[0];//pass straight through
    inputMattes[0] = nullptr;//remove from input
    //
    return true;//succesfully processed
}

void Machine::stepThroughMachine(std::stack<Machine*>& machineStack){
    if(visited) return;//if already visited leave immediately
    //
    for(int i = 0; i < MAX_PUTS; i++){
        if(inputMachs[i] == nullptr) continue;//if no machine at i, skip
        //
        std::optional<Matte*> optOutput = inputMachs[i]->getOutput(i);//get any outputs from the input machine
        if(!optOutput.has_value()){//an output was found
            if(inputMattes[i] == nullptr){//check if slot is open
                inputMattes[i] = optOutput.value();//add output to inputs
            }else{
                //wasteMatte(optOutput.value());//waste material pushed out by input machine
            }
        }
        //
        machineStack.push(inputMachs[i]);//add input machines to stack
    }
    //
    for(int i = 0; i < MAX_PUTS; i++){
        if(outputMachs[i] == nullptr) continue;//if no machine at i, skip
        machineStack.push(outputMachs[i]);
    }
}

std::optional<Matte*> Machine::getOutput(int outputId){
    if(outputMattes[outputId] == nullptr){
        return std::nullopt;
    }else{
        Matte* tempOut = outputMattes[outputId];
        outputMattes[outputId] = nullptr;
        return tempOut;
    }
}

void Machine::connectMachine(bool input, int idx, Machine* mach){
    if(idx > MAX_PUTS) return;
    if(input){
        inputMachs[idx] = mach;
    }else{
        outputMachs[idx] = mach;
    }
}

Belt::Belt(int x_pos, int y_pos, int x_size, int y_size){
    visited = false;
    //
    for(int i = 0; i < MAX_PUTS; i++){
        inputMachs[i] = nullptr;
        outputMachs[i] = nullptr;
        inputMattes[i] = nullptr;
        outputMattes[i] = nullptr;
        inputIdxs[i] = 0;
    }
    //
    //for(int i = 0;)
    processTimer = 0;
    machineSpeed = 128;//starting speed for any machine
}

bool Belt::process(){
    beltEnd = beltStart;//end is always one before start
    beltStart += 1;//rotate start position
    if(beltStart >= beltSize){
        beltStart = 0;
    }
    //
    if(inputMattes[0] != nullptr){//a material has been input
        movingMattes[beltStart] = inputMattes[0];//add matte to belt
        inputMattes[0] = nullptr;//remove from input
    }
    outputMattes[0] = movingMattes[beltEnd];
    //
    return true;
}