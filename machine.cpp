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
        if(mouseState->mouseUpEvent){
            if(placingStage == 1){
                placingStage++;
                return;
            }
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
    RendOb::render(rend, camPos);
}

int Machine::getPlacingStage(){
    return placingStage;
}

SDL_Point Machine::getPlacingCellPos(){
    return placingCellPos;
}

void Machine::setPlacingCellPos(SDL_Point cellPos){
    placingCellPos = cellPos;
}

machineType Machine::getType(){
    return type;
}

Belt::Belt(int x_pos, int y_pos, SDL_Texture *texture, std::function<void(Machine*)> machinePlaced) : Machine(x_pos, y_pos, belt, texture, machinePlaced){
    this->size.x = PX_CELL_SIZE;
    this->size.y = PX_CELL_SIZE;
    //
    beltSize = 0;
    beltStart = 0;
    beltEnd = 0;
}

bool Belt::process(){
    beltEnd = beltStart;//end is always one before start
    beltStart += 1;//rotate start position
    if(beltStart >= beltSize){
        beltStart = 0;
    }
    //
    printf("size: (%d, %d), pos: (%d, %d)\n", size.x, size.y, position.x, position.y);
    //
    if(inputMattes[0] != nullptr){//a material has been input
        movingMattes[beltStart] = inputMattes[0];//add matte to belt
        inputMattes[0] = nullptr;//remove from input
    }
    outputMattes[0] = movingMattes[beltEnd];
    //
    return true;
}

void Belt::update(MouseState *mouseState){
    if(placingStage > 0){
        if(mouseState->mouseUpEvent){
            placingStage++;//moves 1->2, 3->4
            if(placingStage == 2){//first release
                return;
            }
            //
            if(placingStage == 4){//second release (set starting position)
                position.x = placingCellPos.x;
                position.y = placingCellPos.y;
                //
                if(beltSize == -1){//belt start is in collision
                    machinePlaced(this);//destroy self
                }
                //
                printf("set position to %d, %d\n", position.x, position.y);
                //
                usingImage = false;
                color = SDLColor_CLEAR;//make invisible
                //
                return;
            }
            //
            //third release (done placing)
            //
            renderType = 0;
            placingStage = 0;
            //
            machinePlaced(this);//call game function to move layer
            //
            return;
        }else if(placingStage % 2 == 0 && mouseState->mouseDown){
            placingStage++;
        }
        //
        return;
    }
    //
    processTimer++;
    if(processTimer > machineSpeed){
        process();//transform input mattes into output mattes
        processTimer = 0;
    }
    //
    visited = false;
}

void Belt::render(SDL_Renderer* rend, SDL_Point camPos){
    if(placingStage > 0){
        Machine::render(rend, camPos);
    }else{
        //TODO: account for belt direction
        int startOffset = PX_CELL_SIZE * (processTimer % (machineSpeed / 4)) / machineSpeed;
        int colorStart = (processTimer % (machineSpeed / 2)) / (machineSpeed / 4);
        int offset;
        SDL_Rect curRect;
        SDL_Color curColor;
        //
        if(direction % 2 == 0){//vertical belt
            offset = (position.y * PX_CELL_SIZE) - camPos.y;
            curRect = {position.x * PX_CELL_SIZE - camPos.x + (PX_CELL_SIZE / 10), offset, (4 * PX_CELL_SIZE / 5), startOffset};
            curColor = (colorStart % 2 == 0) ? SDLColor_LIGHT_BLACK : SDLColor_DARK_BLUE;
            SDL_SetRenderDrawColor(rend, curColor.r, curColor.g, curColor.b, 255);
            SDL_RenderFillRect(rend, &curRect);
            //
            curRect.h = PX_CELL_SIZE / 4;
        }else{
            offset = (position.x * PX_CELL_SIZE) - camPos.x;
            curRect = {offset, position.y * PX_CELL_SIZE - camPos.y + (PX_CELL_SIZE / 10), startOffset, (4 * PX_CELL_SIZE / 5)};
            curColor = (colorStart % 2 == 0) ? SDLColor_LIGHT_BLACK : SDLColor_DARK_BLUE;
            SDL_SetRenderDrawColor(rend, curColor.r, curColor.g, curColor.b, 255);
            SDL_RenderFillRect(rend, &curRect);
            //
            curRect.w = PX_CELL_SIZE / 4;
        }
        offset += startOffset;
        //
        colorStart = !colorStart;
        for(int i = 0; i < beltSize * 4 - 1; i++){
            ((direction % 2 == 0) ? curRect.y : curRect.x) = offset + (i * (PX_CELL_SIZE / 4));
            curColor = ((i + colorStart) % 2 == 0) ? SDLColor_LIGHT_BLACK : SDLColor_DARK_BLUE;
            SDL_SetRenderDrawColor(rend, curColor.r, curColor.g, curColor.b, 255);
            SDL_RenderFillRect(rend, &curRect);
        }
        //
        colorStart = !colorStart;
        curColor = (colorStart % 2 == 0) ? SDLColor_LIGHT_BLACK : SDLColor_DARK_BLUE;
        ((direction % 2 == 0) ? curRect.y : curRect.x) = offset + ((beltSize * 4 - 1) * (PX_CELL_SIZE / 4));
        ((direction % 2 == 0) ? curRect.h : curRect.w) = (PX_CELL_SIZE / 4) - startOffset;
        SDL_SetRenderDrawColor(rend, curColor.r, curColor.g, curColor.b, 255);
        SDL_RenderFillRect(rend, &curRect);
    }
}

void Belt::setDirection(int direction){
    this->direction = direction;
}

int Belt::getDirection(){
    return direction;
}

void Belt::setUpBelt(int beltSize){
    this->beltSize = beltSize;
    this->beltEnd = beltSize - 1;
    //
    for(int i = 0; i < beltSize; i++){
        movingMattes.push_back(nullptr);
    }
}