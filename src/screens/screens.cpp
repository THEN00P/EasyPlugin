#include "screens.hpp"
#include <psp2/kernel/clib.h>
#include <bits/stdc++.h>

bool sortIndex(Screen *a, Screen *b) {
    return a->zIndex < b->zIndex;
}

void Screens::addScreen(Screen *screen) {
    components.push_back(screen);
}

void Screens::removeScreen(Screen *screen) {
    for(int i=0; i<components.size(); i++) {
        if(screen == components[i]) {
            components.erase(components.begin() + i);
        }
    }
}

void Screens::update(Input input) {
    std::sort(components.begin(), components.end(), sortIndex);

    sceClibPrintf("%i \n", components[0]->zIndex);

    for(Screen *component : components) {
        component->draw();
    }

    for(Screen *component : components) {
        component->handleInput(input);

        if(component->blockInput) break;
    }
}