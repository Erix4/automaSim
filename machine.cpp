#include "machine.h"

Machine::Machine(){
    visited = false;
}

void Machine::updateAction(){
    if(visited){//if already visited, nothing to do
        return;
    }
    visited = true;
    //
    for(int i = 0; i < outputs.size(); i++){//1. recurse outputs
        outputs[i]->updateAction();
    }
    //
    process();
    //
    for(int i = 0; i < inputs.size(); i++){//3. recurse inputs
        inputs[i]->updateAction();
    }
    //
    //5. update anim
    updateAnim();
}

void Machine::pushOutput(){
    cycleState++;//2. process state
    if(cycleState == processCycles){
        cycleState = 0;
        output[0] = current[0];
        output[1] = current[1];
        if(!outputSpent[0]){
            //TODO: loss of material, wasted money
        }
        if(!outputSpent[1]){
            //TODO: loss of material, wasted money
        }
        outputSpent[0] = false;
        outputSpent[1] = false;
    }
}

Matte Machine::getOutput(int outputId){
    if(outputId >= maxOutputs || outputSpent[outputId]){
        return {};
    }else{
        outputSpent[outputId] = true;
        return output;
    }
}


Shifter::Shifter() : Machine(){
    maxOutputs = 1;
    outputSpent = {false, false};
}

void Shifter::process(){
    pushOutput();
    //
    if(cycleState == 0){//current is empty and ready for input
        auto outputAttempt = inputs[0].getOutput(0);
        if(outputAttempt.has_value()){
            Matte rawMatte = outputAttempt.value();
            //
            //TODO: shifter function
        }else{
            return;//no input material to process
        }
    }
    return;
}