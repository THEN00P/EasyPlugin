#include "screens.hpp"
#include <algorithm>
#include <psp2/kernel/clib.h>
#include <bits/stdc++.h>

bool sortIndex(Screen *a, Screen *b) {
    return a->zIndex < b->zIndex;
}

void Screens::addScreen(Screen *screen) {
    components.push_back(screen);
}

//wait until input handling and drawing is finished
void Screens::removeScreen(Screen *screen) {
    removeQueue.push_back(screen);
}

void Screens::nuke() {
    for (size_t i = 0; i < components.size(); i++) {
        removeScreen(components[i]);
    }
    
}

void Screens::log() {
    for (size_t i = 0; i < components.size(); i++)
    {
        sceClibPrintf("%d\n", components[i]->zIndex);
    }    
}

void Screens::update(Input input) {
    for (int i = 0; i < removeQueue.size(); i++) {
        for(int j=0; j<components.size(); j++) {
            if(removeQueue[i] == components[j]) {
                components.erase(components.begin() + j);
                components[j]->free();
                removeQueue.erase(removeQueue.begin() + i);
            }
        }
    }

    int transparencyID = 0;
    // std::sort(components.begin(),components.end(), sortIndex);

    for(int i=components.size(); i --> 0; ) {
        if(!components[i]->transparency) {
            transparencyID = i;
            break;
        }
    }

    for(int i=transparencyID; i < components.size(); i++) {
        components[i]->draw();
    }

    for(int i=components.size(); i --> 0; ) {
        components[i]->handleInput(input);

        if(components[i]->blockInput) break;
    }
}