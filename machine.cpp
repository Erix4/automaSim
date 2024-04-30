#include "machine.h"

Machine::Machine(int x_pos, int y_pos, machineType type) : RendOb(x_pos, y_pos, 2, 2, SDLColor_WHITE, 0){
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
    this->type = type;
    //
    for(int i = 0; i < MAX_CONFIGS; i++){
        configs[i] = 0;
    }
}

Machine::Machine(int x_pos, int y_pos, machineType type, SDL_Texture *texture, std::function<void(Machine*)> machinePlaced) : RendOb(x_pos, y_pos, 2 * PX_CELL_SIZE, 2 * PX_CELL_SIZE, texture, 2){
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
    placingStage = 1;
    placingCellPos = {0, 0};
    //
    this->machinePlaced = machinePlaced;
    //
    this->type = type;
    //
    for(int i = 0; i < MAX_CONFIGS; i++){
        configs[i] = 0;
    }
}

void Machine::update(MouseState *mouseState){
    if(placingStage > 0){
        position.x = mouseState->mx - PX_CELL_SIZE;
        position.y = mouseState->my - PX_CELL_SIZE;
        //
        if(mouseState->mouseUpEvent){
            if(placingStage == 1){
                placingStage++;
                return;
            }
            position.x = placingCellPos.x;
            position.y = placingCellPos.y;
            //
            size.x = 2;//switch to cell sizing
            size.y = 2;
            //
            renderType = 0;
            placingStage = 0;
            //
            machinePlaced(this);//call game function to move layer
            //
            return;
        }else if(placingStage == 2 && mouseState->mouseDown){
            placingStage++;
        }
        //
        return;
    }
    //
    if(processTimer > machineSpeed){
        process();//transform input mattes into output mattes
    }
    //
    visited = false;
}

bool Machine::process(){//default process function
    if(inputMattes[0] == nullptr) return false;//no material to process
    //
    switch(type){
        case shifter:
            //do stuff here
            //break;
        default:
            outputMattes[0] = inputMattes[0];//pass straight through
            inputMattes[0] = nullptr;//remove from input
            break;
    }
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

void Machine::render(SDL_Renderer* rend, SDL_Point camPos){
    if(placingStage > 0){
        placingCellPos.x = (position.x + camPos.x + (PX_CELL_SIZE / 2)) / PX_CELL_SIZE;
        placingCellPos.y = (position.y + camPos.y + (PX_CELL_SIZE / 2)) / PX_CELL_SIZE;
        //
        /*SDL_Rect highlightRect = {((placingCellPos.x * PX_CELL_SIZE) - camPos.x),
                                ((placingCellPos.y * PX_CELL_SIZE) - camPos.y),
                                size.x * PX_CELL_SIZE,
                                size.y * PX_CELL_SIZE};
        //
        //TODO: collision detection for highlight color
        SDL_SetRenderDrawColor(rend, SDLColor_LIGHT_BLUE.r, SDLColor_LIGHT_BLUE.g, SDLColor_LIGHT_BLUE.b, 100);
        SDL_RenderFillRect(rend, &highlightRect);*/
    }
    RendOb::render(rend, camPos);
}

SDL_Point Machine::getPlacingCellPos(){
    return placingCellPos;
}

int Machine::getPlacingStage(){
    return placingStage;
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